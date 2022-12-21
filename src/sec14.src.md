# Functions in GtkNotebook

GtkNotebook is a very important object in the text file editor `tfe`.
It connects the application and TfeTextView objects.
A set of public functions are declared in `tfenotebook.h`.
The word "tfenotebook" is used only in filenames.
There's no "TfeNotebook" object.

@@@include
tfe5/tfenotebook.h
@@@

This header file describes the public functions in `tfenotebook.c`.

- 1-2: `notebook_page_save` saves the current page to the file of which the name specified in the tab.
If the name is `untitled` or `untitled` followed by digits, FileChooserDialog appears and a user can choose or specify a filename.
- 4-5: `notebook_page_close` closes the current page.
- 7-8: `notebook_page_open` shows a file chooser dialog and a user can choose a file. The file is inserted to a new page.
- 10-11: `notebook_page_new_with_file` creates a new page and a file given as an argument is read and inserted into the page.
- 13-14: `notebook_page_new` creates a new empty page.

You probably find that the functions except `notebook_page_close` are higher level functions of

- `tfe_text_view_save`
- `tef_text_view_open`
- `tfe_text_view_new_with_file`
- `tfe_text_view_new`

respectively.

There are two layers.
One of them is `tfe_text_view ...`, which is the lower level layer.
The other is `note_book ...`, which is the higher level layer.

Now let's look at the program of each function.

## notebook\_page\_new

@@@include
tfe5/tfenotebook.c get_untitled notebook_page_build notebook_page_new
@@@

- 26-38: `notebook_page_new` function.
- 28: `g_return_if_fail` is used to check the argument.
- 33-34: Creates TfeTextView object.
If it fails, no notebook page is created and the function returns to the caller.
- 35: Creates filename, which is "Untitled", "Untitled1", ... .
- 1-8: `get_untitled` function.
- 3: Static variable `c` is initialized at the first call of this function. After that `c` keeps its value unless it is changed explicitly.
- 4-7: Increases `c` by one and if it is zero, it returns "Untitled". If it is a positive integer, it returns "Untitled\<the integer\>", for example, "Untitled1", "Untitled2", and so on.
The function `g_strdup_printf` creates a string and it should be freed by `g_free` when it becomes useless.
The caller of `get_untitled` is in charge of freeing the string.
- 36: calls `notebook_page_build` to build the contents of the page.
- 37: frees `filename`.
- 10- 24: `notebook_page_build` function.
A parameter with `const` qualifier doesn't change in the function.
It means that the argument `filename` is owned by the caller.
The caller needs to free it when it becomes useless.
- 12: Creates GtkScrolledWindow.
- 17: Inserts `tv` to GtkscrolledWindow as a child.
- 18-19: Creates GtkLabel, then appends `scr` and `lab` to the GtkNotebook instance `nb`.
- 20-21: Sets "tab-expand" property to TRUE.
The function `g_object_set` sets properties on an object.
The object is any object derived from GObject.
In many cases, an object has its own function to set its properties, but sometimes not.
In that case, use `g_object_set` to set the property.
- 22: Sets the current page to the newly created page.
- 23: Connects "change-file" signal and `file_changed_cb` handler.

## notebook\_page\_new\_with\_file

@@@include
tfe5/tfenotebook.c notebook_page_new_with_file
@@@

- 9-10: Calls `tfe_text_view_new_with_file`.
If the function returns NULL, an error has happend.
Then, it does nothing and returns.
- 11-13: Gets the filename, builds the contents of the page and frees `filename`.

## notebook\_page\_open

@@@include
tfe5/tfenotebook.c open_response notebook_page_open
@@@

- 18-28: `notebook_page_open` function.
- 24-25: Creates TfeTextView object.
If NULL is returned, an error has happened.
Then, it returns to the caller.
- 26: Connects the signal "open-response" and the handler `open_response`.
- 27: Calls `tfe_text_view_open`.
The "open-response" signal will be emitted later to inform the result of opening and reading a file.
- 1-16: `open_response` handler.
- 6-8: If the response code is not `TFE_OPEN_RESPONSE_SUCCESS`, it has failed to open and read a new file.
Then, what `notebook_page_open` did in advance need to be canceled.
The instance `tv` hasn't been a child widget of GtkScrolledWindow yet.
Such instance has floating reference.
Floating reference will be explained later.
You need to call `g_object_ref_sink` first.
Then the floating reference is converted into an ordinary reference.
Now you call `g_object_unref` to decrease the reference count by one.
- 9-15: Otherwise, everything is okay.
Gets the filename, builds the contents of the page and frees `filename`.

## Floating reference

All the widgets are derived from GInitiallyUnowned.
GObject and GInitiallyUnowned are almost the same.
The difference is like this.
When an instance of GInitiallyUnowned is created, the instance has a floating reference and its reference count is zero.
On the other hand, when an instance of GObject (not GInitiallyUnowned) is created, no floating reference is given.
And the instance has a normal reference count instead of floating reference.
Their descendants inherits them, so every widget has a floating reference at first.
Non-widget class, for example, GtkTextBuffer is a direct sub class of GObject and it doesn't have floating reference.
Its reference count is one when it is created.

The function `g_object_ref_sink` converts the floating reference into an ordinary reference.
If the instance doesn't have a floating reference, `g_object_ref_sink` simply increases the reference count by one.
It is used when an widget is added to another widget as a child.

~~~
GtkTextView *tv = gtk_text_view_new (); // floating reference
GtkScrolledWindow *scr = gtk_scrolled_window_new ();
gtk_scrolled_window_set_child (scr, tv); // tv's reference count is one
~~~

When `tv` is added to `scr` as a child, `g_object_ref_sink` is used.

~~~
g_object_ref_sink (tv);
~~~

So, the floating reference is converted into an ordinary reference.
That is to say, floating reference is deleted, and reference count turns to one.
Thanks to this, the caller doesn't need to decrease tv's reference count.
If an Object\_A is not a descendant of GInitiallyUnowned, the program is like this:

~~~
Object_A *obj_a = object_a_new (); // reference count is one
GtkScrolledWindow *scr = gtk_scrolled_window_new ();
gtk_scrolled_window_set_child (scr, obj_a); // obj_a's reference count is two
// obj_a is referred by the caller (this program) and scrolled window
g_object_unref (obj_a); // obj_a's reference count is one because the caller no longer refers obj_a.
~~~

This example tells us that the caller needs to unref `obj_a`.

If you use `g_object_unref` to an instance that has a floating reference, you need to convert the floating reference to a normal reference in advance.
See [GObject API reference](https://docs.gtk.org/gobject/floating-refs.html) for further information.

## notebook\_page\_close

@@@include
tfe5/tfenotebook.c notebook_page_close
@@@

This function closes the current page.
If the page is the only page the notebook has, then the function destroys the top-level window and quits the application.

- 8-10: If the page is the only page the notebook has, it calls `gtk_window_destroy` to destroys the top-level window.
- 11-13: Otherwise, removes the current page.
The child widget (TfeTextView) is also destroyed.

## notebook\_page\_save

@@@include
tfe5/tfenotebook.c get_current_textview notebook_page_save
@@@

- 13-21: `notebook_page_save`.
- 19: Gets TfeTextView belongs to the current page.
- 20: Calls `tfe_text_view_save`.
- 1-11: `get_current_textview`.
This function gets the TfeTextView object belongs to the current page.
- 7: Gets the page number of the current page.
- 8: Gets the child widget `scr`, which is a GtkScrolledWindow instance, of the current page.
- 9-10: Gets the child widget of `scr`, which is a TfeTextView instance, and returns it.

## file\_changed\_cb handler

The function `file_changed_cb` is a handler connected to "change-file" signal.
If a file in a TfeTextView instance is changed, it emits this signal.
This handler changes the label of the GtkNotebookPage.

@@@include
tfe5/tfenotebook.c file_changed_cb
@@@

- 8: Gets the GFile instance from `tv`.
- 9: Gets the GkScrolledWindow instance which is the parent widget of `tv`.
- 10-12: If `file` points GFile, then assigns the filename of the GFile into `filename`.
Then, unref the GFile object `file`.
- 13-14: Otherwise (file is NULL), assigns untitled string to `filename`.
- 15-16: Creates a GtkLabel instance `label` with the filename and set the label of the GtkNotebookPage with `label`.

