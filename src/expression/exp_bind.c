#include <gtk/gtk.h>

GtkExpressionWatch *watch;

static int
f2i (GObject *object, double d) {
  return (int) d;
}

static int
close_request_cb (GtkWindow *win) {
  gtk_expression_watch_unwatch (watch);
  return false;
}

static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  gtk_window_present (gtk_application_get_active_window(app));
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkBuilder *build;
  GtkWidget *win, *label;
  GtkAdjustment *adjustment;
  GtkExpression *expression, *params[1];

  /* Builds a window with exp.ui resource */
  build = gtk_builder_new_from_file ("exp_bind.ui");
  win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  label = GTK_WIDGET (gtk_builder_get_object (build, "label"));
  // scale = GTK_WIDGET (gtk_builder_get_object (build, "scale"));
  adjustment = GTK_ADJUSTMENT (gtk_builder_get_object (build, "adjustment"));
  gtk_window_set_application (GTK_WINDOW (win), app);
  g_signal_connect (win, "close-request", G_CALLBACK (close_request_cb), NULL);
  g_object_unref (build);

  /* GtkExpressionWatch */
  params[0] = gtk_property_expression_new (GTK_TYPE_ADJUSTMENT, NULL, "value");
  expression = gtk_cclosure_expression_new (G_TYPE_INT, NULL, 1, params, G_CALLBACK (f2i), NULL, NULL);
  watch = gtk_expression_bind (expression, label, "label", adjustment); /* watch takes the ownership of the expression. */
}

#define APPLICATION_ID "com.github.ToshioCP.exp_watch"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
