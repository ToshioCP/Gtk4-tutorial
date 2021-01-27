Up: [Readme.md](../Readme.md),  Prev: [Section 5](sec5.md), Next: [Section 7](sec7.md)

# Widgets (3)

## Open signal

### G\_APPLICATION\_HANDLES\_OPEN flag

GtkTextView, GtkTextBuffer and GtkScrolledWindow have given us a minimum editor in the previous section.
Next, we will add a read function to this program and remake it into a file viewer.
There are many ways to implement the function.
However, because this is a tutorial for beginners, we'll take the easiest one.

When the program starts, we give a filename as an argument.

    $ ./a.out filename

Then it opens the file and set it into GtkTextBuffer.

At the beginning of the implementation, we need to know how GtkApplication (or GApplication) recognizes arguments.
It is described in the GIO API reference.

When GtkApplication is generated, a flag (its type is GApplicationFlags) is given as an argument.

~~~C
GtkApplication *
gtk_application_new (const gchar *application_id, GApplicationFlags flags);
~~~

This flag is described in the GApplication section in GIO API reference.

~~~C
GApplicationFlags' Members

G_APPLICATION_FLAGS_NONE  Default. (No argument allowed)
  ... ... ...
G_APPLICATION_HANDLES_OPEN  This application handles opening files (in the primary instance).
  ... ... ...
~~~

There are ten flags.
But we only need two of them so far.
We've already used `G_APPLICATION_FLAGS_NONE`.
It is the simplest option.
No argument is allowed.
If you give arguments and run the application, then error occurs.

`G_APPLICATION_HANDLES_OPEN` is the second simplest option.
It allows arguments but only files.
The application assumes all the arguments are filenames.

Now we use this flag when generating GtkApplication.

~~~C
app = gtk_application_new ("com.github.ToshioCP.tfv3", G_APPLICATION_HANDLES_OPEN);
~~~

### open signal

When the application starts, two signals are possible.

- activate signal --- This signal is emitted when there's no argument.
- open signal --- This signal is emitted when there is at least one argument.

The handler of open signal is called as follows.

~~~C
void user_function (GApplication *application,
                   gpointer      files,
                   gint          n_files,
                   gchar        *hint,
                   gpointer      user_data)
~~~

The parameters are as follows:

- application --- the application (usually GtkApplication)
- files --- an array of GFiles. [array length=n\_files] [element-type GFile]
- n\_files --- the number of files
- hint --- a hint provided by the calling instance (usually it can be ignored)
- user\_data --- user data set when the signal handler was connected.

The way how to read a file using GFiles will be described in the next section.

## Coding a file viewer

### What is a file viewer?

A file viewer is a program that shows a text file given as an argument.
It works as follows.

- If it is given arguments, it recognizes the first argument as a filename and open it.
- If opening the file succeeds, read and set it to GtkTextBuffer and show the window.
- If it fails to open the file, show an error message and quit.
- If there's no argument, show an error message and quit.
- If there are two or more arguments, the second one and after are ignored.

The program is as follows.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 on_activate (GApplication *app, gpointer user_data) {
 5   g_print ("You need a filename argument.\n");
 6 }
 7 
 8 static void
 9 on_open (GApplication *app, GFile ** files, gint n_files, gchar *hint, gpointer user_data) {
10   GtkWidget *win;
11   GtkWidget *scr;
12   GtkWidget *tv;
13   GtkTextBuffer *tb;
14   char *contents;
15   gsize length;
16   char *filename;
17 
18   win = gtk_application_window_new (GTK_APPLICATION (app));
19   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
20 
21   scr = gtk_scrolled_window_new ();
22   gtk_window_set_child (GTK_WINDOW (win), scr);
23 
24   tv = gtk_text_view_new ();
25   tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
26   gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
27   gtk_text_view_set_editable (GTK_TEXT_VIEW (tv), FALSE);
28   gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
29 
30   if (g_file_load_contents (files[0], NULL, &contents, &length, NULL, NULL)) {
31     gtk_text_buffer_set_text (tb, contents, length);
32     g_free (contents);
33     filename = g_file_get_basename (files[0]);
34     gtk_window_set_title (GTK_WINDOW (win), filename);
35     g_free (filename);
36     gtk_widget_show (win);
37   } else {
38     filename = g_file_get_path (files[0]);
39     g_print ("No such file: %s.\n", filename);
40     gtk_window_destroy (GTK_WINDOW (win));
41   }
42 }
43 
44 int
45 main (int argc, char **argv) {
46   GtkApplication *app;
47   int stat;
48 
49   app = gtk_application_new ("com.github.ToshioCP.tfv3", G_APPLICATION_HANDLES_OPEN);
50   g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
51   g_signal_connect (app, "open", G_CALLBACK (on_open), NULL);
52   stat =g_application_run (G_APPLICATION (app), argc, argv);
53   g_object_unref (app);
54   return stat;
55 }
56 
~~~

Save it as `tfv3.c`.
Then compile and run it.

    $ comp tfv3
    $ ./a.out tfv3.c

![File viewer](../image/screenshot_tfv3.png)

Now I want to explain the program `tfv3.c`.
First, the function `main` changes in only two lines.

- `G_APPLICATION_FLAGS_NONE` is replaced with `G_APPLICATION_HANDLES_OPEN`.
- `g_signal_connect (app, "open", G_CALLBACK (on_open), NULL)` is added.

Next, the handler `on_activate` is now very simple.
Just output the error message.
The application quits immediately because no window is generated.

The point is the handler `on_open`.

- It generates GtkApplicationWindow, GtkScrolledWindow, GtkTextView and GtkTextBuffer and connects them.
- Set wrap mode to `GTK_WRAP_WORD_CHAR` in GtktextView.
- Set non-editable to GtkTextView because the program isn't an editor but only a viewer.
- Read the file and set it to GtkTextBuffer (this will be explained in detail later).
- If the file is not opened then output an error message and destroy the window. It makes the application quit.

The file reading part of the program is shown again below.

~~~C
if (g_file_load_contents(files[0], NULL, &contents, &length, NULL, NULL)) {
  gtk_text_buffer_set_text(tb, contents, length);
  g_free(contents);
  filename = g_file_get_basename(files[0]);
  gtk_window_set_title (GTK_WINDOW (win), filename);
  g_free(filename);
  gtk_widget_show (win);
} else {
  filename = g_file_get_path(files[0]);
  g_print ("No such file: %s.\n", filename);
  gtk_window_destroy (GTK_WINDOW (win));
}
~~~

The function `g_file_load_contents` loads the file contents into a buffer, which is automatically allocated, and set the pointer to the buffer into `contents`.
And the length of the buffer is set to `length`.
It returns `TRUE` if the file's contents were successfully loaded. `FALSE` if there were errors.

If the function succeeds, set the contents into GtkTextBuffer, free the buffer memories pointed by `contents`, set the filename to the title of the window,
free the memories pointed by `filename` and show the window.
If it fails, it outputs an error message and destroys the window.

## GtkNotebook

GtkNotebook is a container widget that contains multiple children with tabs in it.

![GtkNotebook](../image/screenshot_gtk_notebook.png)

Look at the screenshots above.
The left one is a window at the startup.
It shows the file `pr1.c`.
The filename is in the left tab.
After clicking on the right tab, then the contents of `tfv1.c` appears.
It is shown in the right screenshot.

GtkNotebook widget is between GtkApplicationWindow and GtkScrolledWindow.
Now I want to show you the program `tfv4.c`.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 on_activate (GApplication *app, gpointer user_data) {
 5   g_print ("You need a filename argument.\n");
 6 }
 7 
 8 static void
 9 on_open (GApplication *app, GFile ** files, gint n_files, gchar *hint, gpointer user_data) {
10   GtkWidget *win;
11   GtkWidget *nb;
12   GtkWidget *lab;
13   GtkNotebookPage *nbp;
14   GtkWidget *scr;
15   GtkWidget *tv;
16   GtkTextBuffer *tb;
17   char *contents;
18   gsize length;
19   char *filename;
20   int i;
21 
22   win = gtk_application_window_new (GTK_APPLICATION (app));
23   gtk_window_set_title (GTK_WINDOW (win), "file viewer");
24   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
25   gtk_window_maximize (GTK_WINDOW (win));
26 
27   nb = gtk_notebook_new ();
28   gtk_window_set_child (GTK_WINDOW (win), nb);
29 
30   for (i = 0; i < n_files; i++) {
31     if (g_file_load_contents (files[i], NULL, &contents, &length, NULL, NULL)) {
32       scr = gtk_scrolled_window_new ();
33       tv = gtk_text_view_new ();
34       tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
35       gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
36       gtk_text_view_set_editable (GTK_TEXT_VIEW (tv), FALSE);
37       gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
38 
39       gtk_text_buffer_set_text (tb, contents, length);
40       g_free (contents);
41       filename = g_file_get_basename (files[i]);
42       lab = gtk_label_new (filename);
43       gtk_notebook_append_page (GTK_NOTEBOOK (nb), scr, lab);
44       nbp = gtk_notebook_get_page (GTK_NOTEBOOK (nb), scr);
45       g_object_set (nbp, "tab-expand", TRUE, NULL);
46       g_free (filename);
47     } else {
48       filename = g_file_get_path (files[i]);
49       g_print ("No such file: %s.\n", filename);
50       g_free (filename);
51     }
52   }
53   if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) > 0)
54     gtk_widget_show (win);
55   else
56     gtk_window_destroy (GTK_WINDOW (win));
57 }
58 
59 int
60 main (int argc, char **argv) {
61   GtkApplication *app;
62   int stat;
63 
64   app = gtk_application_new ("com.github.ToshioCP.tfv4", G_APPLICATION_HANDLES_OPEN);
65   g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
66   g_signal_connect (app, "open", G_CALLBACK (on_open), NULL);
67   stat =g_application_run (G_APPLICATION (app), argc, argv);
68   g_object_unref (app);
69   return stat;
70 }
71 
~~~

Most of the change is in the function `on_open`.
The numbers at the left of the following items are line numbers in the source code.

- 11-13: Variables `nb`, `lab` and `nbp` are defined and point GtkNotebook, GtkLabel and GtkNotebookPage respectively.
- 23: The window's title is set to "file viewer".
- 25: The size of the window is set to maximum because a big window is appropriate for file viewers.
- 27-28 GtkNotebook is generated and set it as a child of the GtkApplicationWindow.
- 30-52 For-loop. Each loop corresponds to an argument. And files[i] is GFile object with respect to the i-th argument.
- 32-37 GtkScrollledWindow, GtkTextView and GtkTextBuffer are generated and GtkTextView is connected to GtkScrolledWindow as a child.
 They corresponds to each file, so they are generated inside the for-loop.
- 39-42 Set the contents of the file into GtkTextBuffer and free the memory pointed by `contents`. Get the filename and generate GtkLabel with the filename.
- 43: Append GtkScrolledWindow and GtkLabel to GtkNotebook. The appended objects are children of automatically generated GtkNotebookPage object. Therefore, the structure is like this:

        GtkNotebook -- GtkNotebookPage -- (GtkScrolledWindow and GtkLabel)

- 44: Get GtkNotebookPage object and set its pointer to `nbp`.
- 45: GtkNotebookPage has a property "tab-expand". If it is set to TRUE then the tab expand horizontally as long as possible. If FALSE, then the width of the tab is determined by the size of the label. `g_object_set` is a general function to set properties in any objects.
- 46: free the memory pointed by `filename`
- 53-56: If at least one file was read, then the number of GtkNotebookPage is greater than zero. If it's true, then show the window. If it's false, then destroy the window.


Up: [Readme.md](../Readme.md),  Prev: [Section 5](sec5.md), Next: [Section 7](sec7.md)
