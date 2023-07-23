#include <gtk/gtk.h>

static cairo_surface_t *surface = NULL;
static cairo_surface_t *surface_save = NULL;
static double start_x;
static double start_y;

static void
copy_surface (cairo_surface_t *src, cairo_surface_t *dst) {
  if (!src || !dst)
    return;
  cairo_t *cr = cairo_create (dst);
  cairo_set_source_surface (cr, src, 0.0, 0.0);
  cairo_paint (cr);
  cairo_destroy (cr);
}

/* This callback is called when the GtkDrawingArea is shown. */
/* It is the only call because the window is not resizable. */
static void
resize_cb (GtkWidget *widget, int width, int height, gpointer user_data) {
  cairo_t *cr;

  if (surface)
    cairo_surface_destroy (surface);
  surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, width, height);
  if (surface_save)
    cairo_surface_destroy (surface_save);
  surface_save = cairo_image_surface_create (CAIRO_FORMAT_RGB24, width, height);
  /* Paint the surface white. It is the background color. */
  cr = cairo_create (surface);
  cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
  cairo_paint (cr);
  cairo_destroy (cr);
}

static void
draw_cb (GtkDrawingArea *da, cairo_t *cr, int width, int height, gpointer user_data) {
  if (surface) {
    cairo_set_source_surface (cr, surface, 0.0, 0.0);
    cairo_paint (cr);
  }
}

static void
drag_begin (GtkGestureDrag *gesture, double x, double y, gpointer user_data) {
  // save the surface and record (x, y)
  copy_surface (surface, surface_save);
  start_x = x;
  start_y = y;
}

static void
drag_update  (GtkGestureDrag *gesture, double offset_x, double offset_y, gpointer user_data) {
  GtkWidget *da = GTK_WIDGET (user_data);
  cairo_t *cr;
  
  copy_surface (surface_save, surface);
  cr = cairo_create (surface);
  cairo_rectangle (cr, start_x, start_y, offset_x, offset_y);
  cairo_set_line_width (cr, 1.0);
  cairo_stroke (cr);
  cairo_destroy (cr);
  gtk_widget_queue_draw (da);
}

static void
drag_end  (GtkGestureDrag *gesture, double offset_x, double offset_y, gpointer user_data) {
  GtkWidget *da = GTK_WIDGET (user_data);
  cairo_t *cr;
  
  copy_surface (surface_save, surface);
  cr = cairo_create (surface);
  cairo_rectangle (cr, start_x, start_y, offset_x, offset_y);
  cairo_set_line_width (cr, 6.0);
  cairo_stroke (cr);
  cairo_destroy (cr);
  gtk_widget_queue_draw (da);
}

static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWindow *win;

  win = gtk_application_get_active_window (app);
  gtk_window_present (win);
}

static void
app_shutdown (GApplication *application) {
  if (surface)
    cairo_surface_destroy (surface);
  if (surface_save)
    cairo_surface_destroy (surface_save);
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkBuilder *build;
  GtkWindow *win;
  GtkDrawingArea *da;
  GtkGesture *drag;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/rect/rect.ui");
  win = GTK_WINDOW (gtk_builder_get_object (build, "win"));
  da = GTK_DRAWING_AREA (gtk_builder_get_object (build, "da"));
  gtk_window_set_application (win, app);
  g_object_unref (build);

  gtk_drawing_area_set_draw_func (da, draw_cb, NULL, NULL);
  g_signal_connect_after (da, "resize", G_CALLBACK (resize_cb), NULL);

  drag = gtk_gesture_drag_new ();
  gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (drag), GDK_BUTTON_PRIMARY);
  gtk_widget_add_controller (GTK_WIDGET (da), GTK_EVENT_CONTROLLER (drag));
  g_signal_connect (drag, "drag-begin", G_CALLBACK (drag_begin), NULL);
  g_signal_connect (drag, "drag-update", G_CALLBACK (drag_update), da);
  g_signal_connect (drag, "drag-end", G_CALLBACK (drag_end), da);
}

#define APPLICATION_ID "com.github.ToshioCP.rect"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_HANDLES_OPEN);
  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_signal_connect (app, "shutdown", G_CALLBACK (app_shutdown), NULL);
  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
