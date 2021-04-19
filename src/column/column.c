#include <gtk/gtk.h>

/* functions (closures) for GtkBuilderListItemFactory */
GIcon *
get_icon_factory (GtkListItem *item, GFileInfo *info) {
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
get_file_name_factory (GtkListItem *item, GFileInfo *info) {
  if (! G_IS_FILE_INFO (info))
    return NULL;
  else
    return g_strdup (g_file_info_get_name (info));
}

char *
get_file_size_factory (GtkListItem *item, GFileInfo *info) {
  /* goffset is gint64 */
  goffset size;

  if (! G_IS_FILE_INFO (info))
    return NULL;
  else {
    size = g_file_info_get_size (info);
    return g_strdup_printf ("%ld", (long int) size);
  }
}

char *
get_file_time_modified_factory (GtkListItem *item, GFileInfo *info) {
  GDateTime *dt;

  if (! G_IS_FILE_INFO (info))
    return NULL;
  else {
    dt = g_file_info_get_modification_date_time (info);
    return g_date_time_format (dt, "%F");
  }
}

/* Functions (closures) for GtkSorter */
char *
get_file_name (GFileInfo *info) {
  g_return_val_if_fail (G_IS_FILE_INFO (info), NULL);

  return g_strdup(g_file_info_get_name (info));
}

goffset
get_file_size (GFileInfo *info) {
  g_return_val_if_fail (G_IS_FILE_INFO (info), -1);

  return g_file_info_get_size (info);
}

gint64
get_file_unixtime_modified (GFileInfo *info) {
  g_return_val_if_fail (G_IS_FILE_INFO (info), -1);

  GDateTime *dt;

  dt = g_file_info_get_modification_date_time (info);
  return g_date_time_to_unix (dt);
}

/* ----- activate, open, startup handlers ----- */
static void
app_activate (GApplication *application) {
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

  gtk_widget_show (win);
}

static void
app_startup (GApplication *application) {
}

#define APPLICATION_ID "com.github.ToshioCP.columnview"

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

