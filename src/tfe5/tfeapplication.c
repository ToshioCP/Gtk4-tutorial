#include <gtk/gtk.h>
#include "tfetextview.h"

/* ----- Private utility functions ----- */
static GtkWidget *
tfe_get_notebook (GtkWidget *win) {
  GtkWidget *boxv = gtk_window_get_child (GTK_WINDOW (win));
  return gtk_widget_get_last_child (boxv);
}

static void
tfe_error_alert (GtkWindow *win, GError *err) {
  GtkAlertDialog *alert_dialog;

  alert_dialog = gtk_alert_dialog_new ("%s", err->message);
  gtk_alert_dialog_show (alert_dialog, win);
  g_object_unref (alert_dialog);
}

static void
notebook_page_build (GtkNotebook *nb, TfeTextView *tv, GFile *file) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));
  g_return_if_fail(TFE_IS_TEXT_VIEW (tv));
  g_return_if_fail(G_IS_FILE (file) || file == NULL);

  GtkWidget *scr = gtk_scrolled_window_new ();
  GtkNotebookPage *nbp;
  GtkWidget *lab;
  char *filename_bin, *filename_utf;
  int i;

  if (file) {
    filename_bin = g_file_get_basename (file);
    filename_utf = g_filename_display_name (filename_bin);
    lab = gtk_label_new (filename_utf);
    g_free (filename_bin);
    g_free (filename_utf);
  } else
    lab = NULL;

  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), GTK_WIDGET (tv));
  i = gtk_notebook_append_page (nb, scr, lab);
  nbp = gtk_notebook_get_page (nb, scr);
  g_object_set (nbp, "tab-expand", TRUE, NULL);
  gtk_notebook_set_current_page (nb, i);
}

static void
notebook_page_new_with_file (GtkNotebook *nb, GFile *file) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));
  g_return_if_fail(G_IS_FILE (file) || file == NULL);

  GtkWidget *tv;
  GtkWidget *win;
  GError *err = NULL;

  if (file) {
    if ((tv = tfe_text_view_new_with_file (file, &err)) == NULL) {
      win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);
      tfe_error_alert (GTK_WINDOW (win), err);
      g_clear_error (&err);
      return;
    }
  } else
    tv = tfe_text_view_new ();

  notebook_page_build (nb, TFE_TEXT_VIEW (tv), file);
}

/* ----- End of private utility functions ----- */

static void
open_dialog_cb (GObject *source_object, GAsyncResult *res, gpointer data) {
  GtkFileDialog *dialog = GTK_FILE_DIALOG (source_object);
  GtkNotebook *nb = GTK_NOTEBOOK (data);
  TfeTextView *tv;
  GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);
  GFile *file;
  GError *err = NULL;

  if ((file = gtk_file_dialog_open_finish (dialog, res, &err)) == NULL) {
    tfe_error_alert (GTK_WINDOW (win), err);
    g_clear_error (&err);
    return;
  }
  tv = TFE_TEXT_VIEW (tfe_text_view_new ());
  tfe_text_view_set_file (tv, file);
  if (! tfe_text_view_read (tv, &err)) {
    tfe_error_alert (GTK_WINDOW (win), err);
    g_clear_error (&err);
    g_object_ref_sink (tv);
    g_object_unref (tv);
    return;
  }
  notebook_page_build (nb, tv, file);
  g_object_unref(file);
}

static void
open_cb (GtkNotebook *nb) {
  GtkFileDialog *dialog;
  GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);

  dialog = gtk_file_dialog_new ();
  gtk_file_dialog_open (dialog, GTK_WINDOW (win), NULL, open_dialog_cb, nb);
  g_object_unref (dialog);
}

static void
new_cb (GtkNotebook *nb) {
  notebook_page_new_with_file (GTK_NOTEBOOK (nb), NULL);
}

static void
save_dialog_cb (GObject *source_object, GAsyncResult *res, gpointer data) {
  GtkFileDialog *dialog;
  GtkWidget *tv;
  GtkWidget *win;
  GtkWidget *scr;
  GtkWidget *nb;
  GFile *file;
  GError *err = NULL;
  char *filename_bin;
  char *filename_utf;

  dialog = GTK_FILE_DIALOG (source_object);
  tv = GTK_WIDGET (data);
  win = gtk_widget_get_ancestor (tv, GTK_TYPE_WINDOW);

  if ((file = gtk_file_dialog_save_finish (dialog, res, &err)) == NULL) {
    if (!g_error_matches (err, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED)) {
      tfe_error_alert (GTK_WINDOW (win), err);
    }
    g_clear_error (&err);
    return;
  }

  tfe_text_view_set_file (TFE_TEXT_VIEW (tv), file);
  
  filename_bin = g_file_get_basename (file);
  filename_utf = g_filename_display_name (filename_bin);
  
  scr = gtk_widget_get_parent (tv);
  nb = gtk_widget_get_ancestor (tv, GTK_TYPE_NOTEBOOK);
  gtk_notebook_set_tab_label_text (GTK_NOTEBOOK (nb), scr, filename_utf);
  
  g_free (filename_bin);
  g_free (filename_utf);
  g_object_unref (file);

  if (! tfe_text_view_write (TFE_TEXT_VIEW (tv), &err)) {
    tfe_error_alert (GTK_WINDOW (win), err);
    g_clear_error (&err);
  }
}

static void
save_cb (GtkNotebook *nb) {
  int i;
  GtkWidget *win;
  GtkWidget *scr;
  GtkWidget *tv;
  GError *err = NULL;
  GtkFileDialog *dialog;
  GtkAlertDialog *alert_dialog;

  i = gtk_notebook_get_current_page (nb);
  win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);

  if (i == -1) {
    alert_dialog = gtk_alert_dialog_new ("No page to save.");
    gtk_alert_dialog_show (alert_dialog, GTK_WINDOW (win));
    g_object_unref (alert_dialog);
    return;
  }

  scr = gtk_notebook_get_nth_page (nb, i);
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
close_cb (GtkNotebook *nb) {
  if (gtk_notebook_get_n_pages (nb) >= 2)
    gtk_notebook_remove_page (nb, gtk_notebook_get_current_page (nb));
  else  /* If the page is the last one or no page exists */
    gtk_window_destroy (GTK_WINDOW (gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW)));
}

static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
  GtkWidget *nb = tfe_get_notebook (win);

  notebook_page_new_with_file (GTK_NOTEBOOK (nb), NULL);
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
  GtkWidget *nb = tfe_get_notebook (win);
  int i;

  for (i = 0; i < n_files; i++)
    notebook_page_new_with_file (GTK_NOTEBOOK (nb), files[i]);
  if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) == 0) /* No files have been opened */
    notebook_page_new_with_file (GTK_NOTEBOOK (nb), NULL);
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkBuilder *build;
  GtkWidget *win;
  GtkWidget *nb;
  GtkButton *btno;
  GtkButton *btnn;
  GtkButton *btns;
  GtkButton *btnc;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe5/tfe.ui");
  win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  gtk_window_set_application (GTK_WINDOW (win), app);
  nb = GTK_WIDGET (gtk_builder_get_object (build, "nb"));
  btno = GTK_BUTTON (gtk_builder_get_object (build, "btno"));
  btnn = GTK_BUTTON (gtk_builder_get_object (build, "btnn"));
  btns = GTK_BUTTON (gtk_builder_get_object (build, "btns"));
  btnc = GTK_BUTTON (gtk_builder_get_object (build, "btnc"));
  g_signal_connect_swapped (btno, "clicked", G_CALLBACK (open_cb), nb);
  g_signal_connect_swapped (btnn, "clicked", G_CALLBACK (new_cb), nb);
  g_signal_connect_swapped (btns, "clicked", G_CALLBACK (save_cb), nb);
  g_signal_connect_swapped (btnc, "clicked", G_CALLBACK (close_cb), nb);
  g_object_unref(build);
}

#define APPLICATION_ID "com.github.ToshioCP.tfe5"

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
