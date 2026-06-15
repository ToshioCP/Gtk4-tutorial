#include <gtk/gtk.h>
#include "tfeapplication.h"
#include "tfewindow.h"
#include "tfestylemanager.h"

struct _TfeApplication {
  GtkApplication parent;
  GSettings *settings;
};

G_DEFINE_FINAL_TYPE (TfeApplication, tfe_application, GTK_TYPE_APPLICATION)


static void
quit_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkApplication *app = GTK_APPLICATION (user_data);
  GList *windows;
  
  for (windows= gtk_application_get_windows (app); windows; windows = windows->next)
    gtk_window_close (GTK_WINDOW (windows->data));
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
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  int i;
  const char *base_css = "textview {padding: 10px;}";

  G_APPLICATION_CLASS (tfe_application_parent_class)->startup (application);

  tfe_style_manager_initialize (base_css);

  GSimpleAction *quit_action = g_simple_action_new ("quit", NULL);

  g_signal_connect (quit_action, "activate", G_CALLBACK (quit_activated), application);
  g_action_map_add_action (G_ACTION_MAP (application), G_ACTION (quit_action));
  g_object_unref (quit_action);

  /* In the future version, settings will be initialized here */

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
tfe_application_dispose (GObject *object) {
  /* In the future version, settings will be disposed here */

  tfe_style_manager_terminate ();

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