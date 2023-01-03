#include <gtk/gtk.h>

/* The provider below provides application wide CSS data. */
static GtkCssProvider *provider;

static void
fullscreen_changed(GSimpleAction *action, GVariant *value, gpointer user_data) {
  GtkWindow *win = GTK_WINDOW (user_data);

  if (g_variant_get_boolean (value))
    gtk_window_maximize (win);
  else
    gtk_window_unmaximize (win);
  g_simple_action_set_state (action, value);
}

static void
color_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  char *color = g_strdup_printf ("label.lb {background-color: %s;}", g_variant_get_string (parameter, NULL));
  /* Change the CSS data in the provider. */
  /* Previous data is thrown away. */
  gtk_css_provider_load_from_data (provider, color, -1);
  g_free (color);
  g_action_change_state (G_ACTION (action), parameter);
}

static void
remove_provider (GApplication *app, GtkCssProvider *provider) {
  GdkDisplay *display = gdk_display_get_default();

  gtk_style_context_remove_provider_for_display (display, GTK_STYLE_PROVIDER (provider));
  g_object_unref (provider);
}

static void
quit_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GApplication *app = G_APPLICATION (user_data);

  g_application_quit (app);
}

static void
app_activate (GApplication *app) {
  GtkWindow *win = GTK_WINDOW (gtk_application_window_new (GTK_APPLICATION (app)));
  gtk_window_set_title (win, "menu2_ui");
  gtk_window_set_default_size (win, 400, 300);

  GtkWidget *lb = gtk_label_new (NULL);
  gtk_widget_add_css_class (lb, "lb"); /* the class is used by CSS Selector */
  gtk_window_set_child (win, lb);

  const GActionEntry win_entries[] = {
    { "fullscreen", NULL, NULL, "false", fullscreen_changed }
  };
  g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);

  gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (win), TRUE);

  gtk_window_present (win);
}

static void
app_startup (GApplication *app) {
  GtkBuilder *builder;
  GMenuModel *menubar;

  const GActionEntry app_entries[] = {
    { "color", color_activated, "s", "'red'", NULL },
    { "quit", quit_activated, NULL, NULL, NULL }
  };
  g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries, G_N_ELEMENTS (app_entries), app);

  builder = gtk_builder_new_from_file ("menu2.ui");
  menubar = G_MENU_MODEL (gtk_builder_get_object (builder, "menubar"));
  gtk_application_set_menubar (GTK_APPLICATION (app), G_MENU_MODEL (menubar));
  g_object_unref (builder);

  provider = gtk_css_provider_new ();
  /* Initialize the css data */
  gtk_css_provider_load_from_data (provider, "label.lb {background-color: red;}", -1);
  /* Add CSS to the default GdkDisplay. */
  GdkDisplay *display = gdk_display_get_default ();
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_signal_connect (app, "shutdown", G_CALLBACK (remove_provider), provider);
}

#define APPLICATION_ID "com.github.ToshioCP.menu2_ui"

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
