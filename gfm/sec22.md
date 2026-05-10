Up: [README.md](../README.md),  Prev: [Section 21](sec21.md), Next: [Section 23](sec23.md)

# TfeWindow

## The Tfe Window and XML Files

The following is the window of Tfe.

![tfe6](/src/images/tfe6.png)

- Open, save and close buttons are placed on the toolbar.
In addition, GtkMenuButton is added to the toolbar.
This button shows a popup menu when clicked on.
Here, popup means widely, including pull-down menu.
- New, save-as, preference and quit items are put into the menu.

This makes the most frequently used operation bound to the tool bar buttons.
And the others are stored behind the menus.
So, it is more practical.

The window is a composite widget.
The definition is described in the XML file `tfewindow.ui`.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <template class="TfeWindow" parent="GtkApplicationWindow">
    <property name="title">Text File Editor</property>
    <property name="default-width">600</property>
    <property name="default-height">400</property>
    <child>
      <object class="GtkBox" id="boxv">
        <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
        <child>
          <object class="GtkBox" id="boxh">
            <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
            <child>
              <object class="GtkLabel">
                <property name="width-chars">10</property>
              </object>
            </child>
            <child>
              <object class="GtkButton">
                <property name="label">Open</property>
                <property name="action-name">win.open</property>
              </object>
            </child>
            <child>
              <object class="GtkButton">
                <property name="label">Save</property>
                <property name="action-name">win.save</property>
              </object>
            </child>
            <child>
              <object class="GtkLabel">
                <property name="hexpand">TRUE</property>
              </object>
            </child>
            <child>
              <object class="GtkButton">
                <property name="label">Close</property>
                <property name="action-name">win.close</property>
              </object>
            </child>
            <child>
              <object class="GtkMenuButton" id="btnm">
                <property name="direction">down</property>
                <property name="icon-name">open-menu-symbolic</property>
              </object>
            </child>
            <child>
              <object class="GtkLabel">
                <property name="width-chars">10</property>
              </object>
            </child>
          </object>
        </child>
        <child>
          <object class="GtkNotebook" id="nb">
            <property name="scrollable">TRUE</property>
            <property name="hexpand">TRUE</property>
            <property name="vexpand">TRUE</property>
          </object>
        </child>
      </object>
    </child>
  </template>
</interface>

```

- Three buttons "Open", "Save" and "Close" are defined.
You can use two ways to catch the button click event.
One is "clicked" signal and the other is to register an action to the button.
The first way is simple.
You can connects the signal and your handler directly.
The second way is like menu items.
When the button is clicked, the corresponding action is activated.
It is a bit complicated because you need to create an action and its "activate" handler in advance.
But one advantage is you can connect two or more things to the action.
For example, an accelerator can be connected to the action.
Accelerators are keys that connect to actions.
For example, Ctrl+O is often connected to a file open action.
So, both open button and Ctrl+O activate an open action.
The latter way is used in the XML file above.
- You can specify a theme icon to GtkMenuButton with "icon-name" property.
The "open-menu-symbolic" is an image that is called hamburger menu.

The `menu.ui` XML file defines the menu for GtkMenuButton.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <menu id="menu">
    <section>
      <item>
        <attribute name="label">New</attribute>
        <attribute name="action">win.new</attribute>
      </item>
      <item>
        <attribute name="label">Save As…</attribute>
        <attribute name="action">win.saveas</attribute>
      </item>
    </section>
    <section>
      <item>
        <attribute name="label">Preference</attribute>
        <attribute name="action">win.pref</attribute>
      </item>
    </section>
    <section>
      <item>
        <attribute name="label">Quit</attribute>
        <attribute name="action">win.close-all</attribute>
      </item>
    </section>
  </menu>
</interface>
```

There are four menu items and they are connected to actions.

## The Header File

The following is the codes of `tfewindow.h`.

```c
#pragma once

#include <gtk/gtk.h>

#define TFE_TYPE_WINDOW tfe_window_get_type ()
G_DECLARE_FINAL_TYPE (TfeWindow, tfe_window, TFE, WINDOW, GtkApplicationWindow)

void
tfe_window_notebook_page_new (TfeWindow *win);

void
tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files);

GtkWidget *
tfe_window_new (GtkApplication *app);
```

- 5-6: `TFE_TYPE_WINDOW` definition and the `G_DECLARE_FINAL_TYPE` macro.
- 8-15: Public functions. The first two functions create a notebook page and the last function creates a window.

## C File

### Composite Widget

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
If your application has two or more windows, both "app.quit" and "win.close-all", which closes all the notebook pages on the window, may be necessary.
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

### The Handlers of the Actions

#### open\_activated

The callback function `open_activated` is an activate signal handler on "open" action.

```c
static void
open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  GtkWidget *tv = tfe_text_view_new ();

  g_signal_connect (tv, "open-response", G_CALLBACK (open_response_cb), win);
  tfe_text_view_open (TFE_TEXT_VIEW (tv), GTK_WINDOW (win));
}
```

It connects the "open-response" signal on the newly created TfeTextView instance and just calls `tfe_text_view_open`.
It leaves the rest of the task to the signal handler `open_response_cb`.

```c
static void
open_response_cb (TfeTextView *tv, int response, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  GFile *file;
  char *filename;

  if (response != TFE_OPEN_RESPONSE_SUCCESS) {
    g_object_ref_sink (tv);
    g_object_unref (tv);
  }else if (! G_IS_FILE (file = tfe_text_view_get_file (tv))) {
    g_object_ref_sink (tv);
    g_object_unref (tv);
  }else {
    filename = g_file_get_basename (file);
    g_object_unref (file);
    notebook_page_build (win, GTK_WIDGET (tv), filename);
    g_free (filename);
  }
}
```

If the TfeTextView instance failed to read a file, it destroys the instance with `g_object_ref_sink` and `g_object_unref`.
Since newly created widgets have a floating reference, you need to convert the floating reference to the normal reference before you release it.
The conversion is done with `g_object_ref_sink`.

If the instance successfully read the file, it calls `notebook_page_build` to build a notebook page and add it to the GtkNotebook object.

```c
static void
notebook_page_build (TfeWindow *win, GtkWidget *tv, char *filename) {
  // The arguments win, tv and filename are owned by the caller.
  // If tv has a floating reference, it is consumed by the function.
  GtkWidget *scr = gtk_scrolled_window_new ();
  GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  GtkNotebookPage *nbp;
  GtkWidget *lab;
  int i;

  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
  lab = gtk_label_new (filename);
  i = gtk_notebook_append_page (win->nb, scr, lab);
  nbp = gtk_notebook_get_page (win->nb, scr);
  g_object_set (nbp, "tab-expand", TRUE, NULL);
  gtk_notebook_set_current_page (win->nb, i);
  g_signal_connect (tv, "change-file", G_CALLBACK (file_changed_cb), win->nb);
  g_signal_connect (tb, "modified-changed", G_CALLBACK (modified_changed_cb), tv);
}
```

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

```c
static void
file_changed_cb (TfeTextView *tv, gpointer user_data) {
  GtkNotebook *nb =  GTK_NOTEBOOK (user_data);
  GtkWidget *scr;
  GtkWidget *label;
  GFile *file;
  char *filename;

  file = tfe_text_view_get_file (tv);
  scr = gtk_widget_get_parent (GTK_WIDGET (tv));
  if (G_IS_FILE (file)) {
    filename = g_file_get_basename (file);
    g_object_unref (file);
  } else
    filename = get_untitled ();
  label = gtk_label_new (filename);
  g_free (filename);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (nb), scr, label);
}

static void
modified_changed_cb (GtkTextBuffer *tb, gpointer user_data) {
  TfeTextView *tv = TFE_TEXT_VIEW (user_data);
  GtkWidget *scr = gtk_widget_get_parent (GTK_WIDGET (tv));
  GtkWidget *nb =  gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_NOTEBOOK);
  GtkWidget *label;
  GFile *file;
  char *filename;
  char *text;

  file = tfe_text_view_get_file (tv);
  filename = g_file_get_basename (file);
  if (gtk_text_buffer_get_modified (tb))
    text = g_strdup_printf ("*%s", filename);
  else
    text = g_strdup (filename);
  g_object_unref (file);
  g_free (filename);
  label = gtk_label_new (text);
  g_free (text);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (nb), scr, label);
}
```

#### save\_activated

The callback function `save_activated` is an activate signal handler on "save" action.

```c
static void
save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  TfeTextView *tv = get_current_textview (win->nb);

  tfe_text_view_save (TFE_TEXT_VIEW (tv));
}
```

This function gets the current TfeTextView instance with the function `get_current_textview`.
And it just calls `tfe_text_view_save`.

```c
static TfeTextView *
get_current_textview (GtkNotebook *nb) {
  int i;
  GtkWidget *scr;
  GtkWidget *tv;

  i = gtk_notebook_get_current_page (nb);
  scr = gtk_notebook_get_nth_page (nb, i);
  tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
  return TFE_TEXT_VIEW (tv);
}
```

#### close\_activated

The callback function `close_activated` is an activate signal handler on "close" action.
It closes the current notebook page.

```c
static void
close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  TfeTextView *tv;
  GtkTextBuffer *tb;
  GtkWidget *alert;

  tv = get_current_textview (win->nb);
  tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  if (! gtk_text_buffer_get_modified (tb)) /* is saved? */
    notebook_page_close (win);
  else {
    win->is_quit = FALSE;
    alert = tfe_alert_new_with_data ("Are you sure?", "Contents aren't saved yet.\nAre you sure to close?", "Close");
    gtk_window_set_transient_for (GTK_WINDOW (alert), GTK_WINDOW (win));
    g_signal_connect (alert, "response", G_CALLBACK (alert_response_cb), win);
    gtk_window_present (GTK_WINDOW (alert));
  }
}
```

If the text in the current page has been saved, it calls `notebook_page_close` to close the page.
Otherwise, it sets `win->is_quit` to FALSE and show the alert dialog.
The "response" signal on the dialog is connected to the handler `alert_response_cb`.

```c
static void
notebook_page_close (TfeWindow *win){
  int i;

  if (gtk_notebook_get_n_pages (win->nb) == 1)
    gtk_window_destroy (GTK_WINDOW (win));
  else {
    i = gtk_notebook_get_current_page (win->nb);
    gtk_notebook_remove_page (win->nb, i);
  }
}
```

If the notebook has only one page, it destroys the window and the application quits.
Otherwise, it removes the current page.

```c
static void
alert_response_cb (TfeAlert *alert, int response_id, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  if (response_id == TFE_ALERT_RESPONSE_ACCEPT) {
    if (win->is_quit)
      gtk_window_destroy(GTK_WINDOW (win));
    else
      notebook_page_close (win);
  }
}
```

If the user clicked on the cancel button, it does nothing.
If the user clicked on the accept button, which is the same as close button, it calls `notebook_page_close`.
Note that `win->is_quit` has been set to FALSE in the `close_activated` function.

#### new\_activated

The callback function `new_activated` is an activate signal handler on "new" action.

```c
static void
new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  tfe_window_notebook_page_new (win);
}
```

It just calls `tfe_window_notebook_page_new`, which is a public method of TfeWindow.

```c
void
tfe_window_notebook_page_new (TfeWindow *win) {
  GtkWidget *tv;
  char *filename;

  tv = tfe_text_view_new ();
  filename = get_untitled ();
  notebook_page_build (win, tv, filename);
  g_free (filename);
}
```

This function creates a new TfeTextView instance, generates a string like "Untitled", and calls `notebook_page_build`.

#### saveas\_activated

The callback function `saveas_activated` is an activate signal handler on "saveas" action.

```c
static void
saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  TfeTextView *tv = get_current_textview (win->nb);

  tfe_text_view_saveas (TFE_TEXT_VIEW (tv));
}
```

This function gets the current page TfeTextView instance and calls `tfe_text_view_saveas`.

#### pref\_activated

The callback function `pref_activated` is an activate signal handler on "pref" action.

```c
static void
pref_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  GtkWidget *pref;

  pref = tfe_pref_new ();
  gtk_window_set_transient_for (GTK_WINDOW (pref), GTK_WINDOW (win));
  gtk_window_present (GTK_WINDOW (pref));
}
```

This function creates a TfePref instance, which is a dialog, and sets the transient parent window to `win`.
And it shows the dialog.

#### close\_all\_activated

The callback function `close_all_activated` is an activate signal handler on "close_all" action.

```c
static void
close_all_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  if (close_request_cb (win) == FALSE)
    gtk_window_destroy (GTK_WINDOW (win));
}
```

It first calls the function `close_request_cb`.
It is a callback function for the "close-request" signal on the top window.
It returns FALSE if all the texts have been saved.
Otherwise it returns TRUE.

Therefore, function `close_all_activated` destroys the top window if all the texts have been saved.
Otherwise it does nothing.
But, the function `close_request_cb` shows an alert dialog and if the user clicks on the accept button, the window will be destroyed.

### Window "close-request" Signal

GtkWindow has a "close-request" signal and it is emitted when the close button, which is the x-shaped button at the top right corner, is clicked on.
And the user handler is called before the default handler.
If the user handler returns TRUE, the rest of the close process is skipped.
If it returns FALSE, the rest will go on and the window will be destroyed.

```c
static gboolean
close_request_cb (TfeWindow *win) {
  TfeAlert *alert;

  if (is_saved_all (win->nb))
    return FALSE;
  else {
    win->is_quit = TRUE;
    alert = TFE_ALERT (tfe_alert_new_with_data ("Are you sure?", "Contents aren't saved yet.\nAre you sure to quit?", "Quit"));
    gtk_window_set_transient_for (GTK_WINDOW (alert), GTK_WINDOW (win));
    g_signal_connect (alert, "response", G_CALLBACK (alert_response_cb), win);
    gtk_window_present (GTK_WINDOW (alert));
    return TRUE;
  }
}
```

First, it calls `is_saved_all` and checks if the texts have been saved.
If so, it returns FALSE and the close process continues.
Otherwise, it sets `win->is_quit` to TRUE and shows an alert dialog.
When the user clicks on the accept or cancel button, the dialog disappears and "response" signal is emitted.
Then, the handler `alert_response_cb` is called.
It destroys the top window if the user clicked on the accept button since `win->is_quit` is TRUE.
Otherwise it does nothing.

```c
static gboolean
is_saved_all (GtkNotebook *nb) {
  int i, n;
  GtkWidget *scr;
  GtkWidget *tv;
  GtkTextBuffer *tb;

  n = gtk_notebook_get_n_pages (nb);
  for (i = 0; i < n; ++i) {
    scr = gtk_notebook_get_nth_page (nb, i);
    tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
    tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
    if (gtk_text_buffer_get_modified (tb))
      return FALSE;
  }
  return TRUE;
}
```

### Public Functions

There are three public functions.

- `void tfe_window_notebook_page_new (TfeWindow *win)`
- `void tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files)`
- `GtkWidget *tfe_window_new (GtkApplication *app)`

The first function is called when the application emits the "activate" signal.
The second is for "open" signal.
It is given three arguments and they are owned by the caller.

```c
void
tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files) {
  int i;
  GtkWidget *tv;
  char *filename;

  for (i = 0; i < n_files; i++)
    if ((tv = tfe_text_view_new_with_file (*(files+i))) != NULL) {
      filename = g_file_get_basename (*(files+i));
      notebook_page_build (win, tv, filename);
      g_free (filename);
    }
  if (gtk_notebook_get_n_pages (win->nb) == 0)
    tfe_window_notebook_page_new (win);
}
```

This function has a loop for the array `files`.
It creates TfeTextView instance with the text from each file.
And build a page with it.

If an error happens and no page is created, it creates a new empty page.

### Full Codes of tfewindow.c

The following is the full source codes of `tfewindow.c`.

```c
#include <gtk/gtk.h>
#include "tfewindow.h"
#include "tfepref.h"
#include "tfealert.h"
#include "../tfetextview/tfetextview.h"

struct _TfeWindow {
  GtkApplicationWindow parent;
  GtkMenuButton *btnm;
  GtkNotebook *nb;
  gboolean is_quit;
};

G_DEFINE_FINAL_TYPE (TfeWindow, tfe_window, GTK_TYPE_APPLICATION_WINDOW);

/* Low level functions */

/* Create a new untitled string */
/* The returned string should be freed with g_free() when no longer needed. */
static char *
get_untitled () {
  static int c = -1;
  if (++c == 0) 
    return g_strdup_printf("Untitled");
  else
    return g_strdup_printf ("Untitled%u", c);
}

/* The returned object is owned by the scrolled window. */
/* The caller won't get the ownership and mustn't release it. */
static TfeTextView *
get_current_textview (GtkNotebook *nb) {
  int i;
  GtkWidget *scr;
  GtkWidget *tv;

  i = gtk_notebook_get_current_page (nb);
  scr = gtk_notebook_get_nth_page (nb, i);
  tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
  return TFE_TEXT_VIEW (tv);
}

/* This call back is called when a TfeTextView instance emits a "change-file" signal. */
static void
file_changed_cb (TfeTextView *tv, gpointer user_data) {
  GtkNotebook *nb =  GTK_NOTEBOOK (user_data);
  GtkWidget *scr;
  GtkWidget *label;
  GFile *file;
  char *filename;

  file = tfe_text_view_get_file (tv);
  scr = gtk_widget_get_parent (GTK_WIDGET (tv));
  if (G_IS_FILE (file)) {
    filename = g_file_get_basename (file);
    g_object_unref (file);
  } else
    filename = get_untitled ();
  label = gtk_label_new (filename);
  g_free (filename);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (nb), scr, label);
}

static void
modified_changed_cb (GtkTextBuffer *tb, gpointer user_data) {
  TfeTextView *tv = TFE_TEXT_VIEW (user_data);
  GtkWidget *scr = gtk_widget_get_parent (GTK_WIDGET (tv));
  GtkWidget *nb =  gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_NOTEBOOK);
  GtkWidget *label;
  GFile *file;
  char *filename;
  char *text;

  file = tfe_text_view_get_file (tv);
  filename = g_file_get_basename (file);
  if (gtk_text_buffer_get_modified (tb))
    text = g_strdup_printf ("*%s", filename);
  else
    text = g_strdup (filename);
  g_object_unref (file);
  g_free (filename);
  label = gtk_label_new (text);
  g_free (text);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (nb), scr, label);
}

static gboolean
is_saved_all (GtkNotebook *nb) {
  int i, n;
  GtkWidget *scr;
  GtkWidget *tv;
  GtkTextBuffer *tb;

  n = gtk_notebook_get_n_pages (nb);
  for (i = 0; i < n; ++i) {
    scr = gtk_notebook_get_nth_page (nb, i);
    tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
    tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
    if (gtk_text_buffer_get_modified (tb))
      return FALSE;
  }
  return TRUE;
}

static void
notebook_page_close (TfeWindow *win){
  int i;

  if (gtk_notebook_get_n_pages (win->nb) == 1)
    gtk_window_destroy (GTK_WINDOW (win));
  else {
    i = gtk_notebook_get_current_page (win->nb);
    gtk_notebook_remove_page (win->nb, i);
  }
}

static void
notebook_page_build (TfeWindow *win, GtkWidget *tv, char *filename) {
  // The arguments win, tv and filename are owned by the caller.
  // If tv has a floating reference, it is consumed by the function.
  GtkWidget *scr = gtk_scrolled_window_new ();
  GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  GtkNotebookPage *nbp;
  GtkWidget *lab;
  int i;

  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
  lab = gtk_label_new (filename);
  i = gtk_notebook_append_page (win->nb, scr, lab);
  nbp = gtk_notebook_get_page (win->nb, scr);
  g_object_set (nbp, "tab-expand", TRUE, NULL);
  gtk_notebook_set_current_page (win->nb, i);
  g_signal_connect (tv, "change-file", G_CALLBACK (file_changed_cb), win->nb);
  g_signal_connect (tb, "modified-changed", G_CALLBACK (modified_changed_cb), tv);
}

static void
open_response_cb (TfeTextView *tv, int response, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  GFile *file;
  char *filename;

  if (response != TFE_OPEN_RESPONSE_SUCCESS) {
    g_object_ref_sink (tv);
    g_object_unref (tv);
  }else if (! G_IS_FILE (file = tfe_text_view_get_file (tv))) {
    g_object_ref_sink (tv);
    g_object_unref (tv);
  }else {
    filename = g_file_get_basename (file);
    g_object_unref (file);
    notebook_page_build (win, GTK_WIDGET (tv), filename);
    g_free (filename);
  }
}

/* alert response signal handler */
static void
alert_response_cb (TfeAlert *alert, int response_id, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  if (response_id == TFE_ALERT_RESPONSE_ACCEPT) {
    if (win->is_quit)
      gtk_window_destroy(GTK_WINDOW (win));
    else
      notebook_page_close (win);
  }
}

/* ----- Close request on the top window ----- */
/* ----- The signal is emitted when the close button is clicked. ----- */
static gboolean
close_request_cb (TfeWindow *win) {
  TfeAlert *alert;

  if (is_saved_all (win->nb))
    return FALSE;
  else {
    win->is_quit = TRUE;
    alert = TFE_ALERT (tfe_alert_new_with_data ("Are you sure?", "Contents aren't saved yet.\nAre you sure to quit?", "Quit"));
    gtk_window_set_transient_for (GTK_WINDOW (alert), GTK_WINDOW (win));
    g_signal_connect (alert, "response", G_CALLBACK (alert_response_cb), win);
    gtk_window_present (GTK_WINDOW (alert));
    return TRUE;
  }
}

/* ----- action activated handlers ----- */
static void
open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  GtkWidget *tv = tfe_text_view_new ();

  g_signal_connect (tv, "open-response", G_CALLBACK (open_response_cb), win);
  tfe_text_view_open (TFE_TEXT_VIEW (tv), GTK_WINDOW (win));
}

static void
save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  TfeTextView *tv = get_current_textview (win->nb);

  tfe_text_view_save (TFE_TEXT_VIEW (tv));
}

static void
close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  TfeTextView *tv;
  GtkTextBuffer *tb;
  GtkWidget *alert;

  tv = get_current_textview (win->nb);
  tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  if (! gtk_text_buffer_get_modified (tb)) /* is saved? */
    notebook_page_close (win);
  else {
    win->is_quit = FALSE;
    alert = tfe_alert_new_with_data ("Are you sure?", "Contents aren't saved yet.\nAre you sure to close?", "Close");
    gtk_window_set_transient_for (GTK_WINDOW (alert), GTK_WINDOW (win));
    g_signal_connect (alert, "response", G_CALLBACK (alert_response_cb), win);
    gtk_window_present (GTK_WINDOW (alert));
  }
}

static void
new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  tfe_window_notebook_page_new (win);
}

static void
saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  TfeTextView *tv = get_current_textview (win->nb);

  tfe_text_view_saveas (TFE_TEXT_VIEW (tv));
}

static void
pref_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  GtkWidget *pref;

  pref = tfe_pref_new ();
  gtk_window_set_transient_for (GTK_WINDOW (pref), GTK_WINDOW (win));
  gtk_window_present (GTK_WINDOW (pref));
}

static void
close_all_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);

  if (close_request_cb (win) == FALSE)
    gtk_window_destroy (GTK_WINDOW (win));
}

/* --- public functions --- */

void
tfe_window_notebook_page_new (TfeWindow *win) {
  GtkWidget *tv;
  char *filename;

  tv = tfe_text_view_new ();
  filename = get_untitled ();
  notebook_page_build (win, tv, filename);
  g_free (filename);
}

void
tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files) {
  int i;
  GtkWidget *tv;
  char *filename;

  for (i = 0; i < n_files; i++)
    if ((tv = tfe_text_view_new_with_file (*(files+i))) != NULL) {
      filename = g_file_get_basename (*(files+i));
      notebook_page_build (win, tv, filename);
      g_free (filename);
    }
  if (gtk_notebook_get_n_pages (win->nb) == 0)
    tfe_window_notebook_page_new (win);
}

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

  g_signal_connect (win, "close-request", G_CALLBACK (close_request_cb), NULL);
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

Up: [README.md](../README.md),  Prev: [Section 21](sec21.md), Next: [Section 23](sec23.md)
