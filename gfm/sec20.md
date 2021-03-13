Up: [Readme.md](../Readme.md),  Prev: [Section 19](sec19.md), Next: [Section 21](sec21.md)

# Template XML

The tfe program in the previous section is not so good because many things are crammed into `tfepplication.c`.
Many static variables in `tfepplication.c` shows that.

~~~C
static GtkDialog *pref;
static GtkFontButton *fontbtn;
static GSettings *settings;
static GtkDialog *alert;
static GtkLabel *lb_alert;
static GtkButton *btn_accept;

static gulong pref_close_request_handler_id = 0;
static gulong alert_close_request_handler_id = 0;
static gboolean is_quit;
~~~

Generally, if there are many global or static variables in the program, it is not a good program.
Such programs are difficult to maintain.

The file `tfeapplication.c` should be divided into several files.

- `tfeapplication.c` only has codes related to GtkApplication.
- A file about GtkApplicationWindow
- A file about a preference dialog
- A file about an alert dialog

The preference dialog is defined by a ui file.
And it has GtkBox, GtkLabel and GtkFontButton in it.
Such widget is called composite widget.
Composite widget is a child object of the parent widget.
For example, the preference composite widget is a child object of GtkDialog.
Composite widget can be built from template XML.
Next subsection shows how to build a preference dialog.

## Preference dialog

First, write a template XML file.

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <interface>
 3   <template class="TfePref" parent="GtkDialog">
 4     <property name="title">Preferences</property>
 5     <property name="resizable">FALSE</property>
 6     <property name="modal">TRUE</property>
 7     <child internal-child="content_area">
 8       <object class="GtkBox" id="content_area">
 9         <child>
10           <object class="GtkBox" id="pref_boxh">
11             <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
12             <property name="spacing">12</property>
13             <property name="margin-start">12</property>
14             <property name="margin-end">12</property>
15             <property name="margin-top">12</property>
16             <property name="margin-bottom">12</property>
17             <child>
18               <object class="GtkLabel" id="fontlabel">
19                 <property name="label">Font:</property>
20                 <property name="xalign">1</property>
21               </object>
22             </child>
23             <child>
24               <object class="GtkFontButton" id="fontbtn">
25               </object>
26             </child>
27           </object>
28         </child>
29       </object>
30     </child>
31   </template>
32 </interface>
33 
~~~

- 3: Template tag specifies a composite widget.
The value of a class attribute is the object name of the composite widget.
This XML file names the object "TfePref".
It is defined in a C source file and it will be shown later.
A parent attribute specifies the direct parent object of the composite widget.
`TfePref` is a child object of `GtkDialog`.
Therefore the value of the attribute is "GtkDialog".
A parent attribute is optional but it is recommended to specify.

Other lines are the same as before.
The object `TfePref` is defined in `tfepref.h` and `tfepref.c`.

~~~C
 1 #ifndef __TFE_PREF_H__
 2 #define __TFE_PREF_H__
 3 
 4 #include <gtk/gtk.h>
 5 
 6 #define TFE_TYPE_PREF tfe_pref_get_type ()
 7 G_DECLARE_FINAL_TYPE (TfePref, tfe_pref, TFE, PREF, GtkDialog)
 8 
 9 GtkWidget *
10 tfe_pref_new (GtkWindow *win);
11 
12 #endif /* __TFE_PREF_H__ */
13 
~~~

- 6-7: When you define a new object, you need to write these two lines.
Refer to [Section 7](sec7.md).
- 9-10: `tfe_pref_new` generates a new TfePref object.
It has a parameter which the object use as a transient parent to show the dialog.

~~~C
 1 #include "tfepref.h"
 2 
 3 struct _TfePref
 4 {
 5   GtkDialog parent;
 6   GSettings *settings;
 7   GtkFontButton *fontbtn;
 8 };
 9 
10 G_DEFINE_TYPE (TfePref, tfe_pref, GTK_TYPE_DIALOG);
11 
12 static void
13 tfe_pref_dispose (GObject *gobject) {
14   TfePref *pref = TFE_PREF (gobject);
15 
16   g_clear_object (&pref->settings);
17   G_OBJECT_CLASS (tfe_pref_parent_class)->dispose (gobject);
18 }
19 
20 static void
21 tfe_pref_init (TfePref *pref) {
22   gtk_widget_init_template (GTK_WIDGET (pref));
23   pref->settings = g_settings_new ("com.github.ToshioCP.tfe");
24   g_settings_bind (pref->settings, "font", pref->fontbtn, "font", G_SETTINGS_BIND_DEFAULT);
25 }
26 
27 static void
28 tfe_pref_class_init (TfePrefClass *class) {
29   GObjectClass *object_class = G_OBJECT_CLASS (class);
30 
31   object_class->dispose = tfe_pref_dispose;
32   gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfepref.ui");
33   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfePref, fontbtn);
34 }
35 
36 GtkWidget *
37 tfe_pref_new (GtkWindow *win) {
38   return GTK_WIDGET (g_object_new (TFE_TYPE_PREF, "transient-for", win, NULL));
39 }
40 
~~~

- 3-8: The structure of an instance of this object.
It has two variables, settings and fontbtn.
- 10: G\_DEFINE\_TYPE macro generates lines to register the type.
- 12-18: dispose handler.
This handler is called when this object is finalizing.
The process has two stages, disposing and finalizing.
When disposing, the object releases all the objects it has had.
TfePref object holds a GSetting object.
It is released in line 16.
After that parents dispose handler is called in line 17.
Refer to [Section 10](sec10.md).
- 27-34: Class initialization function.
This is called in the class generation process.
- 31: Set the dispose handler.
- 32: `gtk_widget_class_set_template_from_resource` function associates the description in the XML file with the widget.
At this moment no object is generated.
It just make the class to know the structure of the object.
That's why the top level tag is not an object but template in the XML file.
- 33: `gtk_widget_class_bind_template_child` function binds a private variable of the object with a child object in the template.
This function is a macro.
The name of the private variable (in the line 7) and the id (in the line 24) in the XML file must be the same.
In the program above, the name is `fontbtn`.
The pointer to the object will be assigned to the variable when an instance is generated.
- 20-25: Instance initialization function.
- 22: Initializes the template of this object.
The template has been made during the class initialization process.
Now it is implemented to the instance.
- 23: Create GSettings object with the id `com.github.ToshioCP.tfe`.
- 24: Bind the font key in the GSettings object and the font property in the GtkFontButton.

- 36-39: The function `tfe_pref_new` creates an instance of TfePref.
The parameter `win` is a transient parent.

Now, It is very simple to use this dialog.
A caller just creates this object and shows it.

~~~C
TfePref *pref;
pref = tfe_pref_new (win) /* win is the top level window */
gtk_widget_show (GTK_WINDOW (win));
~~~

This instance is automatically destroyed when a user clicks on the close button.
That's all.
If you want to show the dialog again, just create and show it.

## Alert dialog

It is almost same as preference dialog.

Its XML file is:

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <interface>
 3   <template class="TfeAlert" parent="GtkDialog">
 4     <property name="title">Are you sure?</property>
 5     <property name="resizable">FALSE</property>
 6     <property name="modal">TRUE</property>
 7     <child internal-child="content_area">
 8       <object class="GtkBox">
 9         <child>
10           <object class="GtkBox">
11             <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
12             <property name="spacing">12</property>
13             <property name="margin-start">12</property>
14             <property name="margin-end">12</property>
15             <property name="margin-top">12</property>
16             <property name="margin-bottom">12</property>
17             <child>
18               <object class="GtkImage">
19                 <property name="icon-name">dialog-warning</property>
20                 <property name="icon-size">GTK_ICON_SIZE_LARGE</property>
21               </object>
22             </child>
23             <child>
24               <object class="GtkLabel" id="lb_alert">
25               </object>
26             </child>
27           </object>
28         </child>
29       </object>
30     </child>
31     <child type="action">
32       <object class="GtkButton" id="btn_cancel">
33         <property name="label">Cancel</property>
34       </object>
35     </child>
36     <child type="action">
37       <object class="GtkButton" id="btn_accept">
38         <property name="label">Close</property>
39       </object>
40     </child>
41     <action-widgets>
42       <action-widget response="cancel" default="true">btn_cancel</action-widget>
43       <action-widget response="accept">btn_accept</action-widget>
44     </action-widgets>
45   </template>
46 </interface>
47 
~~~

The header file is:

~~~C
 1 #ifndef __TFE_ALERT_H__
 2 #define __TFE_ALERT_H__
 3 
 4 #include <gtk/gtk.h>
 5 
 6 #define TFE_TYPE_ALERT tfe_alert_get_type ()
 7 G_DECLARE_FINAL_TYPE (TfeAlert, tfe_alert, TFE, ALERT, GtkDialog)
 8 
 9 void
10 tfe_alert_set_message (TfeAlert *alert, const char *message);
11 
12 void
13 tfe_alert_set_button_label (TfeAlert *alert, const char *label);
14 
15 GtkWidget *
16 tfe_alert_new (GtkWindow *win);
17 
18 #endif /* __TFE_ALERT_H__ */
19 
~~~

There are three public functions.
The functions `tfe_alert_set_message` and `tfe_alert_set_button_label` sets the label and button name of the alert dialog.
For example, if you want to show an alert that the user tries to close without saving the content, set them like:

~~~C
tfe_alert_set_message (alert, "Are you really close without saving?"); /* alert points to a TfeAlert object */
tfe_alert_set_button_label (alert, "Close");
~~~

The function `tfe_alert_new` creates a TfeAlert dialog.

The C source file is:

~~~C
 1 #include "tfealert.h"
 2 
 3 struct _TfeAlert
 4 {
 5   GtkDialog parent;
 6   GtkLabel *lb_alert;
 7   GtkButton *btn_accept;
 8 };
 9 
10 G_DEFINE_TYPE (TfeAlert, tfe_alert, GTK_TYPE_DIALOG);
11 
12 void
13 tfe_alert_set_message (TfeAlert *alert, const char *message) {
14   gtk_label_set_text (alert->lb_alert, message);
15 }
16 
17 void
18 tfe_alert_set_button_label (TfeAlert *alert, const char *label) {
19   gtk_button_set_label (alert->btn_accept, label);
20 }
21 
22 static void
23 tfe_alert_init (TfeAlert *alert) {
24   gtk_widget_init_template (GTK_WIDGET (alert));
25 }
26 
27 static void
28 tfe_alert_class_init (TfeAlertClass *class) {
29   gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfealert.ui");
30   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, lb_alert);
31   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, btn_accept);
32 }
33 
34 GtkWidget *
35 tfe_alert_new (GtkWindow *win) {
36   return GTK_WIDGET (g_object_new (TFE_TYPE_ALERT, "transient-for", win, NULL));
37 }
38 
~~~

The program is almost same as `tfepref.c`.

The instruction how to use this object is as follows.

1. Write a "response" signal handler.
2. Create a TfeAlert object.
3. Connect "response" signal to a handler
4. Show the dialog
5. In the signal handler do something along the response-id.
Then destroy the dialog.

## Top level window

In the same way, create a child object of GtkApplicationWindow.
The object name is "TfeWindow".

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <interface>
 3   <template class="TfeWindow" parent="GtkApplicationWindow">
 4     <property name="title">file editor</property>
 5     <property name="default-width">600</property>
 6     <property name="default-height">400</property>
 7     <child>
 8       <object class="GtkBox" id="boxv">
 9         <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
10         <child>
11           <object class="GtkBox" id="boxh">
12             <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
13             <child>
14               <object class="GtkLabel" id="dmy1">
15                 <property name="width-chars">10</property>
16               </object>
17             </child>
18             <child>
19               <object class="GtkButton" id="btno">
20                 <property name="label">Open</property>
21                 <signal name="clicked" handler="open_cb" swapped="TRUE" object="nb"></signal>
22               </object>
23             </child>
24             <child>
25               <object class="GtkButton" id="btns">
26                 <property name="label">Save</property>
27                 <signal name="clicked" handler="save_cb" swapped="TRUE" object="nb"></signal>
28               </object>
29             </child>
30             <child>
31               <object class="GtkLabel" id="dmy2">
32                 <property name="hexpand">TRUE</property>
33               </object>
34             </child>
35             <child>
36               <object class="GtkButton" id="btnc">
37                 <property name="label">Close</property>
38                 <signal name="clicked" handler="close_cb" swapped="TRUE" object="nb"></signal>
39               </object>
40             </child>
41             <child>
42               <object class="GtkMenuButton" id="btnm">
43                 <property name="direction">down</property>
44                 <property name="halign">start</property>
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
55           <object class="GtkNotebook" id="nb">
56             <property name="scrollable">TRUE</property>
57             <property name="hexpand">TRUE</property>
58             <property name="vexpand">TRUE</property>
59           </object>
60         </child>
61       </object>
62     </child>
63   </template>
64 </interface>
65 
~~~

This XML file is the same as before except template tag.

~~~C
 1 #ifndef __TFE_WINDOW_H__
 2 #define __TFE_WINDOW_H__
 3 
 4 #include <gtk/gtk.h>
 5 
 6 #define TFE_TYPE_WINDOW tfe_window_get_type ()
 7 G_DECLARE_FINAL_TYPE (TfeWindow, tfe_window, TFE, WINDOW, GtkApplicationWindow)
 8 
 9 void
10 tfe_window_notebook_page_new (TfeWindow *win);
11 
12 void
13 tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files);
14 
15 GtkWidget *
16 tfe_window_new (GtkApplication *app);
17 
18 #endif /* __TFE_WINDOW_H__ */
19 
~~~

There are three public functions.
The function `tfe_window_notebook_page_new` creates a new notebook page.
This is a wrapper function for `notebook_page_new`.
It is called by GtkApplication object.
The function `tfe_window_notebook_page_new_with_files` creates notebook pages with a contents read from the given files.
The function `tfe_window_new` creates a TfeWindow instance.

~~~C
  1 #include "tfewindow.h"
  2 #include "tfenotebook.h"
  3 #include "tfepref.h"
  4 #include "tfealert.h"
  5 #include "css.h"
  6 
  7 struct _TfeWindow {
  8   GtkApplicationWindow parent;
  9   GtkButton *btno;
 10   GtkButton *btns;
 11   GtkButton *btnc;
 12   GtkMenuButton *btnm;
 13   GtkNotebook *nb;
 14   GSettings *settings;
 15   gboolean is_quit;
 16 };
 17 
 18 G_DEFINE_TYPE (TfeWindow, tfe_window, GTK_TYPE_APPLICATION_WINDOW);
 19 
 20 /* alert response signal handler */
 21 static void
 22 alert_response_cb (GtkDialog *alert, int response_id, gpointer user_data) {
 23   TfeWindow *win = TFE_WINDOW (user_data);
 24 
 25   if (response_id == GTK_RESPONSE_ACCEPT) {
 26     if (win->is_quit)
 27       gtk_window_destroy(GTK_WINDOW (win));
 28     else
 29       notebook_page_close (win->nb);
 30   }
 31   gtk_window_destroy (GTK_WINDOW (alert));
 32 }
 33 
 34 /* ----- button handlers ----- */
 35 void
 36 open_cb (GtkNotebook *nb) {
 37   notebook_page_open (nb);
 38 }
 39 
 40 void
 41 save_cb (GtkNotebook *nb) {
 42   notebook_page_save (nb);
 43 }
 44 
 45 void
 46 close_cb (GtkNotebook *nb) {
 47   TfeAlert *alert;
 48   TfeWindow *win =  TFE_WINDOW (gtk_widget_get_ancestor (GTK_WIDGET (nb), TFE_TYPE_WINDOW));
 49 
 50   if (has_saved (nb))
 51     notebook_page_close (nb);
 52   else {
 53     win->is_quit = false;
 54     alert = TFE_ALERT (tfe_alert_new (GTK_WINDOW (win)));
 55     tfe_alert_set_message (alert, "Contents aren't saved yet.\nAre you sure to close?");
 56     tfe_alert_set_button_label (alert, "Close");
 57     g_signal_connect (GTK_DIALOG (alert), "response", G_CALLBACK (alert_response_cb), win);
 58     gtk_widget_show (GTK_WIDGET (alert));
 59   }
 60 }
 61 
 62 /* ----- action activated handlers ----- */
 63 static void
 64 open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 65   TfeWindow *win = TFE_WINDOW (user_data);
 66 
 67   open_cb (GTK_NOTEBOOK (win->nb));
 68 }
 69 
 70 static void
 71 save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 72   TfeWindow *win = TFE_WINDOW (user_data);
 73 
 74   save_cb (GTK_NOTEBOOK (win->nb));
 75 }
 76 
 77 static void
 78 close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 79   TfeWindow *win = TFE_WINDOW (user_data);
 80 
 81   close_cb (GTK_NOTEBOOK (win->nb));
 82 }
 83 
 84 static void
 85 new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 86   TfeWindow *win = TFE_WINDOW (user_data);
 87 
 88   notebook_page_new (GTK_NOTEBOOK (win->nb));
 89 }
 90 
 91 static void
 92 saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 93   TfeWindow *win = TFE_WINDOW (user_data);
 94 
 95   notebook_page_saveas (GTK_NOTEBOOK (win->nb));
 96 }
 97 
 98 static void
 99 pref_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
100   TfeWindow *win = TFE_WINDOW (user_data);
101   GtkWidget *pref;
102 
103   pref = tfe_pref_new (GTK_WINDOW (win));
104   gtk_widget_show (pref);
105 }
106 
107 static void
108 quit_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
109   TfeWindow *win = TFE_WINDOW (user_data);
110 
111   TfeAlert *alert;
112 
113   if (has_saved_all (GTK_NOTEBOOK (win->nb)))
114     gtk_window_destroy (GTK_WINDOW (win));
115   else {
116     win->is_quit = true;
117     alert = TFE_ALERT (tfe_alert_new (GTK_WINDOW (win)));
118     tfe_alert_set_message (alert, "Contents aren't saved yet.\nAre you sure to quit?");
119     tfe_alert_set_button_label (alert, "Quit");
120     g_signal_connect (GTK_DIALOG (alert), "response", G_CALLBACK (alert_response_cb), win);
121     gtk_widget_show (GTK_WIDGET (alert));
122   }
123 }
124 
125 /* gsettings changed::font signal handler */
126 static void
127 changed_font_cb (GSettings *settings, char *key, gpointer user_data) {
128   GtkWindow *win = GTK_WINDOW (user_data); 
129   const char *font;
130   PangoFontDescription *pango_font_desc;
131 
132   font = g_settings_get_string (settings, "font");
133   pango_font_desc = pango_font_description_from_string (font);
134   set_font_for_display_with_pango_font_desc (win, pango_font_desc);
135 }
136 
137 /* --- public functions --- */
138 
139 void
140 tfe_window_notebook_page_new (TfeWindow *win) {
141   notebook_page_new (win->nb);
142 }
143 
144 void
145 tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files) {
146   int i;
147 
148   for (i = 0; i < n_files; i++)
149     notebook_page_new_with_file (win->nb, files[i]);
150   if (gtk_notebook_get_n_pages (win->nb) == 0)
151     notebook_page_new (win->nb);
152 }
153 
154 /* --- TfeWindow object construction/destruction --- */ 
155 static void
156 tfe_window_dispose (GObject *gobject) {
157   TfeWindow *window = TFE_WINDOW (gobject);
158 
159   g_clear_object (&window->settings);
160   G_OBJECT_CLASS (tfe_window_parent_class)->dispose (gobject);
161 }
162 
163 static void
164 tfe_window_init (TfeWindow *win) {
165   GtkBuilder *build;
166   GMenuModel *menu;
167 
168   gtk_widget_init_template (GTK_WIDGET (win));
169 
170   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/menu.ui");
171   menu = G_MENU_MODEL (gtk_builder_get_object (build, "menu"));
172   gtk_menu_button_set_menu_model (win->btnm, menu);
173   g_object_unref(build);
174 
175   win->settings = g_settings_new ("com.github.ToshioCP.tfe");
176   g_signal_connect (win->settings, "changed::font", G_CALLBACK (changed_font_cb), win);
177 
178 /* ----- action ----- */
179   const GActionEntry win_entries[] = {
180     { "open", open_activated, NULL, NULL, NULL },
181     { "save", save_activated, NULL, NULL, NULL },
182     { "close", close_activated, NULL, NULL, NULL },
183     { "new", new_activated, NULL, NULL, NULL },
184     { "saveas", saveas_activated, NULL, NULL, NULL },
185     { "pref", pref_activated, NULL, NULL, NULL },
186     { "close-all", quit_activated, NULL, NULL, NULL }
187   };
188   g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);
189 
190   changed_font_cb(win->settings, "font", win);
191 }
192 
193 static void
194 tfe_window_class_init (TfeWindowClass *class) {
195   GObjectClass *object_class = G_OBJECT_CLASS (class);
196 
197   object_class->dispose = tfe_window_dispose;
198   gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfewindow.ui");
199   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, btno);
200   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, btns);
201   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, btnc);
202   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, btnm);
203   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, nb);
204   gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), open_cb);
205   gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), save_cb);
206   gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), close_cb);
207 }
208 
209 GtkWidget *
210 tfe_window_new (GtkApplication *app) {
211   return GTK_WIDGET (g_object_new (TFE_TYPE_WINDOW, "application", app, NULL));
212 }
213 
~~~

- 20-32: `alert_response_cb` is a call back function of the "response" signal of TfeAlert dialog.
This is the same as before except `gtk_window_destroy(GTK_WINDOW (win))` is used instead of `tfe_application_quit`.
- 34-60: Handlers of Button clicked signal.
- 62-123: Handlers of action activated signal.
The `user_data` is a pointer to TfeWindow instance.
- 125-135: A handler of "changed::font" signal of GSettings object.
- 132: Gets the font from GSettings data.
- 133: Gets a PangoFontDescription from the font.
In the previous version, the program gets the font description from the GtkFontButton.
The button data and GSettings data are the same.
Therefore, the data got here is the same as the data in the GtkFontButton.
In addition, we don't need to worry about the preference dialog is alive or not thanks to the GSettings.
- 134: Sets CSS on the display with the font description.
- 137-152: Public functions.
- 155-161: Dispose handler.
The GSettings object needs to be released.
- 163-191: Object initialize function.
- 168: Generates a composite widget with the template.
- 170-173: Insert menu to the menu button.
- 175-176: Creates a GSettings object with the id.
Connects "changed::font" signal to the handler `changed_font_cb`.
This signal emits when the GSettings data is changed.
The second part "font" of the signal name "changed::font" is called details.
Signals can have details.
If a GSettings object has more than one key, "changed" signal emits only if the key which has the same name as the detail changes its value.
For example, Suppose a GSettings object has three keys "a", "b" and "c".
  - "changed::a" is emitted when the value of "a" is changed. It isn't emitted when the value of "b" or "c" is changed.
  - "changed::b" is emitted when the value of "b" is changed. It isn't emitted when the value of "a" or "c" is changed.
  - "changed::c" is emitted when the value of "c" is changed. It isn't emitted when the value of "a" or "b" is changed.
In this version of tfe, there is only one key ("font").
So, even if the signal doesn't have a detail, the result is the same.
But in the future version, it will probably need details.
- 178-188: Creates actions.
- 190: Sets CSS font.
- 193-207: Class initialization function.
- 197: Sets the dispose handler.
- 198: Sets the composite widget template
- 199-203: Binds private variable with child objects in the template.
- 204-206: Binds signal handlers with signal tags in the template.
- 209-212: `tfe_window_new`.
This function creates TfeWindow instance.

## TfeApplication

The file `tfeapplication.c` is now very simple.

~~~C
 1 #include "tfewindow.h"
 2 
 3 /* ----- activate, open, startup handlers ----- */
 4 static void
 5 tfe_activate (GApplication *application) {
 6   GtkApplication *app = GTK_APPLICATION (application);
 7   GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
 8 
 9   tfe_window_notebook_page_new (win);
10   gtk_widget_show (GTK_WIDGET (win));
11 }
12 
13 static void
14 tfe_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
15   GtkApplication *app = GTK_APPLICATION (application);
16   GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
17 
18   tfe_window_notebook_page_new_with_files (win, files, n_files);
19   gtk_widget_show (win);
20 }
21 
22 static void
23 tfe_startup (GApplication *application) {
24   GtkApplication *app = GTK_APPLICATION (application);
25   GtkBuilder *build;
26   TfeWindow *win;
27   int i;
28 
29   win = tfe_window_new (app);
30 
31 /* ----- accelerator ----- */ 
32   struct {
33     const char *action;
34     const char *accels[2];
35   } action_accels[] = {
36     { "win.open", { "<Control>o", NULL } },
37     { "win.save", { "<Control>s", NULL } },
38     { "win.close", { "<Control>w", NULL } },
39     { "win.new", { "<Control>n", NULL } },
40     { "win.saveas", { "<Shift><Control>s", NULL } },
41     { "win.close-all", { "<Control>q", NULL } },
42   };
43 
44   for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
45     gtk_application_set_accels_for_action(GTK_APPLICATION(app), action_accels[i].action, action_accels[i].accels);
46 }
47 
48 /* ----- main ----- */
49 int
50 main (int argc, char **argv) {
51   GtkApplication *app;
52   int stat;
53 
54   app = gtk_application_new ("com.github.ToshioCP.tfe", G_APPLICATION_HANDLES_OPEN);
55 
56   g_signal_connect (app, "startup", G_CALLBACK (tfe_startup), NULL);
57   g_signal_connect (app, "activate", G_CALLBACK (tfe_activate), NULL);
58   g_signal_connect (app, "open", G_CALLBACK (tfe_open), NULL);
59 
60   stat =g_application_run (G_APPLICATION (app), argc, argv);
61   g_object_unref (app);
62   return stat;
63 }
64 
~~~

- 3-11: Activate signal handler.
It uses `tfe_window_notebook_page_new` instead of `notebook_page_new`.
- 13-20: Open signal handler.
Thanks to `tfe_window_notebook_page_new_with_files`, this handler becomes very simple.
- 22-46: Startup signal handler.
Most of the task is moved to TfeWindow, the remaining task is creating a window and setting accelerations.
- 49-63: A function main.

## Other files

Resource XML file.

~~~xml
1 <?xml version="1.0" encoding="UTF-8"?>
2 <gresources>
3   <gresource prefix="/com/github/ToshioCP/tfe">
4     <file>tfewindow.ui</file>
5     <file>tfepref.ui</file>
6     <file>tfealert.ui</file>
7     <file>menu.ui</file>
8   </gresource>
9 </gresources>
~~~

GSchema XML file

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <schemalist>
 3   <schema path="/com/github/ToshioCP/tfe/" id="com.github.ToshioCP.tfe">
 4     <key name="font" type="s">
 5       <default>'Monospace 12'</default>
 6       <summary>Font</summary>
 7       <description>The font to be used for textview.</description>
 8     </key>
 9   </schema>
10 </schemalist>
~~~

Meson.build

~~~meson
 1 project('tfe', 'c')
 2 
 3 gtkdep = dependency('gtk4')
 4 
 5 gnome=import('gnome')
 6 resources = gnome.compile_resources('resources','tfe.gresource.xml')
 7 gnome.compile_schemas(build_by_default: true, depend_files: 'com.github.ToshioCP.tfe.gschema.xml')
 8 
 9 sourcefiles=files('tfeapplication.c', 'tfewindow.c', 'tfenotebook.c', 'tfepref.c', 'tfealert.c', 'css.c', '../tfetextview/tfetextview.c')
10 
11 executable('tfe', sourcefiles, resources, dependencies: gtkdep, export_dynamic: true, install: true)
12 
13 schema_dir = get_option('prefix') / get_option('datadir') / 'glib-2.0/schemas/'
14 install_data('com.github.ToshioCP.tfe.gschema.xml', install_dir: schema_dir)
15 meson.add_install_script('glib-compile-schemas', schema_dir)
16 
~~~

## Compiling and installation.

~~~
$ meson --prefix=$HOME/local _build
$ ninja -C _build
$ ninja -C _build install
~~~

Source files are in [src/tfe7](../src/tfe7) directory.

We made a very small text editor.
You can add features to this editor.
When you add a new feature, care about the structure of the program.
Maybe you need to divide a file into several files like this section.
It isn't good to put many things into one file.
And it is important to think about the relationship between source files and widget structures.
It is appropriate that they correspond to each other in many cases.

Up: [Readme.md](../Readme.md),  Prev: [Section 19](sec19.md), Next: [Section 21](sec21.md)
