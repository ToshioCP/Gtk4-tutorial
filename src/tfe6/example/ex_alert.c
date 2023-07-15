#include <gtk/gtk.h>
#include "../tfealert.h"

static void
alert_response_cb (TfeAlert *alert, int response, gpointer user_data) {
  if (response == TFE_ALERT_RESPONSE_ACCEPT)
    g_print ("%s\n", tfe_alert_get_button_label (alert));
  else if (response == TFE_ALERT_RESPONSE_CANCEL)
    g_print ("Cancel\n");
  else
    g_print ("Unexpected error\n");
}

static void
app_activate (GApplication *application) {
  GtkWidget *alert;
  char *title, *message, *btn_label;

  title = "Example for TfeAlert"; message = "Click on Cancel or Accept button"; btn_label = "Accept";
  alert = tfe_alert_new_with_data (title, message, btn_label);
  g_signal_connect (TFE_ALERT (alert), "response", G_CALLBACK (alert_response_cb), NULL);
  gtk_window_set_application (GTK_WINDOW (alert), GTK_APPLICATION (application));
  gtk_window_present (GTK_WINDOW (alert));
}

static void
app_startup (GApplication *application) {
}

#define APPLICATION_ID "com.github.ToshioCP.example_tfe_alert"

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
