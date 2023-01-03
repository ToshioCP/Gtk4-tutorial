Up: [README.md](../README.md),  Prev: [Section 18](sec18.md), Next: [Section 20](sec20.md)

# Ui file for menu and action entries

## Ui file for menu

You may have thought that building menus was really bothersome.
Yes, the program was complicated and it needs lots of time to code them.
The situation is similar to building widgets.
When we built widgets, using ui file was a good way to avoid such complication.
The same goes for menus.

The ui file for menus has interface and menu tags.
The file starts and ends with interface tags.

~~~xml
<interface>
  <menu id="menubar">
  </menu>
</interface>
~~~

`menu` tag corresponds to GMenu object.
`id` attribute defines the name of the object.
It will be referred by GtkBuilder.

~~~xml
<submenu>
  <attribute name="label">File</attribute>
    <item>
      <attribute name="label">New</attribute>
      <attribute name="action">win.new</attribute>
    </item>
</submenu>
~~~

`item` tag corresponds to an item in the GMenu which has the same structure as GMenuItem.
The item above has a label attribute.
Its value is "New".
The item also has an action attribute and its value is "win.new".
"win" is a prefix and "new" is an action name.
`submenu` tag corresponds to both GMenuItem and GMenu.
The GMenuItem has a link to GMenu.

The ui file above can be described as follows.

~~~xml
<item>
  <attribute name="label">File</attribute>
    <link name="submenu">
      <item>
        <attribute name="label">New</attribute>
        <attribute name="action">win.new</attribute>
      </item>
    </link>
</item>
~~~

`link` tag expresses the link to submenu.
And at the same time it also expresses the submenu itself.
This file illustrates the relationship between the menus and items better than the prior ui file.
But `submenu` tag is simple and easy to understand.
So, we usually prefer the former ui style.

For further information, see [GTK 4 API reference -- PopoverMenu](https://docs.gtk.org/gtk4/class.PopoverMenu.html#menu-models).

The following is a screenshot of the sample program `menu3`.
It is located in the directory [src/menu3](../src/menu3).

![menu3](../image/menu3.png)

The following is the ui file for `menu3`.

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <interface>
 3   <menu id="menubar">
 4     <submenu>
 5       <attribute name="label">File</attribute>
 6       <section>
 7         <item>
 8           <attribute name="label">New</attribute>
 9           <attribute name="action">app.new</attribute>
10         </item>
11         <item>
12           <attribute name="label">Open</attribute>
13           <attribute name="action">app.open</attribute>
14         </item>
15       </section>
16       <section>
17         <item>
18           <attribute name="label">Save</attribute>
19           <attribute name="action">win.save</attribute>
20         </item>
21         <item>
22           <attribute name="label">Save As…</attribute>
23           <attribute name="action">win.saveas</attribute>
24         </item>
25       </section>
26       <section>
27         <item>
28           <attribute name="label">Close</attribute>
29           <attribute name="action">win.close</attribute>
30         </item>
31       </section>
32       <section>
33         <item>
34           <attribute name="label">Quit</attribute>
35           <attribute name="action">app.quit</attribute>
36         </item>
37       </section>
38     </submenu>
39     <submenu>
40       <attribute name="label">Edit</attribute>
41       <section>
42         <item>
43           <attribute name="label">Cut</attribute>
44           <attribute name="action">app.cut</attribute>
45         </item>
46         <item>
47           <attribute name="label">Copy</attribute>
48           <attribute name="action">app.copy</attribute>
49         </item>
50         <item>
51           <attribute name="label">Paste</attribute>
52           <attribute name="action">app.paste</attribute>
53         </item>
54       </section>
55       <section>
56         <item>
57           <attribute name="label">Select All</attribute>
58           <attribute name="action">app.selectall</attribute>
59         </item>
60       </section>
61     </submenu>
62     <submenu>
63       <attribute name="label">View</attribute>
64       <section>
65         <item>
66           <attribute name="label">Full Screen</attribute>
67           <attribute name="action">win.fullscreen</attribute>
68         </item>
69       </section>
70     </submenu>
71   </menu>
72 </interface>
~~~

The ui file is converted to the resource by the resource compiler `glib-compile-resouces` with xml file.

~~~xml
1 <?xml version="1.0" encoding="UTF-8"?>
2 <gresources>
3   <gresource prefix="/com/github/ToshioCP/menu3">
4     <file>menu3.ui</file>
5   </gresource>
6 </gresources>
~~~

GtkBuilder builds menus from the resource.

~~~C
GtkBuilder *builder = gtk_builder_new_from_resource ("/com/github/ToshioCP/menu3/menu3.ui");
GMenuModel *menubar = G_MENU_MODEL (gtk_builder_get_object (builder, "menubar"));

gtk_application_set_menubar (GTK_APPLICATION (app), menubar);
g_object_unref (builder);
~~~

The builder instance is freed after the GMenuModel `menubar` is inserted to the application.
If you do it before the insertion, bad thing will happen -- your computer might freeze.

## Action entry

The coding for building actions and signal handlers is bothersome work as well.
Therefore, it should be automated.
You can implement them easily with GActionEntry structure and `g_action_map_add_action_entries` function.

GActionEntry contains action name, signal handlers, parameter and state.

~~~C
typedef struct _GActionEntry GActionEntry;

struct _GActionEntry
{
  /* action name */
  const char *name;
  /* activate handler */
  void (* activate) (GSimpleAction *action, GVariant *parameter, gpointer user_data);
  /* the type of the parameter given as a single GVariant type string */
  const char *parameter_type;
  /* initial state given in GVariant text format */
  const char *state;
  /* change-state handler */
  void (* change_state) (GSimpleAction *action, GVariant *value, gpointer user_data);
  /*< private >*/
  gsize padding[3];
};
~~~
For example, the actions in the previous section are:

~~~C
{ "fullscreen", NULL, NULL, "false", fullscreen_changed }
{ "color", color_activated, "s", "'red'", NULL }
{ "quit", quit_activated, NULL, NULL, NULL },
~~~

- Fullscreen action is stateful, but doesn't have parameters.
So, the third element (parameter type) is NULL.
[GVariant text format](https://docs.gtk.org/glib/gvariant-text.html) provides "true" and "false" as boolean GVariant values.
The initial state of the action is false (the fourth element).
It doesn't have activate handler, so the second element is NULL.
Instead, it has change-state handler.
The fifth element `fullscreen_changed` is the handler.
- Color action is stateful and has a parameter.
The parameter type is string.
[GVariant format strings](https://docs.gtk.org/glib/gvariant-format-strings.html) provides string formats to represent GVariant types.
The third element "s" means GVariant string type.
GVariant text format defines that strings are surrounded by single or double quotes.
So, the string red is 'red' or "red".
The fourth element is `"'red'"`, which is a C string format and the string is 'red'.
You can write `"\"red\""` instead.
The second element color\_activated is the activate handler.
The action doesn't have change-state handler, so the fifth element is NULL.
- Quit action is non-stateful and has no parameter.
So, the third and fourth elements are NULL.
The second element quit\_activated is the activate handler.
The action doesn't have change-state handler, so the fifth element is NULL.

The function `g_action_map_add_action_entries` does everything
to create GSimpleAction instances and add them to a GActionMap (an application or window).

~~~C
const GActionEntry app_entries[] = {
  { "color", color_activated, "s", "'red'", NULL },
  { "quit", quit_activated, NULL, NULL, NULL }
};
g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries,
                                 G_N_ELEMENTS (app_entries), app);
~~~

The code above does:

- Builds the "color" and "quit" actions
- Connects the action and the "activate" signal handlers (color\_activated and quit\_activated).
- Adds the actions to the action map `app`.

The same goes for the other action.

~~~C
const GActionEntry win_entries[] = {
  { "fullscreen", NULL, NULL, "false", fullscreen_changed }
};
g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries,
                                 G_N_ELEMENTS (win_entries), win);
~~~
The code above does:

- Builds the "fullscreen" action.
- Connects the action and the signal handler `fullscreen_changed`
- Its initial state is set to false.
- Adds the action to the action map `win`.

## Example

Source files are `menu3.c`, `menu3.ui`, `menu3.gresource.xml` and `meson.build`.
They are in the directory [src/menu3](../src/menu3).
The following are `menu3.c` and `meson.build`.

~~~C
  1 #include <gtk/gtk.h>
  2 
  3 static void
  4 new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  5 }
  6 
  7 static void
  8 open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  9 }
 10 
 11 static void
 12 save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 13 }
 14 
 15 static void
 16 saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 17 }
 18 
 19 static void
 20 close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 21   GtkWindow *win = GTK_WINDOW (user_data);
 22 
 23   gtk_window_destroy (win);
 24 }
 25 
 26 static void
 27 cut_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 28 }
 29 
 30 static void
 31 copy_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 32 }
 33 
 34 static void
 35 paste_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 36 }
 37 
 38 static void
 39 selectall_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
 40 }
 41 
 42 static void
 43 fullscreen_changed (GSimpleAction *action, GVariant *state, gpointer user_data) {
 44   GtkWindow *win = GTK_WINDOW (user_data);
 45 
 46   if (g_variant_get_boolean (state))
 47     gtk_window_maximize (win);
 48   else
 49     gtk_window_unmaximize (win);
 50   g_simple_action_set_state (action, state);
 51 }
 52 
 53 static void
 54 quit_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data)
 55 {
 56   GApplication *app = G_APPLICATION (user_data);
 57 
 58   g_application_quit (app);
 59 }
 60 
 61 static void
 62 app_activate (GApplication *app) {
 63   GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));
 64 
 65   const GActionEntry win_entries[] = {
 66     { "save", save_activated, NULL, NULL, NULL },
 67     { "saveas", saveas_activated, NULL, NULL, NULL },
 68     { "close", close_activated, NULL, NULL, NULL },
 69     { "fullscreen", NULL, NULL, "false", fullscreen_changed }
 70   };
 71   g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);
 72 
 73   gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (win), TRUE);
 74 
 75   gtk_window_set_title (GTK_WINDOW (win), "menu3");
 76   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
 77   gtk_widget_show (win);
 78 }
 79 
 80 static void
 81 app_startup (GApplication *app) {
 82   GtkBuilder *builder = gtk_builder_new_from_resource ("/com/github/ToshioCP/menu3/menu3.ui");
 83   GMenuModel *menubar = G_MENU_MODEL (gtk_builder_get_object (builder, "menubar"));
 84 
 85   gtk_application_set_menubar (GTK_APPLICATION (app), menubar);
 86   g_object_unref (builder);
 87 
 88   const GActionEntry app_entries[] = {
 89     { "new", new_activated, NULL, NULL, NULL },
 90     { "open", open_activated, NULL, NULL, NULL },
 91     { "cut", cut_activated, NULL, NULL, NULL },
 92     { "copy", copy_activated, NULL, NULL, NULL },
 93     { "paste", paste_activated, NULL, NULL, NULL },
 94     { "selectall", selectall_activated, NULL, NULL, NULL },
 95     { "quit", quit_activated, NULL, NULL, NULL }
 96   };
 97   g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries, G_N_ELEMENTS (app_entries), app);
 98 }
 99 
100 #define APPLICATION_ID "com.github.ToshioCP.menu3"
101 
102 int
103 main (int argc, char **argv) {
104   GtkApplication *app;
105   int stat;
106 
107   app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
108   g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
109   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
110 
111   stat =g_application_run (G_APPLICATION (app), argc, argv);
112   g_object_unref (app);
113   return stat;
114 }
115 
~~~

meson.build

~~~meson
 1 project('menu3', 'c')
 2 
 3 gtkdep = dependency('gtk4')
 4 
 5 gnome=import('gnome')
 6 resources = gnome.compile_resources('resources','menu3.gresource.xml')
 7 
 8 sourcefiles=files('menu3.c')
 9 
10 executable('menu3', sourcefiles, resources, dependencies: gtkdep)
~~~

Action handlers need to follow the following format.

~~~C
static void
handler (GSimpleAction *action_name, GVariant *parameter, gpointer user_data) { ... ... ... }
~~~

You can't write, for example, "GApplication *app" instead of "gpointer user_data".
Because `g_action_map_add_action_entries` expects that handlers follow the format above.

There are `menu2_ui.c` and `menu2.ui` under the `menu` directory.
They are other examples to show menu ui file and `g_action_map_add_action_entries`.
It includes a stateful action with parameters.

~~~xml
<item>
  <attribute name="label">Red</attribute>
  <attribute name="action">app.color</attribute>
  <attribute name="target">red</attribute>
</item>
~~~

Action name and target are separated like this.
Action attribute includes prefix and name only.
You can't write like `<attribute name="action">app.color::red</attribute>`.

Up: [README.md](../README.md),  Prev: [Section 18](sec18.md), Next: [Section 20](sec20.md)
