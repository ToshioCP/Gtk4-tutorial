Up: [Readme.md](../Readme.md),  Prev: [Section 22](sec22.md), Next: [Section 24](sec24.md)

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

~~~C
  1 static void
  2 draw_clock (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
  3 
  4     // Scale to unit square and translate (0, 0) to be (0.5, 0.5), i.e.
  5     // the center of the window
  6     cairo_scale(cr, width, height);
  7     cairo_translate(cr, 0.5, 0.5);
  8 
  9     // Set the line width and save the cairo drawing state.
 10     cairo_set_line_width(cr, m_line_width);
 11     cairo_save(cr);
 12 
 13     // Set the background to a slightly transparent green.
 14     cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9);   // green
 15     cairo_paint(cr);
 16 
 17     // Resore back to precious drawing state and draw the circular path
 18     // representing the clockface. Save this state (including the path) so we
 19     // can reuse it.
 20     cairo_restore(cr);
 21     cairo_arc(cr, 0.0, 0.0, m_radius, 0.0, 2.0 * M_PI);
 22     cairo_save(cr);
 23 
 24     // Fill the clockface with white
 25     cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.8);
 26     cairo_fill_preserve(cr);
 27     // Restore the path, paint the outside of the clock face.
 28     cairo_restore(cr);
 29     cairo_stroke_preserve(cr);
 30     // Set the 'clip region' to the inside of the path (fill region).
 31     cairo_clip(cr);
 32 
 33     // Clock ticks
 34     for (int i = 0; i < 12; i++)
 35     {
 36         // Major tick size
 37         double inset = 0.05;
 38 
 39         // Save the graphics state, restore after drawing tick to maintain pen
 40         // size
 41         cairo_save(cr);
 42         cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
 43 
 44         // Minor ticks are shorter, and narrower.
 45         if(i % 3 != 0)
 46         {
 47             inset *= 0.8;
 48             cairo_set_line_width(cr, 0.03);
 49         }
 50 
 51         // Draw tick mark
 52         cairo_move_to(
 53             cr,
 54             (m_radius - inset) * cos (i * M_PI / 6.0),
 55             (m_radius - inset) * sin (i * M_PI / 6.0));
 56         cairo_line_to(
 57             cr,
 58             m_radius * cos (i * M_PI / 6.0),
 59             m_radius * sin (i * M_PI / 6.0));
 60         cairo_stroke(cr);
 61         cairo_restore(cr); /* stack-pen-size */
 62     }
 63 
 64     // Draw the analog hands
 65 
 66     // Get the current Unix time, convert to the local time and break into time
 67     // structure to read various time parts.
 68     time_t rawtime;
 69     time(&rawtime);
 70     struct tm * timeinfo = localtime (&rawtime);
 71 
 72     // Calculate the angles of the hands of our clock
 73     double hours   = timeinfo->tm_hour * M_PI / 6.0;
 74     double minutes = timeinfo->tm_min * M_PI / 30.0;
 75     double seconds = timeinfo->tm_sec * M_PI / 30.0;
 76 
 77     // Save the graphics state
 78     cairo_save(cr);
 79     cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
 80 
 81     cairo_save(cr);
 82 
 83     // Draw the seconds hand
 84     cairo_set_line_width(cr, m_line_width / 3.0);
 85     cairo_set_source_rgba(cr, 0.7, 0.7, 0.7, 0.8);   // gray
 86     cairo_move_to(cr, 0.0, 0.0);
 87     cairo_line_to(cr,
 88                   sin(seconds) * (m_radius * 0.9),
 89                   -cos(seconds) * (m_radius * 0.9));
 90     cairo_stroke(cr);
 91     cairo_restore(cr);
 92 
 93     // Draw the minutes hand
 94     cairo_set_source_rgba(cr, 0.117, 0.337, 0.612, 0.9);   // blue
 95     cairo_move_to(cr, 0, 0);
 96     cairo_line_to(cr,
 97                   sin(minutes + seconds / 60) * (m_radius * 0.8),
 98                   -cos(minutes + seconds / 60) * (m_radius * 0.8));
 99     cairo_stroke(cr);
100 
101     // draw the hours hand
102     cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9);   // green
103     cairo_move_to(cr, 0.0, 0.0);
104     cairo_line_to(cr,
105                   sin(hours + minutes / 12.0) * (m_radius * 0.5),
106                   -cos(hours + minutes / 12.0) * (m_radius * 0.5));
107     cairo_stroke(cr);
108     cairo_restore(cr);
109 
110     // Draw a little dot in the middle
111     cairo_arc(cr, 0.0, 0.0, m_line_width / 3.0, 0.0, 2.0 * M_PI);
112     cairo_fill(cr);
113 }
~~~

In order for the clock to be drawn, the drawing function `draw_clock()` needs
to be registered with GTK4. This is done in the `app_activate()` function (on line 24).

Whenever the application needs to redraw the GtkDrawingArea, it will now call `draw_clock()`.

There is still a problem though. In order to animate the clock we need to also
tell the application that the clock needs to be redrawn every second. This
process starts by registering (on the next line, line 15) a timeout function
with `g_timeout_add()` that will wakeup and run another function `time_handler`,
every second (or 1000ms).

~~~C
 1 static void
 2 app_activate (GApplication *app, gpointer user_data) {
 3     GtkWidget *win;
 4     GtkWidget *clock;
 5     GtkBuilder *build;
 6 
 7     build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfc/tfc.ui");
 8     win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
 9     gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
10 
11     clock = GTK_WIDGET (gtk_builder_get_object (build, "clock"));
12     g_object_unref(build);
13 
14     gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (clock), draw_clock, NULL, NULL);
15     g_timeout_add(1000, (GSourceFunc) time_handler, (gpointer) clock);
16     gtk_widget_show(win);
17 
18 }
~~~

Our `time_handler()` function is very simple, as it just calls
`gtk_widget_queue_draw()` which schedules a redraw of the widget.

~~~C
1 gboolean
2 time_handler(GtkWidget* widget) {
3     gtk_widget_queue_draw(widget);
4 
5     return TRUE;
6 }
~~~

.. and that is all there is to it. If you compile and run the example you will
get a ticking analog clock.

If you get this working, you can try modifying some of the code in
`draw_clock()` to tweak the application (such as change the color or size and
length of the hands) or even add text, or create a digital clock.

## The Complete code

You can find the source files in the `tfc` directory. it can be compiled with `./comp tfc`.

`tfc.c`

~~~C
  1 #include <gtk/gtk.h>
  2 #include <math.h>
  3 #include <time.h>
  4 
  5 float m_radius     = 0.42;
  6 float m_line_width = 0.05;
  7 
  8 static void
  9 draw_clock (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
 10 
 11     // Scale to unit square and translate (0, 0) to be (0.5, 0.5), i.e.
 12     // the center of the window
 13     cairo_scale(cr, width, height);
 14     cairo_translate(cr, 0.5, 0.5);
 15 
 16     // Set the line width and save the cairo drawing state.
 17     cairo_set_line_width(cr, m_line_width);
 18     cairo_save(cr);
 19 
 20     // Set the background to a slightly transparent green.
 21     cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9);   // green
 22     cairo_paint(cr);
 23 
 24     // Resore back to precious drawing state and draw the circular path
 25     // representing the clockface. Save this state (including the path) so we
 26     // can reuse it.
 27     cairo_restore(cr);
 28     cairo_arc(cr, 0.0, 0.0, m_radius, 0.0, 2.0 * M_PI);
 29     cairo_save(cr);
 30 
 31     // Fill the clockface with white
 32     cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.8);
 33     cairo_fill_preserve(cr);
 34     // Restore the path, paint the outside of the clock face.
 35     cairo_restore(cr);
 36     cairo_stroke_preserve(cr);
 37     // Set the 'clip region' to the inside of the path (fill region).
 38     cairo_clip(cr);
 39 
 40     // Clock ticks
 41     for (int i = 0; i < 12; i++)
 42     {
 43         // Major tick size
 44         double inset = 0.05;
 45 
 46         // Save the graphics state, restore after drawing tick to maintain pen
 47         // size
 48         cairo_save(cr);
 49         cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
 50 
 51         // Minor ticks are shorter, and narrower.
 52         if(i % 3 != 0)
 53         {
 54             inset *= 0.8;
 55             cairo_set_line_width(cr, 0.03);
 56         }
 57 
 58         // Draw tick mark
 59         cairo_move_to(
 60             cr,
 61             (m_radius - inset) * cos (i * M_PI / 6.0),
 62             (m_radius - inset) * sin (i * M_PI / 6.0));
 63         cairo_line_to(
 64             cr,
 65             m_radius * cos (i * M_PI / 6.0),
 66             m_radius * sin (i * M_PI / 6.0));
 67         cairo_stroke(cr);
 68         cairo_restore(cr); /* stack-pen-size */
 69     }
 70 
 71     // Draw the analog hands
 72 
 73     // Get the current Unix time, convert to the local time and break into time
 74     // structure to read various time parts.
 75     time_t rawtime;
 76     time(&rawtime);
 77     struct tm * timeinfo = localtime (&rawtime);
 78 
 79     // Calculate the angles of the hands of our clock
 80     double hours   = timeinfo->tm_hour * M_PI / 6.0;
 81     double minutes = timeinfo->tm_min * M_PI / 30.0;
 82     double seconds = timeinfo->tm_sec * M_PI / 30.0;
 83 
 84     // Save the graphics state
 85     cairo_save(cr);
 86     cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
 87 
 88     cairo_save(cr);
 89 
 90     // Draw the seconds hand
 91     cairo_set_line_width(cr, m_line_width / 3.0);
 92     cairo_set_source_rgba(cr, 0.7, 0.7, 0.7, 0.8);   // gray
 93     cairo_move_to(cr, 0.0, 0.0);
 94     cairo_line_to(cr,
 95                   sin(seconds) * (m_radius * 0.9),
 96                   -cos(seconds) * (m_radius * 0.9));
 97     cairo_stroke(cr);
 98     cairo_restore(cr);
 99 
100     // Draw the minutes hand
101     cairo_set_source_rgba(cr, 0.117, 0.337, 0.612, 0.9);   // blue
102     cairo_move_to(cr, 0, 0);
103     cairo_line_to(cr,
104                   sin(minutes + seconds / 60) * (m_radius * 0.8),
105                   -cos(minutes + seconds / 60) * (m_radius * 0.8));
106     cairo_stroke(cr);
107 
108     // draw the hours hand
109     cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9);   // green
110     cairo_move_to(cr, 0.0, 0.0);
111     cairo_line_to(cr,
112                   sin(hours + minutes / 12.0) * (m_radius * 0.5),
113                   -cos(hours + minutes / 12.0) * (m_radius * 0.5));
114     cairo_stroke(cr);
115     cairo_restore(cr);
116 
117     // Draw a little dot in the middle
118     cairo_arc(cr, 0.0, 0.0, m_line_width / 3.0, 0.0, 2.0 * M_PI);
119     cairo_fill(cr);
120 }
121 
122 
123 gboolean
124 time_handler(GtkWidget* widget) {
125     gtk_widget_queue_draw(widget);
126 
127     return TRUE;
128 }
129 
130 
131 static void
132 app_activate (GApplication *app, gpointer user_data) {
133     GtkWidget *win;
134     GtkWidget *clock;
135     GtkBuilder *build;
136 
137     build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfc/tfc.ui");
138     win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
139     gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
140 
141     clock = GTK_WIDGET (gtk_builder_get_object (build, "clock"));
142     g_object_unref(build);
143 
144     gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (clock), draw_clock, NULL, NULL);
145     g_timeout_add(1000, (GSourceFunc) time_handler, (gpointer) clock);
146     gtk_widget_show(win);
147 
148 }
149 
150 static void
151 app_open (GApplication *app, GFile **files, gint n_files, gchar *hint, gpointer user_data) {
152     app_activate(app,user_data);
153 }
154 
155 int
156 main (int argc, char **argv) {
157     GtkApplication *app;
158     int stat;
159 
160     app = gtk_application_new ("com.github.ToshioCP.tfc", G_APPLICATION_HANDLES_OPEN);
161     g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
162     g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
163     stat = g_application_run (G_APPLICATION (app), argc, argv);
164     g_object_unref (app);
165     return stat;
166 }
~~~

`tfc.ui`

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <interface>
 3   <object class="GtkApplicationWindow" id="win">
 4     <property name="title">Clock</property>
 5     <property name="default-width">200</property>
 6     <property name="default-height">200</property>
 7     <child>
 8       <object class="GtkDrawingArea" id="clock">
 9         <property name="hexpand">TRUE</property>
10         <property name="vexpand">TRUE</property>
11       </object>
12     </child>
13   </object>
14 </interface>
~~~

`tfc.gresource.xml`

~~~xml
1 <?xml version="1.0" encoding="UTF-8"?>
2 <gresources>
3   <gresource prefix="/com/github/ToshioCP/tfc">
4     <file>tfc.ui</file>
5   </gresource>
6 </gresources>
~~~

`comp`

~~~
1 glib-compile-resources $1.gresource.xml --target=$1.gresource.c --generate-source
2 gcc `pkg-config --cflags gtk4` $1.gresource.c $1.c `pkg-config --libs gtk4` -lm
~~~

Up: [Readme.md](../Readme.md),  Prev: [Section 22](sec22.md), Next: [Section 24](sec24.md)
