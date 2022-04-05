# Periodic Events

This chapter was written by Paul Schulz <paul@mawsonlakes.org>.

## How do we create an animation?

In this section we will continue to build on our previous work. We will create
an analog clock application. By adding a function which periodically redraws
GtkDrawingArea, the clock will be able to continuously display the time.

The application uses a compiled in 'resource' file, so if the GTK4 libraries and
their dependencies are installed and available, the application will run from
anywhere.

The program also makes use of some standard mathematical and time handling
functions.

The clocks mechanics were taken from a Cairo drawing example, using gtkmm4, which can be found
[here](https://developer-old.gnome.org/gtkmm-tutorial/stable/sec-drawing-clock-example.html.en).

The complete code is at the end.

## Drawing the clock face, hour, minute and second hands

The `draw_clock()` function does all the work. See the in-file comments for an
explanation of how the Cairo drawing works.

For a detailed reference of what each of the Cairo functions does see the
[cairo_t reference](https://www.cairographics.org/manual/cairo-cairo-t.html).

@@@include
tfc/tfc.c draw_clock
@@@

In order for the clock to be drawn, the drawing function `draw_clock()` needs
to be registered with GTK4. This is done in the `app_activate()` function (on line 24).

Whenever the application needs to redraw the GtkDrawingArea, it will now call `draw_clock()`.

There is still a problem though. In order to animate the clock we need to also
tell the application that the clock needs to be redrawn every second. This
process starts by registering (on the next line, line 15) a timeout function
with `g_timeout_add()` that will wakeup and run another function `time_handler`,
every second (or 1000ms).

@@@include
tfc/tfc.c app_activate
@@@

Our `time_handler()` function is very simple, as it just calls
`gtk_widget_queue_draw()` which schedules a redraw of the widget.

@@@include
tfc/tfc.c time_handler
@@@

.. and that is all there is to it. If you compile and run the example you will
get a ticking analog clock.

If you get this working, you can try modifying some of the code in
`draw_clock()` to tweak the application (such as change the color or size and
length of the hands) or even add text, or create a digital clock.

## The Complete code

You can find the source files in the `tfc` directory. it can be compiled with `./comp tfc`.

`tfc.c`

@@@include
tfc/tfc.c
@@@

`tfc.ui`

@@@include
tfc/tfc.ui
@@@

`tfc.gresource.xml`

@@@include
tfc/tfc.gresource.xml
@@@

`comp`

@@@include
tfc/comp
@@@
