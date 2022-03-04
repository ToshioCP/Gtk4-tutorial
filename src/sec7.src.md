# Widgets (3)

## Open signal

### G\_APPLICATION\_HANDLES\_OPEN flag

The GtkTextView, GtkTextBuffer and GtkScrolledWindow widgets have given us a minimum editor
in the previous section.
We will now add a function to read a file and rework the program into a file viewer.
There are many ways to implement the function and
because this is a tutorial for beginners, we'll take the easiest one.

When the program starts, we will give the filename to open as an argument.

    $ ./a.out filename

It will open the file and insert its contents into the GtkTextBuffer.

To do this, we need to know how GtkApplication (or GApplication) recognizes arguments.
This is described in the [GIO API Reference, Application](https://docs.gtk.org/gio/class.Application.html).

When GtkApplication is created, a flag (with the type GApplicationFlags) is provided as an argument.

~~~C
GtkApplication *
gtk_application_new (const gchar *application_id, GApplicationFlags flags);
~~~

This tutorial explains only two flags, `G_APPLICATION_FLAGS_NONE` and `G_APPLICATION_HANDLES_OPEN`.
If you want to handle command line arguments, the `G_APPLICATION_HANDLES_COMMAND_LINE` flag is what you need.
How to use the new application method is described in [GIO API Reference, g\_application\_run](https://docs.gtk.org/gio/method.Application.run.html),
and the flag is described in the [GIO API Reference, ApplicationFlags](https://docs.gtk.org/gio/flags.ApplicationFlags.html).

~~~
GApplicationFlags' Members

G_APPLICATION_FLAGS_NONE  Default. (No argument allowed)
  ... ... ...
G_APPLICATION_HANDLES_OPEN  This application handles opening files (in the primary instance).
  ... ... ...
~~~

There are ten flags in total, but we only need two of them so far.
We've already used `G_APPLICATION_FLAGS_NONE`, as
it is the simplest option, and no arguments are allowed.
If you provide arguments when running the application, an error will occur.

The flag `G_APPLICATION_HANDLES_OPEN` is the second simplest option.
It allows arguments but only files.
The application assumes all the arguments are filenames and we will use this flag when creating
our GtkApplication.

~~~C
app = gtk_application_new ("com.github.ToshioCP.tfv3", G_APPLICATION_HANDLES_OPEN);
~~~

### open signal

Now, when the application starts, two signals can be emitted.

- activate signal --- This signal is emitted when there's no argument.
- open signal --- This signal is emitted when there is at least one argument.

The handler of the "open" signal is defined as follows.

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

How to read a specified file (GFile) will be described next.

## Making a file viewer

### What is a file viewer?

A file viewer is a program that displays the text file that is given as an argument.
Our file viewer will work as follows.

- When arguments are given, it treats the first argument as a filename and opens it.
- If opening the file succeeds, it reads the contents of the file and inserts it to GtkTextBuffer and then shows the window.
- If it fails to open the file, it will show an error message and quit.
- If there's no argument, it will shows an error message and quit.
- If there are two or more arguments, the second one and any others are ignored.

The program which does this is shown below.

@@@include
tfv/tfv3.c
@@@

Save it as `tfv3.c`.
Then compile and run it.

    $ comp tfv3
    $ ./a.out tfv3.c

![File viewer](../image/screenshot_tfv3.png){width=6.3cm height=5.325cm}

Let's explain how the program `tfv3.c` works.
First, the function `main` has only two changes from the previous version.

- `G_APPLICATION_FLAGS_NONE` is replaced by `G_APPLICATION_HANDLES_OPEN`; and
- `g_signal_connect (app, "open", G_CALLBACK (on_open), NULL)` is added.

Next, the handler `app_activate` is added and is very simple.
It just outputs the error message and
the application quits immediately because no window is created.

The main functionality is the in the handler `app_open`. It

- Creates GtkApplicationWindow, GtkScrolledWindow, GtkTextView and GtkTextBuffer and connects them together;
- Sets wrap mode to `GTK_WRAP_WORD_CHAR` in GtktextView;
- Sets GtkTextView to non-editable because the program isn't an editor but only a viewer;
- Reads the file and inserts the text into GtkTextBuffer (this will be explained in detail later); and
- If the file is not opened then outputs an error message and destroys the window. This makes the application quit.

The following is the important file reading part of the program and is shown again below.

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

The function `g_file_load_contents` loads the file contents into a buffer,
which is automatically allocated and sets `contents` to point that buffer.
The length of the buffer is set to `length`.
It returns `TRUE` if the file's contents are successfully loaded and `FALSE` if an error occurs.

If this function succeeds, it inserts the contents into GtkTextBuffer,
frees the buffer pointed by `contents`, sets the title of the window,
frees the memories pointed by `filename` and then shows the window.
If it fails, it outputs an error message and destroys the window, causing the program to quit.

## GtkNotebook

GtkNotebook is a container widget that uses tabs and contains multiple children.
The child that is displayed depends on which tab has been selected.

![GtkNotebook](../image/screenshot_gtk_notebook.png){width=13.2cm height=5.325cm}

Looking at the screenshots above,
the left one is the window at the startup.
It shows the file `pr1.c` and the filename is in the left tab.
After clicking on the right tab, the contents of the file `tfv1.c` are shown instead.
This is shown in the right screenshot.

The GtkNotebook widget is inserted as a child of GtkApplicationWindow and contains a GtkScrolledWindow
for each file that is being displayed.
The code to do this is given in `tfv4.c` and is:

@@@include
tfv/tfv4.c
@@@

Most of the changes are in the function `app_open`.
The numbers at the left of the following items are line numbers in the source code.

- 11-13: Variables `nb`, `lab` and `nbp` are defined and will point to a new GtkNotebook, GtkLabel and GtkNotebookPage widget respectively.
- 23: The window's title is set to "file viewer".
- 25: The size of the window is set to maximum because a big window is appropriate for file viewers.
- 27-28 GtkNotebook is created and inserted to the GtkApplicationWindow as a child.
- 30-59 For-loop. Each loop corresponds to a filename argument, and `files[i]` is GFile object containing the i-th argument.
- 32-37 GtkScrollledWindow, GtkTextView are created and GtkTextBuffer found from the new GtkTextView.
GtkTextView is connected to GtkScrolledWindow as a child.
Each file gets their own copy of these widgets, so they are created inside the for-loop.
- 39-40 inserts the contents of the file into GtkTextBuffer and frees the memory pointed by `contents`.
- 41-43: Gets the filename and creates GtkLabel with the filename and then frees `filename`.
- 44-45: If `filename` is NULL, creates GtkLabel with the empty string.
- 46: Appends GtkScrolledWindow as a page, with the tab GtkLabel, to GtkNotebook.
At this time a GtkNoteBookPage widget is created automatically.
The GtkScrolledWindow widget is connected to the GtkNotebookPage.
Therefore, the structure is like this:

~~~
    GtkNotebook -- GtkNotebookPage -- GtkScrolledWindow
~~~

- 47: Gets GtkNotebookPage widget and sets `nbp` to point to this GtkNotebookPage.
- 48: GtkNotebookPage has a property set called "tab-expand".
If it is set to TRUE then the tab expands horizontally as long as possible.
If it is FALSE, then the width of the tab is determined by the size of the label.
`g_object_set` is a general function to set properties in objects.
See [GObject API Reference, g\_object\_set](https://docs.gtk.org/gobject/method.Object.set.html).
- 49-51: If the file cannot be read, "No such file" message is displayed and the `filename` buffer is freed.
- 52-53: If `filename` is NULL, the "No valid file is given" message is outputted.
- 55-58: If at least one file was read, then the number of GtkNotebookPage is greater than zero.
If it's true, it shows the window.
If it's false, it destroys the window, which causes the program to quit.
