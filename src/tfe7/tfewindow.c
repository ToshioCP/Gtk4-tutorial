#include "tfewindow.h"
#include "tfenotebook.h"
#include "tfepref.h"
#include "tfealert.h"
#include "css.h"

struct _TfeWindow {
  GtkApplicationWindow parent;
  GtkMenuButton *btnm;
  GtkNotebook *nb;
  GSettings *settings;
  gboolean is_quit;
};

G_DEFINE_TYPE (TfeWindow, tfe_window, GTK_TYPE_APPLICATION_WINDOW);

/* alert response signal handler */
static void
alert_response_cb (GtkDialog *alert, int response_id, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  if (response_id == GTK_RESPONSE_ACCEPT) {
    if (win->is_quit)
      gtk_window_destroy(GTK_WINDOW (win));
    else
      notebook_page_close (win->nb);
  }
  gtk_window_destroy (GTK_WINDOW (alert));
}

/* ----- action activated handlers ----- */
static void
open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  notebook_page_open (GTK_NOTEBOOK (win->nb));
}

static void
save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  notebook_page_save (GTK_NOTEBOOK (win->nb));
}

static void
close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  TfeAlert *alert;

  if (has_saved (win->nb))
    notebook_page_close (win->nb);
  else {
    win->is_quit = false;
    alert = TFE_ALERT (tfe_alert_new (GTK_WINDOW (win)));
    tfe_alert_set_message (alert, "Contents aren't saved yet.\nAre you sure to close?");
    tfe_alert_set_button_label (alert, "Close");
    g_signal_connect (GTK_DIALOG (alert), "response", G_CALLBACK (alert_response_cb), win);
    gtk_widget_show (GTK_WIDGET (alert));
  }
}

static void
new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  notebook_page_new (GTK_NOTEBOOK (win->nb));
}

static void
saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  notebook_page_saveas (GTK_NOTEBOOK (win->nb));
}

static void
pref_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  GtkWidget *pref;

  pref = tfe_pref_new (GTK_WINDOW (win));
  gtk_widget_show (pref);
}

static void
quit_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  TfeAlert *alert;

  if (has_saved_all (GTK_NOTEBOOK (win->nb)))
    gtk_window_destroy (GTK_WINDOW (win));
  else {
    win->is_quit = true;
    alert = TFE_ALERT (tfe_alert_new (GTK_WINDOW (win)));
    tfe_alert_set_message (alert, "Contents aren't saved yet.\nAre you sure to quit?");
    tfe_alert_set_button_label (alert, "Quit");
    g_signal_connect (GTK_DIALOG (alert), "response", G_CALLBACK (alert_response_cb), win);
    gtk_widget_show (GTK_WIDGET (alert));
  }
}

/* gsettings changed::font signal handler */
static void
changed_font_cb (GSettings *settings, char *key, gpointer user_data) {
  GtkWindow *win = GTK_WINDOW (user_data); 
  const char *font;
  PangoFontDescription *pango_font_desc;

  font = g_settings_get_string (settings, "font");
  pango_font_desc = pango_font_description_from_string (font);
  set_font_for_display_with_pango_font_desc (win, pango_font_desc);
}

/* --- public functions --- */

void
tfe_window_notebook_page_new (TfeWindow *win) {
  notebook_page_new (win->nb);
}

void
tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files) {
  int i;

  for (i = 0; i < n_files; i++)
    notebook_page_new_with_file (win->nb, files[i]);
  if (gtk_notebook_get_n_pages (win->nb) == 0)
    notebook_page_new (win->nb);
}

/* --- TfeWindow object construction/destruction --- */ 
static void
tfe_window_dispose (GObject *gobject) {
  TfeWindow *window = TFE_WINDOW (gobject);

  g_clear_object (&window->settings);
  G_OBJECT_CLASS (tfe_window_parent_class)->dispose (gobject);
}

static void
tfe_window_init (TfeWindow *win) {
  GtkBuilder *build;
  GMenuModel *menu;

  gtk_widget_init_template (GTK_WIDGET (win));

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/menu.ui");
  menu = G_MENU_MODEL (gtk_builder_get_object (build, "menu"));
  gtk_menu_button_set_menu_model (win->btnm, menu);
  g_object_unref(build);

  win->settings = g_settings_new ("com.github.ToshioCP.tfe");
  g_signal_connect (win->settings, "changed::font", G_CALLBACK (changed_font_cb), win);

/* ----- action ----- */
  const GActionEntry win_entries[] = {
    { "open", open_activated, NULL, NULL, NULL },
    { "save", save_activated, NULL, NULL, NULL },
    { "close", close_activated, NULL, NULL, NULL },
    { "new", new_activated, NULL, NULL, NULL },
    { "saveas", saveas_activated, NULL, NULL, NULL },
    { "pref", pref_activated, NULL, NULL, NULL },
    { "close-all", quit_activated, NULL, NULL, NULL }
  };
  g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);

  changed_font_cb(win->settings, "font", win);
}

static void
tfe_window_class_init (TfeWindowClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = tfe_window_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfewindow.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, btnm);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, nb);
}

GtkWidget *
tfe_window_new (GtkApplication *app) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_WINDOW, "application", app, NULL));
}

