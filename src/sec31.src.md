# Drag and Drop

## What is Drag and Drop?

Drag and drop (often hyphenated as "drag-and-drop", or shortened to "DnD") works much like copying and pasting.
When a user drags a UI element --- such as a widget or a selected piece of content --- and drops it onto another area, underlying data is transferred from the source to the target.

You are probably already familiar with this action from everyday tasks like dragging files between folders.

@@@if pdf
![DND on the GUI file manager](/images/dnd.png){width=4.6cm height=3cm}
@@@else
![DND on the GUI file manager](/images/dnd.png)
@@@end

When a drag operation begins, the file `sample_file.txt` is handed over to the system.
When the operation ends with a drop, the system transfers the file to `sample_folder` in the file manager.
In this scenario, it acts much like a "cut and paste" action.
While the actual technical implementation differs, the underlying concept is similar.

## DND Example

This tutorial provides a simple example in the `src/dnd` directory.
The UI consists of three colored source labels ("red", "green", and "blue") at the top and a large target label below them.
When you drag one of the colored labels and drop it onto the target, the target's text color changes accordingly.

@@@if pdf
![DND example](/images/dnd_canvas.png){width=13.5cm height=5.2cm}
@@@else
![DND example](/images/dnd_canvas.png)
@@@end

## UI File

The application's widgets are defined in a `dnd.ui` XML file.

@@@include
dnd/dnd.ui
@@@

This UI file is compiled into a resource file using the `glib-compile-resources` utility.
To do this, the compiler requires a resource description file, `dnd.gresource.xml`.

@@@include
dnd/dnd.gresource.xml
@@@

## The `dnd.c` Source File

The `dnd.c` source file is relatively short, containing fewer than a hundred lines of code.
Inside this file, the `main` function creates a GtkApplication object.

@@@include
dnd/dnd.c main
@@@

The application ID is defined as:

@@@if gfm
```C
@@@else
```{.C}
@@@end
#define APPLICATION_ID "com.github.ToshioCP.dnd"
```

### Startup Signal Handler

Most of the work in this application is done inside the `startup` signal handler.

To implement drag-and-drop, we use two main objects: `GtkDragSource` and `GtkDropTarget`.

- Drag source: GtkDragSource is an event controller.
It initiates a DnD operation when the user clicks and drags a widget.
If you prepare data in advance using a GdkContentProvider, the drag source hands this data over to the system as soon as the drag begins.
- Drop target: GtkDropTarget is also an event controller.
When the dragged data is dropped onto the target widget, you can retrieve that data inside its drop signal handler (`GtkDrpTarget::drop`).

The example below demonstrates a very simple way to use these objects.

For more information, refer to the official documentation:

- [Drag-and-Drop in GTK](https://docs.gtk.org/gtk4/drag-and-drop.html)
- [GtkDragSource](https://docs.gtk.org/gtk4/class.DragSource.html)
- [GtkDropTarget](https://docs.gtk.org/gtk4/class.DropTarget.html)

@@@include
dnd/dnd.c app_startup
@@@

- 15–22: Builds the widgets from the UI file.
The `src_labels[]` array holds references to the red, green, and blue source labels, while the `canvas` variable points to the destination label.
- 24–30: Configures the drag source widgets.
The `for` loop iterates through each of the three colored source labels.
- 25: Creates a new GtkDragSource instance.
- 26: Creates a GdkContentProvider containing the widget's name ("red", "green", or "blue").
This string is the payload transferred during the drag operation.
- 27: Assigns the content provider to the drag source.
- 28: Releases the local reference to the content provider using `g_object_unref()`.
The drag source now holds its own reference, so the local one is no longer needed.
- 29: Adds the drag source (which is an event controller) to the widget.
When a user drags this widget, the drag source activates and provides the data to the system.
- 32–34: Configures the drop target.
- 32: Creates a new GtkDropTarget.
The parameters specify that it accepts string data (`G_TYPE_STRING`) and allows copy operations (`GDK_ACTION_COPY`).
`GDK_ACTION_COPY` is a GdkDragAction enumeration constant.
- 33: Connects the drop signal to the `drop_cb` callback function.
- 34: Adds the drop target event controller to the `canvas` widget.
- 36–43: Applies CSS styling to the application.
- 37: Formats the CSS string using a static variable named `format`, which is defined at the top of the program (shown below).

@@@if gfm
```C
@@@else
```{.C}
@@@end
static GtkCssProvider *provider = NULL;
static const char *format = "label {padding: 20px;} label#red {background: red;} "
  "label#green {background: green;} label#blue {background: blue;} "
  "label#canvas {color: %s; font-weight: bold; font-size: 72pt;}";
```

### Activate Signal Handler

@@@include
dnd/dnd.c app_activate
@@@

This handler just shows the window.

### Drop Signal Handler

@@@include
dnd/dnd.c drop_cb
@@@

The "drop" signal handler takes five parameters:

- `self`: The GtkDropTarget instance that emitted the signal.
- `value`: A GValue containing the data transferred from the drag source.
- `x` and `y`: The coordinates of the mouse pointer when the drop occurred.
- `user_data`: The user data pointer provided when the signal was connected.
In `dnd.c` the `user_data` pointer was set to `NULL`.

The `value` parameter holds the payload string ("red", "green", or "blue").
This string replaces the `%s` placeholder in the format variable to generate a new CSS string.
As a result, the text color of the `canvas` label is updated to match the dropped color.

## Building with Meson

The `meson.build` file controls the build process.

@@@include
dnd/meson.build
@@@

You can build and run the `dnd` application from the command line using the following commands:

```
$ cd src/dnd
$ meson setup _build
$ ninja -C _build
$ _build/dnd
```

The complete source files are located in the [`src/dnd`](/dnd/) directory of the tutorial repository.
Feel free to download it and explore the code yourself.
