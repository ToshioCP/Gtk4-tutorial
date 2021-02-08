# Functions in GtkNotebook

GtkNotebook is a very important object in the text file editor `tfe`.
It connects the application and TfeTextView objects.
A set of functions related to GtkNotebook are declared in `tfenotebook.h`.
The word "tfenotebook" is used only in filenames.
There's no "TfeNotebook" object.

@@@include
tfe5/tfenotebook.h
@@@

This header file describes the public functions in `tfenotebook.c`.

- 10-11: The function `notebook_page_new` generates a new GtkNotebookPage and adds GtkScrolledWindow and TfeTextView to the page.
- 7-8: The function `notebook_page_new_with_file` generates a new GtkNotebookPage and adds GtkScrolledWindow and TfeTextView to the page. A file is read and inserted into GtkTextBuffer.
The GFile `file` is copied and inserted to the TfeTextView object.
- 4-5: `notebook_page_open` shows a file chooser dialog. Then, user chooses a file and the file is inserted into GtkTextBuffer.
- 1-2: `notebook_page_save` saves the contents in GtkTextBuffer into the file, which is got from the TfeTextView.

You probably find that the functions above are higher level functions of

- `tfe_text_view_new`
- `tfe_text_view_new_with_file`
- `tef_text_view_open`
- `tfe_text_view_save`

respectively.

There are two layers.
One of them is `tfe_text_view ...`, which is the lower level layer.
The other is `note_book ...`, which is the higher level layer.

Now let's look at each program of the functions.

## notebook\_page\_new

@@@include
tfe5/tfenotebook.c get_untitled notebook_page_build notebook_page_new
@@@

- 28-38: `notebook_page_new` function.
- 30: `g_return_if_fail` is used to check the argument.
- 35: Generates TfeTextView object.
- 36: Generates filename, which is "Untitled", "Untitled2", ... .
- 1-8: `get_untitled` function.
- 3: Static variable `c` is initialized at the first call of this function. After that `c` keeps its value except it is changed explicitly.
- 4-7: Increases `c` by one and if it is zero then it returns "Untitled". If it is a positive integer then the it returns "Untitled\<the integer\>", for example, "Untitled1", "Untitled2", and so on.
The function `g_strdup_printf` generates a string and it should be freed by `g_free` when it becomes useless.
The caller of `get_untitled` is in charge of freeing the string.
- 37: calls `notebook_page_build` to build the contents of the page.
- 10- 26: `notebook_page_build` function.
- 16: Generates GtkScrolledWindow.
- 18: Sets the wrap mode of `tv` to GTK_WRAP_WORD_CHAR so that lines are broken between words or graphemes.
- 19: Inserts `tv` to GtkscrolledWindow as a child.
- 20-21: Generates GtkLabel, then appends it to GtkNotebookPage.
- 22-23: Sets "tab-expand" property to TRUE.
The function g\_object\_set sets properties on an object.
The object is any object derived from GObject.
In many cases, an object has its own function to set its properties, but sometimes not.
In that case, use g\_object\_set to set the property.
- 24: Sets the current page of `nb` to `i` which is the number of the GtkNotebookPage above.
- 25: Connects "change-file" signal and `file_changed` handler.

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
- 9-11: If `tfe_text_view_get_file` returns a pointer not to point GFile, then something bad happens.
Sink and unref `tv`.
- 12-16: Otherwise, everything is okay.
Gets the filename, builds the contents of the page.

## notebook\_page\_save

@@@include
tfe5/tfenotebook.c notebook_page_save
@@@

- 7-9: Get TfeTextView belongs to the current notebook page.
- 10: Call `tfe_text_view_save`.

## file\_changed handler

The function `file_changed` is a handler connected to "change-file" signal.
If the file in TfeTextView is changed, it emits this signal.
This handler changes the label of GtkNotebookPage.

@@@include
tfe5/tfenotebook.c file_changed
@@@

- 8: Gets GFile from TfeTextView.
- 9: Gets GkScrolledWindow which is the parent widget of `tv`.
- 10-13: If `file` points GFile, then assigns the filename of the GFile into `filename`.
Otherwise (file is NULL), assigns untitled string to `filename`.
- 14-15: Generates a label with the filename and inserts it into GtkNotebookPage.
- 16-17: Unrefs `file` and frees `filename`.

