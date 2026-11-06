#include <gtk/gtk.h>

/* If debug is defined, a content type is printed out to the stdout. */
/* To prevent this, remove or comment out the following line. */
#define debug 1

static void
view_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkStack *stack = GTK_STACK (user_data);
  const char *view = g_variant_get_string (parameter, NULL);

  gtk_stack_set_visible_child_name (stack, view);
  g_action_change_state (G_ACTION (action), parameter);
}

static void
item_activated (GtkWidget *view, guint position, gpointer user_data) {
  GListModel *model = G_LIST_MODEL (user_data);
  GFileInfo *info = G_FILE_INFO (g_list_model_get_item (model, position));
  const char *content_type;
  GAppInfo *appinfo;
  GFile *file;
  GList *files = NULL;
  GError *err = NULL;

  if (! info)
    return;
  content_type = g_file_info_get_content_type (info);
#ifdef debug
  g_print ("%s\n", content_type);
#endif
  if (content_type && g_str_has_prefix (content_type, "text/")) {
    appinfo = g_app_info_create_from_commandline ("tfe", "tfe", G_APP_INFO_CREATE_NONE, &err);
    if (err) {
      g_printerr ("%s\n", err->message);
      g_error_free (err);
    } else {
      file = g_file_new_for_path (g_file_info_get_name (info));
      files = g_list_append (files, file);
      if (! (g_app_info_launch (appinfo, files, NULL, &err))) {
        g_printerr ("%s\n", err->message);
        g_error_free (err);
      }
      g_list_free_full (files, g_object_unref);
      g_object_unref (appinfo);
    }
  }
  g_object_unref (info);
}

GIcon *
get_icon (GtkListItem *item, GFileInfo *info) {
  GIcon *icon;

  /* g_file_info_get_icon can return NULL */
  icon = G_IS_FILE_INFO (info) ? g_file_info_get_icon (info) : NULL;
  return icon ? g_object_ref (icon) : NULL;
}

char *
get_file_name (GtkListItem *item, GFileInfo *info) {
  const char *name;

  name = G_IS_FILE_INFO (info) ? g_file_info_get_name (info) : NULL;
  return name ? g_filename_display_name (name) : g_strdup ("");
}

static void
app_activate (GApplication *application, gpointer user_data) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWindow *win = gtk_application_get_active_window (app);

  if (win == NULL) {
    GtkBuilder *build;
    GtkStack *stack;
    GtkListView *list;
    GtkGridView *grid;
    GtkDirectoryList *directory_list;
    GFile *file;
    GSimpleAction *act_view;

    build = gtk_builder_new_from_resource ("/com/github/ToshioCP/list5/list5.ui");
    win = GTK_WINDOW (gtk_builder_get_object (build, "win"));
    gtk_window_set_application (win, app);

    stack = GTK_STACK (gtk_builder_get_object (build, "stack"));
    list = GTK_LIST_VIEW (gtk_builder_get_object (build, "list"));
    grid = GTK_GRID_VIEW (gtk_builder_get_object (build, "grid"));
    directory_list = GTK_DIRECTORY_LIST (gtk_builder_get_object (build, "directory_list"));
    g_object_unref (build);

    gtk_stack_set_visible_child_name (stack, "list");

    g_signal_connect (list, "activate", G_CALLBACK (item_activated), directory_list);
    g_signal_connect (grid, "activate", G_CALLBACK (item_activated), directory_list);

    file = g_file_new_for_path (".");
    gtk_directory_list_set_file (directory_list, file);
    g_object_unref (file);

    act_view = g_simple_action_new_stateful ("view", G_VARIANT_TYPE_STRING, g_variant_new_string ("list"));
    g_signal_connect (act_view, "activate", G_CALLBACK (view_activated), stack);
    g_action_map_add_action (G_ACTION_MAP (win), G_ACTION (act_view));
    g_object_unref (act_view);
  }
  gtk_window_present (win);
}

#define APPLICATION_ID "com.github.ToshioCP.list5"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
