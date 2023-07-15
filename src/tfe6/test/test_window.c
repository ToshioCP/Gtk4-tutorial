#include <gtk/gtk.h>
#include "../tfewindow.h"

static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  TfeWindow *win = TFE_WINDOW (gtk_application_get_active_window (app));

  tfe_window_notebook_page_new (win);
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_startup (GApplication *application) {
  tfe_window_new (GTK_APPLICATION (application));
}

#define APPLICATION_ID "com.github.ToshioCP.test_window"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_HANDLES_OPEN);
  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

