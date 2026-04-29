Up: [README.md](../README.md),  Prev: [Section 27](sec27.md), Next: [Section 29](sec29.md)

# Drag and Drop

## What is Drag and Drop?

Drag and drop is also written as "Drag-and-Drop", or "DND" in short.
DND is like "copy and paste" or "cut and paste".
If a user drags a UI element, which is a widget, selected part or other elements, data is transferred from the source to the destination.

You probably have experience moving a file with DND.

![DND on the GUI file manager](/src/images/dnd.png)

When the DND starts, the file `sample_file.txt` is provided to the system.
When it ends,  the system provides it to the `sample_folder` in the file manager.
Therefore, it is like "cut and paste".
The actual behavior may be different from the explanation here, but the concept is similar.

## Example for DND

This tutorial provides a simple example in the `src/dnd` directory.
It has three labels for the source and one label for the destination.
The source labels have "red", "green" or "blue" labels.
If a user drags the label to the destination label, the font color will be changed.

![DND example](/src/images/dnd_canvas.png)

## UI File

The widgets are defined in the XML file `dnd.ui`.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <object class="GtkApplicationWindow" id="win">
    <property name="default-width">800</property>
    <property name="default-height">600</property>
    <property name="resizable">FALSE</property>
    <property name="title">Drag and Drop</property>
    <child>
      <object class="GtkBox">
        <property name="hexpand">TRUE</property>
        <property name="vexpand">TRUE</property>
        <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
        <property name="spacing">5</property>
        <child>
          <object class="GtkBox">
            <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
            <property name="homogeneous">TRUE</property>
            <child>
              <object class="GtkLabel" id="red">
                <property name="label">RED</property>
                <property name="justify">GTK_JUSTIFY_CENTER</property>
                <property name="name">red</property>
              </object>
            </child>
            <child>
              <object class="GtkLabel" id="green">
                <property name="label">GREEN</property>
                <property name="justify">GTK_JUSTIFY_CENTER</property>
                <property name="name">green</property>
              </object>
            </child>
            <child>
              <object class="GtkLabel" id="blue">
                <property name="label">BLUE</property>
                <property name="justify">GTK_JUSTIFY_CENTER</property>
                <property name="name">blue</property>
              </object>
            </child>
          </object>
        </child>
        <child>
          <object class="GtkLabel" id="canvas">
            <property name="label">CANVAS</property>
            <property name="justify">GTK_JUSTIFY_CENTER</property>
            <property name="name">canvas</property>
            <property name="hexpand">TRUE</property>
            <property name="vexpand">TRUE</property>
          </object>
        </child>
      </object>
    </child>
  </object>
</interface>

```

It is converted to a resource file by `glib-compile-resources`.
The compiler uses an XML file `dnd.gresource.xml`.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<gresources>
  <gresource prefix="/com/github/ToshioCP/dnd">
    <file>dnd.ui</file>
  </gresource>
</gresources>
```

## C File dnd.c

The C file `dnd.c` isn't a big file.
The number of the lines is less than a hundred.
A GtkApplication object is created in the function `main`.

```c
int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
```

The application ID is defined as:

```C
#define APPLICATION_ID "com.github.ToshioCP.dnd"
```

### Startup Signal Handler

Most of the work is done in the "startup" signal handler.

Two objects GtkDragSource and GtkDropTarget are used for DND implementation.

- Drag source: A drag source (GtkDragSource instance) is an event controller.
It initiates a DND operation when the user clicks and drags the widget.
If data, in the form of GdkContentProvider, is set in advance, it gives the data to the system at the beginning of the drag.
- Drop target: A drop target (GtkDropTarget) is also an event controller.
You can get the data in the GtkDropTarget::drop signal handler.

The example below uses these objects in a very simple way.
You can use a number of features that the two objects have.
See the following links for more information.

- [Drag-and-Drop in GTK](https://docs.gtk.org/gtk4/drag-and-drop.html)
- [GtkDragSource](https://docs.gtk.org/gtk4/class.DragSource.html)
- [GtkDropTarget](https://docs.gtk.org/gtk4/class.DropTarget.html)

```c
static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkBuilder *build;
  GtkWindow *win;
  GtkLabel *src_labels[3];
  int i;
  GtkLabel *canvas;
  GtkDragSource *src;
  GdkContentProvider* content;
  GtkDropTarget *tgt;
  GdkDisplay *display;
  char *s;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/dnd/dnd.ui");
  win = GTK_WINDOW (gtk_builder_get_object (build, "win"));
  src_labels[0] = GTK_LABEL (gtk_builder_get_object (build, "red"));
  src_labels[1] = GTK_LABEL (gtk_builder_get_object (build, "green"));
  src_labels[2] = GTK_LABEL (gtk_builder_get_object (build, "blue"));
  canvas = GTK_LABEL (gtk_builder_get_object (build, "canvas"));
  gtk_window_set_application (win, app);
  g_object_unref (build);

  for (i=0; i<3; ++i) {
    src = gtk_drag_source_new ();
    content = gdk_content_provider_new_typed (G_TYPE_STRING, gtk_widget_get_name (GTK_WIDGET (src_labels[i])));
    gtk_drag_source_set_content (src, content);
    g_object_unref (content);
    gtk_widget_add_controller (GTK_WIDGET (src_labels[i]), GTK_EVENT_CONTROLLER (src)); // The ownership of src is taken by the instance.
  }

  tgt = gtk_drop_target_new (G_TYPE_STRING, GDK_ACTION_COPY);
  g_signal_connect (tgt, "drop", G_CALLBACK (drop_cb), NULL);
  gtk_widget_add_controller (GTK_WIDGET (canvas), GTK_EVENT_CONTROLLER (tgt)); // The ownership of tgt is taken by the instance.

  provider = gtk_css_provider_new ();
  s = g_strdup_printf (format, "black");
  gtk_css_provider_load_from_data (provider, s, -1);
  g_free (s);
  display = gdk_display_get_default ();
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_object_unref (provider); // The provider is still alive because the display owns it.
}
```

- 15-22: Builds the widgets.
The array `src_labels[]` points to the source labels red, green and blue in the UI file.
The variable `canvas` points to the destination label.
- 24-30: Sets the DND source widgets.
The for-loop iterates through the array `src_labels[]` each of which points to the source widget,  red, green or blue label.
- 25: Creates a new GtkDragSource instance.
- 26: Creates a new GdkContentProvider instance with the string "red", "green" or "blue".
They are the name of the widgets.
These strings are the data to transfer through the DND operation.
- 27: Sets the content of the drag source to the GdkContentProvider instance above.
- 28: Content is useless so it is destroyed.
- 29: Add the event controller, which is actually the drag source, to the widget.
If a DND operation starts on the widget, the corresponding drag source works and the data is given to the system.
- 32-34: Sets the DND drop target.
- 32: Creates a new GtkDropTarget instance.
The first parameter is the GType of the data.
The second parameter is a GdkDragAction enumeration constant.
The arguments here are string type and the constant for copy.
- 33: Connects the "drop" signal and the handler `drop_cb`.
- 34: Add the event controller, which is actually the drop target, to the widget.
- 36-43: Sets CSS.
- 37: A variable `format` is static and defined at the top of the program.
Static variables are shown below.

```C
static GtkCssProvider *provider = NULL;
static const char *format = "label {padding: 20px;} label#red {background: red;} "
  "label#green {background: green;} label#blue {background: blue;} "
  "label#canvas {color: %s; font-weight: bold; font-size: 72pt;}";
```

### Activate Signal Handler

```c
static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWindow *win;

  win = gtk_application_get_active_window (app);
  gtk_window_present (win);
}
```

This handler just shows the window.

### Drop Signal Handler

```c
static gboolean
drop_cb (GtkDropTarget* self, const GValue* value, gdouble x, gdouble y, gpointer user_data) {
  char *s;

  s = g_strdup_printf (format, g_value_get_string (value));
  gtk_css_provider_load_from_data (provider, s, -1);
  g_free (s);
  return TRUE;
}
```

The "drop" signal handler has five parameters.

- GtkDropTarget instance on which the signal has been emitted.
- GValue that holds the data from the source.
- The arguments `x` and `y` are the coordinate of the mouse when released.
- User data was set when the signal and the handler were connected.

The string from the GValue is "red", "green" or "blue".
It replaces "%s" in the variable `format`.
That means the font color of the label `canvas` will be updated to the color.

## Meson.build

The file `meson.build` controls the building process.

```
project('dnd', 'c')

gtkdep = dependency('gtk4')

gnome = import('gnome')
resources = gnome.compile_resources('resources','dnd.gresource.xml')

executable(meson.project_name(), 'dnd.c', resources, dependencies: gtkdep, export_dynamic: true, install: false)
```

You can build it from the command line.

```
$ cd src/dnd
$ meson setup _build
$ ninja -C _build
$ _build/dnd
```

The source files are under the directory `src/dnd` of the [repository](https://github.com/ToshioCP/Gtk4-tutorial).
Download it and see the directory.

Up: [README.md](../README.md),  Prev: [Section 27](sec27.md), Next: [Section 29](sec29.md)
