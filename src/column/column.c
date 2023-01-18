#include <gtk/gtk.h>

/* functions (closures) for GtkBuilderListItemFactory */
GIcon *
get_icon_factory (GtkListItem *item, GFileInfo *info) {
  GIcon *icon;

   /* g_file_info_get_icon can return NULL */
  icon = G_IS_FILE_INFO (info) ? g_file_info_get_icon (info) : NULL;
  return icon ? g_object_ref (icon) : NULL;
}

char *
get_file_name_factory (GtkListItem *item, GFileInfo *info) {
  return G_IS_FILE_INFO (info) ? g_strdup (g_file_info_get_name (info)) : NULL;
}

/* goffset is defined as gint64 */
/* It is used for file offsets. */
goffset
get_file_size_factory (GtkListItem *item, GFileInfo *info) {
  return G_IS_FILE_INFO (info) ? g_file_info_get_size (info) : -1;
}

char *
get_file_time_modified_factory (GtkListItem *item, GFileInfo *info) {
  GDateTime *dt;

   /* g_file_info_get_modification_date_time can return NULL */
  dt = G_IS_FILE_INFO (info) ? g_file_info_get_modification_date_time (info) : NULL;
  return dt ? g_date_time_format (dt, "%F") : NULL;
}

/* Functions (closures) for GtkSorter */
char *
get_file_name (GFileInfo *info) {
  return G_IS_FILE_INFO (info) ? g_strdup(g_file_info_get_name (info)) : NULL;
}

goffset
get_file_size (GFileInfo *info) {
  return G_IS_FILE_INFO (info) ? g_file_info_get_size (info): -1;
}

gint64
get_file_unixtime_modified (GFileInfo *info) {
  GDateTime *dt;

  dt = G_IS_FILE_INFO (info) ? g_file_info_get_modification_date_time (info) : NULL;
  return dt ? g_date_time_to_unix (dt) : -1;
}

static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  gtk_window_present (gtk_application_get_active_window(app));
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GFile *file;
  GtkBuilder *build = gtk_builder_new_from_resource ("/com/github/ToshioCP/column/column.ui");
  GtkWidget *win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  GtkDirectoryList *directorylist = GTK_DIRECTORY_LIST (gtk_builder_get_object (build, "directorylist"));
  g_object_unref (build);

  gtk_window_set_application (GTK_WINDOW (win), app);

  file = g_file_new_for_path (".");
  gtk_directory_list_set_file (directorylist, file);
  g_object_unref (file);
}

#define APPLICATION_ID "com.github.ToshioCP.columnview"

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

