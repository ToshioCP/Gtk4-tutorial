#include <gtk/gtk.h>

static GtkCssProvider *provider;
static char *c[3] = {"red", "green", "blue"}; 

static void
fullscreen_changed(GSimpleAction *action, GVariant *value, gpointer win) {
  if (g_variant_get_boolean (value))
    gtk_window_maximize (GTK_WINDOW (win));
  else
    gtk_window_unmaximize (GTK_WINDOW (win));
  g_simple_action_set_state (action, value);
}

static void
color_activated(GSimpleAction *action, GVariant *parameter, gpointer win) {
  gint16 index = g_variant_get_int16 (parameter);
  char *color = g_strdup_printf ("label#lb {background-color: %s;}", c[index]);
  gtk_css_provider_load_from_data (provider, color, -1);
  g_free (color);
  g_action_change_state (G_ACTION (action), parameter);
}

static void
quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  g_application_quit (G_APPLICATION(app));
}

static void
app_activate (GApplication *app, gpointer user_data) {
  GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_title (GTK_WINDOW (win), "menu2");
  gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);

  GtkWidget *lb = gtk_label_new (NULL);
  gtk_widget_set_name (lb, "lb"); /* the name is used by CSS Selector */
  gtk_window_set_child (GTK_WINDOW (win), lb);

  GSimpleAction *act_fullscreen
    = g_simple_action_new_stateful ("fullscreen", NULL, g_variant_new_boolean (FALSE));
  GSimpleAction *act_color
    = g_simple_action_new_stateful ("color", g_variant_type_new("n"), g_variant_new_int16 ((gint16) 0)); /* "n": gint16 */
  GSimpleAction *act_quit
    = g_simple_action_new ("quit", NULL);

  GMenu *menubar = g_menu_new ();
  GMenu *menu = g_menu_new ();
  GMenu *section1 = g_menu_new ();
  GMenu *section2 = g_menu_new ();
  GMenu *section3 = g_menu_new ();
  GMenuItem *menu_item_fullscreen = g_menu_item_new ("Full Screen", "win.fullscreen");
  GMenuItem *menu_item_red = g_menu_item_new ("Red", "win.color(int16 0)");
  GMenuItem *menu_item_green = g_menu_item_new ("Green", "win.color(int16 1)");
  GMenuItem *menu_item_blue = g_menu_item_new ("Blue", "win.color(int16 2)");
  GMenuItem *menu_item_quit = g_menu_item_new ("Quit", "app.quit");

  g_signal_connect (act_fullscreen, "change-state", G_CALLBACK (fullscreen_changed), win);
  g_signal_connect (act_color, "activate", G_CALLBACK (color_activated), win);
  g_signal_connect (act_quit, "activate", G_CALLBACK (quit_activated), app);
  g_action_map_add_action (G_ACTION_MAP (win), G_ACTION (act_fullscreen));
  g_action_map_add_action (G_ACTION_MAP (win), G_ACTION (act_color));
  g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_quit));

  g_menu_append_item (section1, menu_item_fullscreen);
  g_menu_append_item (section2, menu_item_red);
  g_menu_append_item (section2, menu_item_green);
  g_menu_append_item (section2, menu_item_blue);
  g_menu_append_item (section3, menu_item_quit);
  g_object_unref (menu_item_red);
  g_object_unref (menu_item_green);
  g_object_unref (menu_item_blue);
  g_object_unref (menu_item_fullscreen);
  g_object_unref (menu_item_quit);

  g_menu_append_section (menu, NULL, G_MENU_MODEL (section1));
  g_menu_append_section (menu, "Color", G_MENU_MODEL (section2));
  g_menu_append_section (menu, NULL, G_MENU_MODEL (section3));
  g_menu_append_submenu (menubar, "Menu", G_MENU_MODEL (menu));

  gtk_application_set_menubar (GTK_APPLICATION (app), G_MENU_MODEL (menubar));
  gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (win), TRUE);

/*  GtkCssProvider *provider = gtk_css_provider_new ();*/
  provider = gtk_css_provider_new ();
  GdkDisplay *display = gtk_widget_get_display (GTK_WIDGET (win));
  gtk_css_provider_load_from_data (provider, "label#lb {background-color: red;}", -1);
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);

/*  gtk_widget_show (win);*/
  gtk_window_present (GTK_WINDOW (win));
}

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new ("com.github.ToshioCP.menu2", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

