#include <gtk/gtk.h>
#include "../tfepref.h"

GSettings *settings;

// "changed::font-desc" signal handler
static void
changed_font_desc_cb (GSettings *settings, char *key, gpointer user_data) {
  char *s;
  s = g_settings_get_string (settings, key);
  g_print ("%s\n", s);
  g_free (s);
}

static void
app_shutdown (GApplication *application) {
  g_object_unref (settings);
}

static void
app_activate (GApplication *application) {
  GtkWidget *pref = tfe_pref_new ();

  gtk_window_set_application (GTK_WINDOW (pref), GTK_APPLICATION (application));
  gtk_window_present (GTK_WINDOW (pref));
}

static void
app_startup (GApplication *application) {
  settings = g_settings_new ("com.github.ToshioCP.tfe");
  g_signal_connect (settings, "changed::font-desc", G_CALLBACK (changed_font_desc_cb), NULL);
  g_print ("%s\n", "Change the font with the font button. Then the new font will be printed out.\n");
}

#define APPLICATION_ID "com.github.ToshioCP.test_tfe_pref"

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
