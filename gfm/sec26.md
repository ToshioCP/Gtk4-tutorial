Up: [README.md](../README.md),  Prev: [Section 25](sec25.md), Next: [Section 27](sec27.md)

# Combine GtkDrawingArea and TfeTextView

Now, we will make a new application which has GtkDrawingArea and TfeTextView in it.
Its name is "color".
If you write a name of a color in TfeTextView and click on the `run` button, then the color of GtkDrawingArea changes to the color given by you.

![color](../image/color.png)

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

- 10-53: The horizontal box `boxh1` makes a tool bar which has four buttons, `Run`, `Open`, `Save` and `Close`.
This is similar to the `tfe` text editor in [Section 9](sec9.md).
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

~~~xml
1 <?xml version="1.0" encoding="UTF-8"?>
2 <gresources>
3   <gresource prefix="/com/github/ToshioCP/color">
4     <file>color.ui</file>
5   </gresource>
6 </gresources>
~~~

## Drawing function and surface

The main point of this program is a drawing function.

~~~C
1 static void
2 draw_func (GtkDrawingArea *drawing_area, cairo_t *cr, int width, int height, gpointer user_data) {
3   if (surface) {
4     cairo_set_source_surface (cr, surface, 0, 0);
5     cairo_paint (cr);
6   }
7 }
~~~

The `surface` variable in line 3 is a static variable.

~~~C
static cairo_surface_t *surface = NULL;
~~~

The drawing function just copies the `surface` to its own surface with the `cairo_paint` function.
The surface (pointed by the static variable `surface`) is built by the `run` function.

~~~C
 1 static void
 2 run (void) {
 3   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
 4   GtkTextIter start_iter;
 5   GtkTextIter end_iter;
 6   char *contents;
 7   cairo_t *cr;
 8 
 9   gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
10   contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
11   if (surface) {
12     cr = cairo_create (surface);
13     if (g_strcmp0 ("red", contents) == 0)
14       cairo_set_source_rgb (cr, 1, 0, 0);
15     else if (g_strcmp0 ("green", contents) == 0)
16       cairo_set_source_rgb (cr, 0, 1, 0);
17     else if (g_strcmp0 ("blue", contents) == 0)
18       cairo_set_source_rgb (cr, 0, 0, 1);
19     else if (g_strcmp0 ("white", contents) == 0)
20       cairo_set_source_rgb (cr, 1, 1, 1);
21     else if (g_strcmp0 ("black", contents) == 0)
22       cairo_set_source_rgb (cr, 0, 0, 0);
23     else if (g_strcmp0 ("light", contents) == 0)
24       cairo_set_source_rgba (cr, 1, 1, 1, 0.5);
25     else if (g_strcmp0 ("dark", contents) == 0)
26       cairo_set_source_rgba (cr, 0, 0, 0, 0.5);
27     else
28       cairo_set_source_surface (cr, surface, 0, 0);
29     cairo_paint (cr);
30     cairo_destroy (cr);
31   }
32   g_free (contents);
33 }
~~~

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

~~~C
1 static void
2 resize_cb (GtkDrawingArea *drawing_area, int width, int height, gpointer user_data) {
3   if (surface)
4     cairo_surface_destroy (surface);
5   surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
6   run ();
7 }
~~~

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

~~~C
  1 #include <gtk/gtk.h>
  2 #include "../tfetextview/tfetextview.h"
  3 
  4 static GtkWidget *win;
  5 static GtkWidget *tv;
  6 static GtkWidget *da;
  7 
  8 static cairo_surface_t *surface = NULL;
  9 
 10 static void
 11 run (void) {
 12   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
 13   GtkTextIter start_iter;
 14   GtkTextIter end_iter;
 15   char *contents;
 16   cairo_t *cr;
 17 
 18   gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
 19   contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
 20   if (surface) {
 21     cr = cairo_create (surface);
 22     if (g_strcmp0 ("red", contents) == 0)
 23       cairo_set_source_rgb (cr, 1, 0, 0);
 24     else if (g_strcmp0 ("green", contents) == 0)
 25       cairo_set_source_rgb (cr, 0, 1, 0);
 26     else if (g_strcmp0 ("blue", contents) == 0)
 27       cairo_set_source_rgb (cr, 0, 0, 1);
 28     else if (g_strcmp0 ("white", contents) == 0)
 29       cairo_set_source_rgb (cr, 1, 1, 1);
 30     else if (g_strcmp0 ("black", contents) == 0)
 31       cairo_set_source_rgb (cr, 0, 0, 0);
 32     else if (g_strcmp0 ("light", contents) == 0)
 33       cairo_set_source_rgba (cr, 1, 1, 1, 0.5);
 34     else if (g_strcmp0 ("dark", contents) == 0)
 35       cairo_set_source_rgba (cr, 0, 0, 0, 0.5);
 36     else
 37       cairo_set_source_surface (cr, surface, 0, 0);
 38     cairo_paint (cr);
 39     cairo_destroy (cr);
 40   }
 41   g_free (contents);
 42 }
 43 
 44 void
 45 run_cb (GtkWidget *btnr) {
 46   run ();
 47   gtk_widget_queue_draw (GTK_WIDGET (da));
 48 }
 49 
 50 void
 51 open_cb (GtkWidget *btno) {
 52   tfe_text_view_open (TFE_TEXT_VIEW (tv), GTK_WINDOW (win));
 53 }
 54 
 55 void
 56 save_cb (GtkWidget *btns) {
 57   tfe_text_view_save (TFE_TEXT_VIEW (tv));
 58 }
 59 
 60 void
 61 close_cb (GtkWidget *btnc) {
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
 82 app_activate (GApplication *application) {
 83   gtk_window_present (GTK_WINDOW (win));
 84 }
 85 
 86 static void
 87 app_startup (GApplication *application) {
 88   GtkApplication *app = GTK_APPLICATION (application);
 89   GtkBuilder *build;
 90   GdkDisplay *display;
 91 
 92   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/color/color.ui");
 93   win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
 94   gtk_window_set_application (GTK_WINDOW (win), app);
 95   tv = GTK_WIDGET (gtk_builder_get_object (build, "tv"));
 96   da = GTK_WIDGET (gtk_builder_get_object (build, "da"));
 97   g_object_unref(build);
 98   g_signal_connect (GTK_DRAWING_AREA (da), "resize", G_CALLBACK (resize_cb), NULL);
 99   gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (da), draw_func, NULL, NULL);
100 
101   display = gdk_display_get_default ();
102   GtkCssProvider *provider = gtk_css_provider_new ();
103   gtk_css_provider_load_from_data (provider, "textview {padding: 10px; font-family: monospace; font-size: 12pt;}", -1);
104   gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
105 }
106 
107 static void
108 app_shutdown (GApplication *application) {
109   if (surface)
110     cairo_surface_destroy (surface);
111 }
112 
113 #define APPLICATION_ID "com.github.ToshioCP.color"
114 
115 int
116 main (int argc, char **argv) {
117   GtkApplication *app;
118   int stat;
119 
120   app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
121 
122   g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
123   g_signal_connect (app, "shutdown", G_CALLBACK (app_shutdown), NULL);
124   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
125 
126   stat =g_application_run (G_APPLICATION (app), argc, argv);
127   g_object_unref (app);
128   return stat;
129 }
130 
~~~

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

Up: [README.md](../README.md),  Prev: [Section 25](sec25.md), Next: [Section 27](sec27.md)
