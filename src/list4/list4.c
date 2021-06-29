#include <gtk/gtk.h>
#include <string.h>

static GtkWidget *list;
static GtkWidget *grid;
static GdkDisplay *display;
static GtkCssProvider *provider;

static gboolean
before_close (GtkWindow *win, gpointer user_data) {
  g_object_unref (list);
  g_object_unref (grid);
  return FALSE;
}

static void
view_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkScrolledWindow *scr = GTK_SCROLLED_WINDOW (user_data);
  const char *view = g_variant_get_string (parameter, NULL);
  const char *other;
  char *css;

  if (strcmp (view, "list") == 0) {
    other = "grid";
    gtk_scrolled_window_set_child (scr, list);
  }else {
    other = "list";
    gtk_scrolled_window_set_child (scr, grid);
  }
  css = g_strdup_printf ("button#btn%s {background: silver;} button#btn%s {background: white;}", view, other);
  gtk_css_provider_load_from_data (provider, css, -1);
  g_free (css);
  g_action_change_state (G_ACTION (action), parameter);
}

static void
launch_tfe_with_file (GFileInfo *info) {
  GError *err = NULL;
  GFile *file;
  GList *files = NULL;
  const char *content_type;
  const char *text_type = "text/";
  GAppInfo *appinfo;
  int i;

  if (! info)
    return;
  content_type = g_file_info_get_content_type (info);
g_print ("%s\n", content_type);  /* This line can be commented out if unnecessary */
  if (! content_type)
    return;
  for (i=0;i<5;++i) {
    if (content_type[i] != text_type[i])
      return;
  }
  appinfo = g_app_info_create_from_commandline ("tfe", "tfe", G_APP_INFO_CREATE_NONE, &err);
  if (err) {
    g_printerr ("%s\n", err->message);
    g_error_free (err);
    return;
  }
  err = NULL;
  file = g_file_new_for_path (g_file_info_get_name (info));
  files = g_list_append (files, file);
  if (! (g_app_info_launch (appinfo, files, NULL, &err))) {
    g_printerr ("%s\n", err->message);
    g_error_free (err);
  }
  g_list_free_full (files, g_object_unref);
  g_object_unref (appinfo);
}

static void
list_activate (GtkListView *list, int position, gpointer user_data) {
  GFileInfo *info = G_FILE_INFO (g_list_model_get_item (G_LIST_MODEL (gtk_list_view_get_model (list)), position));
  launch_tfe_with_file (info);
}

static void
grid_activate (GtkGridView *grid, int position, gpointer user_data) {
  GFileInfo *info = G_FILE_INFO (g_list_model_get_item (G_LIST_MODEL (gtk_grid_view_get_model (grid)), position));
  launch_tfe_with_file (info);
}

GIcon *
get_icon (GtkListItem *item, GFileInfo *info) {
  GIcon *icon;

  if (! G_IS_FILE_INFO (info))
    return NULL;
  else {
    icon = g_file_info_get_icon (info);
    g_object_ref (icon);
    return icon;
  }
}

char *
get_file_name (GtkListItem *item, GFileInfo *info) {
  if (! G_IS_FILE_INFO (info))
    return NULL;
  else
    return g_strdup (g_file_info_get_name (info));
}

/* ----- activate, open, startup handlers ----- */
static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GFile *file;
  GSimpleAction *act_view;

  GtkBuilder *build = gtk_builder_new_from_resource ("/com/github/ToshioCP/list4/list4.ui");
  GtkWidget *win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  GtkWidget *scr = GTK_WIDGET (gtk_builder_get_object (build, "scr"));
  list = GTK_WIDGET (gtk_builder_get_object (build, "list"));
  grid = GTK_WIDGET (gtk_builder_get_object (build, "grid"));
  GtkDirectoryList *directorylist = GTK_DIRECTORY_LIST (gtk_builder_get_object (build, "directorylist"));
  g_object_ref (list);
  g_object_ref (grid);
  g_object_unref (build);

  GtkListItemFactory *factory_list = gtk_builder_list_item_factory_new_from_resource (NULL, "/com/github/ToshioCP/list4/factory_list.ui");
  GtkListItemFactory *factory_grid = gtk_builder_list_item_factory_new_from_resource (NULL, "/com/github/ToshioCP/list4/factory_grid.ui");

  gtk_window_set_application (GTK_WINDOW (win), app);
  /* First, 'list' is a child of scr. The child will be list or grid according to the clicked button, btnlist or btngrid. */
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), list);

  file = g_file_new_for_path (".");
  gtk_directory_list_set_file (directorylist, file);
  g_object_unref (file);

  gtk_list_view_set_factory (GTK_LIST_VIEW (list), factory_list);
  gtk_grid_view_set_factory (GTK_GRID_VIEW (grid), factory_grid);
  g_signal_connect (GTK_LIST_VIEW (list), "activate", G_CALLBACK (list_activate), NULL);
  g_signal_connect (GTK_GRID_VIEW (grid), "activate", G_CALLBACK (grid_activate), NULL);

  act_view = g_simple_action_new_stateful ("view", g_variant_type_new("s"), g_variant_new_string ("list"));
  g_signal_connect (act_view, "activate", G_CALLBACK (view_activated), scr);
  g_action_map_add_action (G_ACTION_MAP (win), G_ACTION (act_view));

  display = gtk_widget_get_display (GTK_WIDGET (win));
  provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (provider, "button#btnlist {background: silver;} button#btngrid {background: white;}", -1);
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  g_signal_connect (GTK_WINDOW (win), "close-request", G_CALLBACK (before_close), NULL);
  gtk_widget_show (win);
}

static void
app_startup (GApplication *application) {
}

#define APPLICATION_ID "com.github.ToshioCP.list4"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_FLAGS_NONE);

  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
/*  g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);*/

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

