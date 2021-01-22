#include <gtk/gtk.h>

static void
draw_function (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
  cairo_set_source_rgb (cr, 1.0, 1.0, 1.0); /* whilte */
  cairo_paint (cr);
  cairo_set_line_width (cr, 2.0);
  cairo_set_source_rgb (cr, 0.0, 0.0, 0.0); /* black */
  cairo_rectangle (cr, width/2.0 - 20.0, height/2.0 - 20.0, 40.0, 40.0);
  cairo_stroke (cr);
}

static void
on_activate (GApplication *app, gpointer user_data) {
  GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));
  GtkWidget *area = gtk_drawing_area_new ();

  gtk_window_set_title (GTK_WINDOW (win), "da1");
  /* Set initial size of width and height */
  gtk_drawing_area_set_content_width (GTK_DRAWING_AREA (area), 100);
  gtk_drawing_area_set_content_height (GTK_DRAWING_AREA (area), 100);
  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (area), draw_function, NULL, NULL);
  gtk_window_set_child (GTK_WINDOW (win), area);

  gtk_widget_show (win);
}

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new ("com.github.ToshioCP.da1", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

