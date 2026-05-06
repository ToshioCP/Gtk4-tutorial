#include <gtk/gtk.h>

static void
app_activate (GApplication *app) {
  GtkWidget *win;
  GtkWidget *scr;
  GtkWidget *tv;
  GtkTextBuffer *tb;
  gchar *text;

  text =
      "Once upon a time, there lived an old man known as Taketori-no-Okina. "
      "His name, meaning \"The Bamboo Cutter,\" reflected his life's work of crafting bamboo baskets.\n"
      "One day, while deep in the hills, he came across a bamboo stalk that shimmered with a mysterious light. "
      "\"How strange and wondrous!\" he exclaimed. "
      "When he carefully cut it open, he found a tiny, beautiful baby girl nestled inside, glowing with a soft radiance. "
      "Believing she was a gift from the heavens, he brought her home.\n"
      "Though his wife was astonished by his tale, they were both filled with joy, for they had never been blessed with children of their own."
      ;
  win = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_title (GTK_WINDOW (win), "Taketori");
  gtk_window_set_default_size (GTK_WINDOW (win), 800, 600);

  scr = gtk_scrolled_window_new ();
  gtk_window_set_child (GTK_WINDOW (win), scr);

  tv = gtk_text_view_new ();
  tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  gtk_text_buffer_set_text (tb, text, -1);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);

  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);

  gtk_window_present (GTK_WINDOW (win));
}

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new ("com.github.ToshioCP.tfv2", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
