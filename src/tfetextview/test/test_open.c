#include <gtk/gtk.h>
#include "../tfetextview.h"

#define APPLICATION_ID "com.github.ToshioCP.testopen"

static void
open_response_cb (TfeTextView *tv, int response) {
  GFile *file = tfe_text_view_get_file (TFE_TEXT_VIEW (tv));
  char *filename = file ? g_file_get_basename (file) : NULL;
  g_printerr ("New file is %s.\n", filename);
  switch (response) {
  case TFE_OPEN_RESPONSE_SUCCESS:
    g_printerr ("Response is SUCCESS.\n");
    break;
  case TFE_OPEN_RESPONSE_CANCEL:
    g_printerr ("Response is CANCEL.\n");
    break;
  case TFE_OPEN_RESPONSE_ERROR:
    g_printerr ("Response is ERROR.\n");
    break;
  default:
    g_printerr ("Unexpected response.\n");
  }
}

static void
change_file_cb (TfeTextView *tv, int response) {
  g_printerr ("File is changed.\n");
}

/* ----- activate, open, startup handlers ----- */
static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win = gtk_application_window_new (app);
  GtkWidget *scr = gtk_scrolled_window_new ();
  GtkWidget *tv = tfe_text_view_new ();

  gtk_window_set_default_size (GTK_WINDOW (win), 800, 600);
  gtk_window_set_child (GTK_WINDOW (win), scr);
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);

  // tfe_text_view_open
  // It is assumed that FileChooserDialog works correctly.
  // The user (1) selects a file and clicks on "open"  or (2) clicks on "cancel".
  // - (1) && (tv->file == NULL) => 1. buffer is set with file contents, 2. tv->file is updated,
  //                                3. response signal with SUCCESS, 4. file-change signal
  // - (1) && (G_IS_FILE (tv->file)) => 1. buffer is set with file contents, 2. tv->file is updated,
  //                                3. response signal with SUCCESS, (4. file-change signal if file changed)
  // - (2) => response signal with CANCEL
  // The test program outputs logs with regards to above and the user confirms the behavior.
  // The test doesn't assert.
  gtk_widget_show (GTK_WIDGET (win));
  g_signal_connect (TFE_TEXT_VIEW (tv), "open-response", G_CALLBACK (open_response_cb), NULL);
  g_signal_connect (TFE_TEXT_VIEW (tv), "change-file", G_CALLBACK (change_file_cb), NULL);

  // open twice
  tfe_text_view_open (TFE_TEXT_VIEW (tv), GTK_WINDOW (win));
  tfe_text_view_open (TFE_TEXT_VIEW (tv), GTK_WINDOW (win));
}

/* ----- main ----- */
int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

