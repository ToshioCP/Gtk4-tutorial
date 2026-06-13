#include <gtk/gtk.h>
#include "tfewindow.h"
#include "tfepref.h"
#include "tfetextview.h"

struct _TfeWindow {
  GtkApplicationWindow parent;
  GtkMenuButton *btnm;
  GtkNotebook *nb;
  gboolean is_quit;
};

G_DEFINE_FINAL_TYPE (TfeWindow, tfe_window, GTK_TYPE_APPLICATION_WINDOW);

/* Private Utilities */

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

/* Public functions*/
void
tfe_window_append_page (TfeWindow *win, GFile *file) {
  g_return_if_fail(TFE_IS_WINDOW (win));
  g_return_if_fail(G_IS_FILE (file) || file == NULL);

  GtkWidget *tv;
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



/* The returned object is owned by the scrolled window. */
/* The caller won't get the ownership and mustn't release it. */
static TfeTextView *
get_current_textview (GtkNotebook *nb) {
  int i;
  GtkWidget *scr;
  GtkWidget *tv;

  i = gtk_notebook_get_current_page (nb);
  scr = gtk_notebook_get_nth_page (nb, i);
  tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
  return TFE_TEXT_VIEW (tv);
}

/* This call back is called when a TfeTextView instance emits a "change-file" signal. */
static void
file_changed_cb (TfeTextView *tv, gpointer user_data) {
  GtkNotebook *nb =  GTK_NOTEBOOK (user_data);
  GtkWidget *scr;
  GtkWidget *label;
  GFile *file;
  char *filename;

  file = tfe_text_view_get_file (tv);
  scr = gtk_widget_get_parent (GTK_WIDGET (tv));
  if (G_IS_FILE (file)) {
    filename = g_file_get_basename (file);
    g_object_unref (file);
  } else
    filename = get_untitled ();
  label = gtk_label_new (filename);
  g_free (filename);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (nb), scr, label);
}

static void
modified_changed_cb (GtkTextBuffer *tb, gpointer user_data) {
  TfeTextView *tv = TFE_TEXT_VIEW (user_data);
  GtkWidget *scr = gtk_widget_get_parent (GTK_WIDGET (tv));
  GtkWidget *nb =  gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_NOTEBOOK);
  GtkWidget *label;
  GFile *file;
  char *filename;
  char *text;

  file = tfe_text_view_get_file (tv);
  filename = g_file_get_basename (file);
  if (gtk_text_buffer_get_modified (tb))
    text = g_strdup_printf ("*%s", filename);
  else
    text = g_strdup (filename);
  g_object_unref (file);
  g_free (filename);
  label = gtk_label_new (text);
  g_free (text);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (nb), scr, label);
}

static gboolean
is_saved_all (GtkNotebook *nb) {
  int i, n;
  GtkWidget *scr;
  GtkWidget *tv;
  GtkTextBuffer *tb;

  n = gtk_notebook_get_n_pages (nb);
  for (i = 0; i < n; ++i) {
    scr = gtk_notebook_get_nth_page (nb, i);
    tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
    tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
    if (gtk_text_buffer_get_modified (tb))
      return FALSE;
  }
  return TRUE;
}

static void
notebook_page_close (TfeWindow *win){
  int i;

  if (gtk_notebook_get_n_pages (win->nb) == 1)
    gtk_window_destroy (GTK_WINDOW (win));
  else {
    i = gtk_notebook_get_current_page (win->nb);
    gtk_notebook_remove_page (win->nb, i);
  }
}

static void
notebook_page_build (TfeWindow *win, GtkWidget *tv, char *filename) {
  // The arguments win, tv and filename are owned by the caller.
  // If tv has a floating reference, it is consumed by the function.
  GtkWidget *scr = gtk_scrolled_window_new ();
  GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  GtkNotebookPage *nbp;
  GtkWidget *lab;
  int i;

  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
  lab = gtk_label_new (filename);
  i = gtk_notebook_append_page (win->nb, scr, lab);
  nbp = gtk_notebook_get_page (win->nb, scr);
  g_object_set (nbp, "tab-expand", TRUE, NULL);
  gtk_notebook_set_current_page (win->nb, i);
  g_signal_connect (tv, "change-file", G_CALLBACK (file_changed_cb), win->nb);
  g_signal_connect (tb, "modified-changed", G_CALLBACK (modified_changed_cb), tv);
}


/* alert response signal handler */
static void
alert_response_cb (TfeAlert *alert, int response_id, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  if (response_id == TFE_ALERT_RESPONSE_ACCEPT) {
    if (win->is_quit)
      gtk_window_destroy(GTK_WINDOW (win));
    else
      notebook_page_close (win);
  }
}

/* ----- Close request on the top window ----- */
/* ----- The signal is emitted when the close button is clicked. ----- */
static gboolean
close_request_cb (TfeWindow *win) {
  TfeAlert *alert;

  if (is_saved_all (win->nb))
    return FALSE;
  else {
    win->is_quit = TRUE;
    alert = TFE_ALERT (tfe_alert_new_with_data ("Are you sure?", "Contents aren't saved yet.\nAre you sure to quit?", "Quit"));
    gtk_window_set_transient_for (GTK_WINDOW (alert), GTK_WINDOW (win));
    g_signal_connect (alert, "response", G_CALLBACK (alert_response_cb), win);
    gtk_window_present (GTK_WINDOW (alert));
    return TRUE;
  }
}

/* ----- action activated handlers ----- */

static void
open_dialog_cb (GObject *source_object, GAsyncResult *res, gpointer data) {
  GtkFileDialog *dialog = GTK_FILE_DIALOG (source_object);
  TfeWindow *win = TFE_WINDOW (data);
  GFile *file;
  GError *err = NULL;

  if ((file = gtk_file_dialog_open_finish (dialog, res, &err)) == NULL) {
    if (!g_error_matches (err, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED)) {
      tfe_error_alert (GTK_WINDOW (win), err);
    }
    g_clear_error (&err);
    return;
  }
  tfe_window_append_page (win, file);
  g_object_unref (file);
}

static void
open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  dialog = gtk_file_dialog_new ();
  gtk_file_dialog_open (dialog, GTK_WINDOW (win), NULL, open_dialog_cb, win);
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
  lab = tfe_label_from_file (file); /* lab is floating. lab can be NULL */
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
save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  TfeTextView *tv = get_current_textview (win->nb);

  tfe_text_view_save (TFE_TEXT_VIEW (tv));
}

static void
close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  int i;
  GtkWidget *scr;
  TfeTextView *tv;
  GtkTextBuffer *tb;

  i = gtk_notebook_get_current_page (win->nb);
  scr = gtk_notebook_get_nth_page (win->nb, i);
  tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
  tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  if (! gtk_text_buffer_get_modified (tb)) /* is saved? */
    gtk_notebook_remove_page (win->nb, i);
  else
    show_unsaved_dialog_when_closing (win, i, tv);
}

static void
new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  tfe_window_notebook_page_new (win);
}

static void
saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  TfeTextView *tv = get_current_textview (win->nb);

  tfe_text_view_saveas (TFE_TEXT_VIEW (tv));
}

static void
pref_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  GtkWidget *pref;

  pref = tfe_pref_new ();
  gtk_window_set_transient_for (GTK_WINDOW (pref), GTK_WINDOW (win));
  gtk_window_present (GTK_WINDOW (pref));
}

static void
close_all_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  if (close_request_cb (win) == FALSE)
    gtk_window_destroy (GTK_WINDOW (win));
}


static void
tfe_window_dispose (GObject *gobject) {
  gtk_widget_dispose_template (GTK_WIDGET (gobject), TFE_TYPE_WINDOW);
  G_OBJECT_CLASS (tfe_window_parent_class)->dispose (gobject);
}

static void
tfe_window_init (TfeWindow *win) {
  GtkBuilder *build;
  GMenuModel *menu;

  gtk_widget_init_template (GTK_WIDGET (win));

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/menu.ui");
  menu = G_MENU_MODEL (gtk_builder_get_object (build, "menu"));
  gtk_menu_button_set_menu_model (win->btnm, menu);
  g_object_unref(build);

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

  g_signal_connect (win, "close-request", G_CALLBACK (close_request_cb), NULL);
}

static void
tfe_window_class_init (TfeWindowClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = tfe_window_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfewindow.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, btnm);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, nb);
}

GtkWidget *
tfe_window_new (GtkApplication *app) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_WINDOW, "application", app, NULL));
}
