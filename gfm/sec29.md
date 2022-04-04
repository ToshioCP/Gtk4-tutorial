Up: [Readme.md](../Readme.md),  Prev: [Section 28](sec28.md)

# Periodic Events

## How do we create an animation?

In this section we will build on previous sections, and with the addition of a
function which periodically redraws a GtkDrawingArea, will create an analog
clock application.

The application uses a compiled in 'resource' file, so if the GTK4 libraries and
their dependencies are installed and available, the application will run.

The program also makes use of some mathematical and time handling functions.

The complete code is at the end.

## Drawing the clock face and hands

~~~C
 1 static void
 2 draw_clock (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
 3 
 4     // Scale to unit square and translate (0, 0) to be (0.5, 0.5), i.e.
 5     // the center of the window
 6     cairo_scale(cr, width, height);
 7 
 8     cairo_translate(cr, 0.5, 0.5);
 9     cairo_set_line_width(cr, m_line_width);
10 
11     cairo_save(cr);
12     cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9);   // green
13     cairo_paint(cr);
14     cairo_restore(cr);
15     cairo_arc(cr, 0.0, 0.0, m_radius, 0.0, 2.0 * M_PI);
16     cairo_save(cr);
17     cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.8);
18     cairo_fill_preserve(cr);
19     cairo_restore(cr);
20     cairo_stroke_preserve(cr);
21     cairo_clip(cr);
22 
23     // Clock ticks
24     for (int i = 0; i < 12; i++)
25     {
26         double inset = 0.05;
27 
28         cairo_save(cr);
29         cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
30 
31         if(i % 3 != 0)
32         {
33             inset *= 0.8;
34             cairo_set_line_width(cr, 0.03);
35         }
36 
37         cairo_move_to(
38             cr,
39             (m_radius - inset) * cos (i * M_PI / 6.0),
40             (m_radius - inset) * sin (i * M_PI / 6.0));
41         cairo_line_to(
42             cr,
43             m_radius * cos (i * M_PI / 6.0),
44             m_radius * sin (i * M_PI / 6.0));
45         cairo_stroke(cr);
46         cairo_restore(cr); /* stack-pen-size */
47     }
48 
49     // store the current time
50     time_t rawtime;
51     time(&rawtime);
52     struct tm * timeinfo = localtime (&rawtime);
53 
54     // compute the angles of the indicators of our clock
55     double minutes = timeinfo->tm_min * M_PI / 30.0;
56     double hours   = timeinfo->tm_hour * M_PI / 6.0;
57     double seconds = timeinfo->tm_sec * M_PI / 30.0;
58 
59     cairo_save(cr);
60     cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
61 
62     // draw the seconds hand
63     cairo_save(cr);
64     cairo_set_line_width(cr, m_line_width / 3.0);
65     cairo_set_source_rgba(cr, 0.7, 0.7, 0.7, 0.8);   // gray
66     cairo_move_to(cr, 0.0, 0.0);
67     cairo_line_to(cr,
68                   sin(seconds) * (m_radius * 0.9),
69                   -cos(seconds) * (m_radius * 0.9));
70     cairo_stroke(cr);
71     cairo_restore(cr);
72 
73     // draw the minutes hand
74     cairo_set_source_rgba(cr, 0.117, 0.337, 0.612, 0.9);   // blue
75     cairo_move_to(cr, 0, 0);
76     cairo_line_to(cr,
77                   sin(minutes + seconds / 60) * (m_radius * 0.8),
78                   -cos(minutes + seconds / 60) * (m_radius * 0.8));
79     cairo_stroke(cr);
80 
81     // draw the hours hand
82     cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9);   // green
83     cairo_move_to(cr, 0.0, 0.0);
84     cairo_line_to(cr,
85                   sin(hours + minutes / 12.0) * (m_radius * 0.5),
86                   -cos(hours + minutes / 12.0) * (m_radius * 0.5));
87     cairo_stroke(cr);
88     cairo_restore(cr);
89 
90     // draw a little dot in the middle
91     cairo_arc(cr, 0.0, 0.0, m_line_width / 3.0, 0.0, 2.0 * M_PI);
92     cairo_fill(cr);
93 }
~~~


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
 14 
 15     cairo_translate(cr, 0.5, 0.5);
 16     cairo_set_line_width(cr, m_line_width);
 17 
 18     cairo_save(cr);
 19     cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9);   // green
 20     cairo_paint(cr);
 21     cairo_restore(cr);
 22     cairo_arc(cr, 0.0, 0.0, m_radius, 0.0, 2.0 * M_PI);
 23     cairo_save(cr);
 24     cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.8);
 25     cairo_fill_preserve(cr);
 26     cairo_restore(cr);
 27     cairo_stroke_preserve(cr);
 28     cairo_clip(cr);
 29 
 30     // Clock ticks
 31     for (int i = 0; i < 12; i++)
 32     {
 33         double inset = 0.05;
 34 
 35         cairo_save(cr);
 36         cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
 37 
 38         if(i % 3 != 0)
 39         {
 40             inset *= 0.8;
 41             cairo_set_line_width(cr, 0.03);
 42         }
 43 
 44         cairo_move_to(
 45             cr,
 46             (m_radius - inset) * cos (i * M_PI / 6.0),
 47             (m_radius - inset) * sin (i * M_PI / 6.0));
 48         cairo_line_to(
 49             cr,
 50             m_radius * cos (i * M_PI / 6.0),
 51             m_radius * sin (i * M_PI / 6.0));
 52         cairo_stroke(cr);
 53         cairo_restore(cr); /* stack-pen-size */
 54     }
 55 
 56     // store the current time
 57     time_t rawtime;
 58     time(&rawtime);
 59     struct tm * timeinfo = localtime (&rawtime);
 60 
 61     // compute the angles of the indicators of our clock
 62     double minutes = timeinfo->tm_min * M_PI / 30.0;
 63     double hours   = timeinfo->tm_hour * M_PI / 6.0;
 64     double seconds = timeinfo->tm_sec * M_PI / 30.0;
 65 
 66     cairo_save(cr);
 67     cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
 68 
 69     // draw the seconds hand
 70     cairo_save(cr);
 71     cairo_set_line_width(cr, m_line_width / 3.0);
 72     cairo_set_source_rgba(cr, 0.7, 0.7, 0.7, 0.8);   // gray
 73     cairo_move_to(cr, 0.0, 0.0);
 74     cairo_line_to(cr,
 75                   sin(seconds) * (m_radius * 0.9),
 76                   -cos(seconds) * (m_radius * 0.9));
 77     cairo_stroke(cr);
 78     cairo_restore(cr);
 79 
 80     // draw the minutes hand
 81     cairo_set_source_rgba(cr, 0.117, 0.337, 0.612, 0.9);   // blue
 82     cairo_move_to(cr, 0, 0);
 83     cairo_line_to(cr,
 84                   sin(minutes + seconds / 60) * (m_radius * 0.8),
 85                   -cos(minutes + seconds / 60) * (m_radius * 0.8));
 86     cairo_stroke(cr);
 87 
 88     // draw the hours hand
 89     cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9);   // green
 90     cairo_move_to(cr, 0.0, 0.0);
 91     cairo_line_to(cr,
 92                   sin(hours + minutes / 12.0) * (m_radius * 0.5),
 93                   -cos(hours + minutes / 12.0) * (m_radius * 0.5));
 94     cairo_stroke(cr);
 95     cairo_restore(cr);
 96 
 97     // draw a little dot in the middle
 98     cairo_arc(cr, 0.0, 0.0, m_line_width / 3.0, 0.0, 2.0 * M_PI);
 99     cairo_fill(cr);
100 }
101 
102 
103 gboolean time_handler(GtkWidget* widget) {
104     gtk_widget_queue_draw(widget);
105 
106     return TRUE;
107 }
108 
109 
110 static void
111 app_activate (GApplication *app, gpointer user_data) {
112     GtkWidget *win;
113     GtkWidget *clock;
114     GtkBuilder *build;
115 
116     build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfc/tfc.ui");
117     win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
118     gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
119 
120     clock = GTK_WIDGET (gtk_builder_get_object (build, "clock"));
121     g_object_unref(build);
122 
123     gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (clock), draw_clock, NULL, NULL);
124     g_timeout_add(1000, (GSourceFunc) time_handler, (gpointer) clock);
125     gtk_widget_show(win);
126 
127 }
128 
129 static void
130 app_open (GApplication *app, GFile **files, gint n_files, gchar *hint, gpointer user_data) {
131 
132     app_activate(app,user_data);
133 
134 }
135 
136 int
137 main (int argc, char **argv) {
138     GtkApplication *app;
139     int stat;
140 
141     app = gtk_application_new ("com.github.ToshioCP.tfc", G_APPLICATION_HANDLES_OPEN);
142     g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
143     g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
144     stat = g_application_run (G_APPLICATION (app), argc, argv);
145     g_object_unref (app);
146     return stat;
147 }
~~~

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
 14 
 15     cairo_translate(cr, 0.5, 0.5);
 16     cairo_set_line_width(cr, m_line_width);
 17 
 18     cairo_save(cr);
 19     cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9);   // green
 20     cairo_paint(cr);
 21     cairo_restore(cr);
 22     cairo_arc(cr, 0.0, 0.0, m_radius, 0.0, 2.0 * M_PI);
 23     cairo_save(cr);
 24     cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.8);
 25     cairo_fill_preserve(cr);
 26     cairo_restore(cr);
 27     cairo_stroke_preserve(cr);
 28     cairo_clip(cr);
 29 
 30     // Clock ticks
 31     for (int i = 0; i < 12; i++)
 32     {
 33         double inset = 0.05;
 34 
 35         cairo_save(cr);
 36         cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
 37 
 38         if(i % 3 != 0)
 39         {
 40             inset *= 0.8;
 41             cairo_set_line_width(cr, 0.03);
 42         }
 43 
 44         cairo_move_to(
 45             cr,
 46             (m_radius - inset) * cos (i * M_PI / 6.0),
 47             (m_radius - inset) * sin (i * M_PI / 6.0));
 48         cairo_line_to(
 49             cr,
 50             m_radius * cos (i * M_PI / 6.0),
 51             m_radius * sin (i * M_PI / 6.0));
 52         cairo_stroke(cr);
 53         cairo_restore(cr); /* stack-pen-size */
 54     }
 55 
 56     // store the current time
 57     time_t rawtime;
 58     time(&rawtime);
 59     struct tm * timeinfo = localtime (&rawtime);
 60 
 61     // compute the angles of the indicators of our clock
 62     double minutes = timeinfo->tm_min * M_PI / 30.0;
 63     double hours   = timeinfo->tm_hour * M_PI / 6.0;
 64     double seconds = timeinfo->tm_sec * M_PI / 30.0;
 65 
 66     cairo_save(cr);
 67     cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
 68 
 69     // draw the seconds hand
 70     cairo_save(cr);
 71     cairo_set_line_width(cr, m_line_width / 3.0);
 72     cairo_set_source_rgba(cr, 0.7, 0.7, 0.7, 0.8);   // gray
 73     cairo_move_to(cr, 0.0, 0.0);
 74     cairo_line_to(cr,
 75                   sin(seconds) * (m_radius * 0.9),
 76                   -cos(seconds) * (m_radius * 0.9));
 77     cairo_stroke(cr);
 78     cairo_restore(cr);
 79 
 80     // draw the minutes hand
 81     cairo_set_source_rgba(cr, 0.117, 0.337, 0.612, 0.9);   // blue
 82     cairo_move_to(cr, 0, 0);
 83     cairo_line_to(cr,
 84                   sin(minutes + seconds / 60) * (m_radius * 0.8),
 85                   -cos(minutes + seconds / 60) * (m_radius * 0.8));
 86     cairo_stroke(cr);
 87 
 88     // draw the hours hand
 89     cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9);   // green
 90     cairo_move_to(cr, 0.0, 0.0);
 91     cairo_line_to(cr,
 92                   sin(hours + minutes / 12.0) * (m_radius * 0.5),
 93                   -cos(hours + minutes / 12.0) * (m_radius * 0.5));
 94     cairo_stroke(cr);
 95     cairo_restore(cr);
 96 
 97     // draw a little dot in the middle
 98     cairo_arc(cr, 0.0, 0.0, m_line_width / 3.0, 0.0, 2.0 * M_PI);
 99     cairo_fill(cr);
100 }
101 
102 
103 gboolean time_handler(GtkWidget* widget) {
104     gtk_widget_queue_draw(widget);
105 
106     return TRUE;
107 }
108 
109 
110 static void
111 app_activate (GApplication *app, gpointer user_data) {
112     GtkWidget *win;
113     GtkWidget *clock;
114     GtkBuilder *build;
115 
116     build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfc/tfc.ui");
117     win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
118     gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
119 
120     clock = GTK_WIDGET (gtk_builder_get_object (build, "clock"));
121     g_object_unref(build);
122 
123     gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (clock), draw_clock, NULL, NULL);
124     g_timeout_add(1000, (GSourceFunc) time_handler, (gpointer) clock);
125     gtk_widget_show(win);
126 
127 }
128 
129 static void
130 app_open (GApplication *app, GFile **files, gint n_files, gchar *hint, gpointer user_data) {
131 
132     app_activate(app,user_data);
133 
134 }
135 
136 int
137 main (int argc, char **argv) {
138     GtkApplication *app;
139     int stat;
140 
141     app = gtk_application_new ("com.github.ToshioCP.tfc", G_APPLICATION_HANDLES_OPEN);
142     g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
143     g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
144     stat = g_application_run (G_APPLICATION (app), argc, argv);
145     g_object_unref (app);
146     return stat;
147 }
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

~~~xml
1 <?xml version="1.0" encoding="UTF-8"?>
2 <gresources>
3   <gresource prefix="/com/github/ToshioCP/tfc">
4     <file>tfc.ui</file>
5   </gresource>
6 </gresources>
~~~

Up: [Readme.md](../Readme.md),  Prev: [Section 28](sec28.md)
