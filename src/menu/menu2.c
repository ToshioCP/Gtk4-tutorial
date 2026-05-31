#include <gtk/gtk.h>

static void
toggle_switch_changed(GSimpleAction *action, GVariant *value, gpointer user_data) {
  GtkWidget *label = GTK_WIDGET (user_data);

  if (g_variant_get_boolean (value))
    gtk_label_set_text (GTK_LABEL (label), "ON");
  else
    gtk_label_set_text (GTK_LABEL (label), "OFF");
  g_simple_action_set_state (action, value);
}

static void
color_state_changed(GSimpleAction *action, GVariant *value, gpointer user_data) {
  GtkCssProvider *provider = GTK_CSS_PROVIDER (user_data);
  char *color = g_strdup_printf ("label.lb {background-color: %s; font-size: 72pt; color: white;}", g_variant_get_string (value, NULL));
  /* Change the CSS data in the provider. */
  /* Previous data is thrown away. */
  gtk_css_provider_load_from_string (provider, color);
  g_free (color);
  g_simple_action_set_state (action, value);
}

static void
app_shutdown (GApplication *app, GtkCssProvider *provider) {
  gtk_style_context_remove_provider_for_display (gdk_display_get_default(), GTK_STYLE_PROVIDER (provider));
  g_object_unref (provider);
}

static void
app_activate (GApplication *app) {
  GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_title (GTK_WINDOW (win), "menu2");
  gtk_window_set_default_size (GTK_WINDOW (win), 800, 600);

  GtkWidget *label = gtk_label_new ("OFF");
  gtk_widget_add_css_class (label, "lb"); /* the class is used by CSS Selector */
  gtk_window_set_child (GTK_WINDOW (win), label);

  GSimpleAction *act_toggle_switch
    = g_simple_action_new_stateful ("toggle-switch", NULL, g_variant_new_boolean (FALSE));
  g_signal_connect (act_toggle_switch, "change-state", G_CALLBACK (toggle_switch_changed), label);
  g_action_map_add_action (G_ACTION_MAP (win), G_ACTION (act_toggle_switch));
  g_object_unref (act_toggle_switch);
  gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (win), TRUE);

  gtk_window_present (GTK_WINDOW (win));
}

static void
app_startup (GApplication *app) {
  /* The provider below provides application wide CSS data. */
  GtkCssProvider *provider = gtk_css_provider_new ();
  GSimpleAction *act_color
    = g_simple_action_new_stateful ("color", G_VARIANT_TYPE_STRING, g_variant_new_string ("red"));
  GSimpleAction *act_quit
    = g_simple_action_new ("quit", NULL);
  g_signal_connect (act_color, "change-state", G_CALLBACK (color_state_changed), provider);
  g_signal_connect_swapped (act_quit, "activate", G_CALLBACK (g_application_quit), app);
  g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_color));
  g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_quit));
  g_object_unref (act_color);
  g_object_unref (act_quit);

  GMenu *menubar = g_menu_new ();
  GMenu *menu = g_menu_new ();
  GMenu *section1 = g_menu_new ();
  GMenu *section2 = g_menu_new ();
  GMenu *section3 = g_menu_new ();
  GMenuItem *menu_item_toggle_switch = g_menu_item_new ("Toggle Switch", "win.toggle-switch");
  GMenuItem *menu_item_red = g_menu_item_new ("Red", "app.color::red");
  GMenuItem *menu_item_green = g_menu_item_new ("Green", "app.color::green");
  GMenuItem *menu_item_blue = g_menu_item_new ("Blue", "app.color::blue");
  GMenuItem *menu_item_quit = g_menu_item_new ("Quit", "app.quit");

  g_menu_append_item (section1, menu_item_toggle_switch);
  g_menu_append_item (section2, menu_item_red);
  g_menu_append_item (section2, menu_item_green);
  g_menu_append_item (section2, menu_item_blue);
  g_menu_append_item (section3, menu_item_quit);
  g_object_unref (menu_item_red);
  g_object_unref (menu_item_green);
  g_object_unref (menu_item_blue);
  g_object_unref (menu_item_toggle_switch);
  g_object_unref (menu_item_quit);

  g_menu_append_section (menu, NULL, G_MENU_MODEL (section1));
  g_menu_append_section (menu, "Color", G_MENU_MODEL (section2));
  g_menu_append_section (menu, NULL, G_MENU_MODEL (section3));
  g_menu_append_submenu (menubar, "Menu", G_MENU_MODEL (menu));
  g_object_unref (section1);
  g_object_unref (section2);
  g_object_unref (section3);
  g_object_unref (menu);

  gtk_application_set_menubar (GTK_APPLICATION (app), G_MENU_MODEL (menubar));
  g_object_unref (menubar);

  /* Initialize the css data */
  gtk_css_provider_load_from_string (provider, "label.lb {background-color: red; font-size: 72pt; color: white;}");
  /* Add CSS to the default GdkDisplay. */
  gtk_style_context_add_provider_for_display (gdk_display_get_default (),
        GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_signal_connect (app, "shutdown", G_CALLBACK (app_shutdown), provider);
}

#define APPLICATION_ID "com.github.ToshioCP.menu2"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);

  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
