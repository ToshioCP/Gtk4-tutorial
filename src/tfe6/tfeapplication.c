#include <gtk/gtk.h>
#include "../tfetextview/tfetextview.h"
#include "tfenotebook.h"
#include "pfd2css.h"

static GtkDialog *pref;
static GtkFontButton *fontbtn;
static GSettings *settings;
static GtkDialog *alert;
static GtkLabel *lb_alert;
static GtkButton *btn_accept;
static GtkCssProvider *provider0;
static GtkCssProvider *provider;

static gulong pref_close_request_handler_id = 0;
static gulong alert_close_request_handler_id = 0;
static gboolean is_quit;

/* ----- handler for close-request on the main window ----- */
static gboolean
win_close_request_cb (GtkWindow *win, GtkNotebook *nb) {
  is_quit = true;
  if (has_saved_all (nb))
    return false;
  else {
    gtk_label_set_text (lb_alert, "Contents aren't saved yet.\nAre you sure to quit?");
    gtk_button_set_label (btn_accept, "Quit");
    gtk_window_present (GTK_WINDOW (alert));
    return true;
  }
}

/* ----- button handlers ----- */
void
open_cb (GtkNotebook *nb) {
  notebook_page_open (nb);
}

void
save_cb (GtkNotebook *nb) {
  notebook_page_save (nb);
}

void
close_cb (GtkNotebook *nb) {
  is_quit = false;
  if (has_saved (GTK_NOTEBOOK (nb)))
    notebook_page_close (GTK_NOTEBOOK (nb));
  else {
    gtk_label_set_text (lb_alert, "Contents aren't saved yet.\nAre you sure to close?");
    gtk_button_set_label (btn_accept, "Close");
    gtk_window_present (GTK_WINDOW (alert));
  }
}

/* ----- action handlers (menu or accelerator) ----- */
static void
open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkNotebook *nb = GTK_NOTEBOOK (user_data);
  open_cb (nb);
}

static void
save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkNotebook *nb = GTK_NOTEBOOK (user_data);
  save_cb (nb);
}

static void
close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkNotebook *nb = GTK_NOTEBOOK (user_data);
  close_cb (nb);
}

static void
new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkNotebook *nb = GTK_NOTEBOOK (user_data);
  notebook_page_new (nb);
}

static void
saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkNotebook *nb = GTK_NOTEBOOK (user_data);
  notebook_page_saveas (nb);
}

static void
pref_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  gtk_window_present (GTK_WINDOW (pref));
}

static void
close_all_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkNotebook *nb = GTK_NOTEBOOK (user_data);
  GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);

  if (! win_close_request_cb (GTK_WINDOW (win), nb)) // checks whether contents are saved
    gtk_window_destroy (GTK_WINDOW (win));
}

/* ----- handler for close-request on pref or alert dialogs ----- */
static gboolean
dialog_close_cb (GtkDialog *dialog) {
  gtk_widget_set_visible (GTK_WIDGET (dialog), false);
  return TRUE; // not close
}

/* ----- handler for response signal on alert dialog ----- */
/* accept => close, cancel =>  do nothing */
void
alert_response_cb (GtkDialog *alert, int response_id, gpointer user_data) {
  GtkNotebook *nb = GTK_NOTEBOOK (user_data);
  GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);

  gtk_widget_set_visible (GTK_WIDGET (alert), false);
  if (response_id == GTK_RESPONSE_ACCEPT) {
    if (is_quit)
      gtk_window_destroy (GTK_WINDOW (win));
    else
      notebook_page_close (nb);
  }
}

/* ----- handler for font-set signal on fontbutton ----- */
/* update data in the provider */
static void
font_set_cb (GtkFontButton *fontbtn) {
  PangoFontDescription *pango_font_desc;
  char *s, *css;

  pango_font_desc = gtk_font_chooser_get_font_desc (GTK_FONT_CHOOSER (fontbtn));
  s = pfd2css (pango_font_desc); // converts Pango Font Description into CSS style string
  css = g_strdup_printf ("textview {%s}", s);
  gtk_css_provider_load_from_data (provider, css, -1);
  g_free (s);
  g_free (css);
}

/* ----- shutdown handler on the application ----- */
void
app_shutdown (GApplication *application) {
  GdkDisplay *display = gdk_display_get_default();

  gtk_style_context_remove_provider_for_display (display, GTK_STYLE_PROVIDER (provider0));
  gtk_style_context_remove_provider_for_display (display, GTK_STYLE_PROVIDER (provider));
  g_object_unref (provider0);
  g_object_unref (provider);

  g_clear_object (&settings);
  if (pref_close_request_handler_id > 0)
    g_signal_handler_disconnect (pref, pref_close_request_handler_id);
  gtk_window_destroy (GTK_WINDOW (pref));

  if (alert_close_request_handler_id > 0)
    g_signal_handler_disconnect (alert, alert_close_request_handler_id);
  gtk_window_destroy (GTK_WINDOW (alert));
}

/* ----- activate, open, startup handlers on the application ----- */
static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
  GtkWidget *boxv = gtk_window_get_child (GTK_WINDOW (win));
  GtkNotebook *nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));

  notebook_page_new (nb);
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
  GtkWidget *boxv = gtk_window_get_child (GTK_WINDOW (win));
  GtkNotebook *nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));
  int i;

  for (i = 0; i < n_files; i++)
    notebook_page_new_with_file (nb, files[i]);
  if (gtk_notebook_get_n_pages (nb) == 0)
    notebook_page_new (nb);
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkBuilder *build;
  GtkApplicationWindow *win;
  GtkNotebook *nb;
  GtkMenuButton *btnm;
  GMenuModel *menu;
  GdkDisplay *display;
  int i;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/tfe.ui");
  win = GTK_APPLICATION_WINDOW (gtk_builder_get_object (build, "win"));
  gtk_window_set_application (GTK_WINDOW (win), app);
  nb = GTK_NOTEBOOK (gtk_builder_get_object (build, "nb"));
  g_signal_connect (GTK_WINDOW (win), "close-request", G_CALLBACK (win_close_request_cb), nb);

  btnm = GTK_MENU_BUTTON (gtk_builder_get_object (build, "btnm"));

  pref = GTK_DIALOG (gtk_builder_get_object (build, "pref"));
  pref_close_request_handler_id = g_signal_connect (GTK_DIALOG (pref), "close-request", G_CALLBACK (dialog_close_cb), NULL);
  fontbtn = GTK_FONT_BUTTON (gtk_builder_get_object (build, "fontbtn"));
  g_signal_connect (fontbtn, "font-set", G_CALLBACK (font_set_cb), NULL);
  settings = g_settings_new ("com.github.ToshioCP.tfe");
  g_settings_bind (settings, "font", fontbtn, "font", G_SETTINGS_BIND_DEFAULT);

  alert = GTK_DIALOG (gtk_builder_get_object (build, "alert"));
  alert_close_request_handler_id = g_signal_connect (GTK_DIALOG (alert), "close-request", G_CALLBACK (dialog_close_cb), NULL);
  lb_alert = GTK_LABEL (gtk_builder_get_object (build, "lb_alert"));
  btn_accept = GTK_BUTTON (gtk_builder_get_object (build, "btn_accept"));

  g_object_unref(build);

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/menu.ui");
  menu = G_MENU_MODEL (gtk_builder_get_object (build, "menu"));
  gtk_menu_button_set_menu_model (btnm, menu);
  g_object_unref(build);

  const GActionEntry win_entries[] = {
    { "open", open_activated, NULL, NULL, NULL },
    { "save", save_activated, NULL, NULL, NULL },
    { "close", close_activated, NULL, NULL, NULL },
    { "new", new_activated, NULL, NULL, NULL },
    { "saveas", saveas_activated, NULL, NULL, NULL },
    { "pref", pref_activated, NULL, NULL, NULL },
    { "close-all", close_all_activated, NULL, NULL, NULL }
  };
  g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), nb);

  struct {
    const char *action;
    const char *accels[2];
  } action_accels[] = {
    { "win.open", { "<Control>o", NULL } },
    { "win.save", { "<Control>s", NULL } },
    { "win.close", { "<Control>w", NULL } },
    { "win.new", { "<Control>n", NULL } },
    { "win.saveas", { "<Shift><Control>s", NULL } },
    { "win.close-all", { "<Control>q", NULL } },
  };

  for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), action_accels[i].action, action_accels[i].accels);

  provider0 = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (provider0, "textview {padding: 10px;}", -1);
  display = gdk_display_get_default ();
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider0),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  provider = gtk_css_provider_new ();
  font_set_cb (fontbtn); // applies the g_setting font to CSS
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  g_signal_connect (application, "shutdown", G_CALLBACK (app_shutdown), NULL);
}

#define APPLICATION_ID "com.github.ToshioCP.tfe"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_HANDLES_OPEN);

  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

