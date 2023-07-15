# TfeWindow

## The Tfe window and XML files

The following is the window of Tfe.

![tfe6](../image/tfe6.png){width=9.06cm height=6.615cm}

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

@@@include
tfe6/tfewindow.ui
@@@

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

@@@include
tfe6/menu.ui
@@@

There are four menu items and they are connected to actions.

## The header file

The following is the codes of `tfewindow.h`.

@@@include
tfe6/tfewindow.h
@@@

- 5-6: `TFE_TYPE_WINDOW` definition and the `G_DECLARE_FINAL_TYPE` macro.
- 8-15: Public functions. The first two functions creates a notebook page and the last function creates a window.

## C file

### A composite widget

The following codes are extracted from `tfewindow.c`.

@@@if gfm
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
@@@else
```{.C}
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
@@@end

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

@@@if gfm
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
@@@else
```{.C}
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
@@@end

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

@@@include
tfe6/tfewindow.c open_activated
@@@

It connects the "open-response" signal on the newly created TfeTextView instance and just calls `tfe_text_view_open`.
It leaves the rest of the task to the signal handler `open_response_cb`.

@@@include
tfe6/tfewindow.c open_response_cb
@@@

If the TfeTextView instance failed to read a file, it destroys the instance with `g_object_ref_sink` and `g_object_unref`.
Since newly created widgets are floating, you need to convert the floating reference to the normal reference before you release it.
The conversion is done with `g_object_ref_sink`.

If the instance successfully read the file, it calls `notebook_page_build` to build a notebook page and add it to the GtkNotebook object.

@@@include
tfe6/tfewindow.c notebook_page_build
@@@

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

@@@include
tfe6/tfewindow.c file_changed_cb modified_changed_cb
@@@

#### save\_activated

The callback function `save_activated` is an activate signal handler on "save" action.

@@@include
tfe6/tfewindow.c save_activated
@@@

This function gets the current TfeTextView instance with the function `get_current_textview`.
And it just calls `tfe_text_view_save`.

@@@include
tfe6/tfewindow.c get_current_textview
@@@

#### close\_activated

The callback function `close_activated` is an activate signal handler on "close" action.
It closes the current notebook page.

@@@include
tfe6/tfewindow.c close_activated
@@@

If the text in the current page has been saved, it calls `notebook_page_close` to close the page.
Otherwise, it sets `win->is_quit` to FALSE and show the alert dialog.
The "response" signal on the dialog is connected to the handler `alert_response_cb`.

@@@include
tfe6/tfewindow.c notebook_page_close
@@@

If the notebook has only one page, it destroys the window and the application quits.
Otherwise, it removes the current page.

@@@include
tfe6/tfewindow.c alert_response_cb
@@@

If the user clicked on the cacel button, it does nothing.
If the user clicked on the accept button, which is the same as close button, it calls `notebook_page_close`.
Note that `win->is_quit` has been set to FALSE in the `close_activated` function.

#### new\_activated

The callback function `new_activated` is an activate signal handler on "new" action.

@@@include
tfe6/tfewindow.c new_activated
@@@

It just calls `tfe_window_notebook_page_new`, which is a public method of TfeWindow.

@@@include
tfe6/tfewindow.c tfe_window_notebook_page_new
@@@

This function creates a new TfeTextView instance, "Untitled" family string and calls `notebook_page_build`.

#### saveas\_activated

The callback function `saveas_activated` is an activate signal handler on "saveas" action.

@@@include
tfe6/tfewindow.c saveas_activated
@@@

This function gets the current page TfeTextView instance and calls `tfe_text_view_saveas`.

#### pref\_activated

The callback function `pref_activated` is an activate signal handler on "pref" action.

@@@include
tfe6/tfewindow.c pref_activated
@@@

This function creates a TfePref instance, which is a dialog, and sets the transient parent window to `win`.
And it shows the dialog.

#### close\_all\_activated

The callback function `close_all_activated` is an activate signal handler on "close_all" action.

@@@include
tfe6/tfewindow.c close_all_activated
@@@

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

@@@include
tfe6/tfewindow.c close_request_cb
@@@

First, it calls `is_saved_all` and checks if the texts have been saved.
If so, it returns FALSE and the close process continues.
Otherwise, it sets `win->is_quit` to TRUE and shows an alert dialog.
When the user clicks on the accept or cancel button, the dialog disappears and "response" signal is emitted.
Then, the handler `alert_response_cb` is called.
It destroys the top window if the user clicked on the accept button since `win->is_quit` is TRUE.
Otherwise it does nothing.

@@@include
tfe6/tfewindow.c is_saved_all
@@@

### Public functions

There are three public functions.

- `void tfe_window_notebook_page_new (TfeWindow *win)`
- `void tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files)`
- `GtkWidget *tfe_window_new (GtkApplication *app)`

The first function is called when the application emits the "activate" signal.
The second is for "open" signal.
It is given three arguments and they are owned by the caller.

@@@include
tfe6/tfewindow.c tfe_window_notebook_page_new_with_files
@@@

This function has a loop for the array `files`.
It creates TfeTextView instance with the text from each file.
And build a page with it.

If an error happens and no page is created, it creates a new empty page.

### Full codes of tfewindow.c

The following is the full source codes of `tfewindow.c`.

@@@include
tfe6/tfewindow.c
@@@
