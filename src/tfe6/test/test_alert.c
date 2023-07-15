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

  title = "Test title"; message = "Test message"; btn_label = "Test label";
  alert = tfe_alert_new_with_data (title, message, btn_label);

  if (strcmp(title, tfe_alert_get_title (TFE_ALERT (alert))) != 0) {
    g_print ("Title not expected.\n");
    g_print ("Expected: %s\n", title);
    g_print ("Actual: %s\n", tfe_alert_get_title (TFE_ALERT (alert)));
  }
  if (strcmp(message, tfe_alert_get_message (TFE_ALERT (alert))) != 0) {
    g_print ("Message not expected.\n");
    g_print ("Expected: %s\n", message);
    g_print ("Actual: %s\n", tfe_alert_get_message (TFE_ALERT (alert)));
  }
  if (strcmp(btn_label, tfe_alert_get_button_label (TFE_ALERT (alert))) != 0) {
    g_print ("Button label not expected.\n");
    g_print ("Expected: %s\n", btn_label);
    g_print ("Actual: %s\n", tfe_alert_get_button_label (TFE_ALERT (alert)));
  }

  title = "Are you sure?"; message = "Contents aren't saved yet.\nAre you sure to close?"; btn_label = "Close";
  tfe_alert_set_title (TFE_ALERT (alert), title);
  tfe_alert_set_message (TFE_ALERT (alert), message);
  tfe_alert_set_button_label (TFE_ALERT (alert), btn_label);

  if (strcmp(title, tfe_alert_get_title (TFE_ALERT (alert))) != 0) {
    g_print ("Title not expected.\n");
    g_print ("Expected: %s\n", title);
    g_print ("Actual: %s\n", tfe_alert_get_title (TFE_ALERT (alert)));
  }
  if (strcmp(message, tfe_alert_get_message (TFE_ALERT (alert))) != 0) {
    g_print ("Message not expected.\n");
    g_print ("Expected: %s\n", message);
    g_print ("Actual: %s\n", tfe_alert_get_message (TFE_ALERT (alert)));
  }
  if (strcmp(btn_label, tfe_alert_get_button_label (TFE_ALERT (alert))) != 0) {
    g_print ("Button label not expected.\n");
    g_print ("Expected: %s\n", btn_label);
    g_print ("Actual: %s\n", tfe_alert_get_button_label (TFE_ALERT (alert)));
  }

  g_print ("Click a button on the header bar.\n");
  g_print ("Then the name of the button will be printed. Check it.\n\n");
  g_signal_connect (TFE_ALERT (alert), "response", G_CALLBACK (alert_response_cb), NULL);
  gtk_window_set_application (GTK_WINDOW (alert), GTK_APPLICATION (application));
  gtk_window_present (GTK_WINDOW (alert));
}

static void
app_startup (GApplication *application) {
}

#define APPLICATION_ID "com.github.ToshioCP.test_tfe_alert"

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
