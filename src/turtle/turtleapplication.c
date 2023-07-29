#include <gtk/gtk.h>
#include "../tfetextview/tfetextview.h"
#include "turtle_lex.h"
#include "turtle_parser.h"

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
  static gboolean busy = FALSE; /* initialized only once */
  cairo_t *cr;

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
  } else if (surface) {
    cr = cairo_create (surface);
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
    cairo_paint (cr);
    cairo_destroy (cr);
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
  gtk_window_destroy (GTK_WINDOW (win));
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
  g_signal_connect (GTK_DRAWING_AREA (da), "resize", G_CALLBACK (resize_cb), NULL);
  g_signal_connect (tv, "change-file", G_CALLBACK (show_filename), NULL);
  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (da), draw_func, NULL, NULL);

  display = gdk_display_get_default ();
  GtkCssProvider *provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (provider, 
      "textview {border-right: 1px solid gray; padding: 10px; font-family: monospace; font-size: 12pt;}", -1);
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

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
