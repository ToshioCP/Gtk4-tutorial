Up: [README.md](../README.md),  Prev: [Section 27](sec27.md), Next: [Section 29](sec29.md)

# Drag and drop

## What's drag and drop?

Drag and drop is also written as "Drag-and-Drop", or "DND" in short.
DND is like "copy and paste" or "cut and paste".
If a user drags a UI element, which is a widget, selected part or something, data is transferred from the source to the destination.

You probably have experience that you moved a file with DND.

![DND on the GUI file manager](../image/dnd.png)

When the DND starts, the file `sample_file.txt` is given to the system.
When the DND ends,  the system gives `sample_file.txt` to the directory `sample_folder` in the file manager.
Therefore, it is like "cut and paste".
The actual behavior may be different from the explanation here, but the concept is similar.

## Example for DND

This tutorial provides a simple example in the `src/dnd` directory.
It has three labels for the source and one label for the destination.
The source labels have "red", "green" or "blue" labels.
If a user drags the label to the destination label, the font color will be changed.

![DND example](../image/dnd_canvas.png)

## UI file

The widgets are defined in the XML file `dnd.ui`.

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <interface>
 3   <object class="GtkApplicationWindow" id="win">
 4     <property name="default-width">800</property>
 5     <property name="default-height">600</property>
 6     <property name="resizable">FALSE</property>
 7     <property name="title">Drag and Drop</property>
 8     <child>
 9       <object class="GtkBox">
10         <property name="hexpand">TRUE</property>
11         <property name="vexpand">TRUE</property>
12         <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
13         <property name="spacing">5</property>
14         <child>
15           <object class="GtkBox">
16             <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
17             <property name="homogeneous">TRUE</property>
18             <child>
19               <object class="GtkLabel" id="red">
20                 <property name="label">RED</property>
21                 <property name="justify">GTK_JUSTIFY_CENTER</property>
22                 <property name="name">red</property>
23               </object>
24             </child>
25             <child>
26               <object class="GtkLabel" id="green">
27                 <property name="label">GREEN</property>
28                 <property name="justify">GTK_JUSTIFY_CENTER</property>
29                 <property name="name">green</property>
30               </object>
31             </child>
32             <child>
33               <object class="GtkLabel" id="blue">
34                 <property name="label">BLUE</property>
35                 <property name="justify">GTK_JUSTIFY_CENTER</property>
36                 <property name="name">blue</property>
37               </object>
38             </child>
39           </object>
40         </child>
41         <child>
42           <object class="GtkLabel" id="canvas">
43             <property name="label">CANVAS</property>
44             <property name="justify">GTK_JUSTIFY_CENTER</property>
45             <property name="name">canvas</property>
46             <property name="hexpand">TRUE</property>
47             <property name="vexpand">TRUE</property>
48           </object>
49         </child>
50       </object>
51     </child>
52   </object>
53 </interface>
54 
~~~

It is converted to a resource file by `glib-compile-resources`.
The compiler uses an XML file `dnd.gresource.xml`.

~~~xml
1 <?xml version="1.0" encoding="UTF-8"?>
2 <gresources>
3   <gresource prefix="/com/github/ToshioCP/dnd">
4     <file>dnd.ui</file>
5   </gresource>
6 </gresources>
~~~

## C file dnd.c

The C file `dnd.c` isn't a big file.
The number of the lines is less than a hundred.
A GtkApplication object is created in the function `main`.

~~~C
 1 int
 2 main (int argc, char **argv) {
 3   GtkApplication *app;
 4   int stat;
 5 
 6   app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
 7   g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
 8   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
 9   stat =g_application_run (G_APPLICATION (app), argc, argv);
10   g_object_unref (app);
11   return stat;
12 }
~~~

The application ID is defined as:

```C
#define APPLICATION_ID "com.github.ToshioCP.dnd"
```

### Startup signal handler

Most of the work is done in the "startup" signal handler.

Two objects GtkDragSource and GtkDropTarget is used for DND implementation.

- Drag source: A drag source (GtkDragSource instance) is an event controller.
It initiates a DND operation when the user clicks and drags the widget.
If a data, in the form of GdkContentProvider, is set in advance, it gives the data to the system at the beginning of the drag.
- Drop target: A drop target (GtkDropTarget) is also an event controller.
You can get the data in the GtkDropTarget::drop signal handler.

The example below uses these objects in a very simple way.
You can use number of features that the two objects have.
See the following links for more information.

- [Drag-and-Drop in GTK](https://docs.gtk.org/gtk4/drag-and-drop.html)
- [GtkDragSource](https://docs.gtk.org/gtk4/class.DragSource.html)
- [GtkDropTarget](https://docs.gtk.org/gtk4/class.DropTarget.html)

~~~C
 1 static void
 2 app_startup (GApplication *application) {
 3   GtkApplication *app = GTK_APPLICATION (application);
 4   GtkBuilder *build;
 5   GtkWindow *win;
 6   GtkLabel *src_labels[3];
 7   int i;
 8   GtkLabel *canvas;
 9   GtkDragSource *src;
10   GdkContentProvider* content;
11   GtkDropTarget *tgt;
12   GdkDisplay *display;
13   char *s;
14 
15   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/dnd/dnd.ui");
16   win = GTK_WINDOW (gtk_builder_get_object (build, "win"));
17   src_labels[0] = GTK_LABEL (gtk_builder_get_object (build, "red"));
18   src_labels[1] = GTK_LABEL (gtk_builder_get_object (build, "green"));
19   src_labels[2] = GTK_LABEL (gtk_builder_get_object (build, "blue"));
20   canvas = GTK_LABEL (gtk_builder_get_object (build, "canvas"));
21   gtk_window_set_application (win, app);
22   g_object_unref (build);
23 
24   for (i=0; i<3; ++i) {
25     src = gtk_drag_source_new ();
26     content = gdk_content_provider_new_typed (G_TYPE_STRING, gtk_widget_get_name (GTK_WIDGET (src_labels[i])));
27     gtk_drag_source_set_content (src, content);
28     g_object_unref (content);
29     gtk_widget_add_controller (GTK_WIDGET (src_labels[i]), GTK_EVENT_CONTROLLER (src)); // The ownership of src is taken by the instance.
30   }
31 
32   tgt = gtk_drop_target_new (G_TYPE_STRING, GDK_ACTION_COPY);
33   g_signal_connect (tgt, "drop", G_CALLBACK (drop_cb), NULL);
34   gtk_widget_add_controller (GTK_WIDGET (canvas), GTK_EVENT_CONTROLLER (tgt)); // The ownership of tgt is taken by the instance.
35 
36   provider = gtk_css_provider_new ();
37   s = g_strdup_printf (format, "black");
38   gtk_css_provider_load_from_data (provider, s, -1);
39   g_free (s);
40   display = gdk_display_get_default ();
41   gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider),
42                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
43   g_object_unref (provider); // The provider is still alive because the display owns it.
44 }
~~~

- 15-22: Builds the widgets.
The array `source_labels[]` points the source labels red, green and blue in the ui file.
The variable `canvas` points the destination label.
- 24-30: Sets the DND source widgets.
The for-loop carries out through the array `src_labels[]` each of which points the source widget,  red, green or blue label.
- 25: Creates a new GtkDragSource instance.
- 26: Creates a new GdkContentProvider instance with the string "red", "green" or "blue.
They are the name of the widgets.
These strings are the data to transfer through the DND operation.
- 27: Sets the content of the drag source to the GdkContentProvider instance above.
- 28: Content is useless so it is destroyed.
- 29: Add the event controller, which is actually the drag source, to the widget.
If a DND operation starts on the widget, the corresponding drag source works and the data is given to the system.
- 32-34: Sets the DND drop target.
- 32: Creates a new GtkDropTarget instance.
The first parameter is the GType of the data.
The second parameter is a GdkDragAction enumerate constant.
The arguments here are string type and the constant for copy.
- 33: Connects the "drop" signal and the handler `drop_cb`.
- 34: Add the event controller, which is actually the drop target, to the widget.
- 36-43: Sets CSS.
- 37: A varable `format` is static and defined at the top of the program.
Static variables are shown below.

```C
static GtkCssProvider *provider = NULL;
static const char *format = "label {padding: 20px;} label#red {background: red;} "
  "label#green {background: green;} label#blue {background: blue;} "
  "label#canvas {color: %s; font-weight: bold; font-size: 72pt;}";
```

### Activate signal handler

~~~C
1 static void
2 app_activate (GApplication *application) {
3   GtkApplication *app = GTK_APPLICATION (application);
4   GtkWindow *win;
5 
6   win = gtk_application_get_active_window (app);
7   gtk_window_present (win);
8 }
~~~

This handler just shows the window.

### Drop signal handler

~~~C
1 static gboolean
2 drop_cb (GtkDropTarget* self, const GValue* value, gdouble x, gdouble y, gpointer user_data) {
3   char *s;
4 
5   s = g_strdup_printf (format, g_value_get_string (value));
6   gtk_css_provider_load_from_data (provider, s, -1);
7   g_free (s);
8   return TRUE;
9 }
~~~

The "drop" signal handler has five parameters.

- GtkDropTarget instance on which the signal has been emitted.
- GValue that holds the data from the source.
- The arguments `x` and `y` are the coordinate of the mouse when released.
- User data was set when the signal and handler was connected.

The string from the GValue is "red", "green" or "blue".
It replaces "%s" in the variable `format`.
That means the font color of the label `canvas` will turn to the color.

## Meson.build

The file `meson.build` controls the building process.

~~~meson
1 project('dnd', 'c')
2 
3 gtkdep = dependency('gtk4')
4 
5 gnome = import('gnome')
6 resources = gnome.compile_resources('resources','dnd.gresource.xml')
7 
8 executable(meson.project_name(), 'dnd.c', resources, dependencies: gtkdep, export_dynamic: true, install: false)
~~~

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
