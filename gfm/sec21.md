Up: [README.md](../README.md),  Prev: [Section 20](sec20.md), Next: [Section 22](sec22.md)

# Creating TfeWindow

Restructuring the window not just as a separate file from the application, but as a subclass of GtkApplicationWindow, has the following advantages:

- Better encapsulation. It makes it easier to distinguish between external interfaces and internal processing functions.
- Ability to have instance variables. TfeWindow often refers to its internal GtkNotebook. Keeping its pointer in an instance variable is quite useful.
- By introducing composite widgets along with subclassing, you can handle all widgets within the window together.

## Defining the Subclass

The method for defining a subclass of GtkApplicationWindow is the same as for TfeTextView or TfeApplication.

### Writing the Header File

```c
#pragma once

#include <gtk/gtk.h>

#define TFE_TYPE_WINDOW tfe_window_get_type ()
G_DECLARE_FINAL_TYPE (TfeWindow, tfe_window, TFE, WINDOW, GtkApplicationWindow)

void
tfe_window_append_page (TfeWindow *win, GFile *file);

GtkWidget *
tfe_window_new (GtkApplication *app);
```

- 5-6: The macro definition of `TFE_TYPE_WINDOW` and how to write the `G_DECLARE_FINAL_TYPE` macro are the same as before.
- 8-12: This class only needs to be accessed from the application (TfeApplication) through:
  - `tfe_window_new()`: Used when creating a window.
  - `tfe_window_append_page()`: Used within the "open" handler to add the specified file to a page.

### Writing the C File

First, in the C file, you need to write the object structure and the `G_DEFINE_FINAL_TYPE` macro.


````
#include <gtk/gtk.h>
#include "tfewindow.h"
#include "tfetextview.h"

struct _TfeWindow {
  GtkApplicationWindow parent;
  GtkMenuButton *btnm; /* menu button */
  GtkNotebook *nb; /* notebook */
};

G_DEFINE_FINAL_TYPE (TfeWindow, tfe_window, GTK_TYPE_APPLICATION_WINDOW)

````

-  5-9: The first member of the object structure is the parent object's structure.
If a pointer to a TfeWindow object `win` is passed as an argument to functions in the class, you can access the menu button with `win->btnm` and the notebook with `win->nb`.
- 11: Uses the `G_DEFINE_FINAL_TYPE` macro.

## Composite Widgets

Until now, we have built windows by combining widgets like buttons and notebooks, treating them as a single object.
There was no way to represent them as a cohesive unit.
A composite widget provides a way to represent this group; it is a single widget made by combining multiple widgets.

With a composite widget, you create a template for a "special window containing a button and a notebook".
This template is similar to a class.
When you instantiate a composite widget, all its internal widgets are created and assembled based on this template.

Note that existing widgets like GtkWindow cannot be converted into composite widgets directly.
Composite widgets must be implemented programmatically as subclasses.

The template is created from UI data by GtkBuilder, which is similar to how we've used the builder so far, but their roles are clearly different:

- **Creating objects with a builder:** Creates instances of widgets from an XML file and combines them. The output is a set of instances.
- **Creating a template with a builder:** Generates a template for the widget combination from an XML file. The template is not a group of instances.

When a composite widget is instantiated, the individual widgets it contains are created within its instance initialization function (for example, `tfe_window_init`).
This is handled by the `gtk_widget_init_template()` function.

The main advantage of this approach is encapsulation—keeping multiple widgets contained within a single composite widget.
For example, if the application needs to open multiple editor windows, you would normally have to create and manage individual widgets for each window.
With a composite widget, creating the main widget automatically creates its child widgets.
By letting the composite widget manage its internal components, external code doesn't need to do anything extra.

This clarifies the division of responsibilities in the code, leading to cleaner programs with fewer bugs.

In the following subsections, let's see how to actually create a composite window using TfeWindow as an example.

### Structure of the UI File

The UI file `tfewindow.ui` for the composite widget is almost the same as before.
The only difference is using the `<template>` tag instead of the usual `<object>` tag for the top window.


````
<template class="TfeWindow" parent="GtkApplicationWindow">
  ... ... ...
</template>

````

- The `class` attribute specifies TfeWindow, the class name of the composite widget.
- The `parent` attribute specifies GtkApplicationWindow, the parent class of TfeWindow.

### Binding the Template in the C File

Next, we bind the template and child widgets defined in the UI file to the TfeWindow class in the C file.
This is done in the class initialization function (`tfe_window_class_init`).


````
  ... ... ...
struct _TfeWindow {
  GtkApplicationWindow parent;
  GtkMenuButton *btnm; /* menu button */
  GtkNotebook *nb; /* notebook */
};

  ... ... ...

static void
tfe_window_class_init (TfeWindowClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = tfe_window_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe7/tfewindow.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, btnm); /* bind "btnm" in _TfeWindow to "btnm" in the UI resource */
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, nb);  /* bind "nb" in _TfeWindow to "nb" in the UI resource */
}

````

- `gtk_widget_class_set_template_from_resource()` loads the template from the UI data into the class.
- `gtk_widget_class_bind_template_child()` maps the pointer to the widget with `id="btnm"` in the XML to the `win->btnm` variable.
Because of this binding, when a composite widget instance is created, `win->btnm` is automatically initialized to point to that specific widget.
The notebook (`nb`) is handled in the same way.

This function is called only once when the TfeWindow class is created, establishing the connection between the class and the template.

### Creating an Instance

When an instance is created, the initialization function `tfe_window_init()` is called.


````
static void
tfe_window_init (TfeWindow *win) {
  GtkBuilder *builder;

  gtk_widget_init_template (GTK_WIDGET (win));

  ... ... ...
}

````

`gtk_widget_init_template()` instantiates the widgets inside the window based on the template, setting up the proper parent-child relationships.

### Destroying an Instance

The internal widgets of the window must be destroyed just before the composite widget itself is destroyed.
This is handled in the `tfe_window_dispose()` function.


````
static void
tfe_window_dispose (GObject *gobject) {
  gtk_widget_dispose_template (GTK_WIDGET (gobject), TFE_TYPE_WINDOW);
  G_OBJECT_CLASS (tfe_window_parent_class)->dispose (gobject);
}

````

The `gtk_widget_dispose_template()` function destroys the widgets created from the template.
It also sets the instance variables bound by `gtk_widget_class_bind_template_child()` to NULL.

To summarize the process of creating a composite widget:

- Use the `<template>` tag as the root tag in your UI file.
- Load the template into the class using `gtk_widget_class_set_template_from_resource()` during class initialization.
- To link objects defined by IDs in the UI file to instance variables in the object structure:
  - Use the same name for the UI ID and the instance variable.
  - Bind them using `gtk_widget_class_bind_template_child()` during class initialization.
- Call `gtk_widget_init_template()` during object instantiation to create the child widgets.
- Call `gtk_widget_dispose_template()` during object destruction to clean up the child widgets.

## The C File of TfeWindow

This subsection covers the changes made to TfeWindow that are not directly related to composite widgets.

### Setting the Menu to the Menu Button

The menu is set inside `tfe_window_init()`.


````
builder = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe7/menus.ui");
gtk_menu_button_set_menu_model (GTK_MENU_BUTTON (win->btnm), G_MENU_MODEL (gtk_builder_get_object (builder, "menu")));
g_object_unref (builder);

````

Here, we did not use GtkApplication's Automatic resources (which involves `gtk_application_get_menu_by_id()`).
This was done to avoid adding a dependency on GtkApplication.
Setting up the menu entirely within TfeWindow aligns better with the concept of encapsulation.

### Defining Actions

Actions are also defined inside `tfe_window_init()`.


````
const GActionEntry win_entries[] = {
  { "open", open_activated, NULL, NULL, NULL },
  { "save", save_activated, NULL, NULL, NULL },
  { "close", close_activated, NULL, NULL, NULL },
  { "new", new_activated, NULL, NULL, NULL },
  { "saveas", saveas_activated, NULL, NULL, NULL },
};
g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);

````

This is the same code that was previously in the `get_main_window()` function in version `tfe6`.
Since TfeWindow is now a standalone class, the code was simply moved to TfeWindow's initialization.

### Constructor

```c
GtkWidget *
tfe_window_new (GtkApplication *app) {
  GtkWidget *win;

  win = GTK_WIDGET (g_object_new (TFE_TYPE_WINDOW, "application", app, NULL));
  tfe_window_append_page (TFE_WINDOW (win), NULL);
  return win;
}
```

In the constructor, the instance is created using `g_object_new()`, just like with TfeTextView.
The "application" property is set to the application instance here. 
We also added code to open an empty page by default.
This mimics the behavior of many text editors that start with a blank document.

### Instance Methods

The only instance method is `tfe_window_append_page()`.
This is a slightly modified version of the old `notebook_page_new_with_file()` function, keeping the same core logic.

### Saveas Action Handler

```c
static void
saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  TfeWindow *win = TFE_WINDOW (user_data);
  int i;
  GtkWidget *scr;
  GtkWidget *tv;
  GtkFileDialog *dialog;
  GtkAlertDialog *alert_dialog;

  i = gtk_notebook_get_current_page (win->nb);

  if (i == -1) {
    alert_dialog = gtk_alert_dialog_new ("No page to save.");
    gtk_alert_dialog_show (alert_dialog, GTK_WINDOW (win));
    g_object_unref (alert_dialog);
    return;
  }

  scr = gtk_notebook_get_nth_page (win->nb, i);
  tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));

  dialog = gtk_file_dialog_new ();
  gtk_file_dialog_save (dialog, GTK_WINDOW (win), NULL, save_dialog_cb, tv);
  g_object_unref (dialog);
}
```

This handler is very similar to the Save action handler (`save_activated()`).
The main difference is that it always opens a file dialog before saving.
Since the code overlaps significantly with the Save handler, we'll skip a detailed explanation here.

## The C File of TfeApplication

With TfeWindow covered, let's go over the remaining changes in TfeApplication.

### Function to Get the Window

Both the "activate" and "open" handlers need to get a reference to the main window first.
This logic was extracted into a separate `get_main_window()` function.
Since window initialization is now handled by TfeWindow, this function has become much simpler.

```c
static GtkWidget *
get_main_window (GApplication *application) {
  GList *windows = gtk_application_get_windows (GTK_APPLICATION (application));
  GtkWidget *win;

  if (windows)
    win = GTK_WIDGET (windows->data);
  else
    win = tfe_window_new(GTK_APPLICATION (application));
  return win;
}
```

If the application already has a window (i.e., the list of top windows is not empty), it retrieves it; otherwise, it creates a new one.
Notice that we don't need to explicitly link the window to the application here using `gtk_window_set_application()`.
This is because the application is already passed to the "application" property when `tfe_window_new()` is called, which establishes the connection.

```c
GtkWidget *
tfe_window_new (GtkApplication *app) {
  GtkWidget *win;

  win = GTK_WIDGET (g_object_new (TFE_TYPE_WINDOW, "application", app, NULL));
  tfe_window_append_page (TFE_WINDOW (win), NULL);
  return win;
}
```

### Activate and Open Functions

By moving some logic to `get_main_window()`, these handlers are now shorter as well.

```c
static void
app_activate (GApplication *application) {
  GtkWidget *win;

  win = get_main_window (application);
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
  GtkWidget *win;
  int i;

  win = get_main_window (application);
  for (i = 0; i < n_files; i++) {
    tfe_window_append_page (TFE_WINDOW (win), files[i]);
  }
  gtk_window_present (GTK_WINDOW (win));
}
```

You may notice that chaining up is omitted at the beginning of both `app_activate ()` and `app_open ()`.

To understand why, let's look at what GApplication's default implementation of "activate" handler actually does.
The source code of `g_application_real_activate ()` (GApplication's "activate" handler) in GLib shows that it only checks whether any handler is connected.

- Handlers are implemented using `g_signal_connect`.
- Subclasses implement handlers by overriding methods.

And if not, it emits a warning.

In other words, GApplication itself provides no meaningful implementation for "activate" or "open" handler.
It simply requires that the application developer implement them, either externally with `g_signal_connect ()` or internally by overriding the virtual function in a subclass.
This design is analogous to a pure virtual function in C++: the base class declares the interface but intentionally leaves the implementation to the derived class.

Since the default implementation is effectively empty, chaining up would execute no meaningful code.
For this reason, we chose to omit chaining up in these specific functions.

However, chaining up is not always optional.
The necessity and timing depend on the parent class's implementation.
For instance, chaining up is required for "startup" handler and must be done at the beginning of the override.
It is also required for "dispose" handler, but must be done at the end.
When overriding virtual functions, always check the documentation or source code to determine whether chaining up is necessary.

## Compilation and Execution

All the files are located in the [src/tfe7](../src/tfe7/) directory.
Navigate to this directory in your terminal and run the following commands to compile and start the application:

```
$ meson setup _build
$ ninja -C _build
$ _build/tfe
```

As noted earlier, menus does not work correctly in WSL (Windows Subsystem for Linux).
They will work as expected on a native Linux environment.

Up: [README.md](../README.md),  Prev: [Section 20](sec20.md), Next: [Section 22](sec22.md)
