#include <gtk/gtk.h>

#include "../tfetextview/tfetextview.h"
#include "tfenotebook.h"

static void
open_cb (GtkNotebook *nb) {
  notebook_page_open (nb);
}

static void
new_cb (GtkNotebook *nb) {
  notebook_page_new (nb);
}

static void
save_cb (GtkNotebook *nb) {
  notebook_page_save (nb);
}

static void
close_cb (GtkNotebook *nb) {
  notebook_page_close (GTK_NOTEBOOK (nb));
}

static void
before_destroy (GtkWidget *win, GtkCssProvider *provider) {
  GdkDisplay *display = gdk_display_get_default ();
  gtk_style_context_remove_provider_for_display (display, GTK_STYLE_PROVIDER (provider));
}

static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
  GtkWidget *boxv = gtk_window_get_child (GTK_WINDOW (win));
  GtkNotebook *nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));

  notebook_page_new (nb);
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
  GtkWidget *boxv = gtk_window_get_child (GTK_WINDOW (win));
  GtkNotebook *nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));
  int i;

  for (i = 0; i < n_files; i++)
    notebook_page_new_with_file (nb, files[i]);
  if (gtk_notebook_get_n_pages (nb) == 0)
    notebook_page_new (nb);
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkBuilder *build;
  GtkApplicationWindow *win;
  GtkNotebook *nb;
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
  g_signal_connect_swapped (btno, "clicked", G_CALLBACK (open_cb), nb);
  g_signal_connect_swapped (btnn, "clicked", G_CALLBACK (new_cb), nb);
  g_signal_connect_swapped (btns, "clicked", G_CALLBACK (save_cb), nb);
  g_signal_connect_swapped (btnc, "clicked", G_CALLBACK (close_cb), nb);
  g_object_unref(build);

GdkDisplay *display;

  display = gdk_display_get_default ();
  GtkCssProvider *provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (provider, "textview {padding: 10px; font-family: monospace; font-size: 12pt;}", -1);
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  g_signal_connect (win, "destroy", G_CALLBACK (before_destroy), provider);
  g_object_unref (provider);
}

#define APPLICATION_ID "com.github.ToshioCP.tfe"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_HANDLES_OPEN);

  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
