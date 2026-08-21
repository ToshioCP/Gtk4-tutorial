Up: [README.md](../README.md),  Prev: [Section 28](sec28.md), Next: [Section 30](sec30.md)

# Custom Drawing

Custom drawing refers to drawing shapes dynamically.
This section shows a custom drawing example that allows you to draw rectangles by dragging the mouse.
Here is how to draw a rectangle with the example program.

Press down the button.

![Press down the button](/src/images/cd0.png)

Move the mouse.

![Move the mouse](/src/images/cd1.png)

Release the button.

![Release the button](/src/images/cd2.png)

The programs are at `src/custom_drawing` directory.
Download the [repository](https://github.com/ToshioCP/Gtk4-tutorial) and see the directory.
There are four files.

- meson.build
- rect.c
- rect.gresource.xml
- rect.ui

## rect.gresource.xml

This file is a resource manifest used by the glib-compile-resources compiler.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<gresources>
  <gresource prefix="/com/github/ToshioCP/rect">
    <file>rect.ui</file>
  </gresource>
</gresources>
```

The prefix is `/com/github/ToshioCP/rect` and the file is `rect.ui`.
Therefore, the resource compiler compiles and stores the resource at `/com/github/ToshioCP/rect/rect.ui`.
Later, GtkBuilder reads the resource from `/com/github/ToshioCP/rect/rect.ui`.

## rect.ui

The following is the UI file that defines the widgets which are GtkApplicationWindow and GtkDrawingArea.
The ids are `win` and `da` respectively.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <object class="GtkApplicationWindow" id="win">
    <property name="default-width">800</property>
    <property name="default-height">600</property>
    <property name="resizable">FALSE</property>
    <property name="title">Custom drawing</property>
    <child>
      <object class="GtkDrawingArea" id="da">
        <property name="hexpand">TRUE</property>
        <property name="vexpand">TRUE</property>
      </object>
    </child>
  </object>
</interface>

```

## rect.c

### GtkApplication

This program uses GtkApplication.
The application ID is `com.github.ToshioCP.rect`.

```c
#define APPLICATION_ID "com.github.ToshioCP.rect"
```

See [GNOME Developer Documentation](https://developer.gnome.org/documentation/tutorials/application-id.html) for further information.

The function `main` is called at the beginning of the application.

```c
int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_signal_connect (app, "shutdown", G_CALLBACK (app_shutdown), NULL);
  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
```

It connects three signals and handlers.

- startup: Emitted after the application is registered to the system. 
- activate: Emitted when the application is activated.
- shutdown: Emitted just before the application quits.

```c
static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkBuilder *build;
  GtkWindow *win;
  GtkDrawingArea *da;
  GtkGesture *drag;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/rect/rect.ui");
  win = GTK_WINDOW (gtk_builder_get_object (build, "win"));
  da = GTK_DRAWING_AREA (gtk_builder_get_object (build, "da"));
  gtk_window_set_application (win, app);
  g_object_unref (build);

  gtk_drawing_area_set_draw_func (da, draw_cb, NULL, NULL);
  g_signal_connect (da, "resize", G_CALLBACK (resize_cb), NULL);

  drag = gtk_gesture_drag_new ();
  gtk_widget_add_controller (GTK_WIDGET (da), GTK_EVENT_CONTROLLER (drag));
  g_signal_connect (drag, "drag-begin", G_CALLBACK (drag_begin), NULL);
  g_signal_connect (drag, "drag-update", G_CALLBACK (drag_update), da);
  g_signal_connect (drag, "drag-end", G_CALLBACK (drag_end), da);
  g_signal_connect (drag, "cancel", G_CALLBACK (drag_cancel), da); 
}
```

The startup handler performs the following three tasks:

- Builds the widgets.
- Initializes the GtkDrawingArea instance.
  - Sets the drawing function
  - Connects the "resize" signal and the handler.
- Creates the GtkGestureDrag instance and connects it to the drawing area object.
Gestures will be explained in this section later.

```c
static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWindow *win;

  win = gtk_application_get_active_window (app);
  gtk_window_present (win);
}
```

The activate handler just shows the window.

### GtkDrawingArea

The program has two cairo surfaces and they are pointed to by the global variables.

```C
static cairo_surface_t *surface = NULL;
static cairo_surface_t *surface_save = NULL;
```

The drawing process is as follows.

- Creates an image on `surface`.
- Copies `surface` to the cairo surface of the GtkDrawingArea.
- Calls ` gtk_widget_queue_draw (da)`.

They are created in the "resize" signal handler.

```c
static void
resize_cb (GtkWidget *widget, int width, int height, gpointer user_data) {
  cairo_t *cr;
  int scale = gtk_widget_get_scale_factor (widget);
  
  if (surface)
    cairo_surface_destroy (surface);
  surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, width*scale, height*scale);
  cairo_surface_set_device_scale (surface, scale, scale);

  if (surface_save)
    cairo_surface_destroy (surface_save);
  surface_save = cairo_image_surface_create (CAIRO_FORMAT_RGB24, width*scale, height*scale);
  cairo_surface_set_device_scale (surface_save, scale, scale);

  /* Paint the surface white. It is the background color. */
  cr = cairo_create (surface);
  cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
  cairo_paint (cr);
  cairo_destroy (cr);
}
```

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

```c
static void
draw_cb (GtkDrawingArea *da, cairo_t *cr, int width, int height, gpointer user_data) {
  if (surface) {
    cairo_set_source_surface (cr, surface, 0.0, 0.0);
    cairo_paint (cr);
  }
}
```

This function is called by the system when it needs to redraw the drawing area.

Two surfaces `surface` and `surface_save` are destroyed before the application quits.

```c
static void
app_shutdown (GApplication *application) {
  if (surface)
    cairo_surface_destroy (surface);
  if (surface_save)
    cairo_surface_destroy (surface_save);
}
```

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

```c
static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkBuilder *build;
  GtkWindow *win;
  GtkDrawingArea *da;
  GtkGesture *drag;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/rect/rect.ui");
  win = GTK_WINDOW (gtk_builder_get_object (build, "win"));
  da = GTK_DRAWING_AREA (gtk_builder_get_object (build, "da"));
  gtk_window_set_application (win, app);
  g_object_unref (build);

  gtk_drawing_area_set_draw_func (da, draw_cb, NULL, NULL);
  g_signal_connect (da, "resize", G_CALLBACK (resize_cb), NULL);

  drag = gtk_gesture_drag_new ();
  gtk_widget_add_controller (GTK_WIDGET (da), GTK_EVENT_CONTROLLER (drag));
  g_signal_connect (drag, "drag-begin", G_CALLBACK (drag_begin), NULL);
  g_signal_connect (drag, "drag-update", G_CALLBACK (drag_update), da);
  g_signal_connect (drag, "drag-end", G_CALLBACK (drag_end), da);
  g_signal_connect (drag, "cancel", G_CALLBACK (drag_cancel), da); 
}
```

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

```C
static double start_x;
static double start_y;
```

The following is the handler for the "drag-begin" signal.

```c
static void
copy_surface (cairo_surface_t *src, cairo_surface_t *dst) {
  if (!src || !dst)
    return;
  cairo_t *cr = cairo_create (dst);
  cairo_set_source_surface (cr, src, 0.0, 0.0);
  cairo_paint (cr);
  cairo_destroy (cr);
}

static void
drag_begin (GtkGestureDrag *gesture, double x, double y, gpointer user_data) {
  // save the surface and record (x, y)
  copy_surface (surface, surface_save);
  start_x = x;
  start_y = y;
}
```

- Copies `surface` to `surface_save`, which is an image just before the dragging.
- Stores the points to `start_x` and `start_y`.

```c
static void
drag_update  (GtkGestureDrag *gesture, double offset_x, double offset_y, gpointer user_data) {
  GtkWidget *da = GTK_WIDGET (user_data);
  cairo_t *cr;
  
  copy_surface (surface_save, surface);
  cr = cairo_create (surface);
  cairo_rectangle (cr, start_x, start_y, offset_x, offset_y);
  cairo_set_line_width (cr, 1.0);
  cairo_stroke (cr);
  cairo_destroy (cr);
  gtk_widget_queue_draw (da);
}
```

- Restores `surface` from `surface_save`.
- Draws a rectangle with thin lines.
- Calls `gtk_widget_queue_draw` to add the GtkDrawingArea to the queue to redraw.

```c
static void
drag_end  (GtkGestureDrag *gesture, double offset_x, double offset_y, gpointer user_data) {
  GtkWidget *da = GTK_WIDGET (user_data);
  cairo_t *cr;
  
  copy_surface (surface_save, surface);
  cr = cairo_create (surface);
  cairo_rectangle (cr, start_x, start_y, offset_x, offset_y);
  cairo_set_line_width (cr, 6.0);
  cairo_stroke (cr);
  cairo_destroy (cr);
  gtk_widget_queue_draw (da);
}
```

- Restores `surface` from `surface_save`.
- Draws a rectangle with thick lines.
- Calls `gtk_widget_queue_draw` to add the GtkDrawingArea to the queue to redraw.

```c
static void
drag_cancel (GtkGesture *gesture, GdkEventSequence *sequence, gpointer user_data) {
  GtkWidget *da = GTK_WIDGET (user_data);

  copy_surface (surface_save, surface);
  gtk_widget_queue_draw (da);
}
```

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

![The screen of rect program](/src/images/rect.png)

Up: [README.md](../README.md),  Prev: [Section 28](sec28.md), Next: [Section 30](sec30.md)
