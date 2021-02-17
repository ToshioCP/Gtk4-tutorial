Up: [Readme.md](../Readme.md),  Prev: [Section 20](sec20.md)

# Combine GtkDrawingArea and TfeTextView

Now, we will make a new application which has GtkDrawingArea and TfeTextView in it.
Its name is "color".
If you write a color in TfeTextView and click on the `run` button, then the color of GtkDrawingArea changes to the color given by you.

![color](../image/color.png)

The following colors are available.

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
The image in the previous subsection gives us the structure of the widgets.
Title bar, four buttons in the tool bar and two widgets textview and drawing area.
The ui file is as follows.

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <interface>
 3   <object class="GtkApplicationWindow" id="win">
 4     <property name="title">color changer</property>
 5     <property name="default-width">600</property>
 6     <property name="default-height">400</property>
 7     <child>
 8       <object class="GtkBox" id="boxv">
 9         <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
10         <child>
11           <object class="GtkBox" id="boxh1">
12             <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
13             <child>
14               <object class="GtkLabel" id="dmy1">
15                 <property name="width-chars">10</property>
16               </object>
17             </child>
18             <child>
19               <object class="GtkButton" id="btnr">
20                 <property name="label">Run</property>
21                 <signal name="clicked" handler="run_cb"></signal>
22               </object>
23             </child>
24             <child>
25               <object class="GtkButton" id="btno">
26                 <property name="label">Open</property>
27                 <signal name="clicked" handler="open_cb"></signal>
28               </object>
29             </child>
30             <child>
31               <object class="GtkLabel" id="dmy2">
32                 <property name="hexpand">TRUE</property>
33               </object>
34             </child>
35             <child>
36               <object class="GtkButton" id="btns">
37                 <property name="label">Save</property>
38                 <signal name="clicked" handler="save_cb"></signal>
39               </object>
40             </child>
41             <child>
42               <object class="GtkButton" id="btnc">
43                 <property name="label">Close</property>
44                 <signal name="clicked" handler="close_cb"></signal>
45               </object>
46             </child>
47             <child>
48               <object class="GtkLabel" id="dmy3">
49                 <property name="width-chars">10</property>
50               </object>
51             </child>
52           </object>
53         </child>
54         <child>
55           <object class="GtkBox" id="boxh2">
56             <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
57             <property name="homogeneous">TRUE</property>
58             <child>
59               <object class="GtkScrolledWindow" id="scr">
60                 <property name="hexpand">TRUE</property>
61                 <property name="vexpand">TRUE</property>
62                 <child>
63                   <object class="TfeTextView" id="tv">
64                     <property name="wrap-mode">GTK_WRAP_WORD_CHAR</property>
65                   </object>
66                 </child>
67               </object>
68             </child>
69             <child>
70               <object class="GtkDrawingArea" id="da">
71                 <property name="hexpand">TRUE</property>
72                 <property name="vexpand">TRUE</property>
73               </object>
74             </child>
75           </object>
76         </child>
77       </object>
78     </child>
79   </object>
80 </interface>
~~~

- 10-53: This part describes the tool bar which has four buttons, `Run`, `Open`, `Save` and `Close`.
This is similar to the toolbar of tfe text editor in [Section 8](sec8.md).
There are two differences.
`Run` button replaces `New` button.
A signal element is added to each button object.
It has "name" attribute which is a signal name and "handler" attribute which is the name of its signal handler function.
Options "-WI, --export-dynamic" CFLAG is necessary when you compile the application.
You can achieve this by adding "export_dynamic: true" argument to executable function in `meson.build`.
And be careful that the handler must be defined without 'static' class.
- 54-76: Puts GtkScrolledWindow and GtkDrawingArea into GtkBox.
GtkBox has "homogeneous property" with TRUE value, so the two children have the same width in the box.
TfeTextView is a child of GtkScrolledWindow.

The xml file for the resource compiler is almost same as before.
Just substitute "color" for "tfe".

~~~xml
1 <?xml version="1.0" encoding="UTF-8"?>
2 <gresources>
3   <gresource prefix="/com/github/ToshioCP/color">
4     <file>color.ui</file>
5   </gresource>
6 </gresources>
~~~

## Tfetextview.h, tfetextview.c and color.h

First two files are the same as before.
Color.h just includes tfetextview.h.

~~~C
1 #include <gtk/gtk.h>
2 
3 #include "../tfetextview/tfetextview.h"
~~~

## Colorapplication.c

This is the main file.
It deals with:

- Building widgets by GtkBuilder.
- Seting a drawing function of GtkDrawingArea.
And connecting a handler to "resize" signal on GtkDrawingArea.
- Implementing each call back functions.
Particularly, `Run` signal handler is the point in this program.

The following is `colorapplication.c`.

~~~C
  1 #include "color.h"
  2 
  3 static GtkWidget *win;
  4 static GtkWidget *tv;
  5 static GtkWidget *da;
  6 
  7 static cairo_surface_t *surface = NULL;
  8 
  9 static void
 10 run (void) {
 11   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
 12   GtkTextIter start_iter;
 13   GtkTextIter end_iter;
 14   char *contents;
 15   cairo_t *cr;
 16 
 17   gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
 18   contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
 19   if (surface) {
 20     cr = cairo_create (surface);
 21     if (g_strcmp0 ("red", contents) == 0)
 22       cairo_set_source_rgb (cr, 1, 0, 0);
 23     else if (g_strcmp0 ("green", contents) == 0)
 24       cairo_set_source_rgb (cr, 0, 1, 0);
 25     else if (g_strcmp0 ("blue", contents) == 0)
 26       cairo_set_source_rgb (cr, 0, 0, 1);
 27     else if (g_strcmp0 ("white", contents) == 0)
 28       cairo_set_source_rgb (cr, 1, 1, 1);
 29     else if (g_strcmp0 ("black", contents) == 0)
 30       cairo_set_source_rgb (cr, 0, 0, 0);
 31     else if (g_strcmp0 ("light", contents) == 0)
 32       cairo_set_source_rgba (cr, 1, 1, 1, 0.5);
 33     else if (g_strcmp0 ("dark", contents) == 0)
 34       cairo_set_source_rgba (cr, 0, 0, 0, 0.5);
 35     else
 36       cairo_set_source_surface (cr, surface, 0, 0);
 37     cairo_paint (cr);
 38     cairo_destroy (cr);
 39   }
 40 }
 41 
 42 void
 43 run_cb (GtkWidget *btnr) {
 44   run ();
 45   gtk_widget_queue_draw (GTK_WIDGET (da));
 46 }
 47 
 48 void
 49 open_cb (GtkWidget *btno) {
 50   tfe_text_view_open (TFE_TEXT_VIEW (tv), win);
 51 }
 52 
 53 void
 54 save_cb (GtkWidget *btns) {
 55   tfe_text_view_save (TFE_TEXT_VIEW (tv));
 56 }
 57 
 58 void
 59 close_cb (GtkWidget *btnc) {
 60   if (surface)
 61     cairo_surface_destroy (surface);
 62   gtk_window_destroy (GTK_WINDOW (win));
 63 }
 64 
 65 static void
 66 resize_cb (GtkDrawingArea *drawing_area, int width, int height, gpointer user_data) {
 67   if (surface)
 68     cairo_surface_destroy (surface);
 69   surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
 70   run ();
 71 }
 72 
 73 static void
 74 draw_func (GtkDrawingArea *drawing_area, cairo_t *cr, int width, int height, gpointer user_data) {
 75   if (surface) {
 76     cairo_set_source_surface (cr, surface, 0, 0);
 77     cairo_paint (cr);
 78   }
 79 }
 80 
 81 static void
 82 activate (GApplication *application) {
 83   gtk_widget_show (win);
 84 }
 85 
 86 static void
 87 startup (GApplication *application) {
 88   GtkApplication *app = GTK_APPLICATION (application);
 89   GtkBuilder *build;
 90 
 91   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/color/color.ui");
 92   win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
 93   gtk_window_set_application (GTK_WINDOW (win), app);
 94   tv = GTK_WIDGET (gtk_builder_get_object (build, "tv"));
 95   da = GTK_WIDGET (gtk_builder_get_object (build, "da"));
 96   g_object_unref(build);
 97   g_signal_connect (GTK_DRAWING_AREA (da), "resize", G_CALLBACK (resize_cb), NULL);
 98   gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (da), draw_func, NULL, NULL);
 99 
100 GdkDisplay *display;
101 
102   display = gtk_widget_get_display (GTK_WIDGET (win));
103   GtkCssProvider *provider = gtk_css_provider_new ();
104   gtk_css_provider_load_from_data (provider, "textview {padding: 10px; font-family: monospace; font-size: 12pt;}", -1);
105   gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
106 }
107 
108 int
109 main (int argc, char **argv) {
110   GtkApplication *app;
111   int stat;
112 
113   app = gtk_application_new ("com.github.ToshioCP.color", G_APPLICATION_FLAGS_NONE);
114 
115   g_signal_connect (app, "startup", G_CALLBACK (startup), NULL);
116   g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
117 
118   stat =g_application_run (G_APPLICATION (app), argc, argv);
119   g_object_unref (app);
120   return stat;
121 }
122 
~~~

- 108-121: The function `main` is almost same as before but there are some differences.
The application ID is "com.github.ToshioCP.color".
`G_APPLICATION_FLAGS_NONE` is specified so no open signal handler is necessary.
- 86-106: Startup handler.
- 91-96: Builds widgets.
The pointers of the top window, TfeTextView and GtkDrawingArea objects are stored to static variables `win`, `tv` and `da` respectively.
This is because these objects are often used in handlers.
They never be rewritten so they're thread safe.
- 97: connects "resize" signal and the handler.
- 98: sets the drawing function.
- 81-84: Activates handler, which just shows the widgets.
- 73-79: The drawing function.
It just copies `surface` to destination.
- 65-71: Resize handler.
Re-creates the surface to fit the width and height of the drawing area and paints by calling the function `run`.
- 58-63: Closes the handler.
It destroys `surface` if it exists.
Then it destroys the top window and quits the application.
- 48-56: Open and save handler.
They just call the corresponding functions of TfeTextView.
- 42-46: Run handler.
It calls run function to paint the surface.
After that `gtk_widget_queue_draw` is called.
This fhunction adds the widget (GtkDrawingArea) to the queue to be redrawn.
It is important to know that the drawing function is called when it is necessary.
For example, when another window is moved and uncovers part of the widget, or when the window containing it is resized.
But repaint of `surface` is not automatically notified to gtk.
Therefore, you need to call `gtk_widget_queue_draw` to redraw the widget.
- 9-40: Run function paints the surface.
First, it gets the contents of GtkTextBuffer.
Then it compares it to "red", "green" and so on.
If it matches the color, then the surface is painted the color.
If it matches "light" or "dark", then the color of the surface is lightened or darkened respectively.
Alpha channel is used.

## Meson.build

This file is almost same as before.
An argument "export_dynamic: true" is added to executable function.

~~~meson
 1 project('color', 'c')
 2 
 3 gtkdep = dependency('gtk4')
 4 
 5 gnome=import('gnome')
 6 resources = gnome.compile_resources('resources','color.gresource.xml')
 7 
 8 sourcefiles=files('colorapplication.c', '../tfetextview/tfetextview.c')
 9 
10 executable('color', sourcefiles, resources, dependencies: gtkdep, export_dynamic: true)
~~~

## Compile and execute it

First you need to export some variables (refer to [Section 2](sec2.md)).

    $ . env.sh

Then type the following to compile it.

    $ meson _build
    $ ninja -C _build

The application is made in `_build` directory.
Type the following to execute it.

    $ _build/color

Type "red", "green", "blue", "white", black", "light" or "dark" in the TfeTextView.
Then, click on `Run` button.
Make sure the color of GtkDrawingArea changes.

In this program TfeTextView is used to change the color.
You can use buttons or menus instead of textview.
Probably it is more appropriate.
Using textview is unnatural.
It is a good practice to make such application by yourself.

Up: [Readme.md](../Readme.md),  Prev: [Section 20](sec20.md)
