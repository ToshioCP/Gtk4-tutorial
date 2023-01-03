Up: [README.md](../README.md),  Prev: [Section 20](sec20.md), Next: [Section 22](sec22.md)

# Template XML and composite widget

The tfe program in the previous section is not so good because many things are crammed into `tfepplication.c`.
And many static variables in `tfepplication.c`.
The file `tfeapplication.c` should be divided into several files.

- `tfeapplication.c` only has codes related to the application.
- A file for the main window
- A file for a preference dialog
- A file for an alert dialog

The preference dialog is defined by a ui file.
And it has GtkBox, GtkLabel and GtkFontButton in it.
Such widget can be defined as a composite widget.
Composite widget is:

- a child object (not child widget) of a widget.
For example, the preference composite widget is a child object of GtkDialog.
- Composite widget can be built from template XML.
The widget is defined with template tag, not object tag.

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

Template tag specifies a composite widget.
The value of a class attribute is the object name.
It is "TfePref".
A parent attribute specifies the direct parent class of the composite widget.
Therefore. `TfePref` is a child class of `GtkDialog`.
A parent attribute is optional.
But it is recommended to specify it.
Other lines are the same as before.

The class `TfePref` is defined like TfeTextView.
There are two files `tfepref.h` and `tfepref.c`.

The file `tfepref.h` defines types and declares public functions.
The definitions are public and open to any C files.

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
10 tfe_pref_new (void);
11 
12 #endif /* __TFE_PREF_H__ */
13 
~~~

- 6: Defines a type `TFE_TYPE_PREF`, which is a macro replaced by `tfe_pref_get_type ()`.
- 7: The macro `G_DECLAER_FINAL_TYPE` expands to:
  - The function `tfe_pref_get_type ()` is declared.
  - TfePrep type is defined as a typedef of `struct _TfePrep`.
  - TfePrepClass type is defined as a typedef of `struct {GtkDialogClass *parent;}`.
  - Two functions `TFE_PREF ()` and `TFE_IS_PREF ()` is defined.
- 9-10: `tfe_pref_new` creates a new TfePref object.

The file `tfepref.c` includes:

- `struct _TfePrep` structure
- `G_DEFINE_TYPE` macro
- Initialize and dispose functions
- public functions

~~~C
 1 #include <gtk/gtk.h>
 2 #include "tfepref.h"
 3 
 4 struct _TfePref
 5 {
 6   GtkDialog parent;
 7   GSettings *settings;
 8   GtkFontButton *fontbtn;
 9 };
10 
11 G_DEFINE_TYPE (TfePref, tfe_pref, GTK_TYPE_DIALOG);
12 
13 static void
14 tfe_pref_dispose (GObject *gobject) {
15   TfePref *pref = TFE_PREF (gobject);
16 
17   g_clear_object (&pref->settings);
18   G_OBJECT_CLASS (tfe_pref_parent_class)->dispose (gobject);
19 }
20 
21 static void
22 tfe_pref_init (TfePref *pref) {
23   gtk_widget_init_template (GTK_WIDGET (pref));
24   pref->settings = g_settings_new ("com.github.ToshioCP.tfe");
25   g_settings_bind (pref->settings, "font", pref->fontbtn, "font", G_SETTINGS_BIND_DEFAULT);
26 }
27 
28 static void
29 tfe_pref_class_init (TfePrefClass *class) {
30   GObjectClass *object_class = G_OBJECT_CLASS (class);
31 
32   object_class->dispose = tfe_pref_dispose;
33   gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfepref.ui");
34   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfePref, fontbtn);
35 }
36 
37 GtkWidget *
38 tfe_pref_new (void) {
39   return GTK_WIDGET (g_object_new (TFE_TYPE_PREF, NULL));
40 }
41 
~~~

- 4-9: The structure `struct _TfePref` is defined.
Every TfePref instance has its own data of the structure.
The structure has references to:
  - a GSettings instance
  - a FontButton instance
- 11: `G_DEFINE_TYPE` macro.
The macro expands to:
  - the declaration of the class initialization function `tfe_pref_class_init`
  - the declaration of the instance initialization function `tfe_pref_init`
  - a static variable `tfe_pref_parent_class` that points the parent class (GtkDialogClass) structure.
  - a definition of `tfe_pref_get_type ()` function
- 13-19: `tfe_pref_dispose` function.
It is called in the destruction process and releases all the reference to other objects.
For further information about destruction process, refer to [Section 11](sec11.md).
- 17: g\_clear\_object is often used in dispose handlers. `g_clear_object (&pref->gsettings)` does:
  - `g_object_unref (pref->gsettings)`
  - `pref->settings =  NULL`
- 21-26: Instance initialization function.
The argument `pref` points a newly created TfePref instance.
- 23: The function `gtk_widget_init_template` creates and initializes the child widgets.
The widgets are created based on the template which is created in the `gtk_widget_class_set_template_from_resource` function.
- 24: Creates GSettings instance and assigns the pointer to it into `pref->settings`.
The instance refers to a GSetting id `com.github.ToshioCP.tfe`.
- 25: Binds the GSettings data `font` and the `font` property of `pref->fontbtn` (GtkFontButton).
The element `pref->fontbtn` points the GtkFontButton, which is the instance of `fontbtn` in the ui file.
The relation was made by the `gtk_widget_class_bind_template_child` function.
- 28-35: Class initialization function.
- 32: Sets the dispose handler.
- 33: `gtk_widget_class_set_template_from_resource` function associates the description in the XML file (`tfepref.ui`) with the widget.
At this moment no instance is created.
It just makes the class recognize the structure of the object.
That's why the top level tag is not `<object>` but `<template>` in the XML file.
The instance will be created in the `gtk_widget_init_template` function later.
- 34: `gtk_widget_class_bind_template_child` macro binds the structure member (`fontbtn` in `struct _TfePref`) and the id `fontbtn` in the XML file.
The two names must be the same.
This binding is between the member and the template (not an instance).
- 37-40: The function `tfe_pref_new` creates a TfePref instance.

Now, It is very simple to use this dialog.
A caller just creates this object and shows it.

~~~C
TfePref *pref;
pref = tfe_pref_new ();
gtk_window_set_transient_for (GTK_WINDOW (pref), win); /* win is the main window */
gtk_window_present (GTK_WINDOW (pref));
~~~

This instance is automatically destroyed when a user clicks on the close button.
That's all.
If you want to show the dialog again, just create and show it.

![Preference dialog](../image/pref_dialog.png)

## Alert dialog

It is almost same as preference dialog.

Its ui file is:

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
16 tfe_alert_new (void);
17 
18 #endif /* __TFE_ALERT_H__ */
19 
~~~

There are three public functions.
The functions `tfe_alert_set_message` and `tfe_alert_set_button_label` sets the label and button name of the alert dialog.
For example, if you want to show an alert that the user tries to close without saving the content, set them like:

~~~C
tfe_alert_set_message (alert, "Contents aren't saved yet.\nAre you sure to close?");
tfe_alert_set_button_label (alert, "Close");
~~~

The function `tfe_alert_new` creates a TfeAlert dialog.

![Alert dialog](../image/alert_dialog.png)

The C source file is:

~~~C
 1 #include <gtk/gtk.h>
 2 #include "tfealert.h"
 3 
 4 struct _TfeAlert
 5 {
 6   GtkDialog parent;
 7   GtkLabel *lb_alert;
 8   GtkButton *btn_accept;
 9 };
10 
11 G_DEFINE_TYPE (TfeAlert, tfe_alert, GTK_TYPE_DIALOG);
12 
13 void
14 tfe_alert_set_message (TfeAlert *alert, const char *message) {
15   gtk_label_set_text (alert->lb_alert, message);
16 }
17 
18 void
19 tfe_alert_set_button_label (TfeAlert *alert, const char *label) {
20   gtk_button_set_label (alert->btn_accept, label);
21 }
22 
23 static void
24 tfe_alert_init (TfeAlert *alert) {
25   gtk_widget_init_template (GTK_WIDGET (alert));
26 }
27 
28 static void
29 tfe_alert_class_init (TfeAlertClass *class) {
30   gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfealert.ui");
31   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, lb_alert);
32   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, btn_accept);
33 }
34 
35 GtkWidget *
36 tfe_alert_new (void) {
37   return GTK_WIDGET (g_object_new (TFE_TYPE_ALERT, NULL));
38 }
39 
~~~

The program is almost same as `tfepref.c`.

The Usage of the alert object is as follows.

1. Write the "response" signal handler.
2. Create a TfeAlert object.
3. Connect "response" signal to a handler
4. Show the dialog
5. In the signal handler, do something with regard to the response-id and destroy the dialog.

## Top-level window

`TfeWindow` is a child class of GtkApplicationWindow.

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

This XML file is almost same as before except template tag and "action-name" property in buttons.

GtkButton implements GtkActionable interface, which has "action-name" property.
If this property is set, GtkButton activates the action when it is clicked.
For example, if an open button is clicked, "win.open" action will be activated and `open_activated` handler will be invoked.

This action is also used by "\<Control\>o" accelerator (See `tfeapplication.c`).
If you used "clicked" signal for the button, you would need its signal handler.
Then, there would be two handlers:

- a handler for the "clicked" signal on the button
- a handler for the "activate" signal on the "win.open" action, to which "\<Control\>o" accelerator is connected

These two handlers are almost same.
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
It is called by TfeApplication object.
The function `tfe_window_notebook_page_new_with_files` creates notebook pages with a contents read from the given files.
The function `tfe_window_new` creates a TfeWindow instance.

~~~C
  1 #include <gtk/gtk.h>
  2 #include "tfewindow.h"
  3 #include "tfenotebook.h"
  4 #include "tfepref.h"
  5 #include "tfealert.h"
  6 
  7 struct _TfeWindow {
  8   GtkApplicationWindow parent;
  9   GtkMenuButton *btnm;
 10   GtkNotebook *nb;
 11   gboolean is_quit;
 12 };
 13 
 14 G_DEFINE_TYPE (TfeWindow, tfe_window, GTK_TYPE_APPLICATION_WINDOW);
 15 
 16 /* alert response signal handler */
 17 static void
 18 alert_response_cb (GtkDialog *alert, int response_id, gpointer user_data) {
 19   TfeWindow *win = TFE_WINDOW (user_data);
 20 
 21   gtk_window_destroy (GTK_WINDOW (alert));
 22   if (response_id == GTK_RESPONSE_ACCEPT) {
 23     if (win->is_quit)
 24       gtk_window_destroy(GTK_WINDOW (win));
 25     else
 26       notebook_page_close (win->nb);
 27   }
 28 }
 29 
 30 static gboolean
 31 close_request_cb (TfeWindow *win) {
 32   TfeAlert *alert;
 33 
 34   if (has_saved_all (GTK_NOTEBOOK (win->nb)))
 35     return false;
 36   else {
 37     win->is_quit = true;
 38     alert = TFE_ALERT (tfe_alert_new ());
 39     gtk_window_set_transient_for (GTK_WINDOW (alert), GTK_WINDOW (win));
 40     tfe_alert_set_message (alert, "Contents aren't saved yet.\nAre you sure to quit?");
 41     tfe_alert_set_button_label (alert, "Quit");
 42     g_signal_connect (GTK_DIALOG (alert), "response", G_CALLBACK (alert_response_cb), win);
 43     gtk_window_present (GTK_WINDOW (alert));
 44     return true;
 45   }
 46 }
 47 
 48 /* ----- action activated handlers ----- */
 49 static void
 50 open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 51   TfeWindow *win = TFE_WINDOW (user_data);
 52 
 53   notebook_page_open (GTK_NOTEBOOK (win->nb));
 54 }
 55 
 56 static void
 57 save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 58   TfeWindow *win = TFE_WINDOW (user_data);
 59 
 60   notebook_page_save (GTK_NOTEBOOK (win->nb));
 61 }
 62 
 63 static void
 64 close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 65   TfeWindow *win = TFE_WINDOW (user_data);
 66   TfeAlert *alert;
 67 
 68   if (has_saved (win->nb))
 69     notebook_page_close (win->nb);
 70   else {
 71     win->is_quit = false;
 72     alert = TFE_ALERT (tfe_alert_new ());
 73     gtk_window_set_transient_for (GTK_WINDOW (alert), GTK_WINDOW (win));
 74     tfe_alert_set_message (alert, "Contents aren't saved yet.\nAre you sure to close?");
 75     tfe_alert_set_button_label (alert, "Close");
 76     g_signal_connect (GTK_DIALOG (alert), "response", G_CALLBACK (alert_response_cb), win);
 77     gtk_widget_show (GTK_WIDGET (alert));
 78   }
 79 }
 80 
 81 static void
 82 new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 83   TfeWindow *win = TFE_WINDOW (user_data);
 84 
 85   notebook_page_new (GTK_NOTEBOOK (win->nb));
 86 }
 87 
 88 static void
 89 saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 90   TfeWindow *win = TFE_WINDOW (user_data);
 91 
 92   notebook_page_saveas (GTK_NOTEBOOK (win->nb));
 93 }
 94 
 95 static void
 96 pref_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 97   TfeWindow *win = TFE_WINDOW (user_data);
 98   GtkWidget *pref;
 99 
100   pref = tfe_pref_new ();
101   gtk_window_set_transient_for (GTK_WINDOW (pref), GTK_WINDOW (win));
102   gtk_window_present (GTK_WINDOW (pref));
103 }
104 
105 static void
106 close_all_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
107   TfeWindow *win = TFE_WINDOW (user_data);
108 
109   if (close_request_cb (win) == false)
110     gtk_window_destroy (GTK_WINDOW (win));
111 }
112 
113 /* --- public functions --- */
114 
115 void
116 tfe_window_notebook_page_new (TfeWindow *win) {
117   notebook_page_new (win->nb);
118 }
119 
120 void
121 tfe_window_notebook_page_new_with_files (TfeWindow *win, GFile **files, int n_files) {
122   int i;
123 
124   for (i = 0; i < n_files; i++)
125     notebook_page_new_with_file (win->nb, files[i]);
126   if (gtk_notebook_get_n_pages (win->nb) == 0)
127     notebook_page_new (win->nb);
128 }
129 
130 static void
131 tfe_window_init (TfeWindow *win) {
132   GtkBuilder *build;
133   GMenuModel *menu;
134 
135   gtk_widget_init_template (GTK_WIDGET (win));
136 
137   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/menu.ui");
138   menu = G_MENU_MODEL (gtk_builder_get_object (build, "menu"));
139   gtk_menu_button_set_menu_model (win->btnm, menu);
140   g_object_unref(build);
141 
142 /* ----- action ----- */
143   const GActionEntry win_entries[] = {
144     { "open", open_activated, NULL, NULL, NULL },
145     { "save", save_activated, NULL, NULL, NULL },
146     { "close", close_activated, NULL, NULL, NULL },
147     { "new", new_activated, NULL, NULL, NULL },
148     { "saveas", saveas_activated, NULL, NULL, NULL },
149     { "pref", pref_activated, NULL, NULL, NULL },
150     { "close-all", close_all_activated, NULL, NULL, NULL }
151   };
152   g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);
153 
154   g_signal_connect (GTK_WINDOW (win), "close-request", G_CALLBACK (close_request_cb), NULL);
155 }
156 
157 static void
158 tfe_window_class_init (TfeWindowClass *class) {
159   gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfewindow.ui");
160   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, btnm);
161   gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeWindow, nb);
162 }
163 
164 GtkWidget *
165 tfe_window_new (GtkApplication *app) {
166   return GTK_WIDGET (g_object_new (TFE_TYPE_WINDOW, "application", app, NULL));
167 }
~~~

- 7-12: `_TfeWindow` structure.
A TfeWindow instance points the structure.
- 14: `G_DEFINE_TYPE` macro.
- 17-28: `alert_response_cb` is a call back function for the "response" signal of TfeAlert dialog.
- 21: Destroys the alert dialog.
- 22-27: If the user has clicked on the accept button, it destroys the main window or closes the current notebook page.
- 30-46: A "close-request" signal handler on the TfeWindow.
When a user clicked on the close button (top right x-shaped button), the handler is called before the window closes.
If the handler returns true, the default handler isn't called and the window doesn't close.
If the handler returns false, the default handler is called and the window closes.
- 34: If `has_saved_all` returns true, the handler returns false and the window will close.
Otherwise, it shows an alert dialog.
- 48-111: Handlers of action activated signal.
The `user_data` is a pointer to the TfeWindow instance.
- 115-128: Public functions.
- 130-155: Instance initialization function.
- 135: The function `gtk_widget_init_template` creates a child widgets and initializes them.
- 137-140: Builds and inserts `menu`. It is inserted to the menu button.
- 143-152: Creates actions and inserts them to the window.
The scope of the actions is "win".
- 154: Connects the "close-request" signal and a handler.
- 157-162: Class initialization function.
- 159: Sets the composite widget template
- 160-161: Binds private variables with child class templates.
- 164-167: `tfe_window_new`.
This function creates TfeWindow instance.

## TfeApplication

The file `tfeaplication.h` and `tfeapplication.c` are now very simple.
The following is the header file.

~~~C
1 #pragma once
2 
3 #include <gtk/gtk.h>
4 
5 #define TFE_TYPE_APPLICATION tfe_application_get_type ()
6 G_DECLARE_FINAL_TYPE (TfeApplication, tfe_application, TFE, APPLICATION, GtkApplication)
7 
8 TfeApplication *
9 tfe_application_new (const char* application_id, GApplicationFlags flags);
~~~

- 1: `#pragma once` isn't an official pre-processor command, but widely used.
It makes the header file be read only once.
- 5-6: `TFE_TYPE_APPLICATION` is defined as the type of TfeApplication.
`G_DECLARE_FINAL_TYPE` is a macro used in the header file to define a new object.
- 8-9: The function `tfe_application_new` creates a new TfeApplication instance.

The following is `tfeapplication.c`.
It defines the application and supports:

- GSettings
- CSS

~~~C
  1 #include <gtk/gtk.h>
  2 #include "tfeapplication.h"
  3 #include "tfewindow.h"
  4 #include "pfd2css.h"
  5 
  6 struct _TfeApplication {
  7   GtkApplication parent;
  8   TfeWindow *win;
  9   GSettings *settings;
 10   GtkCssProvider *provider;
 11 };
 12 
 13 G_DEFINE_TYPE (TfeApplication, tfe_application, GTK_TYPE_APPLICATION);
 14 
 15 /* gsettings changed::font signal handler */
 16 static void
 17 changed_font_cb (GSettings *settings, char *key, gpointer user_data) {
 18   TfeApplication *app = TFE_APPLICATION (user_data);
 19   char *font, *s, *css;
 20   PangoFontDescription *pango_font_desc;
 21 
 22   font = g_settings_get_string (app->settings, "font");
 23   pango_font_desc = pango_font_description_from_string (font);
 24   g_free (font);
 25   s = pfd2css (pango_font_desc); // converts Pango Font Description into CSS style string
 26   css = g_strdup_printf ("textview {%s}", s);
 27   gtk_css_provider_load_from_data (app->provider, css, -1);
 28   g_free (s);
 29   g_free (css);
 30 }
 31 
 32 /* ----- activate, open, startup handlers ----- */
 33 static void
 34 app_activate (GApplication *application) {
 35   TfeApplication *app = TFE_APPLICATION (application);
 36 
 37   tfe_window_notebook_page_new (app->win);
 38   gtk_window_present (GTK_WINDOW (app->win));
 39 }
 40 
 41 static void
 42 app_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
 43   TfeApplication *app = TFE_APPLICATION (application);
 44 
 45   tfe_window_notebook_page_new_with_files (app->win, files, n_files);
 46   gtk_window_present (GTK_WINDOW (app->win));
 47 }
 48 
 49 static void
 50 app_startup (GApplication *application) {
 51   TfeApplication *app = TFE_APPLICATION (application);
 52   GtkCssProvider *provider0;
 53   GdkDisplay *display;
 54   int i;
 55 
 56   app->win = TFE_WINDOW (tfe_window_new (GTK_APPLICATION (app)));
 57   provider0 = gtk_css_provider_new ();
 58   gtk_css_provider_load_from_data (provider0, "textview {padding: 10px;}", -1);
 59   display = gdk_display_get_default ();
 60   gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider0),
 61                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
 62   g_object_unref (provider0);
 63   app->provider = gtk_css_provider_new ();
 64   gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (app->provider),
 65                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
 66   app->settings = g_settings_new ("com.github.ToshioCP.tfe");
 67   g_signal_connect (app->settings, "changed::font", G_CALLBACK (changed_font_cb), app);
 68   changed_font_cb(app->settings, "font", app);
 69 
 70 /* ----- accelerator ----- */ 
 71   struct {
 72     const char *action;
 73     const char *accels[2];
 74   } action_accels[] = {
 75     { "win.open", { "<Control>o", NULL } },
 76     { "win.save", { "<Control>s", NULL } },
 77     { "win.close", { "<Control>w", NULL } },
 78     { "win.new", { "<Control>n", NULL } },
 79     { "win.saveas", { "<Shift><Control>s", NULL } },
 80     { "win.close-all", { "<Control>q", NULL } },
 81   };
 82 
 83   for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
 84     gtk_application_set_accels_for_action(GTK_APPLICATION(app), action_accels[i].action, action_accels[i].accels);
 85 }
 86 
 87 static void
 88 tfe_application_dispose (GObject *gobject) {
 89   TfeApplication *app = TFE_APPLICATION (gobject);
 90 
 91   g_clear_object (&app->settings);
 92   g_clear_object (&app->provider);
 93   G_OBJECT_CLASS (tfe_application_parent_class)->dispose (gobject);
 94 }
 95 
 96 static void
 97 tfe_application_init (TfeApplication *app) {
 98   g_signal_connect (G_APPLICATION (app), "startup", G_CALLBACK (app_startup), NULL);
 99   g_signal_connect (G_APPLICATION (app), "activate", G_CALLBACK (app_activate), NULL);
100   g_signal_connect (G_APPLICATION (app), "open", G_CALLBACK (app_open), NULL);
101 }
102 
103 static void
104 tfe_application_class_init (TfeApplicationClass *class) {
105   GObjectClass *object_class = G_OBJECT_CLASS (class);
106 
107   object_class->dispose = tfe_application_dispose;
108 }
109 
110 TfeApplication *
111 tfe_application_new (const char* application_id, GApplicationFlags flags) {
112   return TFE_APPLICATION (g_object_new (TFE_TYPE_APPLICATION, "application-id", application_id, "flags", flags, NULL));
113 }
~~~

- 6-11: Defines `_TfeApplication` structure.
The members are:
  - win: main window instance
  - settings: GSettings instance.it is bound to "font" item in the GSettings
  - provider: a provider for the font of the textview.
- `G_DEFINE_TYPE` macro.
- 16-30: `changed_font_cb` is a handler for "changed::font" signal on the GSettings instance.
The signal name is "changed" and "font" is a key name.
When the valeu of "font" key is changed, the signal is emitted.
So, this handler doesn't directly relate to the font button, but through the GSettings database.
A user changes the font in the font button => GSettings font key data is changed => the handler is called.
- 22-24: Retrieves a string from the GSetting database and converts it into a pango font description.
- 25-29: Sets the css provider with the font data.
The provider has been inserted to the current display in advance.
- 33-39: Activate signal handler.
It uses `tfe_window_notebook_page_new` instead of `notebook_page_new`.
- 41-47: Open signal handler.
It just calls `tfe_window_notebook_page_new_with_files` and shows the main window.
Be careful that the activate and open handlers don't create a new window.
They just create a new notebook page.
Therefore, even if the second application runs, no new window appears.
Just a new notebook page is inserted to the same main window.
- 49-85: Startup signal handler.
- 56: Creates a new window (main window) and assigns it to `app->win`.
- 57-61: Creates a css provider (`provider0`).
It includes only the padding data for the textview.
The provider is inserted to the default display.
- 63-65: Another css provider is created (`app->provider`) and inserted to the default display.
It will include the font data for the textview.
- 66-68: Creates a new GSettings instance.
If the GSettings data is changed, the "changed" signal is emitted.
The signal can have a key name like "changed::font".
This style ("changed::font") is called detailed signal.
The detailed signal is emitted only if the font data is changed.
The handler `changed_font_cb` is called to set the CSS with the font data.
The handler gets the font data from the GSettings data which is the last font in the previous run of the application.
- 71-84: Defines accelerators.
- 87-94: A dispose handler. It releases references to the instances of GSettings and GtkCssProvider.
- 96-101: An initialization for the instance.
It connects three signals (activate, open and startup) and their handlers.
- 183-188: An initialization for the class.
It overrides the dispose class method.
- 110-113: `tfe_application_new` creates a new TfeApplication instance.
The parameters are an application-id and flags. 

## Other files

main.c

~~~C
 1 #include <gtk/gtk.h>
 2 #include "tfeapplication.h"
 3 
 4 #define APPLICATION_ID "com.github.ToshioCP.tfe"
 5 
 6 int
 7 main (int argc, char **argv) {
 8   TfeApplication *app;
 9   int stat;
10 
11   app = tfe_application_new (APPLICATION_ID, G_APPLICATION_HANDLES_OPEN);
12   stat =g_application_run (G_APPLICATION (app), argc, argv);
13   g_object_unref (app);
14   return stat;
15 }
16 
~~~

CSS related files `pfd2css.h` and `pfd2css.c` are the same as the previous section.

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
 7       <description>A font to be used for textview.</description>
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
 9 sourcefiles=files('main.c', 'tfeapplication.c', 'tfewindow.c', 'tfenotebook.c', 'tfepref.c', 'tfealert.c', 'pfd2css.c', '../tfetextview/tfetextview.c')
10 
11 executable('tfe', sourcefiles, resources, dependencies: gtkdep, export_dynamic: true, install: true)
12 
13 schema_dir = get_option('prefix') / get_option('datadir') / 'glib-2.0/schemas/'
14 install_data('com.github.ToshioCP.tfe.gschema.xml', install_dir: schema_dir)
15 gnome.post_install (glib_compile_schemas: true)
~~~

## Compilation and installation.

If you want to install it to your local area, use `--prefix=$HOME/.local` or `--prefix=$HOME` option.
If you want to install it to the system area, no option is needed.
It will be installed under `/user/local` directory.

~~~
$ meson --prefix=$HOME/.local _build
$ ninja -C _build
$ ninja -C _build install
~~~

You need root privilege to install it to the system area..

~~~
$ meson _build
$ ninja -C _build
$ sudo ninja -C _build install
~~~

Source files are in [src/tfe7](../src/tfe7) directory.

Composite widgets give us two advantages.

- A set of widgets is better than individual widgets because of the simple coding.
- They hold instance variables (members of the object structure) so static variables are no longer necessary.
It makes the program simpler.

We made a very small text editor.
You can add features to this editor.
When you add a new feature, be careful about the structure of the program.
Maybe you need to divide a file into several files like this section.
It isn't good to put many things into one file.
And it is important to think about the relationship between source files and widget structures.

Up: [README.md](../README.md),  Prev: [Section 20](sec20.md), Next: [Section 22](sec22.md)
