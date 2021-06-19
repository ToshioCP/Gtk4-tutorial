# Functions in TfeTextView

In this section I will explain functions in TfeTextView object.

## tfe.h and tfetextview.h

`tfe.h` is a top header file and it includes `gtk.h` and all the header files.
C source files `tfeapplication.c` and `tfenotebook.c` include `tfe.h` at the beginning.

@@@include
tfe5/tfe.h
@@@

`../tfetextview/tfetextview.h` is a header file which describes the public functions in `tfetextview.c`.

@@@include
tfetextview//tfetextview.h
@@@

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

@@@include
tfetextview/tfetextview.c tfe_text_view_new_with_file tfe_text_view_new
@@@

- 23-25: `tfe_text_view_new` function.
Just returns the value from the function `g_object_new` but casts it to the pointer to GtkWidget.
Initialization is done in `tfe_text_view_init` which is called in the process of `g_object_new` function.
- 1-21: `tfe_text_view_new_with_file` function.
- 3: `g_return_val_if_fail` is described in [Glib API reference](https://developer.gnome.org/glib/stable/glib-Warnings-and-Assertions.html#g-return-val-if-fail).
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

@@@include
tfetextview/tfetextview.c save_file saveas_dialog_response tfe_text_view_save tfe_text_view_saveas
@@@

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

![Saveas process](../image/saveas.png){width=10.7cm height=5.16cm}

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

@@@include
tfetextview/tfetextview.c open_dialog_response tfe_text_view_open
@@@

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

![Caller and TfeTextView](../image/open.png){width=12.405cm height=9.225cm}

1. A caller gets a pointer `tv` to a TfeTextView instance by calling `tfe_text_view_new`.
2. The caller connects the handler (left bottom in the diagram) and the signal "open-response".
3. It calls `tfe_text_view_open` to prompt the user to select a file from GtkFileChooserDialog.
4. The dialog emits a signal and it invokes the handler `open_dialog_response`.
5. The handler reads the file and inserts the text into GtkTextBuffer and emits a signal to inform the status as a response code.
6. The handler out of the TfeTextView receives the signal.

## Getting Gfile

`gtk_text_view_get_file` is a simple function shown as follows.

@@@include
tfetextview/tfetextview.c tfe_text_view_get_file
@@@

The important thing is to duplicate `tv->file`.
Otherwise, if the caller frees the GFile object, `tv->file` is no more guaranteed to point the GFile.
Another reason to use `g_file_dup` is that GFile isn't thread-safe.
If you use GFile in the different thread, the duplication is necessary.
See [Gio API reference](https://developer.gnome.org/gio/stable/GFile.html#g-file-dup).

## The API document and source file of tfetextview.c

@@@if gfm
Refer [API document of TfeTextView](../gfm/tfetextview_doc.md).
It is under the directory `src/tfetextview`.
@@@elif html
Refer [API document of TfeTextView](../html/tfetextview_doc.html).
Its original markdown file is under the directory `src/tfetextview`.
@@@elif latex
Refer API document of TfeTextView in the appendix.
Its original markdown file is under the directory `src/tfetextview`.
@@@end

All the source files are listed in [Section 16](sec16.src.md).
You can find them under [src/tfe5](tfe5) and [src/tfetextview](tfetextview) directories.

