Up: [Readme.md](Readme.md),  Prev: [Section 10](sec10.md), Next: [Section 12](sec12.md)
# Functions with GtkNotebook

GtkNotebook is a very important object in the text file editor `tfe`.
It connects the application and TfeTextView objects.
`tfenotebook.h` and `tfenotebook.c` describe a set of functions related to GtkTextbook.

     1 void
     2 notebook_page_save(GtkNotebook *nb);
     3 
     4 void
     5 notebook_page_open (GtkNotebook *nb);
     6 
     7 void
     8 notebook_page_new_with_file (GtkNotebook *nb, GFile *file);
     9 
    10 void
    11 notebook_page_new (GtkNotebook *nb);
    12 

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

     1 static gchar*
     2 get_untitled () {
     3   static int c = -1;
     4   if (++c == 0) 
     5     return g_strdup_printf("Untitled");
     6   else
     7     return g_strdup_printf ("Untitled%u", c);
     8 }
     9 
    10 static void
    11 notebook_page_build (GtkNotebook *nb, GtkWidget *tv, char *filename) {
    12   GtkWidget *scr;
    13   GtkNotebookPage *nbp;
    14   GtkWidget *lab;
    15   gint i;
    16   scr = gtk_scrolled_window_new ();
    17 
    18   gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
    19   lab = gtk_label_new (filename);
    20   i = gtk_notebook_append_page (nb, scr, lab);
    21   nbp = gtk_notebook_get_page (nb, scr);
    22   g_object_set (nbp, "tab-expand", TRUE, NULL);
    23   gtk_notebook_set_current_page (nb, i);
    24   g_signal_connect (GTK_TEXT_VIEW (tv), "change-file", G_CALLBACK (file_changed), nb);
    25 }
    26 
    27 void
    28 notebook_page_new (GtkNotebook *nb) {
    29   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
    30 
    31   GtkWidget *tv;
    32   char *filename;
    33 
    34   tv = tfe_text_view_new ();
    35   filename = get_untitled ();
    36   notebook_page_build (nb, tv, filename);
    37 }

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

     1 void
     2 notebook_page_new_with_file (GtkNotebook *nb, GFile *file) {
     3   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
     4   g_return_if_fail(G_IS_FILE (file));
     5 
     6   GtkWidget *tv;
     7   char *filename;
     8 
     9   if ((tv = tfe_text_view_new_with_file (file)) == NULL)
    10     return; /* read error */
    11   filename = g_file_get_basename (file);
    12   notebook_page_build (nb, tv, filename);
    13 }

- 9-10: Call `tfe_text_view_new_with_file`.
If it returns NULL, then do nothing and return because of an error.
-11-13: Get the filename , build the contents of the page, then free `filename`.

## notebook\_page\_open

     1 static void
     2 open_response (TfeTextView *tv, gint response, GtkNotebook *nb) {
     3   GFile *file;
     4   char *filename;
     5 
     6   if (response != TFE_OPEN_RESPONSE_SUCCESS)
     7     g_object_unref (tv);
     8   else if (! G_IS_FILE (file = tfe_text_view_get_file (tv)))
     9     g_object_unref (tv);
    10   else {
    11     filename = g_file_get_basename (file);
    12     g_object_unref (file);
    13     notebook_page_build (nb, GTK_WIDGET (tv), filename);
    14   }
    15 }
    16 
    17 void
    18 notebook_page_open (GtkNotebook *nb) {
    19   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
    20 
    21   GtkWidget *tv;
    22 
    23   tv = tfe_text_view_new ();
    24   g_signal_connect (TFE_TEXT_VIEW (tv), "open-response", G_CALLBACK (open_response), nb);
    25   tfe_text_view_open (TFE_TEXT_VIEW (tv));
    26 }

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

     1 void
     2 notebook_page_save(GtkNotebook *nb) {
     3   gint i;
     4   GtkWidget *scr;
     5   GtkWidget *tv;
     6 
     7   i = gtk_notebook_get_current_page (nb);
     8   scr = gtk_notebook_get_nth_page (nb, i);
     9   tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
    10   tfe_text_view_save (TFE_TEXT_VIEW (tv));
    11 }

- 7-9: Get TfeTextView belongs to the current notebook page.
- 10: Call `tfe_text_view_save`.

## file\_changed handler

`file_changed` is a handler connected to "change-file" signal.
If `tv->file` is changed, TfeTextView emits this signal.
This handler changes the label of GtkNotebookPage.

     1 static void
     2 file_changed (TfeTextView *tv, GtkNotebook *nb) {
     3   GFile *file;
     4   char *filename;
     5   GtkWidget *scr;
     6   GtkWidget *label;
     7 
     8   file = tfe_text_view_get_file (tv);
     9   scr = gtk_widget_get_parent (GTK_WIDGET (tv));
    10   if (G_IS_FILE (file))
    11     filename = g_file_get_basename (file);
    12   else
    13     filename = get_untitled ();
    14   label = gtk_label_new (filename);
    15   gtk_notebook_set_tab_label (nb, scr, label);
    16   g_object_unref (file);
    17   g_free (filename);
    18 }

- 8: Get GFile from TfeTextView.
- 9: Get the parent (GkScrolledWindow) of `tv`.
- 10-13: If `file` points GFile, then assign the filename of the GFile into `filename`.
Otherwise (this is the case file is NULL), assign untitled string to `filename`.
- 14-15: Generate a label with the filename and set it into GtkNotebookPage.
- 16-17: Free `filename and unref `file`.


Up: [Readme.md](Readme.md),  Prev: [Section 10](sec10.md), Next: [Section 12](sec12.md)
