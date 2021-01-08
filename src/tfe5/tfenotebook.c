#include "tfe.h"

/* The returned string should be freed with g_free() when no longer needed. */
static gchar*
get_untitled () {
  static int c = -1;
  if (++c == 0) 
    return g_strdup_printf("Untitled");
  else
    return g_strdup_printf ("Untitled%u", c);
}

static void
file_changed (TfeTextView *tv, GtkNotebook *nb) {
  GFile *file;
  char *filename;
  GtkWidget *scr;
  GtkWidget *label;

  file = tfe_text_view_get_file (tv);
  scr = gtk_widget_get_parent (GTK_WIDGET (tv));
  if (G_IS_FILE (file))
    filename = g_file_get_basename (file);
  else
    filename = get_untitled ();
  label = gtk_label_new (filename);
  gtk_notebook_set_tab_label (nb, scr, label);
  g_object_unref (file);
  g_free (filename);
}

/* Save the contents in the current page */
void
notebook_page_save(GtkNotebook *nb) {
  gint i;
  GtkWidget *scr;
  GtkWidget *tv;

  i = gtk_notebook_get_current_page (nb);
  scr = gtk_notebook_get_nth_page (nb, i);
  tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
  tfe_text_view_save (TFE_TEXT_VIEW (tv));
}

static void
notebook_page_build (GtkNotebook *nb, GtkWidget *tv, char *filename) {
  GtkWidget *scr;
  GtkNotebookPage *nbp;
  GtkWidget *lab;
  gint i;
  scr = gtk_scrolled_window_new ();

  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
  lab = gtk_label_new (filename);
  i = gtk_notebook_append_page (nb, scr, lab);
  nbp = gtk_notebook_get_page (nb, scr);
  g_object_set (nbp, "tab-expand", TRUE, NULL);
  gtk_notebook_set_current_page (nb, i);
  g_signal_connect (GTK_TEXT_VIEW (tv), "change-file", G_CALLBACK (file_changed), nb);
}

static void
open_response (TfeTextView *tv, gint response, GtkNotebook *nb) {
  GFile *file;
  char *filename;

  if (response != TFE_OPEN_RESPONSE_SUCCESS) {
    g_object_ref_sink (tv);
    g_object_unref (tv);
  }else if (! G_IS_FILE (file = tfe_text_view_get_file (tv))) {
    g_object_ref_sink (tv);
    g_object_unref (tv);
  }else {
    filename = g_file_get_basename (file);
    g_object_unref (file);
    notebook_page_build (nb, GTK_WIDGET (tv), filename);
  }
}

void
notebook_page_open (GtkNotebook *nb) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));

  GtkWidget *tv;

  tv = tfe_text_view_new ();
  g_signal_connect (TFE_TEXT_VIEW (tv), "open-response", G_CALLBACK (open_response), nb);
  tfe_text_view_open (TFE_TEXT_VIEW (tv), gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW));
}

void
notebook_page_new_with_file (GtkNotebook *nb, GFile *file) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));
  g_return_if_fail(G_IS_FILE (file));

  GtkWidget *tv;
  char *filename;

  if ((tv = tfe_text_view_new_with_file (file)) == NULL)
    return; /* read error */
  filename = g_file_get_basename (file);
  notebook_page_build (nb, tv, filename);
}

void
notebook_page_new (GtkNotebook *nb) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));

  GtkWidget *tv;
  char *filename;

  tv = tfe_text_view_new ();
  filename = get_untitled ();
  notebook_page_build (nb, tv, filename);
}

