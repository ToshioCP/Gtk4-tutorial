# Functions with GtkNotebook

GtkNotebook is a very important object in the text file editor `tfe`.
It connects the application and TfeTextView objects.
`tfenotebook.h` and `tfenotebook.c` describe a set of functions related to GtkTextbook.

@@@ tfe5/tfenotebook.h

This header file shows the public functions in `tfenotebook.c`.

- `notebook_page_new` generates a new GtkNotebookPage and adds GtkScrolledWindow and TfeTextView under the page.
- `notebook_page_new_with_file` generates a new GtkNotebookPage and adds GtkScrolledWindow and TfeTextView under the page. `file` is set to the pointer to GFile in the TfeTextView object and the file is read and set into GtkTextBuffer.
- `notebook_page_open` lets the user select a file and sets it into GtkTextBuffer.
- `notebook_page_save` save the contents in GtkTextBuffer to a file, using the pointer `tv->file`.

You probably find that the functions above are higher level functions of `tfe_text_view_new`, `tfe_text_view_new_with_file`, `tef_text_view_open` and `tfe_text_view_save` respectively.
There are two layers.
One of them is `tfe_text_view ...`, which is the lower level layer.
The other is `note_book ...`, which is the higher level layer.

Now let's look at each program of the functions.

## notebook\_page\_new

@@@ tfe5/tfenotebook.c get_untitled notebook_page_build notebook_page_new

- 27-37: `notebook_page_new` function.
- 29: `g_return_if_fail` is used because `notebook_page_new` is a public function.
- 34: Generate TfeTextView object.
- 35: Generate filename, which is "Untitled", "Untitled2", ... .
- 1-8: `get_untitled` function.
- 3: Static variable `c` is initialized at the first call of this function. After that `c` keeps its value except it is changed explicitly.
- 4-7: Increase `c` by one and if it is zero then the name is "Untitled". If it is a positive integer then the name is "Untitled<the integer>", for example, "Untitled1", "Untitled2", and so on.
It returns the name.
`g_strdup_printf` generates a string and it should be freed by `g_free` function.
The caller of `get_untitled` is in charge of freeing the memories of the string.
- 36: call `notebook_page_build` to build the contents of the page.
- 10- 25: `notebook_page_build` function.
- 17-18: Generate GtkScrolledWindow and set `tv` to its child.
- 19-20: Generate GtkLabel, then GtkNotebookPage.
- 21-22: Set "tab-expand" property to TRUE.
- 23: Set the page to the current page.
- 24: Connect "change-file" signal and `file_changed` handler.

## notebook\_page\_new\_with\_file

@@@ tfe5/tfenotebook.c notebook_page_new_with_file

- 9-10: Call `tfe_text_view_new_with_file`.
If it returns NULL, then do nothing and return because of an error.
-11-13: Get the filename , build the contents of the page, then free `filename`.

## notebook\_page\_open

@@@ tfe5/tfenotebook.c open_response notebook_page_open

- 18-27: `notebook_page_open` function.
- 24: Generate TfeTextView object.
- 25: Connect the signal "open-response" and the handler `open_response`.
- 26: Call `tfe_text_view_open`.
It emits "open-response" signal to inform the status after the series of functions run.
- 1-16: `open_response` handler.
This is the postfunction of `notebook_page_open`.
- 6-7: It the status is NOT `TFE_OPEN_RESPONSE_SUCCESS`, cancel what we did in `notebook_page_open`.
Unref `tv`.
- 8-9: If `tfe_text_view_get_file` returns a pointer not to point GFile, then something bad happens. Cancel what we did. Unref `tv`.
- 10-14: Otherwise, everything was okay.
Get the filename, build the contents of the page, free `filename` and unref `tv`

## notebook\_page\_save

@@@ tfe5/tfenotebook.c notebook_page_save

- 7-9: Get TfeTextView belongs to the current notebook page.
- 10: Call `tfe_text_view_save`.

## file\_changed handler

`file_changed` is a handler connected to "change-file" signal.
If `tv->file` is changed, TfeTextView emits this signal.
This handler changes the label of GtkNotebookPage.

@@@ tfe5/tfenotebook.c file_changed

- 8: Get GFile from TfeTextView.
- 9: Get the parent (GkScrolledWindow) of `tv`.
- 10-13: If `file` points GFile, then assign the filename of the GFile into `filename`.
Otherwise (this is the case file is NULL), assign untitled string to `filename`.
- 14-15: Generate a label with the filename and set it into GtkNotebookPage.
- 16-17: Free `filename and unref `file`.


