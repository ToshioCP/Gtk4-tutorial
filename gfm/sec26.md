Up: [README.md](../README.md),  Prev: [Section 25](sec25.md), Next: [Section 27](sec27.md)

# Custom drawing

Custom drawing is to draw shapes dynamically.
This section shows an example of custom drawing.
You can draw rectangles by dragging the mouse.

Down the button.

![down the button](../image/cd0.png)

Move the mouse

![Move the mouse](../image/cd1.png)

Up the button.

![Up the button](../image/cd2.png)

The programs are at `src/custom_drawing` directory.
Download the [repository](https://github.com/ToshioCP/Gtk4-tutorial) and see the directory.
There are four files.

- meson.build
- rect.c
- rect.gresource.xml
- rect.ui

## rect.gresource.xml

This file describes a ui file to compile.
The compiler glib-compile-resources uses it.

~~~xml
1 <?xml version="1.0" encoding="UTF-8"?>
2 <gresources>
3   <gresource prefix="/com/github/ToshioCP/rect">
4     <file>rect.ui</file>
5   </gresource>
6 </gresources>
~~~

The prefix is `/com/github/ToshioCP/rect` and the file is `rect.ui`.
Therefore, GtkBuilder reads the resource from `/com/github/ToshioCP/rect/rect.ui`.

## rect.ui

The following is the ui file that defines the widgets.
There are two widgets which are GtkApplicationWindow and GtkDrawingArea.
The ids are `win` and `da` respectively.

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <interface>
 3   <object class="GtkApplicationWindow" id="win">
 4     <property name="default-width">800</property>
 5     <property name="default-height">600</property>
 6     <property name="resizable">FALSE</property>
 7     <property name="title">Custom drawing</property>
 8     <child>
 9       <object class="GtkDrawingArea" id="da">
10         <property name="hexpand">TRUE</property>
11         <property name="vexpand">TRUE</property>
12       </object>
13     </child>
14   </object>
15 </interface>
16 
~~~

## rect.c

### GtkApplication

This program uses GtkApplication.
The application ID is `com.github.ToshioCP.rect`.

```c
#define APPLICATION_ID "com.github.ToshioCP.rect"
```

See [GNOME Developer Documentation](https://developer.gnome.org/documentation/tutorials/application-id.html) for further information.

The function `main` is called at the beginning of the application.

~~~C
 1 int
 2 main (int argc, char **argv) {
 3   GtkApplication *app;
 4   int stat;
 5 
 6   app = gtk_application_new (APPLICATION_ID, G_APPLICATION_HANDLES_OPEN);
 7   g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
 8   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
 9   g_signal_connect (app, "shutdown", G_CALLBACK (app_shutdown), NULL);
10   stat =g_application_run (G_APPLICATION (app), argc, argv);
11   g_object_unref (app);
12   return stat;
13 }
~~~

It connects three signals and handlers.

- startup: It is emitted after the application is registered to the system. 
- activate: It is emitted when the application is activated.
- shutdown: It is emitted just before the application quits.

~~~C
 1 static void
 2 app_startup (GApplication *application) {
 3   GtkApplication *app = GTK_APPLICATION (application);
 4   GtkBuilder *build;
 5   GtkWindow *win;
 6   GtkDrawingArea *da;
 7   GtkGesture *drag;
 8 
 9   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/rect/rect.ui");
10   win = GTK_WINDOW (gtk_builder_get_object (build, "win"));
11   da = GTK_DRAWING_AREA (gtk_builder_get_object (build, "da"));
12   gtk_window_set_application (win, app);
13   g_object_unref (build);
14 
15   gtk_drawing_area_set_draw_func (da, draw_cb, NULL, NULL);
16   g_signal_connect_after (da, "resize", G_CALLBACK (resize_cb), NULL);
17 
18   drag = gtk_gesture_drag_new ();
19   gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (drag), GDK_BUTTON_PRIMARY);
20   gtk_widget_add_controller (GTK_WIDGET (da), GTK_EVENT_CONTROLLER (drag));
21   g_signal_connect (drag, "drag-begin", G_CALLBACK (drag_begin), NULL);
22   g_signal_connect (drag, "drag-update", G_CALLBACK (drag_update), da);
23   g_signal_connect (drag, "drag-end", G_CALLBACK (drag_end), da);
24 }
~~~

The startup handler does three things.

- Builds the widgets.
- Initializes the GtkDrawingArea instance.
  - Sets the drawing function
  - Connects the "resize" signal and the handler.
- Creates the GtkGestureDrag instance and initializes it.
Gesture will be explained in this section later.

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

The activate handler just shows the window.

### GtkDrawingArea

The program has two cairo surfaces and they are pointed by the global variables.

```C
static cairo_surface_t *surface = NULL;
static cairo_surface_t *surface_save = NULL;
```

The drawing process is as follows.

- Creates an image on `surface`.
- Copies `surface` to the cairo surface of the GtkDrawingArea.
- Calls ` gtk_widget_queue_draw (da)` to draw it if necessary.

They are created in the "resize" signal handler.

~~~C
 1 static void
 2 resize_cb (GtkWidget *widget, int width, int height, gpointer user_data) {
 3   cairo_t *cr;
 4 
 5   if (surface)
 6     cairo_surface_destroy (surface);
 7   surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, width, height);
 8   if (surface_save)
 9     cairo_surface_destroy (surface_save);
10   surface_save = cairo_image_surface_create (CAIRO_FORMAT_RGB24, width, height);
11   /* Paint the surface white. It is the background color. */
12   cr = cairo_create (surface);
13   cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
14   cairo_paint (cr);
15   cairo_destroy (cr);
16 }
~~~

This callback is called when the GtkDrawingArea is shown.
It is the only call because the window is not resizable.

It creates image surfaces for `surface` and `surface_save`.
The `surface` surface is painted white, which is the background color.

The drawing function copies `surface` to the GtkDrawingArea surface.

~~~C
1 static void
2 draw_cb (GtkDrawingArea *da, cairo_t *cr, int width, int height, gpointer user_data) {
3   if (surface) {
4     cairo_set_source_surface (cr, surface, 0.0, 0.0);
5     cairo_paint (cr);
6   }
7 }
~~~

This function is called by the system when it needs to redraw the drawing area.

Two surfaces `surface` and `surface_save` are destroyed before the application quits.

~~~C
1 static void
2 app_shutdown (GApplication *application) {
3   if (surface)
4     cairo_surface_destroy (surface);
5   if (surface_save)
6     cairo_surface_destroy (surface_save);
7 }
~~~

### GtkGestureDrag

Gesture class is used to recognize human gestures such as click, drag, pan, swipe and so on.
It is a subclass of GtkEventController.
GtkGesture class is abstract and there are several implementations.

- GtkGestureClick
- GtkGestureDrag
- GtkGesturePan
- GtkGestureSwipe
- other implementations

The program `rect.c` uses GtkGestureDrag.
It is the implementation for drags.
The parent-child relationship is as follows.

```
GObject -- GtkEventController -- GtkGesture -- GtkGestureSingle -- GtkGestureDrag
```

GtkGestureSingle is a subclass of GtkGesture and optimized for singe-touch and mouse gestures.

A GtkGestureDrag instance is created and initialized in the startup signal handler in `rect.c`.
See line 18 to 23 in the following.

~~~C
 1 static void
 2 app_startup (GApplication *application) {
 3   GtkApplication *app = GTK_APPLICATION (application);
 4   GtkBuilder *build;
 5   GtkWindow *win;
 6   GtkDrawingArea *da;
 7   GtkGesture *drag;
 8 
 9   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/rect/rect.ui");
10   win = GTK_WINDOW (gtk_builder_get_object (build, "win"));
11   da = GTK_DRAWING_AREA (gtk_builder_get_object (build, "da"));
12   gtk_window_set_application (win, app);
13   g_object_unref (build);
14 
15   gtk_drawing_area_set_draw_func (da, draw_cb, NULL, NULL);
16   g_signal_connect_after (da, "resize", G_CALLBACK (resize_cb), NULL);
17 
18   drag = gtk_gesture_drag_new ();
19   gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (drag), GDK_BUTTON_PRIMARY);
20   gtk_widget_add_controller (GTK_WIDGET (da), GTK_EVENT_CONTROLLER (drag));
21   g_signal_connect (drag, "drag-begin", G_CALLBACK (drag_begin), NULL);
22   g_signal_connect (drag, "drag-update", G_CALLBACK (drag_update), da);
23   g_signal_connect (drag, "drag-end", G_CALLBACK (drag_end), da);
24 }
~~~

- The function `gtk_gesture_drag_new` creates a new GtkGestureDrag instance.
- The function `gtk_gesture_single_set_button` sets the button number to listen to.
The constant `GDK_BUTTON_PRIMARY` is the left button of a mouse.
- The function `gtk_widget_add_controller` adds an event controller, gestures are descendants of the event controller, to a widget.
- Three signals and handlers are connected.
  - drag-begin: Emitted when dragging starts.
  - drag-update: Emitted when the dragging point moves.
  - drag-end: Emitted when the dragging ends.

The process during the drag is as follows.

- start: save the surface and start points
- update: restore the surface and draw a thin rectangle between the start point and the current point of the mouse
- end: restore the surface and draw a thick rectangle between the start and end points.

We need two global variables for the start point.

```C
static double start_x;
static double start_y;
```

The following is the handler for the "drag-begin" signal.

~~~C
 1 static void
 2 copy_surface (cairo_surface_t *src, cairo_surface_t *dst) {
 3   if (!src || !dst)
 4     return;
 5   cairo_t *cr = cairo_create (dst);
 6   cairo_set_source_surface (cr, src, 0.0, 0.0);
 7   cairo_paint (cr);
 8   cairo_destroy (cr);
 9 }
10 
11 static void
12 drag_begin (GtkGestureDrag *gesture, double x, double y, gpointer user_data) {
13   // save the surface and record (x, y)
14   copy_surface (surface, surface_save);
15   start_x = x;
16   start_y = y;
17 }
~~~

- Copies `surface` to `surface_save`, which is an image just before the dragging.
- Stores the points to `start_x` and `start_y`.

~~~C
 1 static void
 2 drag_update  (GtkGestureDrag *gesture, double offset_x, double offset_y, gpointer user_data) {
 3   GtkWidget *da = GTK_WIDGET (user_data);
 4   cairo_t *cr;
 5   
 6   copy_surface (surface_save, surface);
 7   cr = cairo_create (surface);
 8   cairo_rectangle (cr, start_x, start_y, offset_x, offset_y);
 9   cairo_set_line_width (cr, 1.0);
10   cairo_stroke (cr);
11   cairo_destroy (cr);
12   gtk_widget_queue_draw (da);
13 }
~~~

- Restores `surface` from `surface_save`.
- Draws a rectangle with thin lines.
- Calls `gtk_widget_queue_draw` to add the GtkDrawingArea to the queue to redraw.

~~~C
 1 static void
 2 drag_end  (GtkGestureDrag *gesture, double offset_x, double offset_y, gpointer user_data) {
 3   GtkWidget *da = GTK_WIDGET (user_data);
 4   cairo_t *cr;
 5   
 6   copy_surface (surface_save, surface);
 7   cr = cairo_create (surface);
 8   cairo_rectangle (cr, start_x, start_y, offset_x, offset_y);
 9   cairo_set_line_width (cr, 6.0);
10   cairo_stroke (cr);
11   cairo_destroy (cr);
12   gtk_widget_queue_draw (da);
13 }
~~~

- Restores `surface` from `surface_save`.
- Draws a rectangle with thick lines.
- Calls `gtk_widget_queue_draw` to add the GtkDrawingArea to the queue to redraw.

## Build and run

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

![The screen of rect program](../image/rect.png)

Up: [README.md](../README.md),  Prev: [Section 25](sec25.md), Next: [Section 27](sec27.md)
