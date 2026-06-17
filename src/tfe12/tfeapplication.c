#include <gtk/gtk.h>
#include "tfeapplication.h"
#include "tfewindow.h"
#include "tfepref.h"
#include "tfestylemanager.h"

struct _TfeApplication {
  GtkApplication parent;
  GSettings *settings;
};

G_DEFINE_FINAL_TYPE (TfeApplication, tfe_application, GTK_TYPE_APPLICATION)

static void
quit_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkApplication *app = GTK_APPLICATION (user_data);
  
  GList *windows = gtk_application_get_windows (app);
  /*
  * Closing a window removes it from the application's internal list.
  * To avoid mutating the list while iterating over it, we must use a copy.
  */
  GList *list_copy = g_list_copy (windows); 

  for (GList *l = list_copy; l != NULL; l = l->next) {
    gtk_window_close (GTK_WINDOW (l->data));
  }
  
  g_list_free (list_copy); 
}

static void
pref_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkApplication *app = GTK_APPLICATION (user_data);
  GtkWindow *win = gtk_application_get_active_window (app);
  GtkWidget *pref_dialog;

  pref_dialog = tfe_pref_new (app);
  gtk_window_set_transient_for (GTK_WINDOW (pref_dialog), win);
  gtk_window_present (GTK_WINDOW (pref_dialog));
}

static void
changed_font_cb (GSettings *settings, const char *prop_name, gpointer user_data) {
  const char *font_desc = g_settings_get_string (settings, "font-desc");
  PangoFontDescription *desc = pango_font_description_from_string (font_desc);

  tfe_style_manager_set_font (desc);
  pango_font_description_free (desc);
}

GSettings *
tfe_application_get_settings (TfeApplication *application) {
  g_return_val_if_fail (TFE_IS_APPLICATION (application), NULL);

  return application->settings;
}

/* helper function */

static GtkWidget *
get_main_window (GApplication *application) {
  GList *windows = gtk_application_get_windows (GTK_APPLICATION (application));
  GtkWidget *win;

  if (windows)
    win = GTK_WIDGET (windows->data);
  else
    win = tfe_window_new(GTK_APPLICATION (application));
  return win;
}

/* ----- activate, open, startup handlers ----- */

static void
app_activate (GApplication *application) {
  GtkWidget *win;

  win = get_main_window (application);
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
  GtkWidget *win;
  int i;

  win = get_main_window (application);
  for (i = 0; i < n_files; i++) {
    tfe_window_append_page (TFE_WINDOW (win), files[i]);
  }
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_shutdown (GApplication *application) {
  TfeApplication *app = TFE_APPLICATION (application);

  g_clear_object (&app->settings);
  tfe_style_manager_terminate ();

  G_APPLICATION_CLASS (tfe_application_parent_class)->shutdown (application);
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  TfeApplication *tfe_app = TFE_APPLICATION (application);
  int i;
  const char *base_css = "textview {padding: 10px;}";

  G_APPLICATION_CLASS (tfe_application_parent_class)->startup (application);

  tfe_style_manager_initialize (base_css);

  tfe_app->settings = g_settings_new ("com.github.ToshioCP.tfe");
  g_signal_connect (tfe_app->settings, "changed::font-desc", G_CALLBACK (changed_font_cb), tfe_app);
  changed_font_cb (tfe_app->settings, "font-desc", tfe_app); /* set the initial font */

  const GActionEntry app_entries[] = {
    { "pref", pref_activated, NULL, NULL, NULL },
    { "quit", quit_activated, NULL, NULL, NULL }
  };
  g_action_map_add_action_entries (G_ACTION_MAP (application), app_entries, G_N_ELEMENTS (app_entries), application);

  /* Set up accelerators */
  struct {
    const char *action;
    const char *accels[2];
  } action_accels[] = {
    { "win.open", { "<Control>o", NULL } },
    { "win.save", { "<Control>s", NULL } },
    { "win.close", { "<Control>w", NULL } },
    { "win.new", { "<Control>n", NULL } },
    { "win.saveas", { "<Shift><Control>s", NULL } },
    { "app.quit", { "<Control>q", NULL } },
  };

  for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), action_accels[i].action, action_accels[i].accels);
}

static void
tfe_application_init (TfeApplication *app) {
}

static void
tfe_application_class_init (TfeApplicationClass *class) {
  G_APPLICATION_CLASS (class)->startup = app_startup;
  G_APPLICATION_CLASS (class)->shutdown = app_shutdown;
  G_APPLICATION_CLASS (class)->activate = app_activate;
  G_APPLICATION_CLASS (class)->open = app_open;
}

TfeApplication *
tfe_application_new (const char *application_id, GApplicationFlags flag) {
  return TFE_APPLICATION (g_object_new (TFE_TYPE_APPLICATION, "application-id", application_id, "flags", flag, NULL));
}