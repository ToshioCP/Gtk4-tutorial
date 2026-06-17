#include <gtk/gtk.h>
#include "tfetextview.h"
#include "tfewindow.h"

struct _TfeWindow {
  GtkApplicationWindow parent;
  GtkMenuButton *btnm;
  GtkNotebook *nb;
};

G_DEFINE_FINAL_TYPE (TfeWindow, tfe_window, GTK_TYPE_APPLICATION_WINDOW)

/* ----- Private utility functions ----- */

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

/* ----- End of private utility functions ----- */

/* action handlers */

static void
open_dialog_cb (GObject *source_object, GAsyncResult *res, gpointer data) {
  GtkFileDialog *dialog = GTK_FILE_DIALOG (source_object);
  TfeWindow *win = TFE_WINDOW (data);
  GFile *file;
  GError *err = NULL;

  if ((file = gtk_file_dialog_open_finish (dialog, res, &err)) == NULL) {
    if (!g_error_matches (err, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED))
      tfe_error_alert (GTK_WINDOW (win), err);
    g_clear_error (&err);
    return;
  }
  tfe_window_append_page (win, file);
  g_object_unref (file);
}

static void
open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  GtkFileDialog *dialog;

  dialog = gtk_file_dialog_new ();
  gtk_file_dialog_open (dialog, GTK_WINDOW (win), NULL, open_dialog_cb, win);
  g_object_unref (dialog);
}

static void
save_dialog_cb (GObject *source_object, GAsyncResult *res, gpointer data) {
  GtkFileDialog *dialog = GTK_FILE_DIALOG (source_object);
  GtkWidget *tv = GTK_WIDGET (data);
  GtkWidget *win = gtk_widget_get_ancestor (tv, TFE_TYPE_WINDOW);
  GtkWidget *scr;
  GtkWidget *lab;
  GFile *file;
  GError *err = NULL;

  if ((file = gtk_file_dialog_save_finish (dialog, res, &err)) == NULL) {
    if (!g_error_matches (err, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED)) {
      tfe_error_alert (GTK_WINDOW (win), err);
    }
    g_clear_error (&err);
    return;
  }
  tfe_text_view_set_file (TFE_TEXT_VIEW (tv), file);
  lab = tfe_label_from_file (file); /* lab is floating. lab can be NULL */
  g_object_unref (file);
  scr = gtk_widget_get_parent (tv);
  gtk_notebook_set_tab_label (TFE_WINDOW (win)->nb, scr, lab);

  if (! tfe_text_view_write (TFE_TEXT_VIEW (tv), &err)) {
    tfe_error_alert (GTK_WINDOW (win), err);
    g_clear_error (&err);
  }
}

static void
save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  int i;
  GtkWidget *scr;
  GtkWidget *tv;
  GError *err = NULL;
  GtkFileDialog *dialog;
  GtkAlertDialog *alert_dialog;

  i = gtk_notebook_get_current_page (win->nb);

  if (i == -1) {
    alert_dialog = gtk_alert_dialog_new ("No page to save.");
    gtk_alert_dialog_show (alert_dialog, GTK_WINDOW (win));
    g_object_unref (alert_dialog);
    return;
  }

  scr = gtk_notebook_get_nth_page (win->nb, i);
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
close_page (TfeWindow *win) {
  if (gtk_notebook_get_n_pages (win->nb) >= 2)
    gtk_notebook_remove_page (win->nb, gtk_notebook_get_current_page (win->nb));
  else {  /* If the page is the last one or no page exists */
    /*
    * Delay the window destruction until the main loop becomes idle.
    * This function must be called after all other pending operations are completed.
    * For example, we need to wait for the GtkAlertDialog's asynchronous
    * cleanup and internal signal disconnections to safely finish.
    * If the window is destroyed immediately here, it will cause 
    * a "GLib-GObject-CRITICAL" error.
    */
    g_idle_add_once ((GSourceOnceFunc) gtk_window_destroy, win);
  }
}

static void
page_close_alert_cb (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  GtkAlertDialog *dialog = GTK_ALERT_DIALOG (source_object);
  GtkWidget *win;
  GError *err = NULL;
  int response = gtk_alert_dialog_choose_finish (dialog, res, &err);

  if (err != NULL) {
      g_warning ("Alert Dialog Error: %s", err->message);
      g_error_free (err);
      return;
  }
  
  win = GTK_WIDGET (user_data); /* This assignment must be done after the error check */
  if (response == 1) { /* Close Without Saving */
      close_page (TFE_WINDOW (win));
  }
  /* Cancel => do nothing*/
}

static void
close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  GtkWidget *scr = gtk_notebook_get_nth_page (win->nb, gtk_notebook_get_current_page (win->nb));
  GtkWidget *tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
  GtkAlertDialog *dialog;
  const char *buttons[] = {"Cancel", "Close Without Saving", NULL};

    if (!tfe_text_view_get_modified (TFE_TEXT_VIEW (tv))) {
      close_page (win);
      return;
    }
  
  dialog = gtk_alert_dialog_new ("Unsaved changes");
  gtk_alert_dialog_set_detail (dialog, "Are you sure to close this page without saving?");

  gtk_alert_dialog_set_buttons (dialog, buttons);
  gtk_alert_dialog_set_cancel_button (dialog, 0); /* cancel button */
  gtk_alert_dialog_set_default_button (dialog, 0); /* default focus */

  gtk_alert_dialog_choose (dialog, GTK_WINDOW (win), NULL, page_close_alert_cb, win);
  g_object_unref (dialog);
}

static void
new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  tfe_window_append_page (TFE_WINDOW (win), NULL);
}

static void
saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  int i;
  GtkWidget *scr;
  GtkWidget *tv;
  GtkFileDialog *dialog;
  GtkAlertDialog *alert_dialog;

  i = gtk_notebook_get_current_page (win->nb);

  if (i == -1) {
    alert_dialog = gtk_alert_dialog_new ("No page to save.");
    gtk_alert_dialog_show (alert_dialog, GTK_WINDOW (win));
    g_object_unref (alert_dialog);
    return;
  }

  scr = gtk_notebook_get_nth_page (win->nb, i);
  tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));

  dialog = gtk_file_dialog_new ();
  gtk_file_dialog_save (dialog, GTK_WINDOW (win), NULL, save_dialog_cb, tv);
  g_object_unref (dialog);
}

/* end of action handlers */

/* instance method */

void
tfe_window_append_page (TfeWindow *win, GFile *file) {
  g_return_if_fail(TFE_IS_WINDOW (win));
  g_return_if_fail(G_IS_FILE (file) || file == NULL);

  GtkNotebookPage *nbp;
  GtkWidget *scr;
  GtkWidget *tv;
  GtkWidget *lab;
  int i;
  GError *err = NULL;

  if ((tv = tfe_text_view_new_with_file (file, &err)) == NULL) {
    tfe_error_alert (GTK_WINDOW (win), err);
    g_clear_error (&err);
    return;
  }
  lab = tfe_label_from_file (file); /* lab is floating. lab can be NULL */
  scr = gtk_scrolled_window_new ();
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), GTK_WIDGET (tv));
  i = gtk_notebook_append_page (win->nb, scr, lab);
  nbp = gtk_notebook_get_page (win->nb, scr);
  g_object_set (nbp, "tab-expand", TRUE, NULL);
  gtk_notebook_set_current_page (win->nb, i);
}

/* end of instance method */

static void
window_close_alert_cb (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  GtkAlertDialog *dialog = GTK_ALERT_DIALOG (source_object);
  GtkWidget *win;
  GError *err = NULL;
  
  int response = gtk_alert_dialog_choose_finish (dialog, res, &err);

  if (err != NULL) {
      g_warning ("Alert Dialog Error: %s", err->message);
      g_error_free (err);
      return; 
  }

  win = GTK_WIDGET (user_data);
  if (response == 1) { /* Close Without Saving */
      g_idle_add_once ((GSourceOnceFunc) gtk_window_destroy, win);
  }
}

static gboolean
tfe_window_close_request (GtkWindow *window)
{
  TfeWindow *win = TFE_WINDOW (window);
  GtkWidget *scr;
  GtkWidget *tv;
  const char *tab_label;
  int n_pages = gtk_notebook_get_n_pages (win->nb);
  int unsaved_count = 0;
  GString *unsaved_list = g_string_new ("");
  GtkAlertDialog *dialog;
  const char *buttons[] = {"Cancel", "Close Window Without Saving", NULL};

  for (int i = 0; i < n_pages; i++) {
    scr = gtk_notebook_get_nth_page (win->nb, i);
    tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));

    if (tfe_text_view_get_modified (TFE_TEXT_VIEW (tv))) {
      unsaved_count++;
      tab_label = gtk_notebook_get_tab_label_text (win->nb, scr);
      if (tab_label != NULL) {
        g_string_append_printf (unsaved_list, "- %s\n", tab_label);
      } else {
        g_string_append_printf (unsaved_list, "- Page %d\n", i + 1);
      }
    }
  }

  if (unsaved_count == 0) {
    g_string_free (unsaved_list, TRUE);

    /* Chain up to the parent's close_request handler */
    return GTK_WINDOW_CLASS (tfe_window_parent_class)->close_request (window);
  }

  dialog = gtk_alert_dialog_new (
    unsaved_count == 1 ? "1 unsaved change" : "%d unsaved changes", 
    unsaved_count
  );
  gtk_alert_dialog_set_detail (dialog, unsaved_list->str);
  gtk_alert_dialog_set_buttons (dialog, buttons);
  gtk_alert_dialog_set_cancel_button (dialog, 0); 
  gtk_alert_dialog_set_default_button (dialog, 0); 
  gtk_alert_dialog_choose (dialog, GTK_WINDOW (win), NULL, window_close_alert_cb, win);

  g_string_free (unsaved_list, TRUE);
  g_object_unref (dialog);

  /* Stop the close request */
  return TRUE;
}

static void
tfe_window_dispose (GObject *gobject) {
  gtk_widget_dispose_template (GTK_WIDGET (gobject), TFE_TYPE_WINDOW);
  G_OBJECT_CLASS (tfe_window_parent_class)->dispose (gobject);
}

static void
tfe_window_init (TfeWindow *win) {
  GtkBuilder *builder;

  gtk_widget_init_template (GTK_WIDGET (win));

  builder = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/menus.ui");
  gtk_menu_button_set_menu_model (GTK_MENU_BUTTON (win->btnm), G_MENU_MODEL (gtk_builder_get_object (builder, "menu")));
  g_object_unref (builder);

/* ----- action ----- */
  const GActionEntry win_entries[] = {
    { "open", open_activated, NULL, NULL, NULL },
    { "save", save_activated, NULL, NULL, NULL },
    { "close", close_activated, NULL, NULL, NULL },
    { "new", new_activated, NULL, NULL, NULL },
    { "saveas", saveas_activated, NULL, NULL, NULL },
  };
  g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);
}

static void
tfe_window_class_init (TfeWindowClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWindowClass *window_class = GTK_WINDOW_CLASS (class);

  object_class->dispose = tfe_window_dispose;
  window_class->close_request = tfe_window_close_request;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfewindow.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, btnm);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, nb);
}

GtkWidget *
tfe_window_new (GtkApplication *app) {
  GtkWidget *win;

  win = GTK_WIDGET (g_object_new (TFE_TYPE_WINDOW, "application", app, NULL));
  tfe_window_append_page (TFE_WINDOW (win), NULL);
  return win;
}
