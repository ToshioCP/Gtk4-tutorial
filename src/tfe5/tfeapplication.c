#include "tfe.h"

static void
open_clicked (GtkWidget *btno, GtkNotebook *nb) {
  notebook_page_open (nb);
}

static void
new_clicked (GtkWidget *btnn, GtkNotebook *nb) {
  notebook_page_new (nb);
}

static void
save_clicked (GtkWidget *btns, GtkNotebook *nb) {
  notebook_page_save (nb);
}

static void
close_clicked (GtkWidget *btnc, GtkNotebook *nb) {
  GtkWidget *win;
  GtkWidget *boxv;
  gint i;

  if (gtk_notebook_get_n_pages (nb) == 1) {
    boxv = gtk_widget_get_parent (GTK_WIDGET (nb));
    win = gtk_widget_get_parent (boxv);
    gtk_window_destroy (GTK_WINDOW (win));
  } else {
    i = gtk_notebook_get_current_page (nb);
    gtk_notebook_remove_page (GTK_NOTEBOOK (nb), i);
  }
}

static void
tfe_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win;
  GtkWidget *boxv;
  GtkNotebook *nb;

  win = GTK_WIDGET (gtk_application_get_active_window (app));
  boxv = gtk_window_get_child (GTK_WINDOW (win));
  nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));

  notebook_page_new (nb);
  gtk_widget_show (GTK_WIDGET (win));
}

static void
tfe_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win;
  GtkWidget *boxv;
  GtkNotebook *nb;
  int i;

  win = GTK_WIDGET (gtk_application_get_active_window (app));
  boxv = gtk_window_get_child (GTK_WINDOW (win));
  nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));

  for (i = 0; i < n_files; i++)
    notebook_page_new_with_file (nb, files[i]);
  if (gtk_notebook_get_n_pages (nb) == 0)
    notebook_page_new (nb);
  gtk_widget_show (win);
}


static void
tfe_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkApplicationWindow *win;
  GtkNotebook *nb;
  GtkBuilder *build;
  GtkButton *btno;
  GtkButton *btnn;
  GtkButton *btns;
  GtkButton *btnc;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/tfe.ui");
  win = GTK_APPLICATION_WINDOW (gtk_builder_get_object (build, "win"));
  nb = GTK_NOTEBOOK (gtk_builder_get_object (build, "nb"));
  gtk_window_set_application (GTK_WINDOW (win), app);
  btno = GTK_BUTTON (gtk_builder_get_object (build, "btno"));
  btnn = GTK_BUTTON (gtk_builder_get_object (build, "btnn"));
  btns = GTK_BUTTON (gtk_builder_get_object (build, "btns"));
  btnc = GTK_BUTTON (gtk_builder_get_object (build, "btnc"));
  g_signal_connect (btno, "clicked", G_CALLBACK (open_clicked), nb);
  g_signal_connect (btnn, "clicked", G_CALLBACK (new_clicked), nb);
  g_signal_connect (btns, "clicked", G_CALLBACK (save_clicked), nb);
  g_signal_connect (btnc, "clicked", G_CALLBACK (close_clicked), nb);
  g_object_unref(build);

GdkDisplay *display;

  display = gtk_widget_get_display (GTK_WIDGET (win));
  GtkCssProvider *provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (provider, "textview {padding: 10px; font-family: monospace; font-size: 12pt;}", -1);
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new ("com.github.ToshioCP.tfe", G_APPLICATION_HANDLES_OPEN);

  g_signal_connect (app, "startup", G_CALLBACK (tfe_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (tfe_activate), NULL);
  g_signal_connect (app, "open", G_CALLBACK (tfe_open), NULL);

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

