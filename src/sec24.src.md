# Combine GtkDrawingArea and TfeTextView

Now, we will make a new application which has GtkDrawingArea and TfeTextView in it.
Its name is "color".
If you write a name of a color in TfeTextView and click on the `run` button, then the color of GtkDrawingArea changes to the color given by you.

![color](../image/color.png){width=7.0cm height=5.13cm}

The following colors are available.
(without new line charactor)

- white
- black
- red
- green
- blue

In addition the following two options are also available.

- light: Make the color of the drawing area lighter.
- dark: Make the color of the drawing area darker.

This application can only do very simple things.
However, it tells us that if we add powerful parser to it, we will be able to make it more efficient.
I want to show it to you in the later section by making a turtle graphics language like Logo program language.

In this section, we focus on how to bind the two objects.

## Color.ui and color.gresource.xml

First, We need to make the ui file of the widgets.
Title bar, four buttons in the tool bar, textview and drawing area.
The ui file is as follows.

@@@include
color/color.ui
@@@

- 10-53: The horizontal box `boxh1` makes a tool bar which has four buttons, `Run`, `Open`, `Save` and `Close`.
This is similar to the `tfe` text editor in [Section 9](sec9.src.md).
There are two differences.
`Run` button replaces `New` button.
A signal element is added to each button object.
It has "name" attribute which is a signal name and "handler" attribute which is the name of its signal handler.
Options "-WI, --export-dynamic" CFLAG is necessary when you compile the application.
You can achieve this by adding "export_dynamic: true" argument to the executable function in `meson.build`.
And be careful that the handler must be defined without 'static' class.
- 54-76: The horizontal box `boxh2` includes GtkScrolledWindow and GtkDrawingArea.
GtkBox has "homogeneous property" with TRUE value, so the two children have the same width in the box.
TfeTextView is a child of GtkScrolledWindow.

The xml file for the resource compiler is almost same as before.
Just substitute "color" for "tfe".

@@@include
color/color.gresource.xml
@@@

## Drawing function and surface

The main point of this program is a drawing function.

@@@include
color/colorapplication.c draw_func
@@@

The `surface` variable in line 3 is a static variable.

~~~C
static cairo_surface_t *surface = NULL;
~~~

The drawing function just copies the `surface` to its own surface with the `cairo_paint` function.
The surface (pointed by the static variable `surface`) is built by the `run` function.

@@@include
color/colorapplication.c run
@@@

- 9-10: Gets the string in the GtkTextBuffer and inserts it to `contents`.
- 11: If the variable `surface` points a surface instance, it is painted as follows.
- 12- 30: The source is set based on the string `contents` and copied to the surface with `cairo_paint`.
- 24,26: Alpha channel is used in "light" and "dark" procedure.

The drawing area just reflects the `surface`.
But one problem is resizing.
If a user resizes the main window, the drawing area is also resized.
It makes size difference between the surface and the drawing area.
So, the surface needs to be resized to fit the drawing area.

It is accomplished by connecting the "resize" signal on the drawing area to a handler.

~~~C
g_signal_connect (GTK_DRAWING_AREA (da), "resize", G_CALLBACK (resize_cb), NULL);
~~~

The handler is as follows.

@@@include
color/colorapplication.c resize_cb
@@@

If the variable `surface` sets a surface instance, it is destroyed.
A new surface is created and its size fits the drawing area.
The surface is assigned to the variable `surface`.
The function `run` is called and the surface is colored.

The signal is emitted when:

- The drawing area is realized (it appears on the display).
- It is changed (resized) while realized

So, the first surface is created when it is realized.

## Colorapplication.c

This is the main file.

- Builds widgets by GtkBuilder.
- Sets a drawing function for GtkDrawingArea.
And connects a handler to the "resize" signal on the GtkDrawingArea instance.
- Implements each call back function.
Particularly, `Run` signal handler is the point in this program.

The following is `colorapplication.c`.

@@@include
color/colorapplication.c
@@@

- 4-8: Win, tv, da and surface are defined as static variables.
- 10-42: Run function.
- 44-63: Handlers for button signals.
- 65-71: Resize handler.
- 73-79: Drawing function.
- 81-84: Application activate handler.
It just shows the main window.
- 86-105: Application startup handler.
- 92- 97: It builds widgets according to the ui resource.
The static variables win, tv and da are assigned instances.
- 98: Connects "resize" signal and a handler.
- 99: Drawing function is set.
- 101-104: CSS for textview padding is set.
- 107-111: Application shutdown handler.
If there exists a surface instance, it will be destroyed.
- 116-129: A function `main`.
It creates a new application instance.
And connects three signals startup, shutdown and activate to their handlers.
It runs the application.
It releases the reference to the application and returns with `stat` value.

## Meson.build

This file is almost same as before.
An argument "export_dynamic: true" is added to executable function.

@@@include
color/meson.build
@@@

## Build and try

Type the following to compile the program.

    $ meson _build
    $ ninja -C _build

The application is made in `_build` directory.
Type the following to execute it.

    $ _build/color

Type "red", "green", "blue", "white", black", "light" or "dark" in the TfeTextView.
No new line charactor is needed.
Then, click on the `Run` button.
Make sure the color of GtkDrawingArea changes.

In this program TfeTextView is used to change the color.
You can use buttons or menus instead of textview.
Probably it is more appropriate.
Using textview is unnatural.
It is a good practice to make such application by yourself.
