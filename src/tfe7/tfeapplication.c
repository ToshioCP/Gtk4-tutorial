#include <gtk/gtk.h>
#include "tfeapplication.h"
#include "tfewindow.h"

struct _TfeApplication {
  GtkApplication parent;
  GSettings *settings;
};

G_DEFINE_FINAL_TYPE (TfeApplication, tfe_application, GTK_TYPE_APPLICATION)


/* ----- activate, open, startup handlers ----- */
static void
quit_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GApplication *application = G_APPLICATION (user_data);

  g_application_quit (application);
}

static void
pref_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  /* not implemented yet */
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
app_startup (GApplication *application) {
  G_APPLICATION_CLASS (tfe_application_parent_class)->startup (application);

  const GActionEntry app_entries[] = {
    { "pref", pref_activated, NULL, NULL, NULL },
    { "quit", quit_activated, NULL, NULL, NULL }
  };
  g_action_map_add_action_entries (G_ACTION_MAP (application), app_entries, G_N_ELEMENTS (app_entries), application);

  /* In the future version, settings will be initialized here */
}


static void
tfe_application_dispose (GObject *object) {
  /* In the future version, settings will be disposed here */

  G_OBJECT_CLASS (tfe_application_parent_class)->dispose (object);
}

static void
tfe_application_init (TfeApplication *app) {
}

static void
tfe_application_class_init (TfeApplicationClass *class) {
  G_OBJECT_CLASS (class)->dispose = tfe_application_dispose;
  G_APPLICATION_CLASS (class)->startup = app_startup;
  G_APPLICATION_CLASS (class)->activate = app_activate;
  G_APPLICATION_CLASS (class)->open = app_open;
}

TfeApplication *
tfe_application_new (const char *application_id, GApplicationFlags flag) {
  return TFE_APPLICATION (g_object_new (TFE_TYPE_APPLICATION, "application-id", application_id, "flags", flag, NULL));
}