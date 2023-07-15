Up: [README.md](../README.md),  Prev: [Section 21](sec21.md), Next: [Section 23](sec23.md)

# TfeWindow

## The Tfe window and XML files

The following is the window of Tfe.

![tfe6](../image/tfe6.png)

- Open, save and close buttons are placed on the toolbar.
In addition, GtkMenuButton is added to the toolbar.
This button shows a popup menu when clicked on.
Here, popup means widely, including pull-down menu.
- New, save-as, preference and quit items are put into the menu.

This makes the most frequently used operation bound to the tool bar buttons.
And the others are stored in behind the menus.
So, it is more practical.

The window is a composite widget.
The definition is described in the XML file `tfewindow.ui`.

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <interface>
 3   <template class="TfeWindow" parent="GtkApplicationWindow">
 4     <property name="title">Text File Editor</property>
 5     <property name="default-width">600</property>
 6     <property name="default-height">400</property>
 7     <child>
 8       <object class="GtkBox" id="boxv">
 9         <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
10         <child>
11           <object class="GtkBox" id="boxh">
12             <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
13             <child>
14               <object class="GtkLabel">
15                 <property name="width-chars">10</property>
16               </object>
17             </child>
18             <child>
19               <object class="GtkButton">
20                 <property name="label">Open</property>
21                 <property name="action-name">win.open</property>
22               </object>
23             </child>
24             <child>
25               <object class="GtkButton">
26                 <property name="label">Save</property>
27                 <property name="action-name">win.save</property>
28               </object>
29             </child>
30             <child>
31               <object class="GtkLabel">
32                 <property name="hexpand">TRUE</property>
33               </object>
34             </child>
35             <child>
36               <object class="GtkButton">
37                 <property name="label">Close</property>
38                 <property name="action-name">win.close</property>
39               </object>
40             </child>
41             <child>
42               <object class="GtkMenuButton" id="btnm">
43                 <property name="direction">down</property>
44                 <property name="icon-name">open-menu-symbolic</property>
45               </object>
46             </child>
47             <child>
48               <object class="GtkLabel">
49                 <property name="width-chars">10</property>
50               </object>
51             </child>
52           </object>
53         </child>
54         <child>
55           <object class="GtkNotebook" id="nb">
56             <property name="scrollable">TRUE</property>
57             <property name="hexpand">TRUE</property>
58             <property name="vexpand">TRUE</property>
59           </object>
60         </child>
61       </object>
62     </child>
63   </template>
64 </interface>
65 
~~~

- Three buttons "Open", "Save" and "Close" are defined.
You can use two ways to catch the button click event.
The one is "clicked" signal and the other is to register an action to the button.
The first way is simple.
You can connects the signal and your handler directly.
The second way is like menu items.
When the button is clicked, the corresponding action is activated.
It is a bit complicated because you need to create an action and its "activate" handler in advance.
But one advantage is you can connect two or more things to the action.
For example, an accelerator can be connected to the action.
Accelerators are keys that connects to actions.
For example, Ctrl+O is often connected to a file open action.
So, both open button and Ctrl+O activates an open action.
The latter way is used in the XML file above.
- You can specify a theme icon to GtkMenuButton with "icon-name" poperty.
The "open-menu-symbolic" is an image that is called hamburger menu.

The `menu.ui` XML file defines the menu for GtkMenuButton.

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <interface>
 3   <menu id="menu">
 4     <section>
 5       <item>
 6         <attribute name="label">New</attribute>
 7         <attribute name="action">win.new</attribute>
 8       </item>
 9       <item>
10         <attribute name="label">Save As…</attribute>
11         <attribute name="action">win.saveas</attribute>
12       </item>
13     </section>
14     <section>
15       <item>
16         <attribute name="label">Preference</attribute>
17         <attribute name="action">win.pref</attribute>
18       </item>
19     </section>
20     <section>
21       <item>
22         <attribute name="label">Quit</attribute>
23         <attribute name="action">win.close-all</attribute>
24       </item>
25     </section>
26   </menu>
27 </interface>
~~~

There are four menu items and they are connected to actions.

## The header file

The following is the codes of `tfewindow.h`.

~~~C
 1 #pragma once
 2 
 3 #include <gtk/gtk.h>
 4 
 5 #define TFE_TYPE_WINDOW tfe_window_get_type ()
 6 G_DECLARE_FINAL_TYPE (TfeWindow, tfe_window, TFE, WINDOW, GtkApplicationWindow)
 7 
 8 void
 9 tfe_window_notebook_page_new (TfeWindow *win);
10 
11 void
12 tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files);
13 
14 GtkWidget *
15 tfe_window_new (GtkApplication *app);
~~~

- 5-6: `TFE_TYPE_WINDOW` definition and the `G_DECLARE_FINAL_TYPE` macro.
- 8-15: Public functions. The first two functions creates a notebook page and the last function creates a window.

## C file

### A composite widget

The following codes are extracted from `tfewindow.c`.

```C
#include <gtk/gtk.h>
#include "tfewindow.h"

struct _TfeWindow {
  GtkApplicationWindow parent;
  GtkMenuButton *btnm;
  GtkNotebook *nb;
  gboolean is_quit;
};

G_DEFINE_FINAL_TYPE (TfeWindow, tfe_window, GTK_TYPE_APPLICATION_WINDOW);

static void
tfe_window_dispose (GObject *gobject) {
  gtk_widget_dispose_template (GTK_WIDGET (gobject), TFE_TYPE_WINDOW);
  G_OBJECT_CLASS (tfe_window_parent_class)->dispose (gobject);
}

static void
tfe_window_init (TfeWindow *win) {
  GtkBuilder *build;
  GMenuModel *menu;

  gtk_widget_init_template (GTK_WIDGET (win));

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/menu.ui");
  menu = G_MENU_MODEL (gtk_builder_get_object (build, "menu"));
  gtk_menu_button_set_menu_model (win->btnm, menu);
  g_object_unref(build);
... ... ...
}

static void
tfe_window_class_init (TfeWindowClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = tfe_window_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfewindow.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, btnm);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, nb);
}

GtkWidget *
tfe_window_new (GtkApplication *app) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_WINDOW, "application", app, NULL));
}
```

The program above is similar to `tfealert.c` and `tfepref.c`.
It uses the same way to build a composite widget.
But there's one thing new.
It is menu.
The menu is built from the XML resource `menu.ui` and inserted into the menu button.
It is done in the instance initialization function `tfe_window_init`.

### Actions

Actions can belong to an application or window.
Tfe only has one top window and all the actions are registered in the window.
For example, "close-all" action destroys the top level window and that brings the application to quit.
You can make "app.quit"  action instead of "win.close-all".
It's your choice.
If your application has two or more windows, both "app.quit" and "win:close-all", which closes all the notebook pages on the window, may be necessary.
Anyway, you need to consider that each action should belong to the application or a window.

Actions are defined in the instance initialization function.

```C
static void
tfe_window_init (TfeWindow *win) {
... ... ...
/* ----- action ----- */
  const GActionEntry win_entries[] = {
    { "open", open_activated, NULL, NULL, NULL },
    { "save", save_activated, NULL, NULL, NULL },
    { "close", close_activated, NULL, NULL, NULL },
    { "new", new_activated, NULL, NULL, NULL },
    { "saveas", saveas_activated, NULL, NULL, NULL },
    { "pref", pref_activated, NULL, NULL, NULL },
    { "close-all", close_all_activated, NULL, NULL, NULL }
  };
  g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);
... ... ...
}
```

Two things are necessary, an array and the `g_action_map_add_action_entries` function.

- The element of the array is the GActionEntry structure.
The structure has the following members:
  - an action name
  - a handler for the activate signal
  - the type of the parameter or NULL for no parameter
  - the initial state for the action
  - a handler for the change-state signal
- The actions above are stateless and have no parameters.
So, the third parameter and after are all NULL.
- The function `g_action_map_add_action_entries` adds the actions in the `win_entries` array to the action map `win`.
The last argument `win` is the user_data, which is the last argument of handlers.
- All the handlers are in `tfewindow.c` program and shown in the following subsections.

### The handlers of the actions

#### open\_activated

The callback function `open_activated` is an activate signal handler on "open" action.

~~~C
1 static void
2 open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
3   TfeWindow *win = TFE_WINDOW (user_data);
4   GtkWidget *tv = tfe_text_view_new ();
5 
6   g_signal_connect (TFE_TEXT_VIEW (tv), "open-response", G_CALLBACK (open_response_cb), win);
7   tfe_text_view_open (TFE_TEXT_VIEW (tv), GTK_WINDOW (win));
8 }
~~~

It connects the "open-response" signal on the newly created TfeTextView instance and just calls `tfe_text_view_open`.
It leaves the rest of the task to the signal handler `open_response_cb`.

~~~C
 1 static void
 2 open_response_cb (TfeTextView *tv, int response, gpointer user_data) {
 3   TfeWindow *win = TFE_WINDOW (user_data);
 4   GFile *file;
 5   char *filename;
 6 
 7   if (response != TFE_OPEN_RESPONSE_SUCCESS) {
 8     g_object_ref_sink (tv);
 9     g_object_unref (tv);
10   }else if (! G_IS_FILE (file = tfe_text_view_get_file (tv))) {
11     g_object_ref_sink (tv);
12     g_object_unref (tv);
13   }else {
14     filename = g_file_get_basename (file);
15     g_object_unref (file);
16     notebook_page_build (win, GTK_WIDGET (tv), filename);
17     g_free (filename);
18   }
19 }
~~~

If the TfeTextView instance failed to read a file, it destroys the instance with `g_object_ref_sink` and `g_object_unref`.
Since newly created widgets are floating, you need to convert the floating reference to the normal reference before you release it.
The conversion is done with `g_object_ref_sink`.

If the instance successfully read the file, it calls `notebook_page_build` to build a notebook page and add it to the GtkNotebook object.

~~~C
 1 static void
 2 notebook_page_build (TfeWindow *win, GtkWidget *tv, char *filename) {
 3   // The arguments win, tb and filename are owned by the caller.
 4   // If tv has a floating reference, it is consumed by the function.
 5   GtkWidget *scr = gtk_scrolled_window_new ();
 6   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
 7   GtkNotebookPage *nbp;
 8   GtkWidget *lab;
 9   int i;
10 
11   gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
12   gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
13   lab = gtk_label_new (filename);
14   i = gtk_notebook_append_page (win->nb, scr, lab);
15   nbp = gtk_notebook_get_page (win->nb, scr);
16   g_object_set (nbp, "tab-expand", TRUE, NULL);
17   gtk_notebook_set_current_page (win->nb, i);
18   g_signal_connect (GTK_TEXT_VIEW (tv), "change-file", G_CALLBACK (file_changed_cb), win->nb);
19   g_signal_connect (tb, "modified-changed", G_CALLBACK (modified_changed_cb), tv);
20 }
~~~

This function is a kind of library function and it is called from the different three places.

This function creates a new GtkScrolledWindow instance and sets its child to `tv`.
Then it appends it to the GtkNotebook instance `win->nb`.
And it sets the tab label to the filename.

After the building, it connects two signals and handlers.

- "change-file" signal and `file_changed_cb` handler.
If the TfeTextView instance changes the file, the handler is called and the notebook page tab is updated.
- "modified-changed" signal and `modified_changed_cb` handler.
If the text in the buffer of TfeTextView instance is modified, an asterisk is added at the beginning of the filename of the notebook page tab.
If the text is saved to the file, the asterisk is removed.
The asterisk tells the user that the text has been modified or not.

~~~C
 1 static void
 2 file_changed_cb (TfeTextView *tv, gpointer user_data) {
 3   GtkNotebook *nb =  GTK_NOTEBOOK (user_data);
 4   GtkWidget *scr;
 5   GtkWidget *label;
 6   GFile *file;
 7   char *filename;
 8 
 9   file = tfe_text_view_get_file (tv);
10   scr = gtk_widget_get_parent (GTK_WIDGET (tv));
11   if (G_IS_FILE (file)) {
12     filename = g_file_get_basename (file);
13     g_object_unref (file);
14   } else
15     filename = get_untitled ();
16   label = gtk_label_new (filename);
17   g_free (filename);
18   gtk_notebook_set_tab_label (GTK_NOTEBOOK (nb), scr, label);
19 }
20 
21 static void
22 modified_changed_cb (GtkTextBuffer *tb, gpointer user_data) {
23   TfeTextView *tv = TFE_TEXT_VIEW (user_data);
24   GtkWidget *scr = gtk_widget_get_parent (GTK_WIDGET (tv));
25   GtkWidget *nb =  gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_NOTEBOOK);
26   GtkWidget *label;
27   GFile *file;
28   char *filename;
29   char *text;
30 
31   file = tfe_text_view_get_file (tv);
32   filename = g_file_get_basename (file);
33   if (gtk_text_buffer_get_modified (tb))
34     text = g_strdup_printf ("*%s", filename);
35   else
36     text = g_strdup (filename);
37   g_object_unref (file);
38   g_free (filename);
39   label = gtk_label_new (text);
40   g_free (text);
41   gtk_notebook_set_tab_label (GTK_NOTEBOOK (nb), scr, label);
42 }
~~~

#### save\_activated

The callback function `save_activated` is an activate signal handler on "save" action.

~~~C
1 static void
2 save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
3   TfeWindow *win = TFE_WINDOW (user_data);
4   TfeTextView *tv = get_current_textview (win->nb);
5 
6   tfe_text_view_save (TFE_TEXT_VIEW (tv));
7 }
~~~

This function gets the current TfeTextView instance with the function `get_current_textview`.
And it just calls `tfe_text_view_save`.

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
~~~

#### close\_activated

The callback function `close_activated` is an activate signal handler on "close" action.
It closes the current notebook page.

~~~C
 1 static void
 2 close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 3   TfeWindow *win = TFE_WINDOW (user_data);
 4   TfeTextView *tv;
 5   GtkTextBuffer *tb;
 6   GtkWidget *alert;
 7 
 8   tv = get_current_textview (win->nb);
 9   tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
10   if (! gtk_text_buffer_get_modified (tb)) /* is saved? */
11     notebook_page_close (win);
12   else {
13     win->is_quit = FALSE;
14     alert = tfe_alert_new_with_data ("Are you sure?", "Contents aren't saved yet.\nAre you sure to close?", "Close");
15     gtk_window_set_transient_for (GTK_WINDOW (alert), GTK_WINDOW (win));
16     g_signal_connect (TFE_ALERT (alert), "response", G_CALLBACK (alert_response_cb), win);
17     gtk_window_present (GTK_WINDOW (alert));
18   }
19 }
~~~

If the text in the current page has been saved, it calls `notebook_page_close` to close the page.
Otherwise, it sets `win->is_quit` to FALSE and show the alert dialog.
The "response" signal on the dialog is connected to the handler `alert_response_cb`.

~~~C
 1 static void
 2 notebook_page_close (TfeWindow *win){
 3   int i;
 4 
 5   if (gtk_notebook_get_n_pages (win->nb) == 1)
 6     gtk_window_destroy (GTK_WINDOW (win));
 7   else {
 8     i = gtk_notebook_get_current_page (win->nb);
 9     gtk_notebook_remove_page (win->nb, i);
10   }
11 }
~~~

If the notebook has only one page, it destroys the window and the application quits.
Otherwise, it removes the current page.

~~~C
 1 static void
 2 alert_response_cb (TfeAlert *alert, int response_id, gpointer user_data) {
 3   TfeWindow *win = TFE_WINDOW (user_data);
 4 
 5   if (response_id == TFE_ALERT_RESPONSE_ACCEPT) {
 6     if (win->is_quit)
 7       gtk_window_destroy(GTK_WINDOW (win));
 8     else
 9       notebook_page_close (win);
10   }
11 }
~~~

If the user clicked on the cacel button, it does nothing.
If the user clicked on the accept button, which is the same as close button, it calls `notebook_page_close`.
Note that `win->is_quit` has been set to FALSE in the `close_activated` function.

#### new\_activated

The callback function `new_activated` is an activate signal handler on "new" action.

~~~C
1 static void
2 new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
3   TfeWindow *win = TFE_WINDOW (user_data);
4 
5   tfe_window_notebook_page_new (win);
6 }
~~~

It just calls `tfe_window_notebook_page_new`, which is a public method of TfeWindow.

~~~C
 1 void
 2 tfe_window_notebook_page_new (TfeWindow *win) {
 3   GtkWidget *tv;
 4   char *filename;
 5 
 6   tv = tfe_text_view_new ();
 7   filename = get_untitled ();
 8   notebook_page_build (win, tv, filename);
 9   g_free (filename);
10 }
~~~

This function creates a new TfeTextView instance, "Untitled" family string and calls `notebook_page_build`.

#### saveas\_activated

The callback function `saveas_activated` is an activate signal handler on "saveas" action.

~~~C
1 static void
2 saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
3   TfeWindow *win = TFE_WINDOW (user_data);
4   TfeTextView *tv = get_current_textview (win->nb);
5 
6   tfe_text_view_saveas (TFE_TEXT_VIEW (tv));
7 }
~~~

This function gets the current page TfeTextView instance and calls `tfe_text_view_saveas`.

#### pref\_activated

The callback function `pref_activated` is an activate signal handler on "pref" action.

~~~C
1 static void
2 pref_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
3   TfeWindow *win = TFE_WINDOW (user_data);
4   GtkWidget *pref;
5 
6   pref = tfe_pref_new ();
7   gtk_window_set_transient_for (GTK_WINDOW (pref), GTK_WINDOW (win));
8   gtk_window_present (GTK_WINDOW (pref));
9 }
~~~

This function creates a TfePref instance, which is a dialog, and sets the transient parent window to `win`.
And it shows the dialog.

#### close\_all\_activated

The callback function `close_all_activated` is an activate signal handler on "close_all" action.

~~~C
1 static void
2 close_all_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
3   TfeWindow *win = TFE_WINDOW (user_data);
4 
5   if (close_request_cb (win) == FALSE)
6     gtk_window_destroy (GTK_WINDOW (win));
7 }
~~~

It first calls the function `close_request_cb`.
It is a callback function for the "close-request" signal on the top window.
It returns FALSE if all the texts have been saved.
Otherwise it returns TRUE.

Therefore, function `close_all_activated` destroys the top window if all the texts have been saved.
Otherwise it does nothing.
But, the function `close_request_cb` shows an alert dialog and if the user clicks on the accept button, the window will be destroyed.

### Window "close-request" signal

GtkWindow has a "close-request" signal and it is emitted when the close button, which is x-shaped button at the top right corner, is clicked on.
And the user handler is called before the default handler.
If the user handler returns TRUE, the rest of the close process is skipped.
If it returns FALSE, the rest will go on and the window will be destroyed.

~~~C
 1 static gboolean
 2 close_request_cb (TfeWindow *win) {
 3   TfeAlert *alert;
 4 
 5   if (is_saved_all (win->nb))
 6     return FALSE;
 7   else {
 8     win->is_quit = TRUE;
 9     alert = TFE_ALERT (tfe_alert_new_with_data ("Are you sure?", "Contents aren't saved yet.\nAre you sure to quit?", "Quit"));
10     gtk_window_set_transient_for (GTK_WINDOW (alert), GTK_WINDOW (win));
11     g_signal_connect (TFE_ALERT (alert), "response", G_CALLBACK (alert_response_cb), win);
12     gtk_window_present (GTK_WINDOW (alert));
13     return TRUE;
14   }
15 }
~~~

First, it calls `is_saved_all` and checks if the texts have been saved.
If so, it returns FALSE and the close process continues.
Otherwise, it sets `win->is_quit` to TRUE and shows an alert dialog.
When the user clicks on the accept or cancel button, the dialog disappears and "response" signal is emitted.
Then, the handler `alert_response_cb` is called.
It destroys the top window if the user clicked on the accept button since `win->is_quit` is TRUE.
Otherwise it does nothing.

~~~C
 1 static gboolean
 2 is_saved_all (GtkNotebook *nb) {
 3   int i, n;
 4   GtkWidget *scr;
 5   GtkWidget *tv;
 6   GtkTextBuffer *tb;
 7 
 8   n = gtk_notebook_get_n_pages (nb);
 9   for (i = 0; i < n; ++i) {
10     scr = gtk_notebook_get_nth_page (nb, i);
11     tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
12     tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
13     if (gtk_text_buffer_get_modified (tb))
14       return FALSE;
15   }
16   return TRUE;
17 }
~~~

### Public functions

There are three public functions.

- `void tfe_window_notebook_page_new (TfeWindow *win)`
- `void tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files)`
- `GtkWidget *tfe_window_new (GtkApplication *app)`

The first function is called when the application emits the "activate" signal.
The second is for "open" signal.
It is given three arguments and they are owned by the caller.

~~~C
 1 void
 2 tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files) {
 3   int i;
 4   GtkWidget *tv;
 5   char *filename;
 6 
 7   for (i = 0; i < n_files; i++)
 8     if ((tv = tfe_text_view_new_with_file (*(files+i))) != NULL) {
 9       filename = g_file_get_basename (*(files+i));
10       notebook_page_build (win, tv, filename);
11       g_free (filename);
12     }
13   if (gtk_notebook_get_n_pages (win->nb) == 0)
14     tfe_window_notebook_page_new (win);
15 }
~~~

This function has a loop for the array `files`.
It creates TfeTextView instance with the text from each file.
And build a page with it.

If an error happens and no page is created, it creates a new empty page.

### Full codes of tfewindow.c

The following is the full source codes of `tfewindow.c`.

~~~C
  1 #include <gtk/gtk.h>
  2 #include "tfewindow.h"
  3 #include "tfepref.h"
  4 #include "tfealert.h"
  5 #include "../tfetextview/tfetextview.h"
  6 
  7 struct _TfeWindow {
  8   GtkApplicationWindow parent;
  9   GtkMenuButton *btnm;
 10   GtkNotebook *nb;
 11   gboolean is_quit;
 12 };
 13 
 14 G_DEFINE_FINAL_TYPE (TfeWindow, tfe_window, GTK_TYPE_APPLICATION_WINDOW);
 15 
 16 /* Low level functions */
 17 
 18 /* Create a new untitled string */
 19 /* The returned string should be freed with g_free() when no longer needed. */
 20 static char*
 21 get_untitled () {
 22   static int c = -1;
 23   if (++c == 0) 
 24     return g_strdup_printf("Untitled");
 25   else
 26     return g_strdup_printf ("Untitled%u", c);
 27 }
 28 
 29 /* The returned object is owned by the scrolled window. */
 30 /* The caller won't get the ownership and mustn't release it. */
 31 static TfeTextView *
 32 get_current_textview (GtkNotebook *nb) {
 33   int i;
 34   GtkWidget *scr;
 35   GtkWidget *tv;
 36 
 37   i = gtk_notebook_get_current_page (nb);
 38   scr = gtk_notebook_get_nth_page (nb, i);
 39   tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
 40   return TFE_TEXT_VIEW (tv);
 41 }
 42 
 43 /* This call back is called when a TfeTextView instance emits a "change-file" signal. */
 44 static void
 45 file_changed_cb (TfeTextView *tv, gpointer user_data) {
 46   GtkNotebook *nb =  GTK_NOTEBOOK (user_data);
 47   GtkWidget *scr;
 48   GtkWidget *label;
 49   GFile *file;
 50   char *filename;
 51 
 52   file = tfe_text_view_get_file (tv);
 53   scr = gtk_widget_get_parent (GTK_WIDGET (tv));
 54   if (G_IS_FILE (file)) {
 55     filename = g_file_get_basename (file);
 56     g_object_unref (file);
 57   } else
 58     filename = get_untitled ();
 59   label = gtk_label_new (filename);
 60   g_free (filename);
 61   gtk_notebook_set_tab_label (GTK_NOTEBOOK (nb), scr, label);
 62 }
 63 
 64 static void
 65 modified_changed_cb (GtkTextBuffer *tb, gpointer user_data) {
 66   TfeTextView *tv = TFE_TEXT_VIEW (user_data);
 67   GtkWidget *scr = gtk_widget_get_parent (GTK_WIDGET (tv));
 68   GtkWidget *nb =  gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_NOTEBOOK);
 69   GtkWidget *label;
 70   GFile *file;
 71   char *filename;
 72   char *text;
 73 
 74   file = tfe_text_view_get_file (tv);
 75   filename = g_file_get_basename (file);
 76   if (gtk_text_buffer_get_modified (tb))
 77     text = g_strdup_printf ("*%s", filename);
 78   else
 79     text = g_strdup (filename);
 80   g_object_unref (file);
 81   g_free (filename);
 82   label = gtk_label_new (text);
 83   g_free (text);
 84   gtk_notebook_set_tab_label (GTK_NOTEBOOK (nb), scr, label);
 85 }
 86 
 87 static gboolean
 88 is_saved_all (GtkNotebook *nb) {
 89   int i, n;
 90   GtkWidget *scr;
 91   GtkWidget *tv;
 92   GtkTextBuffer *tb;
 93 
 94   n = gtk_notebook_get_n_pages (nb);
 95   for (i = 0; i < n; ++i) {
 96     scr = gtk_notebook_get_nth_page (nb, i);
 97     tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
 98     tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
 99     if (gtk_text_buffer_get_modified (tb))
100       return FALSE;
101   }
102   return TRUE;
103 }
104 
105 static void
106 notebook_page_close (TfeWindow *win){
107   int i;
108 
109   if (gtk_notebook_get_n_pages (win->nb) == 1)
110     gtk_window_destroy (GTK_WINDOW (win));
111   else {
112     i = gtk_notebook_get_current_page (win->nb);
113     gtk_notebook_remove_page (win->nb, i);
114   }
115 }
116 
117 static void
118 notebook_page_build (TfeWindow *win, GtkWidget *tv, char *filename) {
119   // The arguments win, tb and filename are owned by the caller.
120   // If tv has a floating reference, it is consumed by the function.
121   GtkWidget *scr = gtk_scrolled_window_new ();
122   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
123   GtkNotebookPage *nbp;
124   GtkWidget *lab;
125   int i;
126 
127   gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
128   gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
129   lab = gtk_label_new (filename);
130   i = gtk_notebook_append_page (win->nb, scr, lab);
131   nbp = gtk_notebook_get_page (win->nb, scr);
132   g_object_set (nbp, "tab-expand", TRUE, NULL);
133   gtk_notebook_set_current_page (win->nb, i);
134   g_signal_connect (GTK_TEXT_VIEW (tv), "change-file", G_CALLBACK (file_changed_cb), win->nb);
135   g_signal_connect (tb, "modified-changed", G_CALLBACK (modified_changed_cb), tv);
136 }
137 
138 static void
139 open_response_cb (TfeTextView *tv, int response, gpointer user_data) {
140   TfeWindow *win = TFE_WINDOW (user_data);
141   GFile *file;
142   char *filename;
143 
144   if (response != TFE_OPEN_RESPONSE_SUCCESS) {
145     g_object_ref_sink (tv);
146     g_object_unref (tv);
147   }else if (! G_IS_FILE (file = tfe_text_view_get_file (tv))) {
148     g_object_ref_sink (tv);
149     g_object_unref (tv);
150   }else {
151     filename = g_file_get_basename (file);
152     g_object_unref (file);
153     notebook_page_build (win, GTK_WIDGET (tv), filename);
154     g_free (filename);
155   }
156 }
157 
158 /* alert response signal handler */
159 static void
160 alert_response_cb (TfeAlert *alert, int response_id, gpointer user_data) {
161   TfeWindow *win = TFE_WINDOW (user_data);
162 
163   if (response_id == TFE_ALERT_RESPONSE_ACCEPT) {
164     if (win->is_quit)
165       gtk_window_destroy(GTK_WINDOW (win));
166     else
167       notebook_page_close (win);
168   }
169 }
170 
171 /* ----- Close request on the top window ----- */
172 /* ----- The signal is emitted when the close button is clicked. ----- */
173 static gboolean
174 close_request_cb (TfeWindow *win) {
175   TfeAlert *alert;
176 
177   if (is_saved_all (win->nb))
178     return FALSE;
179   else {
180     win->is_quit = TRUE;
181     alert = TFE_ALERT (tfe_alert_new_with_data ("Are you sure?", "Contents aren't saved yet.\nAre you sure to quit?", "Quit"));
182     gtk_window_set_transient_for (GTK_WINDOW (alert), GTK_WINDOW (win));
183     g_signal_connect (TFE_ALERT (alert), "response", G_CALLBACK (alert_response_cb), win);
184     gtk_window_present (GTK_WINDOW (alert));
185     return TRUE;
186   }
187 }
188 
189 /* ----- action activated handlers ----- */
190 static void
191 open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
192   TfeWindow *win = TFE_WINDOW (user_data);
193   GtkWidget *tv = tfe_text_view_new ();
194 
195   g_signal_connect (TFE_TEXT_VIEW (tv), "open-response", G_CALLBACK (open_response_cb), win);
196   tfe_text_view_open (TFE_TEXT_VIEW (tv), GTK_WINDOW (win));
197 }
198 
199 static void
200 save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
201   TfeWindow *win = TFE_WINDOW (user_data);
202   TfeTextView *tv = get_current_textview (win->nb);
203 
204   tfe_text_view_save (TFE_TEXT_VIEW (tv));
205 }
206 
207 static void
208 close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
209   TfeWindow *win = TFE_WINDOW (user_data);
210   TfeTextView *tv;
211   GtkTextBuffer *tb;
212   GtkWidget *alert;
213 
214   tv = get_current_textview (win->nb);
215   tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
216   if (! gtk_text_buffer_get_modified (tb)) /* is saved? */
217     notebook_page_close (win);
218   else {
219     win->is_quit = FALSE;
220     alert = tfe_alert_new_with_data ("Are you sure?", "Contents aren't saved yet.\nAre you sure to close?", "Close");
221     gtk_window_set_transient_for (GTK_WINDOW (alert), GTK_WINDOW (win));
222     g_signal_connect (TFE_ALERT (alert), "response", G_CALLBACK (alert_response_cb), win);
223     gtk_window_present (GTK_WINDOW (alert));
224   }
225 }
226 
227 static void
228 new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
229   TfeWindow *win = TFE_WINDOW (user_data);
230 
231   tfe_window_notebook_page_new (win);
232 }
233 
234 static void
235 saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
236   TfeWindow *win = TFE_WINDOW (user_data);
237   TfeTextView *tv = get_current_textview (win->nb);
238 
239   tfe_text_view_saveas (TFE_TEXT_VIEW (tv));
240 }
241 
242 static void
243 pref_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
244   TfeWindow *win = TFE_WINDOW (user_data);
245   GtkWidget *pref;
246 
247   pref = tfe_pref_new ();
248   gtk_window_set_transient_for (GTK_WINDOW (pref), GTK_WINDOW (win));
249   gtk_window_present (GTK_WINDOW (pref));
250 }
251 
252 static void
253 close_all_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
254   TfeWindow *win = TFE_WINDOW (user_data);
255 
256   if (close_request_cb (win) == FALSE)
257     gtk_window_destroy (GTK_WINDOW (win));
258 }
259 
260 /* --- public functions --- */
261 
262 void
263 tfe_window_notebook_page_new (TfeWindow *win) {
264   GtkWidget *tv;
265   char *filename;
266 
267   tv = tfe_text_view_new ();
268   filename = get_untitled ();
269   notebook_page_build (win, tv, filename);
270   g_free (filename);
271 }
272 
273 void
274 tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files) {
275   int i;
276   GtkWidget *tv;
277   char *filename;
278 
279   for (i = 0; i < n_files; i++)
280     if ((tv = tfe_text_view_new_with_file (*(files+i))) != NULL) {
281       filename = g_file_get_basename (*(files+i));
282       notebook_page_build (win, tv, filename);
283       g_free (filename);
284     }
285   if (gtk_notebook_get_n_pages (win->nb) == 0)
286     tfe_window_notebook_page_new (win);
287 }
288 
289 static void
290 tfe_window_dispose (GObject *gobject) {
291   gtk_widget_dispose_template (GTK_WIDGET (gobject), TFE_TYPE_WINDOW);
292   G_OBJECT_CLASS (tfe_window_parent_class)->dispose (gobject);
293 }
294 
295 static void
296 tfe_window_init (TfeWindow *win) {
297   GtkBuilder *build;
298   GMenuModel *menu;
299 
300   gtk_widget_init_template (GTK_WIDGET (win));
301 
302   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/menu.ui");
303   menu = G_MENU_MODEL (gtk_builder_get_object (build, "menu"));
304   gtk_menu_button_set_menu_model (win->btnm, menu);
305   g_object_unref(build);
306 
307 /* ----- action ----- */
308   const GActionEntry win_entries[] = {
309     { "open", open_activated, NULL, NULL, NULL },
310     { "save", save_activated, NULL, NULL, NULL },
311     { "close", close_activated, NULL, NULL, NULL },
312     { "new", new_activated, NULL, NULL, NULL },
313     { "saveas", saveas_activated, NULL, NULL, NULL },
314     { "pref", pref_activated, NULL, NULL, NULL },
315     { "close-all", close_all_activated, NULL, NULL, NULL }
316   };
317   g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);
318 
319   g_signal_connect (GTK_WINDOW (win), "close-request", G_CALLBACK (close_request_cb), NULL);
320 }
321 
322 static void
323 tfe_window_class_init (TfeWindowClass *class) {
324   GObjectClass *object_class = G_OBJECT_CLASS (class);
325 
326   object_class->dispose = tfe_window_dispose;
327   gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfewindow.ui");
328   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, btnm);
329   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, nb);
330 }
331 
332 GtkWidget *
333 tfe_window_new (GtkApplication *app) {
334   return GTK_WIDGET (g_object_new (TFE_TYPE_WINDOW, "application", app, NULL));
335 }
~~~

Up: [README.md](../README.md),  Prev: [Section 21](sec21.md), Next: [Section 23](sec23.md)
