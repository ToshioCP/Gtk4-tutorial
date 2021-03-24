#include <gtk/gtk.h>

static void
setup_cb (GtkListItemFactory *factory, GtkListItem *listitem, gpointer user_data) {
  GtkWidget *lb = gtk_label_new ("");
  gtk_list_item_set_child (listitem, lb);
}

static void
bind_cb (GtkSignalListItemFactory *self, GtkListItem *listitem, gpointer user_data) {
  GtkWidget *lb = gtk_list_item_get_child (listitem);
  GtkStringObject *strobj = gtk_list_item_get_item (listitem);
  const char *text = gtk_string_object_get_string (strobj);

  gtk_label_set_text (GTK_LABEL (lb), text);
}

static void
unbind_cb (GtkSignalListItemFactory *self, GtkListItem *listitem, gpointer user_data) {
  GtkWidget *lb = gtk_list_item_get_child (listitem);
  gtk_label_set_text (GTK_LABEL (lb), "");
}

static void
teardown_cb (GtkListItemFactory *factory, GtkListItem *listitem, gpointer user_data) {
  gtk_list_item_set_child (listitem, NULL);
/*  When the child of listitem is set to NULL, the reference to GtkLabel will be released and lb will be destroyed. */
/* Therefore, g_object_unref () for the GtkLabel object doesn't need in the user code. */
}

/* ----- activate, open, startup handlers ----- */
static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win = gtk_application_window_new (app);
  gtk_window_set_default_size (GTK_WINDOW (win), 600, 400);
  GtkWidget *scr = gtk_scrolled_window_new ();
  gtk_window_set_child (GTK_WINDOW (win), scr);

  char *array[] = {
    "one", "two", "three", "four", NULL
  };
  GtkStringList *sl = gtk_string_list_new ((const char * const *) array);
  GtkNoSelection *ns = gtk_no_selection_new (G_LIST_MODEL (sl));

  GtkListItemFactory *factory = gtk_signal_list_item_factory_new ();
  g_signal_connect (factory, "setup", G_CALLBACK (setup_cb), NULL);
  g_signal_connect (factory, "bind", G_CALLBACK (bind_cb), NULL);
  g_signal_connect (factory, "unbind", G_CALLBACK (unbind_cb), NULL);
  g_signal_connect (factory, "teardown", G_CALLBACK (teardown_cb), NULL);

  GtkWidget *lv = gtk_list_view_new (GTK_SELECTION_MODEL (ns), factory);
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), lv);
  gtk_widget_show (win);
}

static void
app_startup (GApplication *application) {
}

/* ----- main ----- */
int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new ("com.github.ToshioCP.list1", G_APPLICATION_FLAGS_NONE);

  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

