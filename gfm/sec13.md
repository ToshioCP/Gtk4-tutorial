Up: [README.md](../README.md),  Prev: [Section 12](sec12.md), Next: [Section 14](sec14.md)

# TfeTextView class

The TfeTextView class will be finally completed in this section.
The remaining topic is functions.
TfeTextView functions, which are constructors and instance methods, are described in this section.

The source files are in the directory `src/tfetextview`.
You can get them by downloading the [repository](https://github.com/ToshioCP/Gtk4-tutorial).

## tfetextview.h

The header file `tfetextview.h` provides:

- The type of TfeTextView, which is `TFE_TYPE_TEXT_VIEW`.
- The macro `G_DECLARE_FINAL_TYPE`, the expansion of which includes some useful functions and definitions.
- Constants for the `open-response` signal.
- Public functions of `tfetextview.c`. They are constructors and instance methods.

Therefore, Any programs use TfeTextView needs to include `tfetextview.h`.

~~~C
 1 #pragma once
 2 
 3 #include <gtk/gtk.h>
 4 
 5 #define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type ()
 6 G_DECLARE_FINAL_TYPE (TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView)
 7 
 8 /* "open-response" signal response */
 9 enum TfeTextViewOpenResponseType
10 {
11   TFE_OPEN_RESPONSE_SUCCESS,
12   TFE_OPEN_RESPONSE_CANCEL,
13   TFE_OPEN_RESPONSE_ERROR
14 };
15 
16 GFile *
17 tfe_text_view_get_file (TfeTextView *tv);
18 
19 void
20 tfe_text_view_open (TfeTextView *tv, GtkWindow *win);
21 
22 void
23 tfe_text_view_save (TfeTextView *tv);
24 
25 void
26 tfe_text_view_saveas (TfeTextView *tv);
27 
28 GtkWidget *
29 tfe_text_view_new_with_file (GFile *file);
30 
31 GtkWidget *
32 tfe_text_view_new (void);
~~~

- 1: The preprocessor directive `#pragma once` makes the header file be included only once.
It is non-standard but widely used.
- 3: Includes gtk4 header files.
The header file `gtk4` also has the same mechanism to avoid being included multiple times.
- 5-6: These two lines define TfeTextView type, its class structure and some useful definitions.
  - `TfeTextView` and `TfeTextViewClass` are declared as typedef of C structures.
  - You need to define a structure `_TfeTextView` later.
  - The class structure `_TfeTextViewClass` is defined here. You don't need to define it by yourself.
  - Convenience functions `TFE_TEXT_VIEW ()` for casting and `TFE_IS_TEXT_VIEW` for type check are defined.
- 8-14: A definition of the values of the "open-response" signal parameters.
- 16-32: Declarations of public functions on TfeTextView.

## Constructors

A TfeTextView instance is created with `tfe_text_view_new` or `tfe_text_view_new_with_file`.
These functions are called constructors.

~~~C
GtkWidget *tfe_text_view_new (void);
~~~

It just creates a new TfeTextView instance and returns the pointer to the new instance.

~~~C
GtkWidget *tfe_text_view_new_with_file (GFile *file);
~~~

It is given a Gfile object as an argument and it loads the file into the GtkTextBuffer instance, then returns the pointer to the new instance.
The argument `file` is owned by the caller and the function doesn't change it.
If an error occurs during the creation process, NULL will be returned.

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
13   tv = tfe_text_view_new();
14   tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
15   gtk_text_buffer_set_text (tb, contents, length);
16   TFE_TEXT_VIEW (tv)->file = g_file_dup (file);
17   gtk_text_buffer_set_modified (tb, FALSE);
18   g_free (contents);
19   return tv;
20 }
21 
22 GtkWidget *
23 tfe_text_view_new (void) {
24   return GTK_WIDGET (g_object_new (TFE_TYPE_TEXT_VIEW, "wrap-mode", GTK_WRAP_WORD_CHAR, NULL));
25 }
~~~

- 22-25: `tfe_text_view_new` function.
Just returns the value from the function `g_object_new` but casts it to the pointer to GtkWidget.
The function `g_object_new` creates any instances of its descendant class.
The arguments are the type of the class, property list and NULL, which is the end mark of the property list.
TfeTextView "wrap-mode" property has GTK\_WRAP\_WORD\_CHAR as the default value.
- 1-20: `tfe_text_view_new_with_file` function.
- 3: `g_return_val_if_fail` is described in [GLib API Reference -- g\_return\_val\_if\_fail](https://docs.gtk.org/glib/func.return_val_if_fail.html).
And also [GLib API Reference -- Message Logging](https://docs.gtk.org/glib/logging.html).
It tests whether the argument `file` is a pointer to GFile.
If it's true, the program goes on to the next line.
If it's false, it returns NULL (the second argument) immediately.
And at the same time it logs out the error message (usually the log is outputted to stderr or stdout).
This function is used to check the programmer's error.
If an error occurs, the solution is usually to change the (caller) program and fix the bug.
You need to distinguish programmer's errors and runtime errors.
You shouldn't use this function to find runtime errors.
- 10-11: Reads the file. If an error occurs, NULL is returned.
- 13: Calls the function `tfe_text_view_new`.
The function creates TfeTextView instance and returns the pointer to the instance.
- 14: Gets the pointer to the GtkTextBuffer instance corresponds to `tv`.
The pointer is assigned to `tb`
- 15: Assigns the contents read from the file to `tb`.
- 16: Duplicates `file` and sets `tv->file` to point it.
GFile is *not* thread safe.
The duplication makes sure that the GFile instance of `tv` keeps the file information even if the original one is changed by other thread.
- 17: The function `gtk_text_buffer_set_modified (tb, FALSE)` sets the modification flag of `tb` to FALSE.
The modification flag indicates that the contents has been modified.
It is used when the contents are saved.
If the modification flag is FALSE, it doesn't need to save the contents.
- 18: Frees the memories pointed by `contents`.
- 19: Returns `tv`, which is a pointer to the newly created TfeTextView instance.
If an error happens, NULL is returned.

## Save and saveas functions

Save and saveas functions write the contents in the GtkTextBuffer to a file.

~~~C
void tfe_text_view_save (TfeTextView *tv)
~~~

The function `tfe_text_view_save` writes the contents in the GtkTextBuffer to a file specified by `tv->file`.
If `tv->file` is NULL, then it shows file chooser dialog and prompts the user to choose a file to save.
Then it saves the contents to the file and sets `tv->file` to point the GFile instance for the file.

~~~C
void tfe_text_view_saveas (TfeTextView *tv)
~~~

The function `saveas` shows a file chooser dialog and prompts the user to select a existed file or specify a new file to save.
Then, the function changes `tv->file` and save the contents to the specified file.
If an error occurs, it is shown to the user through the alert dialog.
The error is managed only in the TfeTextView and no information is notified to the caller.

### save\_file function

~~~C
 1 static gboolean
 2 save_file (GFile *file, GtkTextBuffer *tb, GtkWindow *win) {
 3   GtkTextIter start_iter;
 4   GtkTextIter end_iter;
 5   char *contents;
 6   gboolean stat;
 7   GtkAlertDialog *alert_dialog;
 8   GError *err = NULL;
 9 
10   gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
11   contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
12   stat = g_file_replace_contents (file, contents, strlen (contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, &err);
13   if (stat)
14     gtk_text_buffer_set_modified (tb, FALSE);
15   else {
16     alert_dialog = gtk_alert_dialog_new ("%s", err->message);
17     gtk_alert_dialog_show (alert_dialog, win);
18     g_object_unref (alert_dialog);
19     g_error_free (err);
20   }
21   g_free (contents);
22   return stat;
23 }
~~~

- The function `save_file` is called from `saveas_dialog_response` and `tfe_text_view_save`.
This function saves the contents of the buffer to the file given as an argument.
If error happens, it displays an error message.
So, a caller of this function don't need to take care of errors.
The class of this function is `static`.
Therefore, only functions in this file (`tfetextview.c`) call this function.
Such static functions usually don't have `g_return_val_if_fail` functions.
- 10-11: Gets the text contents from the buffer.
- 12: The function `g_file_replace_contents` writes the contents to the file and returns the status (true = success/ false = fail).
It has many parameters, but some of them are almost always given the same values.
  - GFile* file: GFile to which the contents are saved.
  - const char* contents: contents to be saved. The string is owned by the caller.
  - gsize length: the length of the contents
  - const char* etag: entity tag. It is usually NULL.
  - gboolean make_backup: true to make a backup if the file exists. false not to make it. the file will be overwritten.
  - GFileCreateFlags flags: usually `G_FILE_CREATE_NONE` is fine.
  - char** new_etag: new entity tag. It is usually NULL.
  - GCancellable* cancellable: If a cancellable instance is set, the other thread can cancel this operation. it is usually NULL.
  - GError** error: If error happens, GError will be set.
- 13,14: If no error happens, set the modified flag to be FALSE.
This means that the buffer is not modified since it has been saved.
- 16-19: If it fails to save the contents, an error message will be displayed.
- 16: Creates an alert dialog. The parameters are printf-like format string followed by values to insert into the string.
GtkAlertDialog is available since version 4.10.
If your version is older than 4.10, use GtkMessageDialog instead.
GtkMessageDialog is deprecated since version 4.10.
- 17: Show the alert dialog. The parameters are the dialog and the transient parent window.
This allows window managers to keep the dialog on top of the parent window, or center the dialog over the parent window.
It is possible to give no parent window to the dialog by giving NULL as the argument.
However, it is encouraged to give parents to dialogs.
- 18: Releases the dialog.
- 19: Frees the GError struct pointed by `err` with `g_error_free` function.
- 21: Frees `contents`.
- 22: Returns the status to the caller.

### save\_dialog\_cb function

~~~C
 1 static void
 2 save_dialog_cb(GObject *source_object, GAsyncResult *res, gpointer data) {
 3   GtkFileDialog *dialog = GTK_FILE_DIALOG (source_object);
 4   TfeTextView *tv = TFE_TEXT_VIEW (data);
 5   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
 6   GFile *file;
 7   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
 8   GError *err = NULL;
 9   GtkAlertDialog *alert_dialog;
10 
11   if (((file = gtk_file_dialog_save_finish (dialog, res, &err)) != NULL) && save_file(file, tb, GTK_WINDOW (win))) {
12     // The following is complicated. The comments here will help your understanding
13     // G_IS_FILE(tv->file) && tv->file == file  => nothing to do
14     // G_IS_FILE(tv->file) && tv->file != file  => unref(tv->file), tv->file=file, emit change_file signal
15     // tv->file==NULL                           =>                  tv->file=file, emit change_file signal
16     if (! (G_IS_FILE (tv->file) && g_file_equal (tv->file, file))) {
17       if (G_IS_FILE (tv->file))
18         g_object_unref (tv->file);
19       tv->file = file; // The ownership of 'file' moves to TfeTextView.
20       g_signal_emit (tv, tfe_text_view_signals[CHANGE_FILE], 0);
21     }
22   }
23   if (err) {
24     alert_dialog = gtk_alert_dialog_new ("%s", err->message);
25     gtk_alert_dialog_show (alert_dialog, GTK_WINDOW (win));
26     g_object_unref (alert_dialog);
27     g_clear_error (&err);
28   }
29 }
~~~

- The function `save_dialog_cb` is a call back function that is given to the `gtk_file_dialog_save` function as an argument.
The `gtk_file_dialog_save` shows a file chooser dialog to the user.
The user chooses or types a filename and clicks on the `Save` button or just clicks on the `Cancel` button.
Then the call back function is called with the result.
This is the general way in GIO to manage asynchronous operations.
A pair of functions `g_data_input_stream_read_line_async` and `g_data_input_stream_read_line_finish` are one example.
These functions are thread-safe.
The arguments of `save_dialog_cb` are:
  - GObject *source_object: The GObject instance that the operation was started with.
It is actually the GtkFileDialog instance that is shown to the user.
However, the call back function is defined as `AsyncReadyCallback`, which is a general call back function for an asynchronous operation.
So the type is GObject and you need to cast it to GtkFileDialog later.
  - GAsyncResult *res: The result of the asynchronous operation.
It will be given to the `gtk_dialog_save_finish` function.
  - gpointer data: A user data set in the `gtk_dialog_save` function.
- 11: Calls `gtk_dialog_save_finish`.
It is given the result `res` as an argument and returns a pointer to a GFile object the user has chosen.
If the user has canceled or an error happens, it returns NULL, creates a GError object and sets `err` to point it.
If `gtk_dialog_save_finish` returns a GFile, the function `save_file` is called.
- 12-21: If the file is successfully saved, these lines are executed.
See the comments, line 12-15, for the details.
- 23-28: If an error happens, show the error message through the alert dialog.

### tfe\_text\_view\_save function

~~~C
 1 void
 2 tfe_text_view_save (TfeTextView *tv) {
 3   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
 4 
 5   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
 6   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
 7 
 8   if (! gtk_text_buffer_get_modified (tb))
 9     return; /* no need to save it */
10   else if (tv->file == NULL)
11     tfe_text_view_saveas (tv);
12   else
13     save_file (tv->file, tb, GTK_WINDOW (win));
14 }
~~~

- The function `tfe_text_view_save` writes the contents to the `tv->file` file.
It calls `tfe_text_view_saveas` or `save_file`.
- 1-3: The function is public, i.e. it is open to the other objects.
So, it doesn't have `static` class.
Public functions should check the parameter type with `g_return_if_fail` function.
If `tv` is not a pointer to a TfeTextView instance, then it logs an error message and immediately returns.
This function is similar to `g_return_val_if_fail`, but no value is returned because `tfe_text_view_save` doesn't return a value (void).
- 5-6: GtkTextBuffer `tb` and GtkWidget (GtkWindow) `win` are set.
The function `gtk_widget_get_ancestor (widget, type)` returns the first ancestor of the widget with the type, which is a GType.
The parent-child relationship here is the one for widgets, not classes.
More precisely, the returned widget's type is the `type` or a descendant object type of the `type`.
Be careful, the "descendant object" in the previous sentence is *not* "descendant widget".
For example, the type of GtkWindow is `GTK_TYPE_WINDOW` and the one of TfeTextView is `TFE_TYPE_TEXT_VIEW`.
The top level window may be a GtkApplicationWindow, but it is a descendant of GtkWindow.
Therefore, `gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW)` possibly returns GtkWindow or GtkApplicationWindow.
- 8-9: If the buffer hasn't modified, it doesn't need to be saved.
- 10-11: If `tv->file` is NULL, which means no file has given yet, it calls `tfe_text_view_saveas` to prompt a user to select a file and save the contents.
- 12-13: Otherwise, it calls `save_file` to save the contents to the file `tv->file`.

### tfe\_text\_view\_saveas function

~~~C
 1 void
 2 tfe_text_view_saveas (TfeTextView *tv) {
 3   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
 4 
 5   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
 6   GtkFileDialog *dialog;
 7 
 8   dialog = gtk_file_dialog_new ();
 9   gtk_file_dialog_save (dialog, GTK_WINDOW (win), NULL, save_dialog_cb, tv);
10   g_object_unref (dialog);
11 }
~~~

The function `tfe_text_view_saveas` shows a file chooser dialog and prompts the user to choose a file and save the contents.

- 1-3: Check the type of `tv` because the function is public.
- 6: GtkWidget `win` is set to the window which is an ancestor ot `tv`.
- 8: Creates a GtkFileDialog instance.
GtkFileDialog is available since version 4.10.
If your Gtk version is older than 4.10, use GtkFileChooserDialog instead.
GtkFileChooserDialog is deprecated since version 4.10.
- 9: Calls `gtk_file_dialog_save` function. The arguments are:
  - dialog: GtkFileDialog.
  - GTK_WINDOW (win): transient parent window.
  - NULL: NULL means no cancellable object.
If you put a cancellable object here, you can cancel the operation by other thread.
In many cases, it is NULL.
See [GCancellable](https://docs.gtk.org/gio/class.Cancellable.html) for further information.
  - `save_dialog_cb`: A callback to call when the operation is complete.
The type of the pointer to the callback function is [GAsyncReadyCallback](https://docs.gtk.org/gio/callback.AsyncReadyCallback.html).
If a cancellable object is given and the operation is cancelled, the callback won't be called.
  - `tv`: This is an optional user data which is gpointer type.
It is used in the callback function.
- 10: Releases the GtkFileDialog instance because it is useless anymore.

This function just shows the file chooser dialog.
The rest of the operation is done by the callback function.

![Saveas process](../image/saveas.png)

## Open related functions

Open function shows a file chooser dialog to a user and prompts them to choose a file.
Then it reads the file and puts the text into GtkTextBuffer.

~~~C
void tfe_text_view_open (TfeTextView *tv, GtkWindow *win);
~~~

The parameter `win` is the transient window.
A file chooser dialog will be shown at the center of the window.

This function may be called just after `tv` has been created.
In that case, `tv` has not been incorporated into the widget hierarchy.
Therefore it is impossible to get the top-level window from `tv`.
That's why the function needs `win` parameter.

This function is usually called when the buffer of `tv` is empty.
However, even if the buffer is not empty, `tfe_text_view_open` doesn't treat it as an error.
If you want to revert the buffer, calling this function is appropriate.

Open and read process is divided into two phases.
One is creating and showing a file chooser dialog and the other is the callback function.
The former is `tfe_text_view_open` and the latter is `open_dialog_cb`.

### open\_dialog\_cb function

~~~C
 1 static void
 2 open_dialog_cb (GObject *source_object, GAsyncResult *res, gpointer data) {
 3   GtkFileDialog *dialog = GTK_FILE_DIALOG (source_object);
 4   TfeTextView *tv = TFE_TEXT_VIEW (data);
 5   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
 6   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
 7   GFile *file;
 8   char *contents;
 9   gsize length;
10   gboolean file_changed;
11   GtkAlertDialog *alert_dialog;
12   GError *err = NULL;
13 
14   if ((file = gtk_file_dialog_open_finish (dialog, res, &err)) != NULL
15       && g_file_load_contents (file, NULL, &contents, &length, NULL, &err)) {
16     gtk_text_buffer_set_text (tb, contents, length);
17     g_free (contents);
18     gtk_text_buffer_set_modified (tb, FALSE);
19     // G_IS_FILE(tv->file) && tv->file == file => unref(tv->file), tv->file=file, emit response with SUCCESS
20     // G_IS_FILE(tv->file) && tv->file != file => unref(tv->file), tv->file=file, emit response with SUCCESS, emit change-file
21     // tv->file==NULL =>                                           tv->file=file, emit response with SUCCESS, emit change-file
22     // The order is important. If you unref tv->file first, you can't compare tv->file and file anymore.
23     // And the signals are emitted after new tv->file is set. Or the handler can't catch the new file.
24     file_changed = (G_IS_FILE (tv->file) && g_file_equal (tv->file, file)) ? FALSE : TRUE;
25     if (G_IS_FILE (tv->file))
26       g_object_unref (tv->file);
27     tv->file = file; // The ownership of 'file' moves to TfeTextView
28     if (file_changed)
29       g_signal_emit (tv, tfe_text_view_signals[CHANGE_FILE], 0);
30     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_SUCCESS);
31   } else {
32     if (err->code == GTK_DIALOG_ERROR_DISMISSED) // The user canceled the file chooser dialog
33       g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_CANCEL);
34     else {
35       alert_dialog = gtk_alert_dialog_new ("%s", err->message);
36       gtk_alert_dialog_show (alert_dialog, GTK_WINDOW (win));
37       g_object_unref (alert_dialog);
38       g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_ERROR);
39     }
40     g_clear_error (&err);
41   }
42 }
~~~

This function is similar to `save_dialog_cb`.
Both are callback functions on a GtkFileDialog object.

- 2: It has three parameters like `save_dialog_cb`.
They are:
  - GObject *source_object: The GObject instance that the operation was started with.
It is actually the GtkFileDialog instance that is shown to the user.
It will be casted to GtkFileDialog later.
  - GAsyncResult *res: The result of the asynchronous operation.
It will be given to the `gtk_dialog_open_finish` function.
  - gpointer data: A user data set in the `gtk_dialog_open` function.
It is actually a TfeTextView instance and it will be casted to TfeTextView later.
- 14: The function `gtk_file_dialog_open_finish` returns a GFile object if the operation has succeeded.
Otherwise it returns NULL.
- 16-30: If the user selects a file and the file has successfully been read, the codes from 16 to 30 will be executed.
- 16-18: Sets the buffer of `tv` with the text read from the file. And frees `contents`.
Then sets the modified status to false.
- 19-30: The codes are a bit complicated.
See the comments.
If the file (`tv->file`) is changed, "change-file" signal is emitted.
The signal "open-response" is emitted with the parameter `TFE_OPEN_RESPONSE_SUCCESS`.
- 31-41: If the operation failed, the codes from 31 to 41 will be executed.
- 32-33: If the error code is `GTK_DIALOG_ERROR_DISMISSED`, it means that the user has clicked on the "Cancel" button or close button on the header bar.
Then, "open-response" signal is emitted with the parameter `TFE_OPEN_RESPONSE_CANCEL`.
The Dialog error is described [here](https://docs.gtk.org/gtk4/error.DialogError.html) in the GTK API reference.
- 35-38: If another error occurs, it shows an alert dialog to report the error and emits "open-response" signal with the parameter `TFE_OPEN_RESPONSE_ERROR`.
- 40: Clears the error structure.

### tfe\_text\_view\_open function

~~~C
 1 void
 2 tfe_text_view_open (TfeTextView *tv, GtkWindow *win) {
 3   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
 4   // 'win' is used for a transient window of the GtkFileDialog.
 5   // It can be NULL.
 6   g_return_if_fail (GTK_IS_WINDOW (win) || win == NULL);
 7 
 8   GtkFileDialog *dialog;
 9 
10   dialog = gtk_file_dialog_new ();
11   gtk_file_dialog_open (dialog, win, NULL, open_dialog_cb, tv);
12   g_object_unref (dialog);
13 }
~~~

- 3-6: Check the type of the arguments `tv` and `win`.
Public functions always need to check the arguments.
- 10: Creates a GtkFileDialog instance.
- 11: Calls `gtk_file_dialog_open`. The arguments are:
  - `dialog`: the GtkFileDialog instance
  - `win`: the transient window for the file chooser dialog
  - `NULL`: NULL means no cancellable object
  - `open_dialog_cb`: callback function
  - `tv`: user data which is used in the callback function
- 12: Releases the dialog instance because it is useless anymore.

The whole process between the caller and TfeTextView is shown in the following diagram.
It is really complicated.
Because `gtk_file_dialog_open` can't return the status of the operation.

![Caller and TfeTextView](../image/open.png)

1. A caller gets a pointer `tv` to a TfeTextView instance by calling `tfe_text_view_new`.
2. The caller connects the handler (left bottom in the diagram) and the signal "open-response".
3. It calls `tfe_text_view_open` to prompt the user to select a file from the file chooser dialog.
4. When the dialog is closed, the callback `open_dialog_cb` is called.
5. The callback function reads the file and inserts the text into GtkTextBuffer and emits a signal to inform the status as a response code.
6. The handler of the "open-response" signal is invoked and the operation status is given to it as an argument (signal parameter).

## Getting GFile in TfeTextView

You can get the GFile in a TfeTextView instance with `tfe_text_view_get_file`.
It is very simple.

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
See [Gio API Reference -- g\_file\_dup](https://docs.gtk.org/gio/method.File.dup.html).

## The API document and source file of tfetextview.c

Refer [API document of TfeTextView](tfetextview_doc.md).
It is under the directory `src/tfetextview`.

You can find all the TfeTextView source codes under [src/tfetextview](../src/tfetextview) directories.


Up: [README.md](../README.md),  Prev: [Section 12](sec12.md), Next: [Section 14](sec14.md)
