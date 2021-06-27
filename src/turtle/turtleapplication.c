#include "turtle.h"

static GtkWidget *win;
static GtkWidget *tv;
static GtkWidget *da;

cairo_surface_t *surface = NULL;

void
run_cb (GtkWidget *btnr) {
  GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  GtkTextIter start_iter;
  GtkTextIter end_iter;
  char *contents;
  int stat;
  static gboolean busy = FALSE;

  /* yyparse() and run() are NOT thread safe. */
  /* The variable busy avoids reentry. */
  if (busy)
    return;
  busy = TRUE;
  gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
  contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
  if (surface) {
    init_flex (contents);
    stat = yyparse ();
    if (stat == 0) /* No error */ {
      run ();
    }
    finalize_flex ();
  }
  g_free (contents);
  gtk_widget_queue_draw (GTK_WIDGET (da));
  busy = FALSE;
}

void
open_cb (GtkWidget *btno) {
  tfe_text_view_open (TFE_TEXT_VIEW (tv), GTK_WINDOW (win));
}

void
save_cb (GtkWidget *btns) {
  tfe_text_view_save (TFE_TEXT_VIEW (tv));
}

void
close_cb (GtkWidget *btnc) {
  if (surface)
    cairo_surface_destroy (surface);
  gtk_window_destroy (GTK_WINDOW (win));
}

static gboolean
close_request_cb (GtkWindow *win, gpointer user_data) {
  if (surface)
    cairo_surface_destroy (surface);
  return FALSE;
}

static void
show_filename (TfeTextView *tv) {
  GFile *file;
  char *filename;
  char *title;

  file = tfe_text_view_get_file (tv);
  if (G_IS_FILE (file)) {
    filename = g_file_get_basename (file);
    title = g_strdup_printf ("Turtle (%s)", filename);
    g_free (filename);
    g_object_unref (file);
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
}

static void
draw_func (GtkDrawingArea *drawing_area, cairo_t *cr, int width, int height, gpointer user_data) {
  if (surface) {
    cairo_set_source_surface (cr, surface, 0, 0);
    cairo_paint (cr);
  }
}

static void
app_activate (GApplication *application) {
  gtk_widget_show (win);
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkBuilder *build;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/turtle/turtle.ui");
  win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  gtk_window_set_application (GTK_WINDOW (win), app);
  tv = GTK_WIDGET (gtk_builder_get_object (build, "tv"));
  da = GTK_WIDGET (gtk_builder_get_object (build, "da"));
  g_object_unref(build);
  g_signal_connect (win, "close-request", G_CALLBACK (close_request_cb), NULL);
  g_signal_connect (GTK_DRAWING_AREA (da), "resize", G_CALLBACK (resize_cb), NULL);
  g_signal_connect (tv, "change-file", G_CALLBACK (show_filename), NULL);
  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (da), draw_func, NULL, NULL);

GdkDisplay *display;

  display = gtk_widget_get_display (GTK_WIDGET (win));
  GtkCssProvider *provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (provider, "textview {padding: 10px; font-family: monospace; font-size: 12pt;}", -1);
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

#define APPLICATION_ID "com.github.ToshioCP.turtle"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_FLAGS_NONE);

  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

