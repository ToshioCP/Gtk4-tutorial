# Handling Unsaved Text

Up to this point in TFE, pages could be closed or the application quit even if there was unsaved text.
This is a poor design that risks discarding the user's hard work and needs to be improved.
To fix this, we should check for unsaved text beforehand and, if any exists, ask the user whether they want to cancel the closing process.

## Checking for Text Modifications

To check for unsaved text, we use the `gtk_text_buffer_get_modified ()` function from `GtkTextBuffer`.
This returns `TRUE` if the text buffer has been modified, and `FALSE` otherwise.

However, since the buffer is internal to `TfeTextView`, querying `GtkTextBuffer` directly breaks the principle of encapsulation.
Instead, we will create a new instance method in `TfeTextView` to serve as a public interface.

Public functions must be declared in the header file.

@@@if gfm
```C
@@@else
```{.c}
@@@end
gboolean
tfe_text_view_get_modified (TfeTextView *tv);
```

This function queries the buffer, returning `TRUE` if modified and `FALSE` if not.
The corresponding implementation in `tfetextview.c` is shown below.

@@@include
tfe8/tfetextview.c tfe_text_view_get_modified
@@@

## Closing a Page

The current page is closed when its close button is clicked.
Before closing, we check if the page has been modified.
If it has, we prompt the user with an alert dialog; if not, we simply close it.

Because we are using an asynchronous dialog, we need to split this process into two functions:

- One for setting up and displaying the dialog.
- Another for handling the steps after the user makes a selection (the callback handler).

First, let's look at the close action's handler, located in `tfewindow.c`.

@@@include
tfe8/tfewindow.c close_activated
@@@

- 9-12: If the text is unmodified, call `close_page ()` to close the page and return. (This function will be explained later).
- 14-19: Create the alert dialog.
  - 14: Create the dialog and set its title to "Unsaved changes".
  - 15: Set the alert's detailed description.
  - 17: Set the two buttons using the string array prepared on line 7. The final `NULL` serves as the termination marker.
  - 18: Assign the cancel button to index 0. This ensures that pressing the `ESC` key acts exactly like clicking the Cancel button.
  - 19: Assign the default button to index 0. This ensures that pressing the Return key also acts like clicking the Cancel button.
- 21: We use `gtk_alert_dialog_choose` to asynchronously receive the user's selection. The arguments are as follows:
  - `dialog`: The alert dialog instance.
  - `GTK_WINDOW (win)`: The transient parent window. The dialog will be displayed modally on top of this window.
  - `NULL`: No `GCancellable` is set. If provided, this would allow the dialog to be cancelled programmatically from the outside.
  - `page_close_alert_cb`: The callback function, which is invoked after the user operation (dialog selection) is completed or cancelled.
  - `win`: The user data passed into the callback function.
- 22: Free the dialog instance, as it is no longer needed in this function.

Next, let's look at the callback function.

@@@include
tfe8/tfewindow.c page_close_alert_cb
@@@

- 7: Use `gtk_alert_dialog_choose_finish ()` to determine which button was pressed. The return value indicates the index (starting from 0) of the selected button.
- 9-13: If an error occurs, print the message to standard error and return.
- 16-17: If the button at index 1 is selected (which corresponds to "Close Without Saving"), call `close_page ()` to close the tab.

The `close_page ()` function is implemented as follows:

@@@include
tfe8/tfewindow.c close_page
@@@

- 3-4: If two or more pages remain, the current page is removed.
- 5-15: Otherwise (if only one page remains), destroy the window and terminate the program.

However, we must be careful about the order in which the window is destroyed and the dialog is cleaned up.
At the moment the dialog button is pressed and the callback function is invoked, GTK's internal cleanup process for the dialog has not completely finished.
GTK performs this remaining cleanup (such as disconnecting signals from the parent window) *after* the callback function returns.
If we destroy the window immediately inside the callback, the dialog's parent window will no longer exist during GTK's cleanup phase, triggering a `GLib-GObject-CRITICAL` warning.

Therefore, we must wait for all of GTK's internal processes to finish before executing the main window's termination process.
We achieve this using `g_idle_add_once ()`.
This function schedules our destroy operation to run on the main loop once it becomes idle.
By the time it executes, all high-priority operations (like the dialog cleanup) are guaranteed to be finished, allowing the window to be destroyed safely.

## Closing the Window

The "x" button in the top right corner closes the window.
If there is unsaved text when this button is pressed, we must notify the user and let them choose how to proceed, just as we did for a single page.

Since there may be unsaved changes across multiple pages, we will handle it as follows:

1. Intercept the "close-request" signal, which is emitted just before the window closes.
2. Iterate through all pages to create a list of unsaved documents.
3. If there are no unsaved pages, close the window normally.
4. If there are unsaved pages, display the list in an alert dialog and ask the user whether to proceed.
5. Handle the result in a callback function.

### The close-request Signal

The `close-request` signal is defined in `GtkWindow`, and its default handler is a class method.
The following code is an excerpt from `gtkwindow.h`.

@@@if gfm
```C
@@@else
```{.c}
@@@end
struct _GtkWindowClass
{
  GtkWidgetClass parent_class;

  ... ... ...

  /**
   * GtkWindowClass::close_request:
   *
   * Class handler for the [signal@Window::close-request] signal.
   *
   * Returns: Whether the window should be destroyed
   */
  gboolean (* close_request)    (GtkWindow *window);

  ... ... ...
}
```

To use this signal in a subclass, we override the class method rather than using `g_signal_connect` (which is intended for connecting signals **externally**).
This override is performed in the class initialization function.

@@@include
tfe8/tfewindow.c tfe_window_class_init
@@@

- 4: Since the defining class of the "close-request" signal is `GtkWindow`, we cast it to a `GtkWindowClass` pointer and assign it to the `window_class` variable.
- 7: Override the handler.

### The close-request Signal Handler

The code for the handler is as follows:

@@@include
tfe8/tfewindow.c tfe_window_close_request
@@@

- 10: Use a `GString` to generate the description for the alert dialog.
A `GString` is a GLib structure that automatically handles memory allocation as you append text.
It is initialized as an empty string and is perfect for safely building dynamic strings.
- 14-27: Iterate through the pages.
If any text is unsaved, append the page's tab name to `unsaved_list`.
If the page tab lacks a `GtkLabel`, it defaults to displaying a placeholder like "Page 1", so we construct that string in place of a filename.
- 29-34: If there are no unsaved pages, free the `GString`, chain up to the parent class's `close-request` signal handler, and finish.
Since the parent class's handler returns a `gboolean`, we simply return its value directly.
- 36-44: Construct the alert dialog and call `gtk_alert_dialog_choose ()`.
This process is nearly identical to how we constructed the dialog for closing a single page.
- 46-47: Free the `GString` and the dialog instance.
- 50: By returning `TRUE`, we halt the closing process.
The actual window closure will be handled later in the callback function, depending on the user's selection.

### The callback function

The callback function looks like this:

@@@include
tfe8/tfewindow.c window_close_alert_cb
@@@

The logic here is almost identical to the `page_close_alert_cb` explained in the previous section, except that it destroys the entire window at the end instead of closing a single page.
As discussed earlier, we again use `g_idle_add_once ()` to safely wait for the alert dialog's cleanup process to finish.

## Application Quit Process

Quitting the application is triggered by clicking the "quit" menu item.
The corresponding action is `app.quit`, defined in `tfeapplication.c`.

Previously, its handler called the `g_application_quit ()` function, but once invoked, the application termination process cannot be aborted.
To fix this, we will replace it with a routine that closes the main windows instead, allowing the `close-request` handler to intercept the process if there is unsaved text.

@@@include
tfe8/tfeapplication.c quit_activated
@@@

- 6-7: Iterate through the list of top-level windows held by `GtkApplication`, closing all of them with the `gtk_window_close ()` function.
Since this function emits the "close-request" signal, any unsaved text will automatically be caught and handled by our custom handler.
Because TFE only has one main window, iterating through a list like this isn't strictly necessary, but the code is written this way to maintain generality.

## Compiling and Running

All the source files are in the [src/tfe8](tfe8/) directory.
You can build and execute the program as follows.

First, change your current directory to `src/tfe8`.
Then, type below in your command line.

```
$ meson setup _build
$ ninja -C _build
$ _build/tfe
```

Try modifying the page and clicking the close button.
An alert dialog will appear, prompting you to choose either Cancel or Close.
