#include <gtk/gtk.h>
#include "resources.c"

static void
app_activate (GApplication *app) {
  GtkWidget *win;
  GtkWidget *nb;
  GtkWidget *scr;
  GtkNotebookPage *nbp;
  int n_pages;
  GtkBuilder *build;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe3/tfe3.ui");
  win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  nb = GTK_WIDGET (gtk_builder_get_object (build, "nb"));
  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
  g_object_unref(build);

  n_pages = gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb));
  for (int i = 0; i < n_pages; i++) {
    scr = gtk_notebook_get_nth_page (GTK_NOTEBOOK (nb), i);
    nbp = gtk_notebook_get_page (GTK_NOTEBOOK (nb), scr);
    g_object_set (nbp, "tab-expand", TRUE, NULL);
  }

  gtk_window_present (GTK_WINDOW (win));
}

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new ("com.github.ToshioCP.tfe3", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

