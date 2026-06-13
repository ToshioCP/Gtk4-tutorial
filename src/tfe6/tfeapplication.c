#include <gtk/gtk.h>
#include "tfetextview.h"

/* ----- Private utility functions ----- */

/*
 * This function relies on the assumption that the notebook is the last child of the box.
 * Therefore, it will no longer work if another widget is added to the box.
 * There are several ways to solve this issue, which will be explained in a later section.
 */
static GtkWidget *
tfe_get_notebook (GtkWidget *win) {
  GtkWidget *boxv = gtk_window_get_child (GTK_WINDOW (win));
  return gtk_widget_get_last_child (boxv);
}

static void
tfe_error_alert (GtkWindow *win, GError *err) {
  GtkAlertDialog *alert_dialog;
  GtkWindow *parent;

  if (win != NULL && gtk_widget_get_mapped (GTK_WIDGET (win)))
    parent = win;
  else
    parent = NULL;
  alert_dialog = gtk_alert_dialog_new ("%s", err->message);
  gtk_alert_dialog_show (alert_dialog, parent);
  g_object_unref (alert_dialog);
}

/* The return value is floating */
GtkWidget *
tfe_label_from_file (GFile *file) {
  char *filename_bin, *filename_utf;
  GtkWidget *lab;

  if (file == NULL)
    return NULL;
  filename_bin = g_file_get_basename (file);
  filename_utf = g_filename_display_name (filename_bin);
  lab = gtk_label_new (filename_utf);
  g_free (filename_bin);
  g_free (filename_utf);
  return lab;
}

static void
notebook_page_new_with_file (GtkNotebook *nb, GFile *file) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));
  g_return_if_fail(G_IS_FILE (file) || file == NULL);

  GtkWidget *win;
  GtkNotebookPage *nbp;
  GtkWidget *scr;
  GtkWidget *tv;
  GtkWidget *lab;
  int i;
  GError *err = NULL;

  if ((tv = tfe_text_view_new_with_file (file, &err)) == NULL) {
    win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);
    tfe_error_alert (GTK_WINDOW (win), err);
    g_clear_error (&err);
    return;
  }
  lab = tfe_label_from_file (file); /* lab is floating. lab can be NULL */
  scr = gtk_scrolled_window_new ();
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), GTK_WIDGET (tv));
  i = gtk_notebook_append_page (nb, scr, lab);
  nbp = gtk_notebook_get_page (nb, scr);
  g_object_set (nbp, "tab-expand", TRUE, NULL);
  gtk_notebook_set_current_page (nb, i);
}

/* ----- End of private utility functions ----- */

static void
open_dialog_cb (GObject *source_object, GAsyncResult *res, gpointer data) {
  GtkFileDialog *dialog = GTK_FILE_DIALOG (source_object);
  GtkNotebook *nb = GTK_NOTEBOOK (data);
  GtkWidget *win;
  GFile *file;
  GError *err = NULL;

  if ((file = gtk_file_dialog_open_finish (dialog, res, &err)) == NULL) {
    if (!g_error_matches (err, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED)) {
      win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);
      tfe_error_alert (GTK_WINDOW (win), err);
    }
    g_clear_error (&err);
    return;
  }
  notebook_page_new_with_file (nb, file);
  g_object_unref (file);
}

static void
open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkWidget *win = GTK_WIDGET (user_data);
  GtkWidget *nb = tfe_get_notebook (win);
  GtkFileDialog *dialog;

  dialog = gtk_file_dialog_new ();
  gtk_file_dialog_open (dialog, GTK_WINDOW (win), NULL, open_dialog_cb, nb);
  g_object_unref (dialog);
}

static void
save_dialog_cb (GObject *source_object, GAsyncResult *res, gpointer data) {
  GtkFileDialog *dialog = GTK_FILE_DIALOG (source_object);
  GtkWidget *tv = GTK_WIDGET (data);
  GtkWidget *win;
  GtkWidget *scr;
  GtkWidget *nb;
  GtkWidget *lab;
  GFile *file;
  GError *err = NULL;

  win = gtk_widget_get_ancestor (tv, GTK_TYPE_WINDOW);
  if ((file = gtk_file_dialog_save_finish (dialog, res, &err)) == NULL) {
    if (!g_error_matches (err, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED)) {
      tfe_error_alert (GTK_WINDOW (win), err);
    }
    g_clear_error (&err);
    return;
  }
  tfe_text_view_set_file (TFE_TEXT_VIEW (tv), file);
  g_object_unref (file);
  lab = tfe_label_from_file (file); /* lab is floating. lab can be NULL */
  nb = gtk_widget_get_ancestor (tv, GTK_TYPE_NOTEBOOK);
  scr = gtk_widget_get_parent (tv);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (nb), scr, lab);

  if (! tfe_text_view_write (TFE_TEXT_VIEW (tv), &err)) {
    tfe_error_alert (GTK_WINDOW (win), err);
    g_clear_error (&err);
  }
}

static void
save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkWidget *win = GTK_WIDGET (user_data);
  GtkWidget *nb = tfe_get_notebook (win);
  int i;
  GtkWidget *scr;
  GtkWidget *tv;
  GError *err = NULL;
  GtkFileDialog *dialog;
  GtkAlertDialog *alert_dialog;

  i = gtk_notebook_get_current_page (GTK_NOTEBOOK (nb));

  if (i == -1) {
    alert_dialog = gtk_alert_dialog_new ("No page to save.");
    gtk_alert_dialog_show (alert_dialog, GTK_WINDOW (win));
    g_object_unref (alert_dialog);
    return;
  }

  scr = gtk_notebook_get_nth_page (GTK_NOTEBOOK (nb), i);
  tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));

  if (tfe_text_view_get_file (TFE_TEXT_VIEW (tv))) { /* File is already set */
    if (! tfe_text_view_write (TFE_TEXT_VIEW (tv), &err)) {
      tfe_error_alert (GTK_WINDOW (win), err);
      g_clear_error (&err);
    }
  } else {
    dialog = gtk_file_dialog_new ();
    gtk_file_dialog_save (dialog, GTK_WINDOW (win), NULL, save_dialog_cb, tv);
    g_object_unref (dialog);
  }
}

static void
close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkWidget *win = GTK_WIDGET (user_data);
  GtkWidget *nb = tfe_get_notebook (win);

  if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) >= 2)
    gtk_notebook_remove_page (GTK_NOTEBOOK (nb), gtk_notebook_get_current_page (GTK_NOTEBOOK (nb)));
  else  /* If the page is the last one or no page exists */
    gtk_window_destroy (GTK_WINDOW (gtk_widget_get_ancestor (nb, GTK_TYPE_WINDOW)));
}

static void
new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkWidget *win = GTK_WIDGET (user_data);
  GtkWidget *nb = tfe_get_notebook (win);

  notebook_page_new_with_file (GTK_NOTEBOOK (nb), NULL);
}

static void
saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  /* not implemented yet */
}

static void
pref_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  /* not implemented yet */
}

static void
quit_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GApplication *application = G_APPLICATION (user_data);

  g_application_quit (application);
}

/* Helper function for activate and open handlers*/
static GtkWidget *
get_main_window (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GList *windows = gtk_application_get_windows (app);
  GtkWidget *win;
  GtkWidget *btnm;
  GtkBuilder *build;

  if (windows)
    return GTK_WIDGET (windows->data);

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe6/tfe.ui");
  win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  gtk_window_set_application (GTK_WINDOW (win), app);
  btnm = GTK_WIDGET (gtk_builder_get_object (build, "btnm"));
  g_object_unref(build);

  gtk_menu_button_set_menu_model (GTK_MENU_BUTTON (btnm), G_MENU_MODEL (gtk_application_get_menu_by_id (app, "menu")));

  /* ----- action ----- */
  const GActionEntry win_entries[] = {
    { "open", open_activated, NULL, NULL, NULL },
    { "save", save_activated, NULL, NULL, NULL },
    { "close", close_activated, NULL, NULL, NULL },
    { "new", new_activated, NULL, NULL, NULL },
    { "saveas", saveas_activated, NULL, NULL, NULL },
    { "pref", pref_activated, NULL, NULL, NULL },
  };
  g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);

  return win;
}

void
app_activate (GApplication *application) {
  GtkWidget *win;
  GtkWidget *nb;

  win = get_main_window (application);
  nb = tfe_get_notebook (win);
  if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) == 0) /* No page exists */
    notebook_page_new_with_file (GTK_NOTEBOOK (nb), NULL);
  gtk_window_present (GTK_WINDOW (win));
}

void
app_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
  GtkWidget *win;
  GtkWidget *nb;
  int i;

  win = get_main_window (application);

  nb = tfe_get_notebook (win);
  for (i = 0; i < n_files; i++) {
    notebook_page_new_with_file (GTK_NOTEBOOK (nb), files[i]);
  }
  if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) == 0) /* No files have been opened */
    notebook_page_new_with_file (GTK_NOTEBOOK (nb), NULL);
  gtk_window_present (GTK_WINDOW (win));
}

void
app_startup (GApplication *application) {
  GSimpleAction *quit_action = g_simple_action_new ("quit", NULL);

  g_signal_connect (quit_action, "activate", G_CALLBACK (quit_activated), application);
  g_action_map_add_action (G_ACTION_MAP (application), G_ACTION (quit_action));
  g_object_unref (quit_action);
}

#define APPLICATION_ID "com.github.ToshioCP.tfe6"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_HANDLES_OPEN);

  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);

  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
