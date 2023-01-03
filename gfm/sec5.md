Up: [README.md](../README.md),  Prev: [Section 4](sec4.md), Next: [Section 6](sec6.md)

# Widgets (2)

## GtkTextView, GtkTextBuffer and GtkScrolledWindow

### GtkTextView and GtkTextBuffer

GtkTextView is a widget for multi-line text editing.
GtkTextBuffer is a text buffer which is connected to GtkTextView.
See the sample program `tfv1.c` below.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 app_activate (GApplication *app) {
 5   GtkWidget *win;
 6   GtkWidget *tv;
 7   GtkTextBuffer *tb;
 8   gchar *text;
 9 
10   text =
11       "Once upon a time, there was an old man who was called Taketori-no-Okina. "
12       "It is a japanese word that means a man whose work is making bamboo baskets.\n"
13       "One day, he went into a mountain and found a shining bamboo. "
14       "\"What a mysterious bamboo it is!,\" he said. "
15       "He cut it, then there was a small cute baby girl in it. "
16       "The girl was shining faintly. "
17       "He thought this baby girl is a gift from Heaven and took her home.\n"
18       "His wife was surprized at his story. "
19       "They were very happy because they had no children. "
20       ;
21   win = gtk_application_window_new (GTK_APPLICATION (app));
22   gtk_window_set_title (GTK_WINDOW (win), "Taketori");
23   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
24 
25   tv = gtk_text_view_new ();
26   tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
27   gtk_text_buffer_set_text (tb, text, -1);
28   gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
29 
30   gtk_window_set_child (GTK_WINDOW (win), tv);
31 
32   gtk_window_present (GTK_WINDOW (win));
33 }
34 
35 int
36 main (int argc, char **argv) {
37   GtkApplication *app;
38   int stat;
39 
40   app = gtk_application_new ("com.github.ToshioCP.tfv1", G_APPLICATION_DEFAULT_FLAGS);
41   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
42   stat = g_application_run (G_APPLICATION (app), argc, argv);
43   g_object_unref (app);
44   return stat;
45 }
~~~

Look at line 25.
A GtkTextView instance is created and its pointer is assigned to `tv`.
When the GtkTextView instance is created, a GtkTextBuffer instance is also created and connected to the GtkTextView automatically.
"GtkTextBuffer instance" will be referred to simply as "GtkTextBuffer" or "buffer".
In the next line, the pointer to the buffer is assigned to `tb`.
Then, the text from line 10 to 20 is assigned to the buffer.
If the third argument of `gtk_text_buffer_set_text` is a positive integer, it is the length of the text.
It it is -1, the string terminates with NULL.

GtkTextView has a wrap mode.
When it is set to `GTK_WRAP_WORD_CHAR`, text wraps in between words, or if that is not enough, also between graphemes.

Wrap mode is written in [Gtk\_WrapMode](https://docs.gtk.org/gtk4/enum.WrapMode.html) in the GTK 4 API document.

In line 30, `tv` is added to `win` as a child.

Now compile and run it.

```
$ cd src/tfv
$ comp tfv1
$ ./a.out
```

![GtkTextView](../image/screenshot_tfv1.png)

There's an I-beam pointer in the window.
You can add or delete any characters on the GtkTextView,
and your changes are kept in the GtkTextBuffer.
If you add more characters beyond the limit of the window, the height increases and the window extends.
If the height gets bigger than the height of the display screen, you won't be
able to control the size of the window or change it back to the original size.
This is a problem, that is to say a bug.
This can be solved by adding a GtkScrolledWindow between the GtkApplicationWindow and GtkTextView.

### GtkScrolledWindow

What we need to do is:

- Create a GtkScrolledWindow and insert it as a child of the GtkApplicationWindow
- Insert the GtkTextView widget to the GtkScrolledWindow as a child.

Modify `tfv1.c` and save it as `tfv2.c`.
There is only a few difference between these two files.

~~~
$ cd tfv; diff tfv1.c tfv2.c
5a6
>   GtkWidget *scr;
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
~~~

The whole code of `tfv2.c` is as follows.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 app_activate (GApplication *app) {
 5   GtkWidget *win;
 6   GtkWidget *scr;
 7   GtkWidget *tv;
 8   GtkTextBuffer *tb;
 9   gchar *text;
10 
11   text =
12       "Once upon a time, there was an old man who was called Taketori-no-Okina. "
13       "It is a japanese word that means a man whose work is making bamboo baskets.\n"
14       "One day, he went into a mountain and found a shining bamboo. "
15       "\"What a mysterious bamboo it is!,\" he said. "
16       "He cut it, then there was a small cute baby girl in it. "
17       "The girl was shining faintly. "
18       "He thought this baby girl is a gift from Heaven and took her home.\n"
19       "His wife was surprized at his story. "
20       "They were very happy because they had no children. "
21       ;
22   win = gtk_application_window_new (GTK_APPLICATION (app));
23   gtk_window_set_title (GTK_WINDOW (win), "Taketori");
24   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
25 
26   scr = gtk_scrolled_window_new ();
27   gtk_window_set_child (GTK_WINDOW (win), scr);
28 
29   tv = gtk_text_view_new ();
30   tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
31   gtk_text_buffer_set_text (tb, text, -1);
32   gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
33 
34   gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
35 
36   gtk_window_present (GTK_WINDOW (win));
37 }
38 
39 int
40 main (int argc, char **argv) {
41   GtkApplication *app;
42   int stat;
43 
44   app = gtk_application_new ("com.github.ToshioCP.tfv2", G_APPLICATION_DEFAULT_FLAGS);
45   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
46   stat = g_application_run (G_APPLICATION (app), argc, argv);
47   g_object_unref (app);
48   return stat;
49 }
~~~

Compile and run it.

Now, the window doesn't extend even if you type a lot of characters,
it just scrolls.

Up: [README.md](../README.md),  Prev: [Section 4](sec4.md), Next: [Section 6](sec6.md)
