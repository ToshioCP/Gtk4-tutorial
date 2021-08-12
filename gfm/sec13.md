Up: [Readme.md](../Readme.md),  Prev: [Section 12](sec12.md), Next: [Section 14](sec14.md)

# Functions in TfeTextView

In this section I will explain functions in TfeTextView object.

## tfe.h and tfetextview.h

`tfe.h` is a top header file and it includes `gtk.h` and all the header files.
C source files `tfeapplication.c` and `tfenotebook.c` include `tfe.h` at the beginning.

~~~C
1 #include <gtk/gtk.h>
2 
3 #include "../tfetextview/tfetextview.h"
4 #include "tfenotebook.h"
~~~

`../tfetextview/tfetextview.h` is a header file which describes the public functions in `tfetextview.c`.

~~~C
 1 #ifndef __TFE_TEXT_VIEW_H__
 2 #define __TFE_TEXT_VIEW_H__
 3 
 4 #include <gtk/gtk.h>
 5 
 6 #define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type ()
 7 G_DECLARE_FINAL_TYPE (TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView)
 8 
 9 /* "open-response" signal response */
10 enum TfeTextViewOpenResponseType
11 {
12   TFE_OPEN_RESPONSE_SUCCESS,
13   TFE_OPEN_RESPONSE_CANCEL,
14   TFE_OPEN_RESPONSE_ERROR
15 };
16 
17 GFile *
18 tfe_text_view_get_file (TfeTextView *tv);
19 
20 void
21 tfe_text_view_open (TfeTextView *tv, GtkWindow *win);
22 
23 void
24 tfe_text_view_save (TfeTextView *tv);
25 
26 void
27 tfe_text_view_saveas (TfeTextView *tv);
28 
29 GtkWidget *
30 tfe_text_view_new_with_file (GFile *file);
31 
32 GtkWidget *
33 tfe_text_view_new (void);
34 
35 #endif /* __TFE_TEXT_VIEW_H__ */
~~~

- 1,2,35: Thanks to these three lines, the following lines are included only once. 
- 4: Includes gtk4 header files.
The header file `gtk4` also has the same mechanism to avoid including it multiple times.
- 6-7: These two lines define TfeTextView type, its class structure and some useful macros.
- 9-15: A definition of the value of the parameter of "open-response" signal.
- 17-33: Declarations of public functions on TfeTextView.

## Functions to create TfeTextView instances

A TfeTextView instance is created with `tfe_text_view_new` or `tfe_text_view_new_with_file`.

~~~C
GtkWidget *tfe_text_view_new (void);
~~~

`tfe_text_view_new` just creates a new TfeTextView instance and returns the pointer to the new instance.

~~~C
GtkWidget *tfe_text_view_new_with_file (GFile *file);
~~~

`tfe_text_view_new_with_file` is given a Gfile object as an argument and it loads the file into the GtkTextBuffer instance, then returns the pointer to the new instance.
If an error occurs during the creation process, NULL is returned.

Each function is defined as follows.

~~~C
 1 GtkWidget *
 2 tfe_text_view_new_with_file (GFile *file) {
 3   g_return_val_if_fail (G_IS_FILE (file), NULL);
 4 
 5   GtkWidget *tv;
 6   GtkTextBuffer *tb;
 7   char *contents;
 8   gsize length;
 9 
10   if (! g_file_load_contents (file, NULL, &contents, &length, NULL, NULL)) /* read error */
11     return NULL;
12 
13   if ((tv = tfe_text_view_new()) != NULL) {
14     tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
15     gtk_text_buffer_set_text (tb, contents, length);
16     TFE_TEXT_VIEW (tv)->file = g_file_dup (file);
17     gtk_text_buffer_set_modified (tb, FALSE);
18   }
19   g_free (contents);
20   return tv;
21 }
22 
23 GtkWidget *
24 tfe_text_view_new (void) {
25   return GTK_WIDGET (g_object_new (TFE_TYPE_TEXT_VIEW, NULL));
26 }
~~~

- 23-25: `tfe_text_view_new` function.
Just returns the value from the function `g_object_new` but casts it to the pointer to GtkWidget.
Initialization is done in `tfe_text_view_init` which is called in the process of `g_object_new` function.
- 1-21: `tfe_text_view_new_with_file` function.
- 3: `g_return_val_if_fail` is described in [GLib API Reference, g\_return\_val\_if\_fail](https://docs.gtk.org/glib/func.return_val_if_fail.html).
And also [GLib API Reference, Message Logging](https://docs.gtk.org/glib/logging.html).
It tests whether the argument `file` is a pointer to GFile.
If it's true, then the program goes on to the next line.
If it's false, then it returns NULL (the second argument) immediately.
And at the same time it logs out the error message (usually the log is outputted to stderr or stdout).
This function is used to check the programmer's error.
If an error occurs, the solution is usually to change the (caller) program and fix the bug.
You need to distinguish programmer's errors and runtime errors.
You shouldn't use this function to find runtime errors.
- 10-11: If an error occurs when reading the file, then the function returns NULL.
- 13: Calls the function `tfe_text_view_new`.
The function creates TfeTextView instance and returns the pointer to the instance.
If an error happens in `tfe_text_view_new`, it returns NULL.
- 14: Gets the pointer to GtkTextBuffer corresponds to `tv`.
The pointer is assigned to `tb`
- 15: Assigns the contents read from the file to GtkTextBuffer pointed by `tb`.
- 16: Duplicates `file` and sets `tv->file` to point it.
- 17: The function `gtk_text_buffer_set_modified (tb, FALSE)` sets the modification flag of `tb` to FALSE.
The modification flag indicates that the contents of the buffer is modified.
It is used when the contents are saved.
If the modification flag is FALSE, it doesn't need to save the contents.
- 19: Frees the memories pointed by `contents`.
- 20: Returns `tv`, which is a pointer to the newly created TfeTextView instance.
If an error happens, NULL is returned.

## Save and saveas functions

Save and saveas functions write the contents in the GtkTextBuffer to a file.

~~~C
void tfe_text_view_save (TfeTextView *tv)
~~~

The function `tfe_text_view_save` writes the contents in the GtkTextBuffer to a file specified by `tv->file`.
If `tv->file` is NULL, then it shows GtkFileChooserDialog and prompts the user to choose a file to save.
Then it saves the contents to the file and sets `tv->file` to point the GFile instance for the file.

~~~C
void tfe_text_view_saveas (TfeTextView *tv)
~~~

The function `saveas` uses GtkFileChooserDialog and prompts the user to select a existed file or specify a new file to save.
Then, the function changes `tv->file` and save the contents to the specified file.
If an error occurs, it is shown to the user through the message dialog.
The error is managed only in the TfeTextView and no information is notified to the caller.

~~~C
 1 static gboolean
 2 save_file (GFile *file, GtkTextBuffer *tb, GtkWindow *win) {
 3   GtkTextIter start_iter;
 4   GtkTextIter end_iter;
 5   gchar *contents;
 6   gboolean stat;
 7   GtkWidget *message_dialog;
 8   GError *err = NULL;
 9 
10   gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
11   contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
12   if (g_file_replace_contents (file, contents, strlen (contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, &err)) {
13     gtk_text_buffer_set_modified (tb, FALSE);
14     stat = TRUE;
15   } else {
16     message_dialog = gtk_message_dialog_new (win, GTK_DIALOG_MODAL,
17                                              GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
18                                             "%s.\n", err->message);
19     g_signal_connect (message_dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
20     gtk_widget_show (message_dialog);
21     g_error_free (err);
22     stat = FALSE;
23   }
24   g_free (contents);
25   return stat;
26 }
27 
28 static void
29 saveas_dialog_response (GtkWidget *dialog, gint response, TfeTextView *tv) {
30   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
31   GFile *file;
32   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
33 
34   if (response == GTK_RESPONSE_ACCEPT) {
35     file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));
36     if (! G_IS_FILE (file))
37       g_warning ("TfeTextView: gtk_file_chooser_get_file returns non GFile.\n");
38     else if (save_file(file, tb, GTK_WINDOW (win))) {
39       if (G_IS_FILE (tv->file))
40         g_object_unref (tv->file);
41       tv->file = file;
42       g_signal_emit (tv, tfe_text_view_signals[CHANGE_FILE], 0);
43     } else
44       g_object_unref (file);
45   }
46   gtk_window_destroy (GTK_WINDOW (dialog));
47 }
48 
49 void
50 tfe_text_view_save (TfeTextView *tv) {
51   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
52 
53   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
54   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
55 
56   if (! gtk_text_buffer_get_modified (tb))
57     return; /* no need to save it */
58   else if (tv->file == NULL)
59     tfe_text_view_saveas (tv);
60   else if (! G_IS_FILE (tv->file))
61     g_error ("TfeTextView: The pointer tv->file isn't NULL nor GFile.\n");
62   else
63     save_file (tv->file, tb, GTK_WINDOW (win));
64 }
65 
66 void
67 tfe_text_view_saveas (TfeTextView *tv) {
68   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
69 
70   GtkWidget *dialog;
71   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
72 
73   dialog = gtk_file_chooser_dialog_new ("Save file", GTK_WINDOW (win), GTK_FILE_CHOOSER_ACTION_SAVE,
74                                       "Cancel", GTK_RESPONSE_CANCEL,
75                                       "Save", GTK_RESPONSE_ACCEPT,
76                                       NULL);
77   g_signal_connect (dialog, "response", G_CALLBACK (saveas_dialog_response), tv);
78   gtk_widget_show (dialog);
79 }
~~~

- 1-26: `save_file` function.
This function is called from `saveas_dialog_response` and `tfe_text_view_save`.
This function saves the contents of the buffer to the file given as an argument.
If error happens, it displays an error message.
The class of this function is `static`.
Therefore, only functions in this file (`tfeTetview.c`) call this function.
Such static functions usally don't have `g_return_val_if_fail` function.
- 10-11: Gets the text contents from the buffer.
- 12-14: Saves the contents to the file.
If no error happens, set the modified flag to be FALSE.
This means that the buffer is not modified since it has been saved.
And set the return status `stat` to be TRUE.
- 15-23: If it  fails to save the contents, displays an error message.
- 16-18: Creates a message dialog with the error message.
- 19: Connects the "response" signal  to `gtk_window_destroy`, so that the dialog disappears when a user clicked on the button.
- 20-21: Shows the window, frees `err` and set `stat` to be FLASE.
- 24: Frees `contents`.
- 25: Returns to the caller.
- 28-47: `saveas_dialog_response` function.
This is a signal handler for the "response" signal on GtkFileChooserDialog instance created by `tfe_text_view_saveas` function.
This handler analyzes the response and determines whether to save the contents.
- 34-45: If the response is `GTK_RESPONSE_ACCEPT`, the user has clicked on the `Save` button. So, it tries to save.
- 35: Gets the GFile `file` from GtkFileChooserDialog.
- 36-37: If it doesn't point GFile, it outputs an error message to the log.
- 38: Otherwise, it calls `save_file` to save the contents to the file.
- 39-42: If `save_file` has successfully saved the contents, `tv->file` is updated.
If the old GFile pointed by `tv->file` exists, it is freed in advance.
Emits "change-file" signal.
- 44: Unrefs `file`.
- 46: destroys the file chooser dialog.
- 49-64: `tfe_text_view_save` function.
- 51: `tfe_text_view_save` is public, i.e. it is open to the other files.
So, it doesn't have `static` class.
Public functions should check the parameter type with `g_return_if_fail` function.
If `tv` is not a pointer to a TfeTextView instance, then it logs an error message and immediately returns.
This function is similar to `g_return_val_if_fail`, but no value is returned because `tfe_text_view_save` doesn't return a value.
- 53-54: Gets GtkTextBuffer instance and GtkWidget instance and assignes them to `tb` and`win` respectively.
- 56-57: If the buffer hasn't modified, then it doesn't need to save it.
So the function returns.
- 58-59: If `tv->file` is NULL, no file has given yet.
It calls `tfe_text_view_saveas` which prompts a user to select a file or specify a new file to save.
- 60-61: If `tv->file` doesn't point GFile, somethig bad has happened.
Logs an error message.
- 62-63: Calls `save_file` to save the contents to the file.
- 66-79: `tfe_text_view_saveas` function.
It shows GtkFileChooserDialog and prompts the user to choose a file.
- 73-76: Creates GtkFileChooserDialog.
The title is "Save file".
Transient parent of the dialog is `win`, which is the top-level window.
The action is save mode.
The buttons are Cancel and Save.
- 77: connects the "response" signal of the dialog and `saveas_dialog_response` handler.
- 78: Shows the dialog.

![Saveas process](../image/saveas.png)

When you use GtkFileChooserDialog, you need to divide the program into two parts.
One is a function which creates GtkFileChooserDialog and the other is a signal handler.
The function just creates and shows GtkFileChooserDialog.
The rest is done by the handler.
It gets Gfile from GtkFileChooserDialog and saves the buffer to the file by calling `save_file`.

## Open function

Open function shows GtkFileChooserDialog to users and prompts them to choose a file.
Then it reads the file and puts the text into GtkTextBuffer.

~~~C
void tfe_text_view_open (TfeTextView *tv, GtkWindow *win);
~~~

The parameter `win` is the top-level window.
It will be a transient parent window of GtkFileChooserDialog when the dialog is created.
This allows window managers to keep the dialog on top of the parent window, or center the dialog over the parent window.
It is possible to give no parent window to the dialog.
However, it is encouraged to give a parent window to dialog.
This function might be called just after `tv` has been created.
In that case, `tv` has not been incorporated into the widget hierarchy.
Therefore it is impossible to get the top-level window from `tv`.
That's why the function needs `win` parameter.

This function is usually called when the buffer of `tv` is empty.
However, even if the buffer is not empty, `tfe_text_view_open` doesn't treat it as an error.
If you want to revert the buffer, calling this function is appropriate.
Otherwise probably bad things will happen.

~~~C
 1 static void
 2 open_dialog_response(GtkWidget *dialog, gint response, TfeTextView *tv) {
 3   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
 4   GFile *file;
 5   char *contents;
 6   gsize length;
 7   GtkWidget *message_dialog;
 8   GError *err = NULL;
 9 
10   if (response != GTK_RESPONSE_ACCEPT)
11     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_CANCEL);
12   else if (! G_IS_FILE (file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog)))) {
13     g_warning ("TfeTextView: gtk_file_chooser_get_file returns non GFile.\n");
14     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_ERROR);
15   } else if (! g_file_load_contents (file, NULL, &contents, &length, NULL, &err)) { /* read error */
16     g_object_unref (file);
17     message_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog), GTK_DIALOG_MODAL,
18                                              GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
19                                             "%s.\n", err->message);
20     g_signal_connect (message_dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
21     gtk_widget_show (message_dialog);
22     g_error_free (err);
23     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_ERROR);
24   } else {
25     gtk_text_buffer_set_text (tb, contents, length);
26     g_free (contents);
27     if (G_IS_FILE (tv->file))
28       g_object_unref (tv->file);
29     tv->file = file;
30     gtk_text_buffer_set_modified (tb, FALSE);
31     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_SUCCESS);
32     g_signal_emit (tv, tfe_text_view_signals[CHANGE_FILE], 0);
33   }
34   gtk_window_destroy (GTK_WINDOW (dialog));
35 }
36 
37 void
38 tfe_text_view_open (TfeTextView *tv, GtkWindow *win) {
39   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
40   g_return_if_fail (GTK_IS_WINDOW (win));
41 
42   GtkWidget *dialog;
43 
44   dialog = gtk_file_chooser_dialog_new ("Open file", win, GTK_FILE_CHOOSER_ACTION_OPEN,
45                                         "Cancel", GTK_RESPONSE_CANCEL,
46                                         "Open", GTK_RESPONSE_ACCEPT,
47                                         NULL);
48   g_signal_connect (dialog, "response", G_CALLBACK (open_dialog_response), tv);
49   gtk_widget_show (dialog);
50 }
~~~

- 37-50: `tfe_text_view_open` function.
- 44-47: Creates GtkFileChooserDialog.
The title is "Open file".
Transient parent window is the top-level window of the application, which is given by the caller.
The action is open mode.
The buttons are Cancel and Open.
- 48: connects the "response" signal of the dialog and `open_dialog_response` signal handler.
- 49: Shows the dialog.
- 1-35: `open_dialog_response` signal handler.
- 10-11: If the response from GtkFileChooserDialog is not `GTK_RESPONSE_ACCEPT`, the user has clicked on the "Cancel" button or close button on the header bar.
Then, "open-response" signal is emitted.
The parameter of the signal is `TFE_OPEN_RESPONSE_CANCEL`.
- 12-14: Gets the pointer to the Gfile by `gtk_file_chooser_get_file`.
If it doesn't point GFile, maybe an error has occurred.
Then it emits "open-response" signal with the parameter `TFE_OPEN_RESPONSE_ERROR`.
- 15-23: If an error occurs at file reading, then it decreases the reference count of the Gfile, shows a message dialog to report the error to the user and emits "open-response" signal with the parameter `TFE_OPEN_RESPONSE_ERROR`.
- 24-33: If the file has successfully been read, then the text is inserted to GtkTextBuffer, frees the temporary buffer pointed by `contents` and sets `tv->file` to point the file (no duplication is not necessary).
Then, it emits "open-response" signal with the parameter `TFE_OPEN_RESPONSE_SUCCESS` and emits "change-file" signal.
- 34: destroys GtkFileCooserDialog.

Now let's think about the whole process between the caller and TfeTextView.
It is shown in the following diagram and you would think that it is really complicated.
Because signal is the only way for GtkFileChooserDialog to communicate with others.
In Gtk3, `gtk_dialog_run` function is available.
It simplifies the process.
However, in Gtk4, `gtk_dialog_run` is unavailable any more.

![Caller and TfeTextView](../image/open.png)

1. A caller gets a pointer `tv` to a TfeTextView instance by calling `tfe_text_view_new`.
2. The caller connects the handler (left bottom in the diagram) and the signal "open-response".
3. It calls `tfe_text_view_open` to prompt the user to select a file from GtkFileChooserDialog.
4. The dialog emits a signal and it invokes the handler `open_dialog_response`.
5. The handler reads the file and inserts the text into GtkTextBuffer and emits a signal to inform the status as a response code.
6. The handler out of the TfeTextView receives the signal.

## Getting Gfile

`gtk_text_view_get_file` is a simple function shown as follows.

~~~C
1 GFile *
2 tfe_text_view_get_file (TfeTextView *tv) {
3   g_return_val_if_fail (TFE_IS_TEXT_VIEW (tv), NULL);
4 
5   if (G_IS_FILE (tv->file))
6     return g_file_dup (tv->file);
7   else
8     return NULL;
9 }
~~~

The important thing is to duplicate `tv->file`.
Otherwise, if the caller frees the GFile object, `tv->file` is no more guaranteed to point the GFile.
Another reason to use `g_file_dup` is that GFile isn't thread-safe.
If you use GFile in the different thread, the duplication is necessary.
See [Gio API Reference, g\_file\_dup](https://docs.gtk.org/gio/method.File.dup.html).

## The API document and source file of tfetextview.c

Refer [API document of TfeTextView](tfetextview_doc.md).
It is under the directory `src/tfetextview`.

All the source files are listed in [Section 16](sec16.md).
You can find them under [src/tfe5](../src/tfe5) and [src/tfetextview](../src/tfetextview) directories.


Up: [Readme.md](../Readme.md),  Prev: [Section 12](sec12.md), Next: [Section 14](sec14.md)
