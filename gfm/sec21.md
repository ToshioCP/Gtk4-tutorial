Up: [Readme.md](../Readme.md),  Prev: [Section 20](sec20.md), Next: [Section 22](sec22.md)

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
- A file for GtkApplicationWindow
- A file for a preference dialog
- A file for an alert dialog

The preference dialog is defined by a ui file.
And it has GtkBox, GtkLabel and GtkFontButton in it.
Such widget is called composite widget.
Composite widget is a child object (not child widget) of a widget.
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
Refer to [Section 8](sec8.md).
- 9-10: `tfe_pref_new` generates a new TfePref object.
It has a parameter which the object uses as a transient parent to show the dialog.

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
Refer to [Section 11](sec11.md).
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
- 24: Bind the font key in the GSettings object to the font property in the GtkFontButton.

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
5. In the signal handler do something with regard to the response-id.
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
21                 <property name="action-name">win.open</property>
22               </object>
23             </child>
24             <child>
25               <object class="GtkButton" id="btns">
26                 <property name="label">Save</property>
27                 <property name="action-name">win.save</property>
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
38                 <property name="action-name">win.close</property>
39               </object>
40             </child>
41             <child>
42               <object class="GtkMenuButton" id="btnm">
43                 <property name="direction">down</property>
44                 <property name="halign">start</property>
45                 <property name="icon-name">open-menu-symbolic</property>
46               </object>
47             </child>
48             <child>
49               <object class="GtkLabel" id="dmy3">
50                 <property name="width-chars">10</property>
51               </object>
52             </child>
53           </object>
54         </child>
55         <child>
56           <object class="GtkNotebook" id="nb">
57             <property name="scrollable">TRUE</property>
58             <property name="hexpand">TRUE</property>
59             <property name="vexpand">TRUE</property>
60           </object>
61         </child>
62       </object>
63     </child>
64   </template>
65 </interface>
66 
~~~

This XML file is almost same as before except template tag and "action-name" property.

GtkButton implements GtkActionable interface, which has "action-name" property.
If this property is set, GtkButton activates the action when it is clicked.
For example, if an open button is clicked, "win.open" action will be activated and `open_activated` handler will be invoked.

This action is also used by "\<Control\>o" accelerator (See the source code of `tfewindow.c` below).
If you use "clicked" signal for the button, you need its signal handler.
Then, there are two handlers:

- a handler for the "clicked" signal on the button
- a handler for the "activate" signal on the "win.open" action, to which "\<Control\>o" accelerator is connected

These two handlers do almost same thing.
It is inefficient.
Connecting buttons to actions is a good way to reduce unnecessary codes.


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
  9   GtkMenuButton *btnm;
 10   GtkNotebook *nb;
 11   GSettings *settings;
 12   gboolean is_quit;
 13 };
 14 
 15 G_DEFINE_TYPE (TfeWindow, tfe_window, GTK_TYPE_APPLICATION_WINDOW);
 16 
 17 /* alert response signal handler */
 18 static void
 19 alert_response_cb (GtkDialog *alert, int response_id, gpointer user_data) {
 20   TfeWindow *win = TFE_WINDOW (user_data);
 21 
 22   if (response_id == GTK_RESPONSE_ACCEPT) {
 23     if (win->is_quit)
 24       gtk_window_destroy(GTK_WINDOW (win));
 25     else
 26       notebook_page_close (win->nb);
 27   }
 28   gtk_window_destroy (GTK_WINDOW (alert));
 29 }
 30 
 31 /* ----- action activated handlers ----- */
 32 static void
 33 open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 34   TfeWindow *win = TFE_WINDOW (user_data);
 35 
 36   notebook_page_open (GTK_NOTEBOOK (win->nb));
 37 }
 38 
 39 static void
 40 save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 41   TfeWindow *win = TFE_WINDOW (user_data);
 42 
 43   notebook_page_save (GTK_NOTEBOOK (win->nb));
 44 }
 45 
 46 static void
 47 close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 48   TfeWindow *win = TFE_WINDOW (user_data);
 49   TfeAlert *alert;
 50 
 51   if (has_saved (win->nb))
 52     notebook_page_close (win->nb);
 53   else {
 54     win->is_quit = false;
 55     alert = TFE_ALERT (tfe_alert_new (GTK_WINDOW (win)));
 56     tfe_alert_set_message (alert, "Contents aren't saved yet.\nAre you sure to close?");
 57     tfe_alert_set_button_label (alert, "Close");
 58     g_signal_connect (GTK_DIALOG (alert), "response", G_CALLBACK (alert_response_cb), win);
 59     gtk_widget_show (GTK_WIDGET (alert));
 60   }
 61 }
 62 
 63 static void
 64 new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 65   TfeWindow *win = TFE_WINDOW (user_data);
 66 
 67   notebook_page_new (GTK_NOTEBOOK (win->nb));
 68 }
 69 
 70 static void
 71 saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 72   TfeWindow *win = TFE_WINDOW (user_data);
 73 
 74   notebook_page_saveas (GTK_NOTEBOOK (win->nb));
 75 }
 76 
 77 static void
 78 pref_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 79   TfeWindow *win = TFE_WINDOW (user_data);
 80   GtkWidget *pref;
 81 
 82   pref = tfe_pref_new (GTK_WINDOW (win));
 83   gtk_widget_show (pref);
 84 }
 85 
 86 static void
 87 quit_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 88   TfeWindow *win = TFE_WINDOW (user_data);
 89 
 90   TfeAlert *alert;
 91 
 92   if (has_saved_all (GTK_NOTEBOOK (win->nb)))
 93     gtk_window_destroy (GTK_WINDOW (win));
 94   else {
 95     win->is_quit = true;
 96     alert = TFE_ALERT (tfe_alert_new (GTK_WINDOW (win)));
 97     tfe_alert_set_message (alert, "Contents aren't saved yet.\nAre you sure to quit?");
 98     tfe_alert_set_button_label (alert, "Quit");
 99     g_signal_connect (GTK_DIALOG (alert), "response", G_CALLBACK (alert_response_cb), win);
100     gtk_widget_show (GTK_WIDGET (alert));
101   }
102 }
103 
104 /* gsettings changed::font signal handler */
105 static void
106 changed_font_cb (GSettings *settings, char *key, gpointer user_data) {
107   GtkWindow *win = GTK_WINDOW (user_data); 
108   const char *font;
109   PangoFontDescription *pango_font_desc;
110 
111   font = g_settings_get_string (settings, "font");
112   pango_font_desc = pango_font_description_from_string (font);
113   set_font_for_display_with_pango_font_desc (win, pango_font_desc);
114 }
115 
116 /* --- public functions --- */
117 
118 void
119 tfe_window_notebook_page_new (TfeWindow *win) {
120   notebook_page_new (win->nb);
121 }
122 
123 void
124 tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files) {
125   int i;
126 
127   for (i = 0; i < n_files; i++)
128     notebook_page_new_with_file (win->nb, files[i]);
129   if (gtk_notebook_get_n_pages (win->nb) == 0)
130     notebook_page_new (win->nb);
131 }
132 
133 /* --- TfeWindow object construction/destruction --- */ 
134 static void
135 tfe_window_dispose (GObject *gobject) {
136   TfeWindow *window = TFE_WINDOW (gobject);
137 
138   g_clear_object (&window->settings);
139   G_OBJECT_CLASS (tfe_window_parent_class)->dispose (gobject);
140 }
141 
142 static void
143 tfe_window_init (TfeWindow *win) {
144   GtkBuilder *build;
145   GMenuModel *menu;
146 
147   gtk_widget_init_template (GTK_WIDGET (win));
148 
149   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/menu.ui");
150   menu = G_MENU_MODEL (gtk_builder_get_object (build, "menu"));
151   gtk_menu_button_set_menu_model (win->btnm, menu);
152   g_object_unref(build);
153 
154   win->settings = g_settings_new ("com.github.ToshioCP.tfe");
155   g_signal_connect (win->settings, "changed::font", G_CALLBACK (changed_font_cb), win);
156 
157 /* ----- action ----- */
158   const GActionEntry win_entries[] = {
159     { "open", open_activated, NULL, NULL, NULL },
160     { "save", save_activated, NULL, NULL, NULL },
161     { "close", close_activated, NULL, NULL, NULL },
162     { "new", new_activated, NULL, NULL, NULL },
163     { "saveas", saveas_activated, NULL, NULL, NULL },
164     { "pref", pref_activated, NULL, NULL, NULL },
165     { "close-all", quit_activated, NULL, NULL, NULL }
166   };
167   g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);
168 
169   changed_font_cb(win->settings, "font", win);
170 }
171 
172 static void
173 tfe_window_class_init (TfeWindowClass *class) {
174   GObjectClass *object_class = G_OBJECT_CLASS (class);
175 
176   object_class->dispose = tfe_window_dispose;
177   gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfewindow.ui");
178   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, btnm);
179   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, nb);
180 }
181 
182 GtkWidget *
183 tfe_window_new (GtkApplication *app) {
184   return GTK_WIDGET (g_object_new (TFE_TYPE_WINDOW, "application", app, NULL));
185 }
186 
~~~

- 17-29: `alert_response_cb` is a call back function of the "response" signal of TfeAlert dialog.
This is the same as before except `gtk_window_destroy(GTK_WINDOW (win))` is used instead of `tfe_application_quit`.
- 31-102: Handlers of action activated signal.
The `user_data` is a pointer to TfeWindow instance.
- 104-114: A handler of "changed::font" signal of GSettings object.
- 111: Gets the font from GSettings data.
- 112: Gets a PangoFontDescription from the font.
In the previous version, the program gets the font description from the GtkFontButton.
The button data and GSettings data are the same.
Therefore, the data got here is the same as the data in the GtkFontButton.
In addition, we don't need to worry about the preference dialog is alive or not thanks to the GSettings.
- 113: Sets CSS on the display with the font description.
- 116-131: Public functions.
- 133-140: Dispose handler.
The GSettings object needs to be released.
- 142-170: Object initialize function.
- 147: Generates a composite widget with the template.
- 150-152: Insert menu to the menu button.
- 154-155: Creates a GSettings object with the id.
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
- 157-167: Creates actions.
- 169: Sets CSS font.
- 172-180: Class initialization function.
- 176: Sets the dispose handler.
- 177: Sets the composite widget template
- 178-179: Binds private variable with child objects in the template.
- 182-185: `tfe_window_new`.
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
 9   tfe_window_notebook_page_new (TFE_WINDOW (win));
10   gtk_widget_show (GTK_WIDGET (win));
11 }
12 
13 static void
14 tfe_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
15   GtkApplication *app = GTK_APPLICATION (application);
16   GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
17 
18   tfe_window_notebook_page_new_with_files (TFE_WINDOW (win), files, n_files);
19   gtk_widget_show (win);
20 }
21 
22 static void
23 tfe_startup (GApplication *application) {
24   GtkApplication *app = GTK_APPLICATION (application);
25   int i;
26 
27   tfe_window_new (app);
28 
29 /* ----- accelerator ----- */ 
30   struct {
31     const char *action;
32     const char *accels[2];
33   } action_accels[] = {
34     { "win.open", { "<Control>o", NULL } },
35     { "win.save", { "<Control>s", NULL } },
36     { "win.close", { "<Control>w", NULL } },
37     { "win.new", { "<Control>n", NULL } },
38     { "win.saveas", { "<Shift><Control>s", NULL } },
39     { "win.close-all", { "<Control>q", NULL } },
40   };
41 
42   for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
43     gtk_application_set_accels_for_action(GTK_APPLICATION(app), action_accels[i].action, action_accels[i].accels);
44 }
45 
46 /* ----- main ----- */
47 int
48 main (int argc, char **argv) {
49   GtkApplication *app;
50   int stat;
51 
52   app = gtk_application_new ("com.github.ToshioCP.tfe", G_APPLICATION_HANDLES_OPEN);
53 
54   g_signal_connect (app, "startup", G_CALLBACK (tfe_startup), NULL);
55   g_signal_connect (app, "activate", G_CALLBACK (tfe_activate), NULL);
56   g_signal_connect (app, "open", G_CALLBACK (tfe_open), NULL);
57 
58   stat =g_application_run (G_APPLICATION (app), argc, argv);
59   g_object_unref (app);
60   return stat;
61 }
62 
~~~

- 4-11: Activate signal handler.
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

Up: [Readme.md](../Readme.md),  Prev: [Section 20](sec20.md), Next: [Section 22](sec22.md)
