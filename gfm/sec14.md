Up: [README.md](../README.md),  Prev: [Section 13](sec13.md), Next: [Section 15](sec15.md)

# Functions in GtkNotebook

GtkNotebook is a very important object in the text file editor `tfe`.
It connects the application and TfeTextView objects.
A set of public functions are declared in `tfenotebook.h`.
The word "tfenotebook" is used only in filenames.
There's no "TfeNotebook" object.

The source files are in the directory `src/tfe5`.
You can get them by downloading the [repository](https://github.com/ToshioCP/Gtk4-tutorial).

~~~C
 1 void
 2 notebook_page_save(GtkNotebook *nb);
 3 
 4 void
 5 notebook_page_close (GtkNotebook *nb);
 6 
 7 void
 8 notebook_page_open (GtkNotebook *nb);
 9 
10 void
11 notebook_page_new_with_file (GtkNotebook *nb, GFile *file);
12 
13 void
14 notebook_page_new (GtkNotebook *nb);
15 
~~~

This header file describes the public functions in `tfenotebook.c`.

- 1-2: `notebook_page_save` saves the current page to the file of which the name specified in the tab.
If the name is `untitled` or `untitled` followed by digits, a file chooser dialog appears and a user can choose or specify a filename.
- 4-5: `notebook_page_close` closes the current page.
- 7-8: `notebook_page_open` shows a file chooser dialog and a user can choose a file. The contents of the file is inserted to a new page.
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
The other is `notebook ...`, which is the higher level layer.

Now let's look at the program of each function.

## notebook\_page\_new

~~~C
 1 static char*
 2 get_untitled () {
 3   static int c = -1;
 4   if (++c == 0) 
 5     return g_strdup_printf("Untitled");
 6   else
 7     return g_strdup_printf ("Untitled%u", c);
 8 }
 9 
10 static void
11 notebook_page_build (GtkNotebook *nb, GtkWidget *tv, const char *filename) {
12   GtkWidget *scr = gtk_scrolled_window_new ();
13   GtkNotebookPage *nbp;
14   GtkWidget *lab;
15   int i;
16 
17   gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
18   lab = gtk_label_new (filename);
19   i = gtk_notebook_append_page (nb, scr, lab);
20   nbp = gtk_notebook_get_page (nb, scr);
21   g_object_set (nbp, "tab-expand", TRUE, NULL);
22   gtk_notebook_set_current_page (nb, i);
23   g_signal_connect (GTK_TEXT_VIEW (tv), "change-file", G_CALLBACK (file_changed_cb), nb);
24 }
25 
26 void
27 notebook_page_new (GtkNotebook *nb) {
28   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
29 
30   GtkWidget *tv;
31   char *filename;
32 
33   tv = tfe_text_view_new ();
34   filename = get_untitled ();
35   notebook_page_build (nb, tv, filename);
36   g_free (filename);
37 }
~~~

- 26-37: The function `notebook_page_new`.
- 28: The function `g_return_if_fail` checks the argument. It's necessary because the function is public.
- 33: Creates TfeTextView object.
- 34: Creates filename, which is "Untitled", "Untitled1", ... .
- 1-8: The function `get_untitled`.
- 3: Static variable `c` is initialized at the first call of this function. After that `c` keeps its value unless it is changed explicitly.
- 4-7: Increases `c` by one and if it is zero, it returns "Untitled". If it is a positive integer, it returns "Untitled\<the integer\>", for example, "Untitled1", "Untitled2", and so on.
The function `g_strdup_printf` creates a string and it should be freed by `g_free` when it becomes useless.
The caller of `get_untitled` is in charge of freeing the string.
- 36: Calls `notebook_page_build` to build a new page.
- 37: Frees `filename`.
- 10- 24: The function `notebook_page_build`.
A parameter with `const` qualifier doesn't change in the function.
It means that the argument `filename` is owned by the caller.
The caller needs to free it when it becomes useless.
- 12: Creates GtkScrolledWindow.
- 17: Inserts `tv` to GtkScrolledWindow as a child.
- 18-19: Creates GtkLabel, then appends `scr` and `lab` to the GtkNotebook instance `nb`.
- 20-21: Sets "tab-expand" property to TRUE.
The function `g_object_set` sets properties on an object.
The object can be any object derived from GObject.
In many cases, an object has its own function to set its properties, but sometimes doesn't.
In that case, use `g_object_set` to set the property.
- 22: Sets the current page to the newly created page.
- 23: Connects "change-file" signal and the handler `file_changed_cb`.

## notebook\_page\_new\_with\_file

~~~C
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
13   g_free (filename);
14 }
~~~

- 9-10: Calls `tfe_text_view_new_with_file`.
If the function returns NULL, an error has happened.
Then, it does nothing and returns.
- 11-13: Gets the filename, builds a new page and frees `filename`.

## notebook\_page\_open

~~~C
 1 static void
 2 open_response_cb (TfeTextView *tv, int response, GtkNotebook *nb) {
 3   GFile *file;
 4   char *filename;
 5 
 6   if (response != TFE_OPEN_RESPONSE_SUCCESS) {
 7     g_object_ref_sink (tv);
 8     g_object_unref (tv);
 9   }else {
10     file = tfe_text_view_get_file (tv);
11     filename = g_file_get_basename (file);
12     g_object_unref (file);
13     notebook_page_build (nb, GTK_WIDGET (tv), filename);
14     g_free (filename);
15   }
16 }
17 
18 void
19 notebook_page_open (GtkNotebook *nb) {
20   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
21 
22   GtkWidget *tv;
23 
24   tv = tfe_text_view_new ();
25   g_signal_connect (TFE_TEXT_VIEW (tv), "open-response", G_CALLBACK (open_response_cb), nb);
26   tfe_text_view_open (TFE_TEXT_VIEW (tv), GTK_WINDOW (gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW)));
27 }
~~~

- 18-27: The function `notebook_page_open`.
- 24: Creates TfeTextView object.
- 25: Connects the signal "open-response" and the handler `open_response_cb`.
- 26: Calls `tfe_text_view_open`.
The "open-response" signal will be emitted later in this function to inform the result.
- 1-16: The handler `open_response_cb`.
- 6-8: If the response code is not `TFE_OPEN_RESPONSE_SUCCESS`, the instance `tv` will be destroyed.
It has floating reference, which will be explained later.
A floating reference needs to be converted into an ordinary reference before releasing it.
The function `g_object_ref_sink` does that.
After that, the function `g_object_unref` releases `tv` and decreases the reference count by one.
Finally the reference count becomes zero and `tv` is destroyed.
- 9-15: Otherwise, it builds a new page with `tv`.

## Floating reference

All the widgets are derived from GInitiallyUnowned.
GObject and GInitiallyUnowned are almost the same.
The difference is like this.
When an instance of GInitiallyUnowned is created, the instance has a "floating reference".
On the other hand, when an instance of GObject (not GInitiallyUnowned) is created, it has "normal reference".
Their descendants inherits them, so every widget has a floating reference just after the creation.
Non-widget class, for example, GtkTextBuffer is a direct sub class of GObject and it has normal reference.

The function `g_object_ref_sink` converts the floating reference into a normal reference.
If the instance doesn't have a floating reference, `g_object_ref_sink` simply increases the reference count by one.
It is used when an widget is added to another widget as a child.

~~~
GtkTextView *tv = gtk_text_view_new (); // Floating reference
GtkScrolledWindow *scr = gtk_scrolled_window_new ();
gtk_scrolled_window_set_child (scr, tv); // Scrolled window sink the tv's floating reference and tv's reference count becomes one.
~~~

When `tv` is added to `scr` as a child, `g_object_ref_sink` is used.

~~~
g_object_ref_sink (tv);
~~~

So, the floating reference is converted into an ordinary reference.
That is to say, floating reference is removed, and the normal reference count is one.
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

~~~C
 1 void
 2 notebook_page_close (GtkNotebook *nb) {
 3   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
 4 
 5   GtkWidget *win;
 6   int i;
 7 
 8   if (gtk_notebook_get_n_pages (nb) == 1) {
 9     win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);
10     gtk_window_destroy(GTK_WINDOW (win));
11   } else {
12     i = gtk_notebook_get_current_page (nb);
13     gtk_notebook_remove_page (GTK_NOTEBOOK (nb), i);
14   }
15 }
~~~

This function closes the current page.
If the page is the only page the notebook has, then the function destroys the top-level window and quits the application.

- 8-10: If the page is the only page the notebook has, it calls `gtk_window_destroy` to destroy the top-level window.
- 11-13: Otherwise, removes the current page.
The child widget (TfeTextView) is also destroyed.

## notebook\_page\_save

~~~C
 1 static TfeTextView *
 2 get_current_textview (GtkNotebook *nb) {
 3   int i;
 4   GtkWidget *scr;
 5   GtkWidget *tv;
 6 
 7   i = gtk_notebook_get_current_page (nb);
 8   scr = gtk_notebook_get_nth_page (nb, i);
 9   tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
10   return TFE_TEXT_VIEW (tv);
11 }
12 
13 void
14 notebook_page_save (GtkNotebook *nb) {
15   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
16 
17   TfeTextView *tv;
18 
19   tv = get_current_textview (nb);
20   tfe_text_view_save (tv);
21 }
~~~

- 13-21: `notebook_page_save`.
- 19: Gets the TfeTextView instance belongs to the current page.
The caller doesn't have the ownership of `tv` so you don't need to care about freeing it.
- 20: Calls `tfe_text_view_save`.
- 1-11: `get_current_textview`.
This function gets the TfeTextView object belongs to the current page.
- 7: Gets the page number of the current page.
- 8: Gets the child widget `scr`, which is a GtkScrolledWindow instance, of the current page. The object `scr` is owned by the notebook `nb`. So, the caller doesn't need to free it.
- 9-10: Gets the child widget of `scr`, which is a TfeTextView instance, and returns it.
The returned instance is owned by `scr` and the caller of `get_cuurent_textview` doesn't need to care about freeing it.

## file\_changed\_cb handler

The function `file_changed_cb` is a handler connected to "change-file" signal.
If a file in a TfeTextView instance is changed, the instance emits this signal.
This handler changes the label of the GtkNotebookPage.

~~~C
 1 static void
 2 file_changed_cb (TfeTextView *tv, GtkNotebook *nb) {
 3   GtkWidget *scr;
 4   GtkWidget *label;
 5   GFile *file;
 6   char *filename;
 7 
 8   file = tfe_text_view_get_file (tv);
 9   scr = gtk_widget_get_parent (GTK_WIDGET (tv));
10   if (G_IS_FILE (file)) {
11     filename = g_file_get_basename (file);
12     g_object_unref (file);
13   } else
14     filename = get_untitled ();
15   label = gtk_label_new (filename);
16   g_free (filename);
17   gtk_notebook_set_tab_label (nb, scr, label);
18 }
~~~

- 8: Gets the GFile instance from `tv`.
- 9: Gets the GkScrolledWindow instance which is the parent widget of `tv`.
- 10-12: If `file` points a GFile instance, the filename of the GFile is assigned to `filename`.
Then, unref the GFile object `file`.
- 13-14: Otherwise (file is NULL), a string `Untitled(number)` is assigned to `filename`.
- 15-17: Creates a GtkLabel instance `label` with the filename and set the label of the GtkNotebookPage with `label`.

Up: [README.md](../README.md),  Prev: [Section 13](sec13.md), Next: [Section 15](sec15.md)
