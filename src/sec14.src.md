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
- 10-11: `notebook_page_new_with_file` creates a new page and the file given as an argument is read and inserted into the page.
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

- 27-38: `notebook_page_new` function.
- 29: `g_return_if_fail` is used to check the argument.
- 34: Creates TfeTextView object.
If it fails, it returns to the caller.
- 36: Creates filename, which is "Untitled", "Untitled1", ... .
- 1-8: `get_untitled` function.
- 3: Static variable `c` is initialized at the first call of this function. After that `c` keeps its value unless it is changed explicitly.
- 4-7: Increases `c` by one and if it is zero then it returns "Untitled". If it is a positive integer then it returns "Untitled\<the integer\>", for example, "Untitled1", "Untitled2", and so on.
The function `g_strdup_printf` creates a string and it should be freed by `g_free` when it becomes useless.
The caller of `get_untitled` is in charge of freeing the string.
- 37: calls `notebook_page_build` to build the contents of the page.
- 10- 25: `notebook_page_build` function.
- 12: Creates GtkScrolledWindow.
- 17: Sets the wrap mode of `tv` to GTK_WRAP_WORD_CHAR so that lines are broken between words or graphemes.
- 18: Inserts `tv` to GtkscrolledWindow as a child.
- 19-20: Creates GtkLabel, then appends `scr` and `lab` to the GtkNotebook instance `nb`.
- 21-22: Sets "tab-expand" property to TRUE.
The function `g_object_set` sets properties on an object.
The object is any object derived from GObject.
In many cases, an object has its own function to set its properties, but sometimes not.
In that case, use `g_object_set` to set the property.
- 23: Sets the current page of `nb` to the newly created page.
- 24: Connects "change-file" signal and `file_changed_cb` handler.

## notebook\_page\_new\_with\_file

@@@include
tfe5/tfenotebook.c notebook_page_new_with_file
@@@

- 9-10: Calls `tfe_text_view_new_with_file`.
If the function returns NULL, an error has happend.
Then, it does nothing and returns.
- 11-12: Gets the filename and builds the contents of the page.

## notebook\_page\_open

@@@include
tfe5/tfenotebook.c open_response notebook_page_open
@@@

- 16-26: `notebook_page_open` function.
- 22-23: Creates TfeTextView object.
If NULL is returned, an error has happened.
Then, it returns to the caller.
- 24: Connects the signal "open-response" and the handler `open_response`.
- 25: Calls `tfe_text_view_open`.
The "open-response" signal will be emitted later to inform the result of opening and reading a file.
- 1-14: `open_response` handler.
- 6-8: If the response code is NOT `TFE_OPEN_RESPONSE_SUCCESS` or `tfe_text_view_get_file` doesn't return the pointer to a GFile, 
it has failed to open and read a new file.
Then, what `notebook_page_open` did in advance need to be canceled.
The instance `tv` hasn't been a child widget of GtkScrolledWindow yet.
Such instance has floating reference.
Floating reference will be explained later in this subsection.
You need to call `g_object_ref_sink` first.
Then the floating reference is converted into an ordinary reference.
Now you call `g_object_unref` to decrease the reference count by one.
- 9-13: Otherwise, everything is okay.
Gets the filename, builds the contents of the page.

All the widgets are derived from GInitiallyUnowned.
When an instance of GInitiallyUnowned or its descendant is created, the instance has a floating reference.
The function `g_object_ref_sink` converts the floating reference into an ordinary reference.
If the instance doesn't have a floating reference, `g_object_ref_sink` simply increases the reference count by one.
On the other hand, when an instance of GObject (not GInitiallyUnowned) is created, no floating reference is given.
And the instance has a normal reference count instead of floating reference.

If you use `g_object_unref` to an instance that has a floating reference, you need to convert the floating reference to a normal reference in advance.
See [GObject Reference Manual](https://developer-old.gnome.org/gobject/stable/gobject-The-Base-Object-Type.html#gobject-The-Base-Object-Type.description) for further information.

## notebook\_page\_close

@@@include
tfe5/tfenotebook.c notebook_page_close
@@@

This function closes the current page.
If the page is the only page the notebook has, then the function destroys the top-level window and quits the application.

- 8-10: If the page is the only page the notebook has, it calls `gtk_window_destroy` to destroys the top-level window.
- 11-13: Otherwise, removes the current page.

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
This handler changes the label of GtkNotebookPage.

@@@include
tfe5/tfenotebook.c file_changed_cb
@@@

- 8: Gets the GFile instance from `tv`.
- 9: Gets the GkScrolledWindow instance which is the parent widget of `tv`.
- 10-12: If `file` points GFile, then assigns the filename of the GFile into `filename`.
Then, unref the GFile object `file`.
- 13-14: Otherwise (file is NULL), assigns untitled string to `filename`.
- 15-16: Creates a GtkLabel instance `label` with the filename and set the label of the GtkNotebookPage with `label`.

