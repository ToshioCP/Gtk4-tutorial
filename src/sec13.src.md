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

@@@include
tfetextview/tfetextview.h
@@@

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

@@@include
tfetextview/tfetextview.c tfe_text_view_new_with_file tfe_text_view_new
@@@

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

@@@include
tfetextview/tfetextview.c save_file
@@@

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

@@@include
tfetextview/tfetextview.c save_dialog_cb
@@@

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

@@@include
tfetextview/tfetextview.c tfe_text_view_save
@@@

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

@@@include
tfetextview/tfetextview.c tfe_text_view_saveas
@@@

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

![Saveas process](../image/saveas.png){width=10.7cm height=5.16cm}

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

@@@include
tfetextview/tfetextview.c open_dialog_cb
@@@

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

@@@include
tfetextview/tfetextview.c tfe_text_view_open
@@@

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

![Caller and TfeTextView](../image/open.png){width=12.405cm height=9.225cm}

1. A caller gets a pointer `tv` to a TfeTextView instance by calling `tfe_text_view_new`.
2. The caller connects the handler (left bottom in the diagram) and the signal "open-response".
3. It calls `tfe_text_view_open` to prompt the user to select a file from the file chooser dialog.
4. When the dialog is closed, the callback `open_dialog_cb` is called.
5. The callback function reads the file and inserts the text into GtkTextBuffer and emits a signal to inform the status as a response code.
6. The handler of the "open-response" signal is invoked and the operation status is given to it as an argument (signal parameter).

## Getting GFile in TfeTextView

You can get the GFile in a TfeTextView instance with `tfe_text_view_get_file`.
It is very simple.

@@@include
tfetextview/tfetextview.c tfe_text_view_get_file
@@@

The important thing is to duplicate `tv->file`.
Otherwise, if the caller frees the GFile object, `tv->file` is no more guaranteed to point the GFile.
Another reason to use `g_file_dup` is that GFile isn't thread-safe.
If you use GFile in the different thread, the duplication is necessary.
See [Gio API Reference -- g\_file\_dup](https://docs.gtk.org/gio/method.File.dup.html).

## The API document and source file of tfetextview.c

@@@if gfm
Refer [API document of TfeTextView](../gfm/tfetextview_doc.md).
It is under the directory `src/tfetextview`.
@@@elif html
Refer [API document of TfeTextView](https://toshiocp.github.io/Gtk4-tutorial/tfetextview_doc.html).
The markdown file is under the directory `src/tfetextview`.
@@@elif latex
Refer API document of TfeTextView in the appendix.
The markdown file is under the directory `src/tfetextview`.
@@@end

You can find all the TfeTextView source codes under [src/tfetextview](tfetextview) directories.

