Up: [Readme.md](../Readme.md),  Prev: [Section 21](sec21.md), Next: [Section 23](sec23.md)

# GtkDrawingArea and Cairo

If you want to draw dynamically, like an image window of gimp graphics editor, GtkDrawingArea widget is the most suitable widget.
You can draw or redraw an image in this widget freely.
It is called custom drawing.

GtkDrawingArea provides a cairo context so users can draw images by cairo functions.
In this section, I will explain:

1. Cairo, but briefly.
2. GtkDrawingArea with very simple example.

## Cairo

Cairo is a two dimensional graphics library.
First, you need to know surface, source, mask, destination, cairo context and transformation.

- Surface represents an image.
It is like a canvas.
We can draw shapes and images with different colors on surfaces.
- Source pattern, or simply source, is a kind of paint, which will be transferred to destination surface by cairo functions.
- Mask is image mask used in the transference.
- Destination is a target surface.
- Cairo context manages the transference from source to destination through mask with its functions.
For example, `cairo_stroke` is a function to draw a path to the destination by the transference.
- Transformation is applied before the transfer completes.
The transformation is called affine, which is a mathematics terminology, and represented by matrix multiplication and vector addition.
Scaling, rotation, reflection, shearing and translation are examples of affine transformation.
In this section, we don't use it.
That means we only use identity transformation.
Therefore, the coordinate in source and mask is the same as the coordinate in destination.

![Stroke a rectangle](../image/cairo.png)

The instruction is as follows:

1. Create a surface.
This will be a destination.
2. Create a cairo context with the surface and the surface will be the destination of the context.
3. Create a source pattern within the context.
4. Create paths, which are lines, rectangles, arcs, texts or more complicated shapes in the mask.
5. Use drawing operator such as `cairo_stroke` to transfer the paint in the source to the destination.
6. Save the destination surface to a file if necessary.

Here's a simple example code that draws a small square and save it as a png file.

~~~C
 1 #include <cairo.h>
 2 
 3 int
 4 main (int argc, char **argv)
 5 {
 6   cairo_surface_t *surface;
 7   cairo_t *cr;
 8   int width = 100;
 9   int height = 100;
10   int square_size = 40.0;
11 
12   /* Create surface and cairo */
13   surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, width, height);
14   cr = cairo_create (surface);
15 
16   /* Drawing starts here. */
17   /* Paint the background white */
18   cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
19   cairo_paint (cr);
20   /* Draw a black rectangle */
21   cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
22   cairo_set_line_width (cr, 2.0);
23   cairo_rectangle (cr, width/2.0 - square_size/2, height/2.0 - square_size/2, square_size, square_size);
24   cairo_stroke (cr);
25 
26   /* Write the surface to a png file and clean up cairo and surface. */
27   cairo_surface_write_to_png (surface, "rectangle.png");
28   cairo_destroy (cr);
29   cairo_surface_destroy (surface);
30 
31   return 0;
32 }
~~~

- 1: Includes the header file of Cairo.
- 6: `cairo_surface_t` is the type of a surface.
- 7: `cairo_t` is the type of a cairo context.
- 8-10: `width` and `height` is the size of `surface`.
`square_size` is the size of a square drawn on the surface.
- 13: `cairo_image_surface_create` creates an image surface.
`CAIRO_FORMAT_RGB24` is a constant which means that each pixel has red, green and blue data.
Each data has 8 bits quantity.
Therefore, 24 bits (3x8=24) is the size of RGB24.
Modern displays have this type of color depth.
Width and height are pixels and given as integers.
- 14: Creates cairo context.
The surface given as an argument will be the destination of the context.
- 18: `cairo_set_source_rgb` creates a source pattern, which is a solid white paint.
The second to fourth argument is red, green and blue color depth respectively.
Their type is float and the values are between zero and one.
(0,0,0) is black and (1,1,1) is white.
- 19: `cairo_paint` copies everywhere in the source to destination.
The destination is filled with white pixels with this command.
- 21: Sets the source color to black.
- 22: `cairo_set_line_width` set the width of lines.
In this case, the line width is set to two pixels.
(It is because the transformation is identity.
If the transformation isn't identity, for example scaling with the factor three, the actual width in destination will be six (2x3=6) pixels.)
- 23: Draws a rectangle (square) on the mask.
The square is located at the center.
- 24: `cairo_stroke` transfer the source to destination through the rectangle in the mask.
- 27: Outputs the image to a png file `rectangle.png`.
- 28: Destroys the context. At the same time the source is destroyed.
- 29: Destroys the surface.

To compile this, type the following.

    $ gcc `pkg-config --cflags cairo` cairo.c `pkg-config --libs cairo`

![rectangle.png](misc/rectangle.png)

There are lots of documentations in [Cairo's website](https://www.cairographics.org/).
If you aren't familiar with Cairo, it is strongly recommended to read the [tutorial](https://www.cairographics.org/tutorial/) in the website.

## GtkDrawingArea

The following is a very simple example.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 draw_function (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
 5   int square_size = 40.0;
 6 
 7   cairo_set_source_rgb (cr, 1.0, 1.0, 1.0); /* whilte */
 8   cairo_paint (cr);
 9   cairo_set_line_width (cr, 2.0);
10   cairo_set_source_rgb (cr, 0.0, 0.0, 0.0); /* black */
11   cairo_rectangle (cr, width/2.0 - square_size/2, height/2.0 - square_size/2, square_size, square_size);
12   cairo_stroke (cr);
13 }
14 
15 static void
16 app_activate (GApplication *app, gpointer user_data) {
17   GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));
18   GtkWidget *area = gtk_drawing_area_new ();
19 
20   gtk_window_set_title (GTK_WINDOW (win), "da1");
21   gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (area), draw_function, NULL, NULL);
22   gtk_window_set_child (GTK_WINDOW (win), area);
23 
24   gtk_widget_show (win);
25 }
26 
27 #define APPLICATION_ID "com.github.ToshioCP.da1"
28 
29 int
30 main (int argc, char **argv) {
31   GtkApplication *app;
32   int stat;
33 
34   app = gtk_application_new (APPLICATION_ID, G_APPLICATION_FLAGS_NONE);
35   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
36   stat =g_application_run (G_APPLICATION (app), argc, argv);
37   g_object_unref (app);
38   return stat;
39 }
40 
~~~

The function `main` is almost same as before.
The two functions `app_activate` and `draw_function` is important in this example.

- 18: Creates a GtkDrawingArea instance.
- 21: Sets a drawing function of the widget.
GtkDrawingArea widget uses the function to draw the contents of itself whenever its necessary.
For example, when a user drag a mouse pointer and resize a top-level window, GtkDrawingArea also changes the size.
Then, the whole window needs to be redrawn.
For the information of `gtk_drawing_area_set_draw_func`, see [Gtk4 reference manual](https://developer.gnome.org/gtk4/stable/GtkDrawingArea.html#gtk-drawing-area-set-draw-func).

The drawing function has five parameters.

~~~C
void drawing_function (GtkDrawingArea *drawing_area, cairo_t *cr, int width, int height,
                       gpointer user_data);
~~~

The first parameter is the GtkDrawingArea widget.
You can't change any properties, for example `content-width` or `content-height`, in this function.
The second parameter is a cairo context given by the widget.
The destination surface of the context is connected to the contents of the widget.
What you draw to this surface will appear in the widget on the screen.
The third and fourth parameters are the size of the destination surface.
Now, look at the program of the example again.

- 3-13: The drawing function.
- 7-8: Sets the source to be white and paint the destination white.
- 9: Sets the line width to be 2.
- 10: Sets the source to be black.
- 11: Adds a rectangle to the mask.
- 12: Draws the rectangle with black color to the destination.

Compile and run it, then a window with a black rectangle (square) appears.
Try resizing the window.
The square always appears at the center of the window because the drawing function is invoked every moment the window is resized.

![Square in the window](../image/da1.png)


Up: [Readme.md](../Readme.md),  Prev: [Section 21](sec21.md), Next: [Section 23](sec23.md)
