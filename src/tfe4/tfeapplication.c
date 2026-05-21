#include <gtk/gtk.h>
#include "tfetextview.h"

/* ---------- end of the definition of TfeTextView ---------- */

static gboolean
before_close (GtkWindow *win, GtkWidget *tv) {
  /* release GFile in tv. */
  tfe_text_view_set_file (TFE_TEXT_VIEW (tv), NULL);
  return FALSE;
}

static void
app_activate (GApplication *app) {
  g_print ("You need to give filenames as arguments.\n");
}

static void
app_open (GApplication *app, GFile ** files, gint n_files, gchar *hint) {
  GtkWidget *win;
  GtkWidget *nb;
  GtkWidget *lab;
  GtkNotebookPage *nbp;
  GtkWidget *scr;
  GtkWidget *tv;
  GtkTextBuffer *tb;
  char *contents;
  gsize length;
  char *filename_bin, *filename_utf;
  int i;
  GError *err = NULL;
  GtkBuilder *build;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/tfe.ui");
  win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
  nb = GTK_WIDGET (gtk_builder_get_object (build, "nb"));
  g_object_unref (build);
  for (i = 0; i < n_files; i++) {
    if (g_file_load_contents (files[i], NULL, &contents, &length, NULL, &err)) {
      scr = gtk_scrolled_window_new ();
      tv = tfe_text_view_new ();
      tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
      gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
      gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);

      tfe_text_view_set_file (TFE_TEXT_VIEW (tv), files[i]);
      gtk_text_buffer_set_text (tb, contents, length);
      g_free (contents);
      filename_bin = g_file_get_basename (files[i]);
      filename_utf = g_filename_display_name (filename_bin);
      lab = gtk_label_new (filename_utf);
      g_free (filename_bin);
      g_free (filename_utf);
      gtk_notebook_append_page (GTK_NOTEBOOK (nb), scr, lab);
      nbp = gtk_notebook_get_page (GTK_NOTEBOOK (nb), scr);
      g_object_set (nbp, "tab-expand", TRUE, NULL);
    } else {
      g_printerr ("%s.\n", err->message);
      g_clear_error (&err);
    }
  }
  if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) > 0) {
    g_signal_connect (win, "close-request", G_CALLBACK (before_close), tv);
    gtk_window_present (GTK_WINDOW (win));
  } else
    gtk_window_destroy (GTK_WINDOW (win));
}

#define APPLICATION_ID "com.github.ToshioCP.tfe4"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_HANDLES_OPEN);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

