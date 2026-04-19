Up: [README.md](../README.md),  Prev: [Section 4](sec4.md), Next: [Section 6](sec6.md)

# Widgets (2)

## GtkTextView, GtkTextBuffer and GtkScrolledWindow

### GtkTextView and GtkTextBuffer

GtkTextView is a widget for multi-line text editing.
GtkTextBuffer is a text buffer which is connected to GtkTextView.
See the sample program `tfv1.c` below.

```c
#include <gtk/gtk.h>

static void
app_activate (GApplication *app) {
  GtkWidget *win;
  GtkWidget *tv;
  GtkTextBuffer *tb;
  gchar *text;

  text =
      "Once upon a time, there was an old man who was called Taketori-no-Okina. "
      "It is a Japanese word that means a man whose work is making bamboo baskets.\n"
      "One day, he went into a hill and found a shining bamboo. "
      "\"What a mysterious bamboo it is!,\" he said. "
      "He cut it, then there was a small cute baby girl in it. "
      "The girl was shining faintly. "
      "He thought this baby girl was a gift from Heaven and took her home.\n"
      "His wife was surprized at his story. "
      "They were very happy because they had no children. "
      ;
  win = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_title (GTK_WINDOW (win), "Taketori");
  gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);

  tv = gtk_text_view_new ();
  tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  gtk_text_buffer_set_text (tb, text, -1);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);

  gtk_window_set_child (GTK_WINDOW (win), tv);

  gtk_window_present (GTK_WINDOW (win));
}

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new ("com.github.ToshioCP.tfv1", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
```

Look at line 25.
A GtkTextView instance is created and its pointer is assigned to `tv`.
When the GtkTextView instance is created, a GtkTextBuffer instance is also created and connected to the GtkTextView automatically.
"GtkTextBuffer instance" will be referred to simply as "GtkTextBuffer" or "buffer".
In the next line, the pointer to the buffer is assigned to `tb`.
Then, the text from line 10 to 20 is assigned to the buffer.
If the third argument of `gtk_text_buffer_set_text` is a positive integer, it is the length of the text.
If it is -1, the string terminates with NULL.

GtkTextView has a wrap mode.
When it is set to `GTK_WRAP_WORD_CHAR`, text wraps in between words, or if that is not enough, also between graphemes.

Wrap mode is written in [Gtk\_WrapMode](https://docs.gtk.org/gtk4/enum.WrapMode.html) in the GTK 4 API document.

In line 30, `tv` is added to `win` as a child.

Now compile and run it.
If you've downloaded this repository, its pathname is `src/tfv/tfv1.c`.

```
$ cd src/tfv
$ comp tfv1
$ ./a.out
```

![GtkTextView](/src/images/screenshot_tfv1.png)

There's an I-beam pointer in the window.
You can add or delete any characters on the GtkTextView, and your changes are kept in the GtkTextBuffer.
If you add more characters beyond the limit of the window, the height increases and the window extends.
If the height gets bigger than the height of the screen,
you won't be able to control the size of the window or change it back to the original size.
This is essentially a bug.
This can be solved by adding a GtkScrolledWindow between the GtkApplicationWindow and GtkTextView.

### GtkScrolledWindow

What we need to do is:

- Create a GtkScrolledWindow and insert it as a child of the GtkApplicationWindow
- Insert the GtkTextView widget to the GtkScrolledWindow as a child.

Modify `tfv1.c` and save it as `tfv2.c`.
There are only a few differences between these two files.

```
$ cd tfv; diff tfv1.c tfv2.c
5a6
>   GtkWidget *scr;
12c13
<       "It is a Japanese word that means a man whose work is making bamboo baskets.\n"
---
>       "It is a japanese word that means a man whose work is making bamboo baskets.\n"
17c18
<       "He thought this baby girl was a gift from Heaven and took her home.\n"
---
>       "He thought this baby girl is a gift from Heaven and took her home.\n"
24a26,28
>   scr = gtk_scrolled_window_new ();
>   gtk_window_set_child (GTK_WINDOW (win), scr);
> 
30c34
<   gtk_window_set_child (GTK_WINDOW (win), tv);
---
>   gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
40c44
<   app = gtk_application_new ("com.github.ToshioCP.tfv1", G_APPLICATION_DEFAULT_FLAGS);
---
>   app = gtk_application_new ("com.github.ToshioCP.tfv2", G_APPLICATION_DEFAULT_FLAGS);
$
```

The whole code of `tfv2.c` is as follows.

```c
#include <gtk/gtk.h>

static void
app_activate (GApplication *app) {
  GtkWidget *win;
  GtkWidget *scr;
  GtkWidget *tv;
  GtkTextBuffer *tb;
  gchar *text;

  text =
      "Once upon a time, there was an old man who was called Taketori-no-Okina. "
      "It is a japanese word that means a man whose work is making bamboo baskets.\n"
      "One day, he went into a hill and found a shining bamboo. "
      "\"What a mysterious bamboo it is!,\" he said. "
      "He cut it, then there was a small cute baby girl in it. "
      "The girl was shining faintly. "
      "He thought this baby girl is a gift from Heaven and took her home.\n"
      "His wife was surprized at his story. "
      "They were very happy because they had no children. "
      ;
  win = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_title (GTK_WINDOW (win), "Taketori");
  gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);

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
```

Compile and run it.

Now, the window doesn't extend even if you type a lot of characters,
it just scrolls.

Up: [README.md](../README.md),  Prev: [Section 4](sec4.md), Next: [Section 6](sec6.md)
