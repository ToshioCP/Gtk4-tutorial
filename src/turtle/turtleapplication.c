#include <gtk/gtk.h>
#include "tfetextview.h"
#include "turtle_lex.h"
#include "turtle_parser.h"

static GtkWidget *win;
static GtkWidget *tv;
static GtkWidget *da;

cairo_surface_t *surface = NULL;

static void
tfe_error_alert (GtkWindow *win, GError *err) {
  GtkAlertDialog *alert_dialog;
  GtkWindow *parent;

  if (win != NULL && gtk_widget_get_mapped (GTK_WIDGET (win)))
    parent = win;
  else
    parent = NULL;
  alert_dialog = gtk_alert_dialog_new ("%s", err->message);
  gtk_alert_dialog_show (alert_dialog, parent);
  g_object_unref (alert_dialog);
}

void
run_cb (GtkWidget *btnr) {
  GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  GtkTextIter start_iter;
  GtkTextIter end_iter;
  char *contents;
  int stat;
  static gboolean busy = FALSE; /* initialized only once */

  /* yyparse() and run() are NOT thread safe. */
  /* The variable busy avoids reentrance. */
  if (busy)
    return;
  busy = TRUE;
  gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
  contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
  if (surface && contents[0] != '\0') {
    init_flex (contents);
    stat = yyparse ();
    if (stat == 0) { /* No error */
      run ();
    }
    finalize_flex ();
  }
  g_free (contents);
  gtk_widget_queue_draw (GTK_WIDGET (da));
  busy = FALSE;
}

static void
open_dialog_cb (GObject *source_object, GAsyncResult *res, gpointer data) {
  GtkFileDialog *dialog = GTK_FILE_DIALOG (source_object);
  GtkWindow *win = GTK_WINDOW (data);
  GFile *file;
  GError *err = NULL;

  if ((file = gtk_file_dialog_open_finish (dialog, res, &err)) == NULL) {
    if (!g_error_matches (err, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED))
      tfe_error_alert (win, err);
    g_clear_error (&err);
    return;
  }
  tfe_text_view_set_file (TFE_TEXT_VIEW (tv), file);
  if (tfe_text_view_read (TFE_TEXT_VIEW (tv), &err) == FALSE)
    tfe_error_alert (win, err);
  g_object_unref (file);
}

void
open_cb (GtkWidget *btno) {
  GtkFileDialog *dialog;

  dialog = gtk_file_dialog_new ();
  gtk_file_dialog_open (dialog, GTK_WINDOW (win), NULL, open_dialog_cb, win);
  g_object_unref (dialog);
}

static void
save_dialog_cb (GObject *source_object, GAsyncResult *res, gpointer data) {
  GtkFileDialog *dialog = GTK_FILE_DIALOG (source_object);
  GtkWidget *tv = GTK_WIDGET (data);
  GFile *file;
  GError *err = NULL;

  if ((file = gtk_file_dialog_save_finish (dialog, res, &err)) == NULL) {
    if (!g_error_matches (err, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED)) {
      tfe_error_alert (GTK_WINDOW (win), err);
    }
    g_clear_error (&err);
    return;
  }

  tfe_text_view_set_file (TFE_TEXT_VIEW (tv), file);
  if (! tfe_text_view_write (TFE_TEXT_VIEW (tv), &err)) {
    tfe_error_alert (GTK_WINDOW (win), err);
    g_clear_error (&err);
  }
}

void
save_cb (GtkWidget *btns) {
  GtkFileDialog *dialog;
  GError *err = NULL;

  if (tfe_text_view_get_file (TFE_TEXT_VIEW (tv))) { /* File is already set */
    if (! tfe_text_view_write (TFE_TEXT_VIEW (tv), &err)) {
      tfe_error_alert (GTK_WINDOW (win), err);
      g_clear_error (&err);
    }
  } else {
    dialog = gtk_file_dialog_new ();
    gtk_file_dialog_save (dialog, GTK_WINDOW (win), NULL, save_dialog_cb, tv);
    g_object_unref (dialog);
  }
}

void
close_cb (GtkWidget *btnc) {
  gtk_window_destroy (GTK_WINDOW (win));
}

static void
show_filename (TfeTextView *tv) {
  GFile *file;
  char *filename_bin, *filename_utf;
  char *title;

  /* The instance (TfeTextView) holds the ownership of the returned GFile. */
  file = tfe_text_view_get_file (tv);
  if (G_IS_FILE (file)) {
    filename_bin = g_file_get_basename (file);
    filename_utf = g_filename_display_name (filename_bin);
    title = g_strdup_printf ("Turtle (%s)", filename_utf);
    g_free (filename_bin);
    g_free (filename_utf);
  } else
    title = g_strdup ("Turtle");
  gtk_window_set_title (GTK_WINDOW (win), title);
  g_free (title);
}

static void
resize_cb (GtkDrawingArea *drawing_area, int width, int height, gpointer user_data) {
  if (surface)
    cairo_surface_destroy (surface);
  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
  run_cb (NULL); // NULL is a fake (run button).
}

static void
draw_func (GtkDrawingArea *drawing_area, cairo_t *cr, int width, int height, gpointer user_data) {
  if (surface) {
    cairo_set_source_surface (cr, surface, 0, 0);
    cairo_paint (cr);
  }
}

static void
app_shutdown (GApplication *application) {
  if (surface) {
    cairo_surface_destroy (surface);
    surface = NULL;
  }
}

static void
app_activate (GApplication *application) {
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkBuilder *build;
  GdkDisplay *display;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/turtle/turtle.ui");
  win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  gtk_window_set_application (GTK_WINDOW (win), app);
  tv = GTK_WIDGET (gtk_builder_get_object (build, "tv"));
  da = GTK_WIDGET (gtk_builder_get_object (build, "da"));
  g_object_unref(build);
  g_signal_connect (da, "resize", G_CALLBACK (resize_cb), NULL);
  g_signal_connect (TFE_TEXT_VIEW (tv), "change-file", G_CALLBACK (show_filename), NULL);
  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (da), draw_func, NULL, NULL);

  display = gdk_display_get_default ();
  GtkCssProvider *provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_string (provider, 
      "textview {border-right: 1px solid gray; padding: 10px; font-family: monospace; font-size: 12pt;}");
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
  g_object_unref (provider);
}

#define APPLICATION_ID "com.github.ToshioCP.turtle"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_signal_connect (app, "shutdown", G_CALLBACK (app_shutdown), NULL);

  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
