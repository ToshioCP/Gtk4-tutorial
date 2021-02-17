Up: [Readme.md](../Readme.md),  Prev: [Section 19](sec19.md), Next: [Section 21](sec21.md)

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
4. Create paths, which are lines, rectangles, arcs, texts or more complicated shapes, to generate a mask.
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
10 
11   /* Generate surface and cairo */
12   surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, width, height);
13   cr = cairo_create (surface);
14 
15   /* Drawing starts here. */
16   /* Paint the background white */
17   cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
18   cairo_paint (cr);
19   /* Draw a black rectangle */
20   cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
21   cairo_set_line_width (cr, 2.0);
22   cairo_rectangle (cr, width/2.0 - 20.0, height/2.0 - 20.0, 40.0, 40.0);
23   cairo_stroke (cr);
24 
25   /* Write the surface to a png file and clean up cairo and surface. */
26   cairo_surface_write_to_png (surface, "rectangle.png");
27   cairo_destroy (cr);
28   cairo_surface_destroy (surface);
29 
30   return 0;
31 }
~~~

- 1: Includes the header file of cairo.
- 12: `cairo_image_surface_create` creates an image surface.
`CAIRO_FORMAT_RGB24` is a constant which means that each pixel has red, green and blue data.
Each data has 8 bit quantity.
Modern displays have this type of color depth.
Width and height are pixels and given as integers.
- 13: Creates cairo context.
The surface given as an argument will be the destination of the context.
- 17: `cairo_set_source_rgb` creates a source pattern, which is a solid white paint.
The second to fourth argument is red, green and blue color depth respectively.
Their type is float and the values are between zero and one.
(0,0,0) is black and (1,1,1) is white.
- 18: `cairo_paint` copies everywhere in the source to destination.
The destination is filled with white pixels by this command.
- 20: Sets the source color to black.
- 21: `cairo_set_line_width` set the width of lines.
In this case, the line width is set to two pixels.
(It is because the transformation is identity.
If the transformation isn't identity, for example scaling with the factor three, the actual width in destination will be six (2x3=6) pixels.)
- 22: Draws a rectangle (square).
The top-left coordinate is (width/2.0-20.0, height/2.0-20.0) and the width and height have the same length 40.0.
- 23: `cairo_stroke` transfer the source to destination through the rectangle in mask.
- 26: Outputs the image to a png file `rectangle.png`.
- 27: Destroys the context. At the same time the source is destroyed.
- 28: Destroys the destination surface.

To compile this, type the following.

    $ gcc `pkg-config --cflags cairo` cairo.c `pkg-config --libs cairo`

![rectangle.png](../src/misc/rectangle.png)

There are lots of documentations in [Cairo's website](https://www.cairographics.org/).
If you aren't familiar with cairo, it is strongly recommended to read the [tutorial](https://www.cairographics.org/tutorial/) in the website.

## GtkDrawingArea

The following is a very simple example.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 draw_function (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
 5   cairo_set_source_rgb (cr, 1.0, 1.0, 1.0); /* whilte */
 6   cairo_paint (cr);
 7   cairo_set_line_width (cr, 2.0);
 8   cairo_set_source_rgb (cr, 0.0, 0.0, 0.0); /* black */
 9   cairo_rectangle (cr, width/2.0 - 20.0, height/2.0 - 20.0, 40.0, 40.0);
10   cairo_stroke (cr);
11 }
12 
13 static void
14 on_activate (GApplication *app, gpointer user_data) {
15   GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));
16   GtkWidget *area = gtk_drawing_area_new ();
17 
18   gtk_window_set_title (GTK_WINDOW (win), "da1");
19   /* Set initial size of width and height */
20   gtk_drawing_area_set_content_width (GTK_DRAWING_AREA (area), 100);
21   gtk_drawing_area_set_content_height (GTK_DRAWING_AREA (area), 100);
22   gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (area), draw_function, NULL, NULL);
23   gtk_window_set_child (GTK_WINDOW (win), area);
24 
25   gtk_widget_show (win);
26 }
27 
28 int
29 main (int argc, char **argv) {
30   GtkApplication *app;
31   int stat;
32 
33   app = gtk_application_new ("com.github.ToshioCP.da1", G_APPLICATION_FLAGS_NONE);
34   g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
35   stat =g_application_run (G_APPLICATION (app), argc, argv);
36   g_object_unref (app);
37   return stat;
38 }
39 
~~~

The function `main` is almost same as before.
The two functions `on_activate` and `draw_function` is important in this example.

- 16: Generates a GtkDrawingArea object.
- 20,21: Sets the width and height of the contents of the GtkDrawingArea widget.
These width and height is the size of the destination surface of the cairo context provided by the widget.
- 22: Sets a drawing function of the widget.
GtkDrawingArea widget uses the function to draw the contents of itself whenever its necessary.
For example, when a user drag a mouse pointer and resize a top level window, GtkDrawingArea also changes the size.
Then, the whole window needs to be redrawn.

The drawing function has five parameters.

    void drawing_function (GtkDrawingArea *drawing_area, cairo_t *cr, int width, int height,
                           gpointer user_data);

The first parameter is the GtkDrawingArea widget which calls the drawing function.
However, you can't change any properties, for example `content-width` or `content-height`, in this function.
The second parameter is a cairo context given by the widget.
The destination surface of the context is connected to the contents of the widget.
What you draw to this surface will appear in the widget on the screen.
The third and fourth parameters are the size of the destination surface.

- 3-11: The drawing function.
- 4-5: Sets the source to be white and paint the destination white.
- 7: Sets the line width to be 2.
- 8: Sets the source to be black.
- 9: Adds a rectangle to the mask.
- 10: Draws the rectangle with black color to the destination.

Compile and run it, then a window with a black rectangle (square) appears.
Try resizing the window.
The square always appears at the center of the window because the drawing function is invoked every moment the window is resized.

![Square in the window](../image/da1.png)


Up: [Readme.md](../Readme.md),  Prev: [Section 19](sec19.md), Next: [Section 21](sec21.md)
