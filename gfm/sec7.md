Up: [Readme.md](../Readme.md),  Prev: [Section 6](sec6.md), Next: [Section 8](sec8.md)

# Widgets (3)

## Open signal

### G\_APPLICATION\_HANDLES\_OPEN flag

GtkTextView, GtkTextBuffer and GtkScrolledWindow have given us a minimum editor in the previous section.
Next, we will add a function to read a file and remake the program into a file viewer.
There are many ways to implement the function.
Because this is a tutorial for beginners, we'll take the easiest one.

When the program starts, we give a filename as an argument.

    $ ./a.out filename

Then it opens the file and inserts its contents into the GtkTextBuffer.

At the beginning of the implementation, we need to know how GtkApplication (or GApplication) recognizes arguments.
It is described in the [GIO reference manual](https://developer.gnome.org/gio/stable/GApplication.html#GApplication.description).

When GtkApplication is created, a flag (its type is GApplicationFlags) is given as an argument.

~~~C
GtkApplication *
gtk_application_new (const gchar *application_id, GApplicationFlags flags);
~~~

This tutorial explains only two flags, `G_APPLICATION_FLAGS_NONE` and `G_APPLICATION_HANDLES_OPEN`.
If you want to handle command line arguments, `G_APPLICATION_HANDLES_COMMAND_LINE` flag is what you need.
How to program it is written in [GIO reference manual, g\_application\_run](https://developer.gnome.org/gio/stable/GApplication.html#g-application-run).
And the flag is described in the [GApplication section](https://developer.gnome.org/gio/stable/GApplication.html#GApplicationFlags) in GIO reference manual.

~~~
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

Now we use this flag when creating GtkApplication.

~~~C
app = gtk_application_new ("com.github.ToshioCP.tfv3", G_APPLICATION_HANDLES_OPEN);
~~~

### open signal

When the application starts, two signals are possible.

- activate signal --- This signal is emitted when there's no argument.
- open signal --- This signal is emitted when there is at least one argument.

The handler of "open" signal is defined as follows.

~~~C
void user_function (GApplication *application,
                   gpointer      files,
                   gint          n_files,
                   gchar        *hint,
                   gpointer      user_data)
~~~

The parameters are:

- `application` --- the application (usually GtkApplication)
- `files` --- an array of GFiles. [array length=n\_files] [element-type GFile]
- `n_files` --- the number of the elements of `files`
- `hint` --- a hint provided by the calling instance (usually it can be ignored)
- `user_data` --- user data set when the signal handler was connected.

The way how to read a file (GFile) will be described in the next subsection.

## Making a file viewer

### What is a file viewer?

A file viewer is a program that shows a text file given as an argument.
It works as follows.

- If it is given arguments, it recognizes the first argument as a filename and opens it.
- If opening the file succeeds, it reads the contents of the file and inserts it to GtkTextBuffer and shows the window.
- If it fails to open the file, shows an error message and quits.
- If there's no argument, it shows an error message and quits.
- If there are two or more arguments, the second one and after are ignored.

The program is as follows.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 app_activate (GApplication *app, gpointer user_data) {
 5   g_print ("You need a filename argument.\n");
 6 }
 7 
 8 static void
 9 app_open (GApplication *app, GFile ** files, gint n_files, gchar *hint, gpointer user_data) {
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
33     if ((filename = g_file_get_basename (files[0])) != NULL) {
34       gtk_window_set_title (GTK_WINDOW (win), filename);
35       g_free (filename);
36     }
37     gtk_widget_show (win);
38   } else {
39     if ((filename = g_file_get_path (files[0])) != NULL) {
40       g_print ("No such file: %s.\n", filename);
41       g_free (filename);
42     }
43     gtk_window_destroy (GTK_WINDOW (win));
44   }
45 }
46 
47 int
48 main (int argc, char **argv) {
49   GtkApplication *app;
50   int stat;
51 
52   app = gtk_application_new ("com.github.ToshioCP.tfv3", G_APPLICATION_HANDLES_OPEN);
53   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
54   g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
55   stat =g_application_run (G_APPLICATION (app), argc, argv);
56   g_object_unref (app);
57   return stat;
58 }
59 
~~~

Save it as `tfv3.c`.
Then compile and run it.

    $ comp tfv3
    $ ./a.out tfv3.c

![File viewer](../image/screenshot_tfv3.png)

Now I want to explain the program `tfv3.c`.
First, the function `main` changes in only two lines from the previous version.

- `G_APPLICATION_FLAGS_NONE` is replaced by `G_APPLICATION_HANDLES_OPEN`.
- `g_signal_connect (app, "open", G_CALLBACK (on_open), NULL)` is added.

Next, the handler `app_activate` is now very simple.
It just outputs the error message.
The application quits immediately because no window is created.

The point is the handler `app_open`.

- It creates GtkApplicationWindow, GtkScrolledWindow, GtkTextView and GtkTextBuffer and connects them.
- Sets wrap mode to `GTK_WRAP_WORD_CHAR` in GtktextView.
- Sets GtkTextView to non-editable because the program isn't an editor but only a viewer.
- Reads the file and inserts the text into GtkTextBuffer (this will be explained in detail later).
- If the file is not opened then outputs an error message and destroys the window. It makes the application quit.

The file reading part of the program is shown again below.

~~~C
if (g_file_load_contents (files[0], NULL, &contents, &length, NULL, NULL)) {
  gtk_text_buffer_set_text (tb, contents, length);
  g_free (contents);
  if ((filename = g_file_get_basename (files[0])) != NULL) {
    gtk_window_set_title (GTK_WINDOW (win), filename);
    g_free (filename);
  }
  gtk_widget_show (win);
} else {
  if ((filename = g_file_get_path (files[0])) != NULL) {
    g_print ("No such file: %s.\n", filename);
    g_free (filename);
  }
  gtk_window_destroy (GTK_WINDOW (win));
}
~~~

The function `g_file_load_contents` loads the file contents into a buffer, which is automatically allocated, and sets `contents` to point the buffer.
And the length of the buffer is set to `length`.
It returns `TRUE` if the file's contents are successfully loaded. `FALSE` if an error happens.

If the function succeeds, it inserts the contents into GtkTextBuffer, frees the buffer memories pointed by `contents`, sets the title of the window,
frees the memories pointed by `filename` and shows the window.
If it fails, it outputs an error message and destroys the window.

## GtkNotebook

GtkNotebook is a container widget that contains multiple children with tabs in it.

![GtkNotebook](../image/screenshot_gtk_notebook.png)

Look at the screenshots above.
The left one is a window at the startup.
It shows the file `pr1.c`.
The filename is in the left tab.
After clicking on the right tab, the contents of `tfv1.c` appears.
It is shown in the right of the screenshot.

GtkNotebook widget is between GtkApplicationWindow and GtkScrolledWindow.
Now I want to show you the program `tfv4.c`.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 app_activate (GApplication *app, gpointer user_data) {
 5   g_print ("You need a filename argument.\n");
 6 }
 7 
 8 static void
 9 app_open (GApplication *app, GFile ** files, gint n_files, gchar *hint, gpointer user_data) {
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
41       if ((filename = g_file_get_basename (files[i])) != NULL) {
42         lab = gtk_label_new (filename);
43         g_free (filename);
44       } else
45         lab = gtk_label_new ("");
46       gtk_notebook_append_page (GTK_NOTEBOOK (nb), scr, lab);
47       nbp = gtk_notebook_get_page (GTK_NOTEBOOK (nb), scr);
48       g_object_set (nbp, "tab-expand", TRUE, NULL);
49     } else if ((filename = g_file_get_path (files[i])) != NULL) {
50         g_print ("No such file: %s.\n", filename);
51         g_free (filename);
52     } else
53         g_print ("No valid file is given\n");
54   }
55   if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) > 0)
56     gtk_widget_show (win);
57   else
58     gtk_window_destroy (GTK_WINDOW (win));
59 }
60 
61 int
62 main (int argc, char **argv) {
63   GtkApplication *app;
64   int stat;
65 
66   app = gtk_application_new ("com.github.ToshioCP.tfv4", G_APPLICATION_HANDLES_OPEN);
67   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
68   g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
69   stat =g_application_run (G_APPLICATION (app), argc, argv);
70   g_object_unref (app);
71   return stat;
72 }
73 
~~~

Most of the change is in the function `app_open`.
The numbers at the left of the following items are line numbers in the source code.

- 11-13: Variables `nb`, `lab` and `nbp` are defined and points GtkNotebook, GtkLabel and GtkNotebookPage respectively.
- 23: The window's title is set to "file viewer".
- 25: The size of the window is set to maximum because a big window is appropriate for file viewers.
- 27-28 GtkNotebook is created and inserted to the GtkApplicationWindow as a child.
- 30-59 For-loop. Each loop corresponds to an argument. And files[i] is GFile object with respect to the i-th argument.
- 32-37 GtkScrollledWindow, GtkTextView are created and GtkTextBuffer is get from the GtkTextView.
GtkTextView is connected to GtkScrolledWindow as a child.
They corresponds to each file, so they are created inside the for-loop.
- 39-40 inserts the contents of the file into GtkTextBuffer and frees the memory pointed by `contents`.
- 41-43: Gets the filename and creates GtkLabel with the filename.
Frees `filename`.
- 44-45: If `filename` is NULL, creates GtkLabel with the empty string.
- 46: Appends GtkScrolledWindow and GtkLabel to GtkNotebook.
At the same time, a GtkNoteBookPage widget is created automatically.
The GtkScrolledWindow widget is connected to the GtkNotebookPage.
Therefore, the structure is like this:

~~~
    GtkNotebook -- GtkNotebookPage -- GtkScrolledWindow
~~~

- 47: Gets GtkNotebookPage widget and sets `nbp` to point the GtkNotebookPage.
- 48: GtkNotebookPage has a property "tab-expand".
If it is set to TRUE then the tab expands horizontally as long as possible.
If it is FALSE, then the width of the tab is determined by the size of the label.
`g_object_set` is a general function to set properties in any objects.
See [GObject reference manual](https://developer.gnome.org/gobject/stable/gobject-The-Base-Object-Type.html#g-object-set).
- 49-51: If it fails to read the file, "No such file" message is outputted.
Frees `filename`.
- 52-53: If `filename` is NULL, "No valid file is given" message is outputted.
- 55-58: If at least one file was read, then the number of GtkNotebookPage is greater than zero.
If it's true, it shows the window.
If it's false, it destroys the window.


Up: [Readme.md](../Readme.md),  Prev: [Section 6](sec6.md), Next: [Section 8](sec8.md)
