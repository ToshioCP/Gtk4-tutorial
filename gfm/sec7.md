Up: [README.md](../README.md),  Prev: [Section 6](sec6.md), Next: [Section 8](sec8.md)

# Widgets (3)

## Open signal

### G\_APPLICATION\_HANDLES\_OPEN flag

We made a very simple editor in the previous section with GtkTextView, GtkTextBuffer and GtkScrolledWindow.
We will add file-reading ability to the program and improve it to a file viewer.

The easiest way to give a filename is to use a command line argument.

    $ ./a.out filename

The program will open the file and insert its contents into the GtkTextBuffer.

To do this, we need to know how GtkApplication (or GApplication) recognizes arguments.
This is described in the [GIO API Reference -- Application](https://docs.gtk.org/gio/class.Application.html).

When GtkApplication is created, a flag (GApplicationFlags) is given as an argument.

~~~C
GtkApplication *
gtk_application_new (const gchar *application_id, GApplicationFlags flags);
~~~

This tutorial explains only two flags, `G_APPLICATION_DEFAULT_FLAGS` and `G_APPLICATION_HANDLES_OPEN`.
(`G_APPLICATION_FLAGS_NONE` was used instead of `G_APPLICATION_DEFAULT_FLAGS` before GIO version2.73.3 (GLib 2.73.3 5/Aug/2022).
Some GTK 4 applications still use `G_APPLICATION_FLAGS_NONE`.
But now it is deprecated and `G_APPLICATION_DEFAULT_FLAGS` is recommended.)
If you want to handle command line arguments, the `G_APPLICATION_HANDLES_COMMAND_LINE` flag is what you need.

For further information, see [GIO API Reference -- ApplicationFlags](https://docs.gtk.org/gio/flags.ApplicationFlags.html) and
[GIO API Reference -- g\_application\_run](https://docs.gtk.org/gio/method.Application.run.html).

We've already used `G_APPLICATION_DEFAULT_FLAGS`, as it is the simplest option, and no command line arguments are allowed.
If you give arguments, an error will occur.

The flag `G_APPLICATION_HANDLES_OPEN` is the second simplest option.
It allows arguments but only files.
The application assumes all the arguments are filenames.

~~~C
app = gtk_application_new ("com.github.ToshioCP.tfv3", G_APPLICATION_HANDLES_OPEN);
~~~

### open signal

When `G_APPLICATION_HANDLES_OPEN` flag is given to the application, two signals are available.

- activate signal --- This signal is emitted when there's no argument.
- open signal --- This signal is emitted when there is at least one argument.

The handler of the "open" signal is defined as follows.

~~~C
void
open (
  GApplication* self,
  gpointer files,
  gint n_files,
  gchar* hint,
  gpointer user_data
)
~~~

The parameters are:

- `self` --- the application instance (usually GtkApplication)
- `files` --- an array of GFiles. [array length=n\_files] [element-type GFile]
- `n_files` --- the number of the elements of `files`
- `hint` --- a hint provided by the calling instance (usually it can be ignored)
- `user_data` --- user data set when the signal handler was connected.

## Making a file viewer

### What is a file viewer?

A file viewer is a program that displays text files.
Our file viewer will work as follows.

- When arguments are given, it recognizes the first argument as a filename and opens it.
- The second argument and after are ignored.
- If there's no argument, it shows an error message and quit.
- If it successfully opens the file, it reads the contents of the file, inserts them to GtkTextBuffer and shows the window.
- If it fails to open the file, it shows an error message and quit.

The program is shown below.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 app_activate (GApplication *app) {
 5   g_printerr ("You need a filename argument.\n");
 6 }
 7 
 8 static void
 9 app_open (GApplication *app, GFile ** files, int n_files, char *hint) {
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
37     gtk_window_present (GTK_WINDOW (win));
38   } else {
39     if ((filename = g_file_get_path (files[0])) != NULL) {
40       g_printerr ("No such file: %s.\n", filename);
41       g_free (filename);
42     } else
43       g_printerr ("File can't be opened.\n");
44     gtk_window_destroy (GTK_WINDOW (win));
45   }
46 }
47 
48 int
49 main (int argc, char **argv) {
50   GtkApplication *app;
51   int stat;
52 
53   app = gtk_application_new ("com.github.ToshioCP.tfv3", G_APPLICATION_HANDLES_OPEN);
54   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
55   g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
56   stat = g_application_run (G_APPLICATION (app), argc, argv);
57   g_object_unref (app);
58   return stat;
59 }
~~~

Save it as `tfv3.c`.
Then compile and run it.

    $ comp tfv3
    $ ./a.out tfv3.c

![File viewer](../image/screenshot_tfv3.png)

The function `main` has only two changes from the previous version.

- `G_APPLICATION_DEFAULT_FLAGS` is replaced by `G_APPLICATION_HANDLES_OPEN`
- `g_signal_connect (app, "open", G_CALLBACK (app_open), NULL)` is added.

When the flag `G_APPLICATION_HANDLES_OPEN` is given to `gtk_application_new` function, the application behaves like this:

- If the application is run without command line arguments, it emits "activate" signal when it is activated.
- If the application is run with command line arguments, it emits "open" signal when it is activated.

The handler `app_activate` becomes very simple.
It just outputs the error message and return to the caller.
Then the application quits immediately because no window is created.

The main work is done in the handler `app_open`.

- Creates GtkApplicationWindow, GtkScrolledWindow, GtkTextView and GtkTextBuffer and connects them together
- Sets wrap mode to `GTK_WRAP_WORD_CHAR` in GtktextView
- Sets GtkTextView to non-editable because the program isn't an editor but only a viewer
- Reads the file and inserts the text into GtkTextBuffer (this will be explained later)
- If the file is not opened, outputs an error message and destroys the window. This makes the application quit.

The following is the file reading part of the program again.

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
    g_printerr ("No such file: %s.\n", filename);
    g_free (filename);
  } else
    g_printerr ("File can't be opened.\n");
  gtk_window_destroy (GTK_WINDOW (win));
}
~~~

The function `g_file_load_contents` loads the file contents into a temporary buffer,
which is automatically allocated and sets `contents` to point the buffer.
The length of the buffer is assigned to `length`.
It returns `TRUE` if the file's contents are successfully loaded or `FALSE` if an error occurs.
If you want to know the details about g\_file\_load\_contents, see [g file load contents](https://docs.gtk.org/gio/method.File.load_contents.html).

If it has successfully read the file, it inserts the contents into GtkTextBuffer,
frees the temporary buffer pointed by `contents`, sets the title of the window,
frees the memories pointed by `filename` and then shows the window.
If it fails, it outputs an error message and destroys the window and finally make the program quit.

## GtkNotebook

GtkNotebook is a container widget that contains multiple widgets with tabs.
It shows only one child at a time.
Another child will be shown when its tab is clicked.

![GtkNotebook](../image/screenshot_gtk_notebook.png)

The left image is the window at the startup.
The file `pr1.c` is shown and its filename is in the left tab.
After clicking on the right tab, the contents of the file `tfv1.c` is shown.
The right image is the screenshot.

The following is `tfv4.c`.
It has GtkNoteBook widget.
It is inserted as a child of GtkApplicationWindow and contains multiple GtkScrolledWindow.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 app_activate (GApplication *app) {
 5   g_printerr ("You need a filename argument.\n");
 6 }
 7 
 8 static void
 9 app_open (GApplication *app, GFile ** files, gint n_files, gchar *hint) {
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
24   gtk_window_set_default_size (GTK_WINDOW (win), 600, 400);
25 
26   nb = gtk_notebook_new ();
27   gtk_window_set_child (GTK_WINDOW (win), nb);
28 
29   for (i = 0; i < n_files; i++) {
30     if (g_file_load_contents (files[i], NULL, &contents, &length, NULL, NULL)) {
31       scr = gtk_scrolled_window_new ();
32       tv = gtk_text_view_new ();
33       tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
34       gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
35       gtk_text_view_set_editable (GTK_TEXT_VIEW (tv), FALSE);
36       gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
37 
38       gtk_text_buffer_set_text (tb, contents, length);
39       g_free (contents);
40       if ((filename = g_file_get_basename (files[i])) != NULL) {
41         lab = gtk_label_new (filename);
42         g_free (filename);
43       } else
44         lab = gtk_label_new ("");
45       gtk_notebook_append_page (GTK_NOTEBOOK (nb), scr, lab);
46       nbp = gtk_notebook_get_page (GTK_NOTEBOOK (nb), scr);
47       g_object_set (nbp, "tab-expand", TRUE, NULL);
48     } else if ((filename = g_file_get_path (files[i])) != NULL) {
49         g_printerr ("No such file: %s.\n", filename);
50         g_free (filename);
51     } else
52         g_printerr ("No valid file is given\n");
53   }
54   if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) > 0)
55     gtk_window_present (GTK_WINDOW (win));
56   else
57     gtk_window_destroy (GTK_WINDOW (win));
58 }
59 
60 int
61 main (int argc, char **argv) {
62   GtkApplication *app;
63   int stat;
64 
65   app = gtk_application_new ("com.github.ToshioCP.tfv4", G_APPLICATION_HANDLES_OPEN);
66   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
67   g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
68   stat = g_application_run (G_APPLICATION (app), argc, argv);
69   g_object_unref (app);
70   return stat;
71 }
~~~

Most of the changes are in the function `app_open`.
The numbers at the left of the following items are line numbers in the source code.

- 11-13: Variables `nb`, `lab` and `nbp` are defined. They point GtkNotebook, GtkLabel and GtkNotebookPage respectively.
- 23: The window's title is set to "file viewer".
- 24: The default size of the window is 600x400.
- 26-27 GtkNotebook is created and inserted to the GtkApplicationWindow as a child.
- 29-58 For-loop. The variable `files[i]` points i-th GFile, which is created by the GtkApplication from the i-th command line argument.
- 31-36 GtkScrollledWindow, GtkTextView are created. GtkTextBuffer is got from the GtkTextView.
The GtkTextView is connected to the GtkScrolledWindow as a child.
- 38-39 inserts the contents of the file into GtkTextBuffer and frees the memory pointed by `contents`.
- 40-42: If the filename is taken from the GFile, GtkLabel is created with the filename. The string `filename` is freed..
- 43-44: If it fails to take the filename, empty string GtkLabel is created.
- 45-46: Appends a GtkScrolledWindow to the GtkNotebook as a child.
And the GtkLabel is set as the child's tab.
At the same time, a GtkNoteBookPage is created automatically.
The function `gtk_notebook_get_page` returns the GtkNotebookPage of the child (GtkScrolledWindow).
- 47: GtkNotebookPage has "tab-expand" property.
If it is set to TRUE then the tab expands horizontally as long as possible.
If it is FALSE, then the width of the tab is determined by the size of the label.
`g_object_set` is a general function to set properties of objects.
See [GObject API Reference -- g\_object\_set](https://docs.gtk.org/gobject/method.Object.set.html).
- 48-50: If it fails to read the file and a filename is taken from the GFile, "No such file" message is displayed. The `filename` is freed.
- 51-52: If `filename` is NULL, the "No valid file is given" message is displayed.
- 54-57: If at least one page exists, the window is shown.
Otherwise, the window is destroyed and the application quits.

Up: [README.md](../README.md),  Prev: [Section 6](sec6.md), Next: [Section 8](sec8.md)
