#include <gtk/gtk.h>
#include "tfeapplication.h"
#include "tfewindow.h"
#include "pfd2css.h"

struct _TfeApplication {
  GtkApplication parent;
  TfeWindow *win;
  GSettings *settings;
  GtkCssProvider *provider;
};

G_DEFINE_TYPE (TfeApplication, tfe_application, GTK_TYPE_APPLICATION);

/* gsettings changed::font signal handler */
static void
changed_font_cb (GSettings *settings, char *key, gpointer user_data) {
  TfeApplication *app = TFE_APPLICATION (user_data);
  char *font, *s, *css;
  PangoFontDescription *pango_font_desc;

  font = g_settings_get_string (app->settings, "font");
  pango_font_desc = pango_font_description_from_string (font);
  g_free (font);
  s = pfd2css (pango_font_desc); // converts Pango Font Description into CSS style string
  css = g_strdup_printf ("textview {%s}", s);
  gtk_css_provider_load_from_data (app->provider, css, -1);
  g_free (s);
  g_free (css);
}

/* ----- activate, open, startup handlers ----- */
static void
app_activate (GApplication *application) {
  TfeApplication *app = TFE_APPLICATION (application);

  tfe_window_notebook_page_new (app->win);
  gtk_window_present (GTK_WINDOW (app->win));
}

static void
app_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
  TfeApplication *app = TFE_APPLICATION (application);

  tfe_window_notebook_page_new_with_files (app->win, files, n_files);
  gtk_window_present (GTK_WINDOW (app->win));
}

static void
app_startup (GApplication *application) {
  TfeApplication *app = TFE_APPLICATION (application);
  GtkCssProvider *provider0;
  GdkDisplay *display;
  int i;

  app->win = TFE_WINDOW (tfe_window_new (GTK_APPLICATION (app)));
  provider0 = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (provider0, "textview {padding: 10px;}", -1);
  display = gdk_display_get_default ();
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider0),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_object_unref (provider0);
  app->provider = gtk_css_provider_new ();
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (app->provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  app->settings = g_settings_new ("com.github.ToshioCP.tfe");
  g_signal_connect (app->settings, "changed::font", G_CALLBACK (changed_font_cb), app);
  changed_font_cb(app->settings, "font", app);

/* ----- accelerator ----- */ 
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
}

static void
tfe_application_dispose (GObject *gobject) {
  TfeApplication *app = TFE_APPLICATION (gobject);

  g_clear_object (&app->settings);
  g_clear_object (&app->provider);
  G_OBJECT_CLASS (tfe_application_parent_class)->dispose (gobject);
}

static void
tfe_application_init (TfeApplication *app) {
  g_signal_connect (G_APPLICATION (app), "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (G_APPLICATION (app), "activate", G_CALLBACK (app_activate), NULL);
  g_signal_connect (G_APPLICATION (app), "open", G_CALLBACK (app_open), NULL);
}

static void
tfe_application_class_init (TfeApplicationClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = tfe_application_dispose;
}

TfeApplication *
tfe_application_new (const char* application_id, GApplicationFlags flags) {
  return TFE_APPLICATION (g_object_new (TFE_TYPE_APPLICATION, "application-id", application_id, "flags", flags, NULL));
}
