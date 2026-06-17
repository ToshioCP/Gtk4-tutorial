Up: [README.md](../README.md),  Prev: [Section 11](sec11.md), Next: [Section 13](sec13.md)

# Error Handling in GLib

## Programmer Errors and Runtime Errors

In GLib, errors are generally categorized into two types: Programmer Errors and Runtime Errors.

Programmer Errors are bugs within your source code.
You should check for these errors during development; if one is detected, it is a sign that you must find the bug and fix your code immediately.

Runtime Errors, on the other hand, occur due to external factors during program execution, such as user operations or environmental issues (e.g., trying to read a file that does not exist).
These are not bugs in your code.
When a runtime error occurs, you should notify the user or prompt them to take action; you should never simply terminate the program.

When designing objects, you must consider both types of error handling.

## Programmer Error Handling

A well-known programmer error is passing an argument of the wrong type.
While the C compiler can detect some of these issues, it cannot verify GType because they are determined at runtime, not at compile time.

To perform runtime type checking, we use the following macros:

- `g_return_if_fail (cond)`: If the condition `cond` is false, it prints an error message and returns from the function.
- `g_return_val_if_fail (cond, val)`: If the condition `cond` is false, it prints an error message, returns from the function with the value val.

While internal helper functions are less prone to type mismatches, public functions are highly susceptible.
You should treat these checks as mandatory for all public-facing functions.
If these errors are triggered, it almost always indicates a bug in the code calling your library.

```C
GFile *
tfe_text_view_get_file (TfeTextView *tv) {
  g_return_val_if_fail (TFE_IS_TEXT_VIEW (tv), NULL);
```

In addition to type checking, GLib provides functions to report other anomalies that suggest programmer error:

- `g_warning (...)`: Used for non-fatal but important issues, such as system configuration errors or anomalies detected in trusted files.
The `...` follows the printf format.
Example: `g_warning ("%s is not a GType name.", string);`
- `g_error (...)`: Used for fatal errors caused by programming mistakes.
This function reports the error and terminates the program immediately.

**Note:** These functions are intended for developers, not for application end-users.

## Runtime Error Handling

Runtime errors are recoverable issues that occur during execution, such as failing to find a file you are trying to open.
In many other languages, these are called "exceptions".
GLib provides a structured way to handle these using the GError structure.
You can also define custom error domains for your own objects.

### GError

GError is a C structure that carries three key pieces of information:

- Domain: The category or library to which the error belongs.
- Code: An integer identifying the specific type of error.
- Message: A human-readable string explaining the error.

In most cases, error handling involves displaying this error message in a dialog box to inform the user.

```C
void
tfe_error_alert (GtkWindow *win, GError *err) {
  GtkAlertDialog *alert_dialog;

  alert_dialog = gtk_alert_dialog_new ("%s", err->message);
  gtk_alert_dialog_show (alert_dialog, win);
  g_object_unref (alert_dialog);
}

void
example_function (GtkWidget *win, GtkWidget *tv) {
  GError *err = NULL;

  if (! tfe_text_view_write (TFE_TEXT_VIEW (tv), &err)) {
    tfe_error_alert (GTK_WINDOW (win), err);
    g_clear_error (&err);
  }
}
```

The function `tfe_text_view_write` saves the content of the buffer to the GFile held by the TfeTextView.
It may fail if:

- No GFile is set (i.e., the target file is undefined).
- A write error occurs (e.g., lack of write permissions).

When an error occurs, `tfe_text_view_write` creates a GError, populates it with details, assigns it to the pointer provided by the caller, and returns FALSE.

Steps for handling runtime errors:

- Define a GError pointer and initialize it to NULL.
- Pass the address of that pointer (`&err`) to the function.
- Check the return value. If it is FALSE, handle the error (e.g., showing an alert dialog).
- Free the GError using `g_error_free` or, preferably, `g_clear_error (&err)`, which also sets the pointer back to NULL.

If you don't need detailed error reports, you can pass NULL to the error argument:

```C
tfe_text_view_write (TFE_TEXT_VIEW (tv), NULL);
```

All functions that support error reporting must be designed to accept NULL for this argument.

## Customizing Error Domains

To define errors specific to your own objects, you need to create a custom error domain.

In your header file (`tfetextview.h`):

```C
/* User Error Definition */
#define TFE_TEXT_VIEW_ERROR (tfe_text_view_error_quark ())
GQuark tfe_text_view_error_quark (void);

typedef enum {
  TFE_TEXT_VIEW_ERROR_NO_FILE,   /* No file is set */
  TFE_TEXT_VIEW_ERROR_FAILED,    /* Generic or unrecoverable error */
} TfeTextViewError;
```

In your source file (tfetextview.c):

```C
G_DEFINE_QUARK (tfe-text-view-error-quark, tfe_text_view_error)
```

Key Rules for Custom Errors:

- Error Domain Macro: Defined as `<NAMESPACE>_<MODULE>_ERROR`.
For TfeTextView, it is `TFE_TEXT_VIEW_ERROR`.
It maps to a quark function named `namespace_module_error_quark()`.
- Error Codes: Use an enum following the `<Namespace><Module>Error` convention.
Individual error codes should be named `<NAMESPACE>_<MODULE>_ERROR_<CODE>`.
Always include a `_FAILED` member for generic or unrecognized errors.
- `G_DEFINE_QUARK`: This macro automatically defines the `..._quark()` function.
The first argument is the domain name string (kebab-case: `tfe-text-view-error-quark`).
The second argument is the C function prefix (snake_case: `tfe_text_view_error`).

To set a custom error within a function, use `g_set_error_literal`:

```C
if (tv->file == NULL) {
  g_set_error_literal (err, TFE_TEXT_VIEW_ERROR, TFE_TEXT_VIEW_ERROR_NO_FILE, 
                       "No file is set in TfeTextView.");
  return FALSE;
}
```

The function `g_error_set_literal` creates a GError structure and sets the domain, code, and message with the second to fourth arguments.
Furthermore, it updates err to point to the GError.
If you want to create a message using printf type arguments, use `g_eror_set` instead.

Refer to the files `tfetextview.h` and `tfetextview.c` in [src/tfe5](../src/tfe5/) for further information.

Up: [README.md](../README.md),  Prev: [Section 11](sec11.md), Next: [Section 13](sec13.md)
