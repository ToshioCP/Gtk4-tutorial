# Completing the custom object implementation

The TfeTextView class will be finally completed in this section.

The source files are in the directory [src/tfe5](tfe5/).
You can get them by downloading the [repository](https://github.com/ToshioCP/Gtk4-tutorial).

## Header File

The header file `tfetextview.h` includes:

- The macro definition for TfeTextView type (`TFE_TYPE_TEXT_VIEW`).
- The macro `G_DECLARE_FINAL_TYPE`.
- GError domain and code definition and declaration.
- Public function declarations. They are instance methods and constructors.

@@@include
tfe5/tfetextview.h
@@@

- 1: The preprocessor directive `#pragma once` makes the header file be included only once.
It is non-standard but widely used.
- 3: Includes gtk4 header files.
- 5: Defines TfeTextView type (`TFE_TYPE_TEXT_VIEW`).
- 6: The macro `G_DECLARE_FINAL_TYPE` defines the class structure and some useful functions.
  - `TfeTextView` and `TfeTextViewClass` are declared as typedef of C structures.
  - You need to define a structure `_TfeTextView` later.
  - The class structure `_TfeTextViewClass` is defined here. You don't need to define it by yourself.
  - Convenience functions `TFE_TEXT_VIEW ()` for casting and `TFE_IS_TEXT_VIEW` for type check are defined.
- 8-14: Custom GError domain and code definition as you learned in the previous section.
- 16-32: Declarations of public functions on TfeTextView.

The content of header files are open to C source files.
So, the functions here are public functions.
Don't write any private functions (functions for only the corresponding C file) here.

## Constructors

A TfeTextView instance is created with `tfe_text_view_new` or `tfe_text_view_new_with_file`.
These functions are called constructors.

@@@include
tfe5/tfetextview.c tfe_text_view_new tfe_text_view_new_with_file
@@@

- 1-4: `tfe_text_view_new` function.
It just returns the value from the function `g_object_new` and casts it to the pointer to GtkWidget.
It is a GTK convention that constructors of GtkWidget subclasses return a `GtkWidget *`.
The function `g_object_new` creates any instances of its descendant class.
The arguments are the type of the class, property list and NULL, which is the end mark of the property list.
Here, the property "wrap-mode" is set to `GTK_WRAP_WORD_CHAR` for breaking lines in between words, or if that is not enough, also between characters (graphemes).
The default value of the property "wrap-mode" in GtkTextView (not TfeTextView) is `GTK_WRAP_NONE` (no wrapping).
- 6-23: `tfe_text_view_new_with_file` function.
- 8: `g_return_val_if_fail` tests whether the argument `file` is a pointer to GFile.
If it's true, the program goes on to the next line.
If it's false, it returns NULL (the second argument) immediately.
And at the same time it logs out the error message (usually the log is outputted to stderr).
- 9: Another `g_return_val_if_fail` tests `err`.
This can be NULL when the caller does not need detailed error information.
Otherwise it must point to NULL (Note: Being NULL and pointing to NULL is totally different).
- 13: Creates a TfeTextView instance.
- 14-15: If `file` is NULL, just returns `tv`.
- 16: sets the file of the TfeTextView instance to `file`.
- 17-18: If the function `tfe_text_view_read` successfully reads the content of the file, returns the TfeTextView instance.
- 20-22: If an error happens during the read process, Releases `tv` and returns NULL.
The function `g_object_ref_sink` will be explained in the next subsection.

## Floating References

All widgets are derived from GInitiallyUnowned.
While GObject and GInitiallyUnowned are very similar, they differ in their initial reference count behavior.

When an instance of GInitiallyUnowned is created, it has a "floating reference", whereas a GObject instance has a "normal reference".
The reference count of both the objects is one, but the type of the references is different. 

The descendants of GInitiallyUnowned inherit this behavior, so every widget has a floating reference immediately after the creation.
On the other hand, non-widget classes, such as GtkTextBuffer, are direct subclasses of GObject and do not have a floating reference.

The function `g_object_ref_sink (object)` behaves:

- If the object has a floating reference, it converts the floating reference to the normal reference.
The reference count will be one.
- If the object does not have a floating reference, it increases the normal reference count by one.
In this case, `g_object_ref_sink (object)` behaves the same as `g_object_ref (object)`.

The function is used when an widget is added to another widget as a child.

@@@if gfm
```C
@@@else
```{.c}
@@@end
GtkTextView *tv = gtk_text_view_new (); /* Floating reference */
GtkScrolledWindow *scr = gtk_scrolled_window_new ();
gtk_scrolled_window_set_child (scr, tv); /* Scrolled window sinks the tv's floating and tv's normal reference count becomes one. */
```

This program works because `gtk_scrolled_window_set_child` uses `g_object_ref_sink` instead of `g_object_ref`.
If `tv` does not have a floating reference, the program needs to decrement the `tv` reference count:

@@@if gfm
```C
@@@else
```{.c}
@@@end
GtkTextView *tv = gtk_text_view_new ();  /* Floating reference: tv is not owned by anyone */
g_object_ref_sink (tv); /* Normal reference count == 1: this program owns tv. */
GtkScrolledWindow *scr = gtk_scrolled_window_new ();
gtk_scrolled_window_set_child (scr, tv); /* tv's reference count becomes 2. */
/* At this moment, both of the caller of the function and GtkScrolledWindow instance refer to tv. */
/* But the caller does not use tv any more. So, it releases tv using g_object_unref. */ 
g_object_unref (tv); /* tv's reference count becomes 1 because the caller decrements tv's reference count. */
```

From the comparison of these two programs, we can say that floating reference makes the program simpler because we don't need to release `tv`.

Conversely, it gets more complicated to release an InitiallyUnowned instance just after the creation.
Because we cannot use `g_object_unref` to floating object.

@@@if gfm
```C
@@@else
```{.c}
@@@end
GtkTextView *tv = gtk_text_view_new (); /* Floating reference */
/* We need to convert the floating reference to the normal reference before releasing it. */
g_object_ref_sink (tv); /* Floating reference is converted to normal reference. The reference count is one. */
g_object_unref (tv); /* Now reference count becomes zero and tv will be destroyed. */
```

See [GObject API reference - g\_object\_ref\_sink](https://docs.gtk.org/gobject/method.Object.ref_sink.html) for further information.

## Instance Methods

### Getting and setting the GFile

A TfeTextView instance holds the GFile instance in it.
External code can access the GFile with `tfe_text_view_get_file` and `tfe_text_view_set_file`.

@@@include
tfe5/tfetextview.c tfe_text_view_get_file tfe_text_view_set_file
@@@

There are two new functions in the code.

- `g_set_object (&tv->file, file)`: Updates `tv->file` to refer to `file`.
It increments the reference count of the new `file` (if non-NULL), decrements the reference count of the current `tv->file` (if non-NULL), and updates the pointer.
This reference counting ensures that the instance releases the old `GFile` and assumes ownership of the new one.
- `gtk_text_buffer_set_modified (tb, TRUE)`: Sets the "modified" flag to `TRUE`.
`GtkTextBuffer` maintains an internal flag to track whether its content has been modified.
A value of `TRUE` indicates that the buffer has been modified since it was last saved.
When `tfe_text_view_set_file` changes the `GFile`, the content is considered unsaved, so the modified flag must be set to `TRUE`.

### Ownership of Data

When working with getter and setter functions, understanding data "ownership" is crucial.

For "get" functions, there are two patterns for handling the ownership of the returned object:

- **The caller gets ownership:** 
  In this case, the documentation usually states something like, "The caller of the method takes ownership of the returned data, and is responsible for freeing it."
  Example: `g_list_model_get_item`.
- **The caller does NOT get ownership:** 
  In this case, the documentation typically says, "The returned data is owned by the instance."
  Example: `gtk_text_view_get_buffer`.

If you are given ownership, you must free or unreference the data when you no longer need it.
Conversely, if you are not given ownership, you must **never** free it.

#### The `const` Qualifier as an Ownership Hint for Strings

For string data, the `const` qualifier serves as a very helpful built-in hint for ownership in C:

- **Get functions:** If a function returns a `const char *`, it means the caller does not get ownership. You must not free it.
- **Set functions:** If a function argument is `const char *`, it means the instance will not take ownership of your string (it will usually make its own internal copy if it needs the ownership). 

This convention is standard throughout the GTK and GLib documentation, and you should apply this same `const` rule when writing your own programs to make ownership clear.

#### How to Acquire Ownership Manually

But what if you aren't given ownership of the return value, yet you need to keep the data for a while?
You must explicitly acquire ownership yourself:

- For objects with a **normal reference**, use `g_object_ref` to increase the reference count by one.
- For objects that might have a **floating reference**, use `g_object_ref_sink`.
This converts a floating reference into a normal one, or simply increments the reference count if it's already a normal reference.
- For data without reference counts, such as **strings** (especially those returned as `const char *`), you need to make a copy.
Use `g_strdup` to duplicate the string, and later free it using `g_free`.
- **Note:** In the method above, the caller and the instance share the object.
Therefore, any modification made by one affects the other.
In contrast, the string is duplicated rather than shared.
Because the caller and the instance own their respective copies, changing one string will not affect the other.

In the case of `tfe_text_view_get_file`, the function simply returns `tv->file` without increasing its reference count.
It does not give you ownership.

#### Ownership in "Set" Functions

On the other hand, in "set" functions, the instance receiving the data usually claims its own ownership by calling `g_object_ref` or `g_object_ref_sink` internally. This means the caller's ownership remains unchanged before and after the function call.
For example, `tfe_text_view_set_file (tv, file)` does not change the caller's ownership of the `file`.

However, be aware that some GTK and GLib functions completely transfer ownership. 
For example, `GValue` has a function called `g_value_take_object (GValue* value, gpointer v_object)`.
In this case, the ownership of `v_object` moves entirely to `value`.
The caller no longer owns it and must **not** free it after setting it.

In short, always pay close attention to the ownership of arguments and return values to avoid memory bugs.

### Reading the File Content

The function `tfe_text_view_read` reads the file content into the buffer.

@@@include
tfe5/tfetextview.c tfe_text_view_read
@@@

- 11-14: If `tv->file` is `NULL`, the function sets `err` and returns `FALSE`.
`g_set_error_literal` creates a `GError` structure with the domain, code, and message.
Then it sets `*err` to point to the `GError`.
- 16: `g_file_load_contents` reads the file content into memory.
If an error occurs, it returns `FALSE` and sets `err` to a newly created `GError`.
See the [GIO documentation](https://docs.gtk.org/gio/method.File.load_contents.html) for details.
- 17-31: After successfully loading the content, the function performs two checks before updating the GtkTextBuffer.
First, it checks the file size.
Since the maximum capacity of the buffer is `G_MAXINT` (the upper limit of the int type), if length exceeds this value, `err` is set and `stat` becomes FALSE.
Second, it validates the UTF-8 encoding. The `g_utf8_validate` function ensures the content is a valid UTF-8 string.
If it is not, `err` is set and `stat` becomes FALSE.
If both checks pass, the function updates the buffer with the loaded text and sets the modified flag to FALSE, as the buffer now exactly matches the file.
Finally, the memory for `contents` is freed.

### Writing the buffer content

The function `tfe_text_view_write` saves the content of the text buffer to the file.

@@@include
tfe5/tfetextview.c tfe_text_view_write
@@@

- 12-15: If `tv->file` is not set, it means there is no file to write to.
The function sets `err` with a specific error message and returns `FALSE`.
- 17: To extract text from a `GtkTextBuffer`, you need to specify a range using **iterators**.
A `GtkTextIter` is a structure that represents a specific position between characters within a buffer.
`gtk_text_buffer_get_bounds` initializes `start_iter` to the first position in the buffer, and `end_iter` to the end.
- 18: `gtk_text_buffer_get_text` retrieves the text between the start and end iterators.
Note that this function allocates memory for the returned string, meaning the caller takes ownership and is responsible for freeing it later.
See the [GIO documentation](https://docs.gtk.org/gtk4/method.TextBuffer.get_text.html) for details.
- 19: `g_file_replace_contents` writes the `contents` string to the file, replacing any existing data.
It returns `TRUE` on success or `FALSE` on failure, and sets `err` appropriately if an error occurs.
- 20: Since the text has been written to the file and is no longer needed in memory, the function frees the `contents` using `g_free`.
- 21-22: If the write operation was successful (`stat` is `TRUE`), it sets the modified flag of the buffer to `FALSE`.
This is because the content in the buffer is now safely saved and exactly matches the file.
- 23: Finally, it returns the status of the write operation.

## Summary

We have successfully completed the `TfeTextView` class by implementing its header, constructors, and instance methods.
We also learned essential GTK memory management concepts, such as floating references and data ownership.
With our custom widget fully operational, we are now ready to integrate it into the main application.
