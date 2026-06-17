Up: [README.md](../README.md),  Prev: [Section 21](sec21.md), Next: [Section 23](sec23.md)

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

```C
gboolean
tfe_text_view_get_modified (TfeTextView *tv);
```

This function queries the buffer, returning `TRUE` if modified and `FALSE` if not.
The corresponding implementation in `tfetextview.c` is shown below.

```c
gboolean
tfe_text_view_get_modified (TfeTextView *tv) {
  g_return_val_if_fail (TFE_IS_TEXT_VIEW (tv), FALSE);

  GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));

  return gtk_text_buffer_get_modified (tb);
}
```

## Closing a Page

The current page is closed when its close button is clicked.
Before closing, we check if the page has been modified.
If it has, we prompt the user with an alert dialog; if not, we simply close it.

Because we are using an asynchronous dialog, we need to split this process into two functions:

- One for setting up and displaying the dialog.
- Another for handling the steps after the user makes a selection (the callback handler).

First, let's look at the close action's handler, located in `tfewindow.c`.

```c
static void
close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  GtkWidget *scr = gtk_notebook_get_nth_page (win->nb, gtk_notebook_get_current_page (win->nb));
  GtkWidget *tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
  GtkAlertDialog *dialog;
  const char *buttons[] = {"Cancel", "Close Without Saving", NULL};

    if (!tfe_text_view_get_modified (TFE_TEXT_VIEW (tv))) {
      close_page (win);
      return;
    }
  
  dialog = gtk_alert_dialog_new ("Unsaved changes");
  gtk_alert_dialog_set_detail (dialog, "Are you sure to close this page without saving?");

  gtk_alert_dialog_set_buttons (dialog, buttons);
  gtk_alert_dialog_set_cancel_button (dialog, 0); /* cancel button */
  gtk_alert_dialog_set_default_button (dialog, 0); /* default focus */

  gtk_alert_dialog_choose (dialog, GTK_WINDOW (win), NULL, page_close_alert_cb, win);
  g_object_unref (dialog);
}
```

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

```c
static void
page_close_alert_cb (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  GtkAlertDialog *dialog = GTK_ALERT_DIALOG (source_object);
  GtkWidget *win;
  GError *err = NULL;
  int response = gtk_alert_dialog_choose_finish (dialog, res, &err);

  if (err != NULL) {
      g_warning ("Alert Dialog Error: %s", err->message);
      g_error_free (err);
      return;
  }
  
  win = GTK_WIDGET (user_data); /* This assignment must be done after the error check */
  if (response == 1) { /* Close Without Saving */
      close_page (TFE_WINDOW (win));
  }
  /* Cancel => do nothing*/
}
```

- 7: Use `gtk_alert_dialog_choose_finish ()` to determine which button was pressed. The return value indicates the index (starting from 0) of the selected button.
- 9-13: If an error occurs, print the message to standard error and return.
- 16-17: If the button at index 1 is selected (which corresponds to "Close Without Saving"), call `close_page ()` to close the tab.

The `close_page ()` function is implemented as follows:

```c
static void
close_page (TfeWindow *win) {
  if (gtk_notebook_get_n_pages (win->nb) >= 2)
    gtk_notebook_remove_page (win->nb, gtk_notebook_get_current_page (win->nb));
  else {  /* If the page is the last one or no page exists */
    /*
    * Delay the window destruction until the main loop becomes idle.
    * This function must be called after all other pending operations are completed.
    * For example, we need to wait for the GtkAlertDialog's asynchronous
    * cleanup and internal signal disconnections to safely finish.
    * If the window is destroyed immediately here, it will cause 
    * a "GLib-GObject-CRITICAL" error.
    */
    g_idle_add_once ((GSourceOnceFunc) gtk_window_destroy, win);
  }
}
```

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

```C
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

```c
static void
tfe_window_class_init (TfeWindowClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWindowClass *window_class = GTK_WINDOW_CLASS (class);

  object_class->dispose = tfe_window_dispose;
  window_class->close_request = tfe_window_close_request;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe8/tfewindow.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, btnm);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, nb);
}
```

- 4: Since the defining class of the "close-request" signal is `GtkWindow`, we cast it to a `GtkWindowClass` pointer and assign it to the `window_class` variable.
- 7: Override the handler.

### The close-request Signal Handler

The code for the handler is as follows:

```c
static gboolean
tfe_window_close_request (GtkWindow *window)
{
  TfeWindow *win = TFE_WINDOW (window);
  GtkWidget *scr;
  GtkWidget *tv;
  const char *tab_label;
  int n_pages = gtk_notebook_get_n_pages (win->nb);
  int unsaved_count = 0;
  GString *unsaved_list = g_string_new ("");
  GtkAlertDialog *dialog;
  const char *buttons[] = {"Cancel", "Close Window Without Saving", NULL};

  for (int i = 0; i < n_pages; i++) {
    scr = gtk_notebook_get_nth_page (win->nb, i);
    tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));

    if (tfe_text_view_get_modified (TFE_TEXT_VIEW (tv))) {
      unsaved_count++;
      tab_label = gtk_notebook_get_tab_label_text (win->nb, scr);
      if (tab_label != NULL) {
        g_string_append_printf (unsaved_list, "- %s\n", tab_label);
      } else {
        g_string_append_printf (unsaved_list, "- Page %d\n", i + 1);
      }
    }
  }

  if (unsaved_count == 0) {
    g_string_free (unsaved_list, TRUE);

    /* Chain up to the parent's close_request handler */
    return GTK_WINDOW_CLASS (tfe_window_parent_class)->close_request (window);
  }

  dialog = gtk_alert_dialog_new (
    unsaved_count == 1 ? "1 unsaved change" : "%d unsaved changes", 
    unsaved_count
  );
  gtk_alert_dialog_set_detail (dialog, unsaved_list->str);
  gtk_alert_dialog_set_buttons (dialog, buttons);
  gtk_alert_dialog_set_cancel_button (dialog, 0); 
  gtk_alert_dialog_set_default_button (dialog, 0); 
  gtk_alert_dialog_choose (dialog, GTK_WINDOW (win), NULL, window_close_alert_cb, win);

  g_string_free (unsaved_list, TRUE);
  g_object_unref (dialog);

  /* Stop the close request */
  return TRUE;
}
```

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

```c
static void
window_close_alert_cb (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  GtkAlertDialog *dialog = GTK_ALERT_DIALOG (source_object);
  GtkWidget *win;
  GError *err = NULL;
  
  int response = gtk_alert_dialog_choose_finish (dialog, res, &err);

  if (err != NULL) {
      g_warning ("Alert Dialog Error: %s", err->message);
      g_error_free (err);
      return; 
  }

  win = GTK_WIDGET (user_data);
  if (response == 1) { /* Close Without Saving */
      g_idle_add_once ((GSourceOnceFunc) gtk_window_destroy, win);
  }
}
```

The logic here is almost identical to the `page_close_alert_cb` explained in the previous section, except that it destroys the entire window at the end instead of closing a single page.
As discussed earlier, we again use `g_idle_add_once ()` to safely wait for the alert dialog's cleanup process to finish.

## Application Quit Process

Quitting the application is triggered by clicking the "quit" menu item.
The corresponding action is `app.quit`, defined in `tfeapplication.c`.

Previously, its handler called the `g_application_quit ()` function, but once invoked, the application termination process cannot be aborted.
To fix this, we will replace it with a routine that closes the main windows instead, allowing the `close-request` handler to intercept the process if there is unsaved text.

```c
static void
quit_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkApplication *app = GTK_APPLICATION (user_data);
  
  GList *windows = gtk_application_get_windows (app);
  /*
  * Closing a window removes it from the application's internal list.
  * To avoid mutating the list while iterating over it, we must use a copy.
  */
  GList *list_copy = g_list_copy (windows); 

  for (GList *l = list_copy; l != NULL; l = l->next) {
    gtk_window_close (GTK_WINDOW (l->data));
  }
  
  g_list_free (list_copy); 
}
```

- 5: Obtains the list of top-level windows of the application.
- 10: Makes a shallow copy of the list.
Because closing a window removes it from the application's internal list (`windows`), the original list will change during the loop.
To safely iterate over the windows without the list mutating under us, we must use a copy.
- 12-14: Iterate through the list of top-level windows held by `GtkApplication`, closing all of them with the `gtk_window_close ()` function.
Since this function emits the "close-request" signal, any unsaved text will automatically be caught and handled by our custom handler.
Because TFE only has one main window, iterating through a list like this isn't strictly necessary, but the code is written this way to maintain generality.

## Compiling and Running

All the source files are in the [src/tfe8](../src/tfe8/) directory.
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

Up: [README.md](../README.md),  Prev: [Section 21](sec21.md), Next: [Section 23](sec23.md)
