#include <gtk/gtk.h>
#include <string.h>

/* If debug is defined, a content type is printed out to the stdout. (line 49) */
/* To prevent this, remove or comment out the following line. */
#define debug 1

/* Only one main window exists even if the second application runs. */
static GtkWindow *win;
static GtkScrolledWindow *scr;

static GtkListView *list;
static GtkGridView *grid;

static GtkCssProvider *provider;

static void
view_activated(GSimpleAction *action, GVariant *parameter) {
  const char *view = g_variant_get_string (parameter, NULL);
  const char *other;
  char *css;

  if (strcmp (view, "list") == 0) {
    other = "grid";
    gtk_scrolled_window_set_child (scr, GTK_WIDGET (list));
  }else {
    other = "list";
    gtk_scrolled_window_set_child (scr, GTK_WIDGET (grid));
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
#ifdef debug
  g_print ("%s\n", content_type);
#endif
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
list_activate (GtkListView *list, int position) {
  GFileInfo *info = G_FILE_INFO (g_list_model_get_item (G_LIST_MODEL (gtk_list_view_get_model (list)), position));
  launch_tfe_with_file (info);
}

static void
grid_activate (GtkGridView *grid, int position) {
  GFileInfo *info = G_FILE_INFO (g_list_model_get_item (G_LIST_MODEL (gtk_grid_view_get_model (grid)), position));
  launch_tfe_with_file (info);
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
  return G_IS_FILE_INFO (info) ? g_strdup (g_file_info_get_name (info)) : NULL;
}

static void
window_destroy (GtkWindow *win) {
  g_clear_object (&list);
  g_clear_object (&grid);
  g_clear_object (&provider);
}

/* ----- activate, startup handlers ----- */
static void
app_activate (GApplication *application) {
  gtk_window_present (win);
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkDirectoryList *directory_list;
  GFile *file;
  GSimpleAction *act_view;
  GdkDisplay *display;

  GtkBuilder *build = gtk_builder_new_from_resource ("/com/github/ToshioCP/list5/list5.ui");
  win = GTK_WINDOW (gtk_builder_get_object (build, "win"));
  gtk_window_set_application (win, app);
  scr = GTK_SCROLLED_WINDOW (gtk_builder_get_object (build, "scr"));
  list = GTK_LIST_VIEW (gtk_builder_get_object (build, "list"));
  grid = GTK_GRID_VIEW (gtk_builder_get_object (build, "grid"));
  directory_list = GTK_DIRECTORY_LIST (gtk_builder_get_object (build, "directory_list"));
  /* necessary to own list and grid */
  g_object_ref (list);
  g_object_ref (grid);
  g_object_unref (build);

  g_signal_connect (list, "activate", G_CALLBACK (list_activate), NULL);
  g_signal_connect (grid, "activate", G_CALLBACK (grid_activate), NULL);

  /* First, 'list' is a child of scr. The child will be list or grid according to the clicked button, btnlist or btngrid. */
  gtk_scrolled_window_set_child (scr, GTK_WIDGET (list));

  file = g_file_new_for_path (".");
  gtk_directory_list_set_file (directory_list, file);
  g_object_unref (file);

  act_view = g_simple_action_new_stateful ("view", g_variant_type_new("s"), g_variant_new_string ("list"));
  g_signal_connect (act_view, "activate", G_CALLBACK (view_activated), NULL);
  g_action_map_add_action (G_ACTION_MAP (win), G_ACTION (act_view));

  display = gdk_display_get_default ();
  provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (provider, "button#btnlist {background: silver;} button#btngrid {background: white;}", -1);
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  g_signal_connect (win, "destroy", G_CALLBACK (window_destroy), NULL);
}

#define APPLICATION_ID "com.github.ToshioCP.list5"

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

