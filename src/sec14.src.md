# File Dialogs and the GIO Asynchronous Model

## What is Asynchronous Operation?

GUI applications, like those built with GTK, run in a continuous loop called the **main loop**.
This loop constantly waits for user events—such as mouse clicks or key presses—and redraws the screen as needed. 

Because the main loop must keep running quickly to keep the application responsive, you should never perform heavy or time-consuming tasks (like reading a large file or downloading data from the network) **directly inside it**. 
If you use a **synchronous** function for a slow I/O operation, the main loop will block and wait for the operation to finish.
During this time, your application will freeze, and the user will not be able to click or type anything.

To solve this problem, we use **asynchronous** operations. 
When you call an asynchronous function, it immediately requests the slow operation to start in the background and instantly returns control to the main loop.
The application remains responsive.
Once the background operation is complete, the application is notified so it can handle the result.

## The GIO Asynchronous Model

GIO is the underlying I/O library used by GTK.
It provides a consistent, powerful model for handling asynchronous operations using a **Callback** mechanism.

@@@if pdf
![Async-Finish Pattern](images/Async_pattern.png){width=13cm height=9cm}
@@@elif html
![Async-Finish Pattern](images/Async_pattern.png){width=75% height=75%}
@@@else
![Async-Finish Pattern](images/Async_pattern.png)
@@@end

The GIO asynchronous model always follows a strict pattern using a pair of functions: `_async` and `_finish`.

1. Start the operation (`_async`):
You call a function ending in `_async` (e.g., `do_something_async`).
You pass your parameters, along with a pointer to a callback function that you have written.
2. Wait in the background:
The `_async` function returns immediately.
The main loop continues running smoothly while GIO does the hard work in the background.
3. Execute the callback:
When the operation completes (either successfully or with an error), GIO automatically calls your callback function.
4. Get the result (`_finish`):
Inside your callback function, you must call the corresponding `_finish` function (e.g., `do_something_finish`).
This function collects the actual result (like a `GFile`) or any `GError` that occurred during the process.

## Getting a File Name with GtkFileDialog

In GTK4, `GtkFileDialog` is the modern way to ask the user to choose a file.
It uses the native file chooser dialog of the user's operating system or desktop environment and is designed entirely around the GIO asynchronous model.

There are two main operations when dealing with files:

- Opening a file: Uses `gtk_file_dialog_open_async` and `gtk_file_dialog_open_finish`.
- Saving a file: Uses `gtk_file_dialog_save_async` and `gtk_file_dialog_save_finish`.

Here is a practical code example of how to show an "Open" file dialog and handle the user's selection using the `_async` and `_finish` pattern.

### The Callback Function

First, we define the callback function that will run when the user selects a file or cancels the dialog.

@@@if gfm
```C
@@@else
```{.c}
@@@end
static void
open_dialog_cb (GObject *source_object, GAsyncResult *res, gpointer data) {
  /* The source_object is the GtkFileDialog that started the operation */
  GtkFileDialog *dialog = GTK_FILE_DIALOG (source_object);
  GError *err = NULL;
  GFile *file;

  /* Call the _finish function to get the result of the async operation */
  /* If an error occurs during the interaction process, the function returns NULL. */
  file = gtk_file_dialog_open_finish (dialog, res, &err);

  if (file != NULL) {
    /* The user selected a file successfully */
    g_print ("File selected: %s\n", g_file_get_path (file));
    
    /* We own the GFile, so we must unreference it when done */
    g_object_unref (file);
  } else {
    /* The user cancelled the dialog or an error occurred */
    g_print ("Dialog cancelled or error: %s\n", err->message);
    g_error_free (err);
  }
}
```

### Setting up and showing the dialog

Next, we write the function that creates the dialog and starts the asynchronous operation.

@@@if gfm
```C
@@@else
```{.c}
@@@end
static void
show_open_dialog (GtkWindow *parent_window) {
  /* Create a new GtkFileDialog instance */
  GtkFileDialog *dialog = gtk_file_dialog_new ();

  /* Start the asynchronous open operation.
   * We pass 'open_dialog_cb' as the callback function. */
  gtk_file_dialog_open_async (dialog, 
                              parent_window, 
                              NULL,           /* GCancellable (NULL means we won't cancel it programmatically) */
                              open_dialog_cb, /* The callback function to run when done */
                              NULL);          /* User data to pass to the callback (NULL here) */
}
```

If you wanted to implement a "Save As" feature instead, the structure would be exactly the same.
You would simply change `gtk_file_dialog_open_async` to `gtk_file_dialog_save_async`, and inside the callback, you would use `gtk_file_dialog_save_finish`.

With this pattern, your application remains fully responsive while the user browses their folders, and you safely receive the selected GFile once they are ready.

Keep in mind that GIO uses this exact same asynchronous pattern for almost all input and output operations, such as reading and writing files.
Therefore, mastering this `_async` and `_finish` pattern is an absolutely essential skill for whenever you write any I/O-related code in the future.

## Clicked Signal Handler on Open Button

Now, we will look at the TFE (Text File Editor) main program `tfeapplication.c`.
First, we will focus on the "clicked" signal handler on "Open" button.
The handler shows the file dialog then reads the file to the newly created TfeTextView instance.

@@@include
tfe5/tfeapplication.c open_cb
@@@

This callback function is called when the "Open" button is clicked.
This program just create a file dialog and call `gtk_file_dialog_open` with the callback function `open_dialog_cb`.

@@@include
tfe5/tfeapplication.c open_dialog_cb
@@@

- 10-14: Calls `gtk_file_dialog_open_finish` to get a GFile.
If the return value is NULL, it calls `tfe_error_alert` to show the error dialog.
- 15-16: Creates a TfeTextView instance and set its file using `tfe_text_view_set_file`.
- 17-23: Calls `tfe_text_view_read` to read the file content into the buffer.
If an error occurs, it calls `tfe_error_alert` to show the error dialog.
Then, it frees GError and `tv`.
- 24: Calls `notebook_page_build` to build a new page in the GtkNoteBook.
The function will be shown later.
- 25: Frees `file`

The function `tfe_error_alert` is as follows.
This is a private function.

@@@include
tfe5/tfeapplication.c tfe_error_alert
@@@

### Page build Utility

A private function `notebook_page_build` builds a new page in the GtkNotebook.

@@@include
tfe5/tfeapplication.c notebook_page_build
@@@

- 13-20: Creates a GtkLabel instance with the UTF-8 based filename and assigns it to the variable `lab` if `file` points to a GFile instance (non-NULL).
Otherwise NULL is assigned to `lab`.
- 22-23: Sets the child of the scrolled window to `tv`.
Then the function `gtk_notebook_append_page` appends `scr` and `lab` as the child and tab of the GtkNotebook `nb`.
- 24-25: Sets the "tab-expand" property of the notebook page to TRUE using `g_object_set`
- 26: Sets the current page to the new page.

## Clicked Signal Handler on Save Button

Next, we will go on to the "clicked" signal handler on "Save" button.

@@@include
tfe5/tfeapplication.c save_cb
@@@


- 11-19: If the GtkNotebook has no page, it shows the alert dialog with "No page to save." and returns.
- 24-28: If the TfeTextView holds a GFile, it just writes the content of the buffer to the file.
- 30-32: If the TfeTextView does not hold a GFile, it shows a Filedialog. The callback functio is `save_dialog_cb`.

The `save_dialog_cb` function is shown below.

@@@include
tfe5/tfeapplication.c save_dialog_cb
@@@

- 17-23: Calls `gtk_file_dialog_save_finish` to get a GFile.
If the return value is NULL and the user did not cancel the operation (`GTK_DIALOG_ERROR_DISMISSED`), it calls `tfe_error_alert` to show the error dialog.
- 25: Sets the file of `tv` to the GFile that is returned by `gtk_file_dialog_save_finish`.
- 27-36: Sets the tab of the notebook page to the new filename.
- 38-41: Calls `tfe_text_view_write` to write the content of the buffer to the file.
If an error occurs, it calls `tfe_error_alert` to show the error dialog.
Then, it frees GError.
