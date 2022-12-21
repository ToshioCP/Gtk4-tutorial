#include <stdio.h>
#include <gtk/gtk.h>
#include "../tfetextview.h"

#define APPLICATION_ID "com.github.ToshioCP.testtfetextview"

// NOTICE!! -- Don't choose any existing file to save in FileChooserDialog (except 'yesy.txt')
// senario:
// setup: create test.txt
// create tv (TfeTextView) with test.txt => gtk_text_view_get_text => is the file correct?
// change the buffer with gtk_buffer_insert => save => check the file's contents
// saveas => is change-file signal emitted?
// teardown: remove test.txt

static GFile *
test_create_file (const char *filename, const char *sample) {
  FILE *fp = fopen (filename, "w");
  fprintf (fp, "%s", sample);
  fclose (fp);
  return g_file_new_for_path (filename);
}

static char *
test_read_file (const char *filename) {
  FILE *fp = fopen (filename, "r");
  int c;
  int length;
  char *s, *t;

  // get file size (length)
  for (length=0; (c = fgetc (fp)) != EOF; ++length)
    ;
  t = s = g_new (char, length + 1);
  fseek (fp, 0, SEEK_SET); // reset the file pointer
  while ((c = fgetc (fp)) != EOF)
    *t++ = (char) c;
  *t = '\0';
  fclose (fp);
  return s;
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
  GtkWidget *tv;
  char *s;

  gtk_window_set_default_size (GTK_WINDOW (win), 800, 600);
  gtk_window_set_child (GTK_WINDOW (win), scr);

  // test for tfe_text_view_new_with_file and tfe_text_view_get_file
  GFile *file = test_create_file ("test.txt", "I wasn't the outdoor type in my childhood.\n");
  tv = tfe_text_view_new_with_file (file);
  g_object_ref_sink (tv);
  GFile *file2 = tfe_text_view_get_file (TFE_TEXT_VIEW (tv));
  if (! g_file_equal (file, file2)) {
    g_printerr ("tfe_text_view_get_file didn't return the expected GFile.\n");
    g_printerr ("Expected: %s, actual: %s", g_file_get_basename (file), g_file_get_basename(file2));
  }
  g_object_unref (file2);

  // tfe_text_view_save
  GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  GtkTextIter iter;
  g_signal_connect (TFE_TEXT_VIEW (tv), "change-file", G_CALLBACK (change_file_cb), NULL);
  gtk_text_buffer_get_end_iter (tb, &iter);
  gtk_text_buffer_insert (tb, &iter, "My mom always got me to play outside.\n", -1);
  tfe_text_view_save (TFE_TEXT_VIEW (tv));
  s = test_read_file ("test.txt");
  char *expected_s = "I wasn't the outdoor type in my childhood.\n"
                     "My mom always got me to play outside.\n";
  if (strcmp (expected_s, s) != 0) {
    g_printerr ("tfe_text_view_save didn't work.\n");
    g_printerr ("Expected:\n%s\nactual:\n%s\n", expected_s, s);
  }
  g_free (s);

  // tfe_text_view_saveas
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
  gtk_widget_show (win);
  tfe_text_view_saveas (TFE_TEXT_VIEW (tv));
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

