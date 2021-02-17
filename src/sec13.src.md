# Functions in GtkNotebook

GtkNotebook is a very important object in the text file editor `tfe`.
It connects the application and TfeTextView objects.
A set of public functions related to GtkNotebook are declared in `tfenotebook.h`.
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
- 10-11: `notebook_page_new_with_file` generates a new page and the file give as an argument is read and inserted into the page.
- 13-14: `notebook_page_new` generates a new empty page.

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

- 27-37: `notebook_page_new` function.
- 29: `g_return_if_fail` is used to check the argument.
- 34: Generates TfeTextView object.
- 35: Generates filename, which is "Untitled", "Untitled1", ... .
- 1-8: `get_untitled` function.
- 3: Static variable `c` is initialized at the first call of this function. After that `c` keeps its value unless it is changed explicitly.
- 4-7: Increases `c` by one and if it is zero then it returns "Untitled". If it is a positive integer then it returns "Untitled\<the integer\>", for example, "Untitled1", "Untitled2", and so on.
The function `g_strdup_printf` generates a string and it should be freed by `g_free` when it becomes useless.
The caller of `get_untitled` is in charge of freeing the string.
- 36: calls `notebook_page_build` to build the contents of the page.
- 10- 25: `notebook_page_build` function.
- 12: Generates GtkScrolledWindow.
- 17: Sets the wrap mode of `tv` to GTK_WRAP_WORD_CHAR so that lines are broken between words or graphemes.
- 18: Inserts `tv` to GtkscrolledWindow as a child.
- 19-20: Generates GtkLabel, then appends it to GtkNotebookPage.
- 21-22: Sets "tab-expand" property to TRUE.
The function g\_object\_set sets properties on an object.
The object is any object derived from GObject.
In many cases, an object has its own function to set its properties, but sometimes not.
In that case, use g\_object\_set to set the property.
- 23: Sets the current page of `nb` to the newly generated page.
- 24: Connects "change-file" signal and `file_changed_cb` handler.

## notebook\_page\_new\_with\_file

@@@include
tfe5/tfenotebook.c notebook_page_new_with_file
@@@

- 9-10: Calls `tfe_text_view_new_with_file`.
If the function returns NULL, then it does nothing and returns.
The return value NULL means that an error has happened.
- 11-12: Gets the filename and builds the contents of the page.

## notebook\_page\_open

@@@include
tfe5/tfenotebook.c open_response notebook_page_open
@@@

- 19-28: `notebook_page_open` function.
- 25: Generates TfeTextView object.
- 26: Connects the signal "open-response" and the handler `open_response`.
- 27: Calls `tfe_text_view_open`.
The function emits an "open-response" signal to inform the status.
- 1-17: `open_response` handler.
This is the post-function of `notebook_page_open`.
- 6-8: If the status is NOT `TFE_OPEN_RESPONSE_SUCCESS`, it cancels what it did in `notebook_page_open`.
The object `tv` hasn't been a child widget of some other widget yet.
Such object has floating reference.
You need to call `g_object_ref_sink` first.
Then the floating reference is converted into an ordinary reference.
Now you call `g_object_unref` to decrease the reference count by one.
- 9-11: If `tfe_text_view_get_file` returns a pointer not to point GFile, it means that an error has happened.
Sink and unref `tv`.
- 12-16: Otherwise, everything is okay.
Gets the filename, builds the contents of the page.

## notebook\_page\_close

@@@include
tfe5/tfenotebook.c notebook_page_close
@@@

This function closes the current page.
If the page is the only page the notebook has, then the function destroys the top window and quits the application.

- 8-10: If the page is the only page the notebook has, it calls gtk\_window\_destroy to destroys the top window.
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
- 8: Gets the child object `scr`, which is GtkScrolledWindow object, of the current page.
- 9-10: Gets the child object of `scr`, which is TfeTextView object, and return it.

## file\_changed\_cb handler

The function `file_changed_cb` is a handler connected to "change-file" signal.
If the file in TfeTextView is changed, it emits this signal.
This handler changes the label of GtkNotebookPage.

@@@include
tfe5/tfenotebook.c file_changed_cb
@@@

- 8: Gets GFile from TfeTextView.
- 9: Gets GkScrolledWindow which is the parent widget of `tv`.
- 10-12: If `file` points GFile, then assigns the filename of the GFile into `filename`.
Then, unref the GFile object `file`.
- 13-14: Otherwise (file is NULL), assigns untitled string to `filename`.
- 15-16: Generates a label with the filename and inserts it into GtkNotebookPage.

The string `filename` is used in the GtkLabel object.
You mustn't free it.
