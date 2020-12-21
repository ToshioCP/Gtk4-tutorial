#include <gtk/gtk.h>

static void
on_activate (GApplication *app, gpointer user_data) {
  GtkWidget *win;
  GtkWidget *lab;

  win = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_title (GTK_WINDOW (win), "lb4");
  gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);

  lab = gtk_label_new ("Hello.");
  gtk_window_set_child (GTK_WINDOW (win), lab);

  gtk_widget_show (win);
}

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new ("com.github.ToshioCP.lb1", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

