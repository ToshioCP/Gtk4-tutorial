# Custom Drawing

Custom drawing refers to drawing shapes dynamically.
This section shows a custom drawing example that allows you to draw rectangles by dragging the mouse.
Here is how to draw a rectangle with the example program.

Press down the button.

@@@if pdf
![Press down the button](/images/cd0.png){width=6cm height=4.83cm}
@@@else
![Press down the button](/images/cd0.png)
@@@end

Move the mouse.

@@@if pdf
![Move the mouse](/images/cd1.png){width=6cm height=4.83cm}
@@@else
![Move the mouse](/images/cd1.png)
@@@end

Release the button.

@@@if pdf
![Release the button](/images/cd2.png){width=6cm height=4.83cm}
@@@else
![Release the button](/images/cd2.png)
@@@end

The programs are at `src/custom_drawing` directory.
Download the [repository](https://github.com/ToshioCP/Gtk4-tutorial) and see the directory.
There are four files.

- meson.build
- rect.c
- rect.gresource.xml
- rect.ui

## rect.gresource.xml

This file is a resource manifest used by the glib-compile-resources compiler.

@@@include
custom_drawing/rect.gresource.xml
@@@

The prefix is `/com/github/ToshioCP/rect` and the file is `rect.ui`.
Therefore, the resource compiler compiles and stores the resource at `/com/github/ToshioCP/rect/rect.ui`.
Later, GtkBuilder reads the resource from `/com/github/ToshioCP/rect/rect.ui`.

## rect.ui

The following is the UI file that defines the widgets which are GtkApplicationWindow and GtkDrawingArea.
The ids are `win` and `da` respectively.

@@@include
custom_drawing/rect.ui
@@@

## rect.c

### GtkApplication

This program uses GtkApplication.
The application ID is `com.github.ToshioCP.rect`.

```c
#define APPLICATION_ID "com.github.ToshioCP.rect"
```

See [GNOME Developer Documentation](https://developer.gnome.org/documentation/tutorials/application-id.html) for further information.

The function `main` is called at the beginning of the application.

@@@include
custom_drawing/rect.c main
@@@

It connects three signals and handlers.

- startup: Emitted after the application is registered to the system. 
- activate: Emitted when the application is activated.
- shutdown: Emitted just before the application quits.

@@@include
custom_drawing/rect.c app_startup
@@@

The startup handler performs the following three tasks:

- Builds the widgets.
- Initializes the GtkDrawingArea instance.
  - Sets the drawing function
  - Connects the "resize" signal and the handler.
- Creates the GtkGestureDrag instance and connects it to the drawing area object.
Gestures will be explained in this section later.

@@@include
custom_drawing/rect.c app_activate
@@@

The activate handler just shows the window.

### GtkDrawingArea

The program has two cairo surfaces and they are pointed to by the global variables.

@@@if gfm
```C
static cairo_surface_t *surface = NULL;
static cairo_surface_t *surface_save = NULL;
```
@@@else
```{.C}
static cairo_surface_t *surface = NULL;
static cairo_surface_t *surface_save = NULL;
```
@@@end

The drawing process is as follows.

- Creates an image on `surface`.
- Copies `surface` to the cairo surface of the GtkDrawingArea.
- Calls ` gtk_widget_queue_draw (da)`.

They are created in the "resize" signal handler.

@@@include
custom_drawing/rect.c resize_cb
@@@

This callback is called when the GtkDrawingArea is shown.
This is called only once since the window is not resizable.
It creates image surfaces for `surface` and `surface_save`.
The `surface` surface is painted white, which is the background color.

The drawing function copies `surface` to the GtkDrawingArea surface.

#### Scale factor

Modern displays, especially HiDPI (or "Retina") displays, render more physical pixels per logical pixel than traditional displays.
GTK reports this ratio as the scale factor of a widget, obtained with `gtk_widget_get_scale_factor()`.
A scale factor of 1 means one logical pixel equals one physical pixel, while a scale factor of 2 means one logical pixel is rendered using a 2x2 block of physical pixels.

The `surface` surface must have the same scale factor and the same physical pixel dimensions as the drawing area's cairo destination surface, since the former is copied onto the latter in the draawing function.
If `surface` is created using only the logical width and height passed to the "resize" signal, the surface will contain far fewer physical pixels than the display can actually show on a HiDPI screen.
As a result, the drawn image will look blurry when it is scaled up to fill the display.

The physical pixel sizes can be obtained by multiplying logical pixel sizes by the scale factor.
The surface scale is set with `cairo_surface_set_device_scale`.

@@@include
custom_drawing/rect.c draw_cb
@@@

This function is called by the system when it needs to redraw the drawing area.

Two surfaces `surface` and `surface_save` are destroyed before the application quits.

@@@include
custom_drawing/rect.c app_shutdown
@@@

### GtkGestureDrag

Gesture class is used to recognize human gestures such as click, drag, pan, swipe and so on.
It is a subclass of GtkEventController.
GtkGesture class is abstract and there are several implementations.

- GtkGestureClick
- GtkGestureDrag
- GtkGesturePan
- GtkGestureSwipe
- And others

The program `rect.c` uses GtkGestureDrag.
It is the implementation for drag gestures.
The parent-child relationship is as follows.

```
GObject -- GtkEventController -- GtkGesture -- GtkGestureSingle -- GtkGestureDrag
```

GtkGestureSingle is a subclass of GtkGesture and optimized for single-touch and mouse interactions.

A GtkGestureDrag instance is created and initialized in the startup signal handler in `rect.c`.
See line 18 to 23 in the following.

@@@include
custom_drawing/rect.c app_startup
@@@

- The function `gtk_gesture_drag_new` creates a new GtkGestureDrag instance.
The default button number is 1 (`GDK_BUTTON_PRIMARY`), which is the left button of a mouse or touch events.
- The function `gtk_widget_add_controller` adds an event controller, which is a base class for gestures.
- Four signals and handlers are connected.
  - drag-begin: Emitted when dragging starts.
  - drag-update: Emitted when the dragging pointer moves.
  - drag-end: Emitted when the dragging ends.
  - cancel: Emitted when the dragging was canceled. This signal belongs to GtkGesture.

The process during the drag is as follows.

- start: save the surface and start points
- update: restore the surface and draw a thin rectangle between the start point and the current point of the mouse
- end: restore the surface and draw a thick rectangle between the start and end points
- cancel: restore the surface

We need two global variables for the start point.

@@@if gfm
```C
static double start_x;
static double start_y;
```
@@@else
```{.C}
static double start_x;
static double start_y;
```
@@@end

The following is the handler for the "drag-begin" signal.

@@@include
custom_drawing/rect.c copy_surface drag_begin
@@@

- Copies `surface` to `surface_save`, which is an image just before the dragging.
- Stores the points to `start_x` and `start_y`.

@@@include
custom_drawing/rect.c drag_update
@@@

- Restores `surface` from `surface_save`.
- Draws a rectangle with thin lines.
- Calls `gtk_widget_queue_draw` to add the GtkDrawingArea to the queue to redraw.

@@@include
custom_drawing/rect.c drag_end
@@@

- Restores `surface` from `surface_save`.
- Draws a rectangle with thick lines.
- Calls `gtk_widget_queue_draw` to add the GtkDrawingArea to the queue to redraw.

@@@include
custom_drawing/rect.c drag_cancel
@@@

- Restores `surface` from `surface_save`.
- Calls `gtk_widget_queue_draw` to add the GtkDrawingArea to the queue to redraw.

## Build and Run

Download the [repository](https://github.com/ToshioCP/Gtk4-tutorial).
Change your current directory to `src/custom_drawing`.
Run meson and ninja to build the program.
Type `_build/rect` to run the program.
Try to draw rectangles.

```
$ cd src/custom_drawing
$ meson setup _build
$ ninja -C _build
$ _build/rect
```

@@@if pdf
![The screen of rect program](/images/rect.png){width=12.4cm height=10cm}
@@@else
![The screen of rect program](/images/rect.png)
@@@end
