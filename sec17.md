Up: [Readme.md](Readme.md),  Prev: [Section 16](sec16.md)

# Ui file for menu and action entries

## Ui file for menu

You might have thought that building menus is really bothersome.
Yes, the program was complicated and it needs lots of time to code it.
The situation is similar to building widgets.
When we built widgets, using ui file was a good way to avoid such complicated coding.
The same goes for menus.

The ui file for menus has interface, menu tags.
The file starts and ends with interface tag.

    <interface>
      <menu id="menubar">
      </menu>
    </interface>

`menu` tag corresponds to GMenu object.
`id` attribute defines the name of the object.
It will be refered by GtkBuilder.

    <submenu>
      <attribute name="label">File</attribute>
        <item>
          <attribute name="label">New</attribute>
          <attribute name="action">win.new</attribute>
        </item>
    </submenu>

`item` tag corresponds to item in GMenu which has the same structure as GMenuItem.
The item above has a label attribute.
Its value is "New".
The item also has an action attribute and its value is "win.new".
"win" is a prefix and "new" is an action name.
`submenu` tag corresponds to both GMenuItem and GMenu.
The GMenuItem has a link to GMenu.

The ui file above can be described as follows.

    <item>
      <attribute name="label">File</attribute>
        <link name="submenu">
          <item>
            <attribute name="label">New</attribute>
            <attribute name="action">win.new</attribute>
          </item>
        </link>
    </item>

`link` tag expresses the link to submenu.
And at the same time it also expresses the submenu itself.
This file illustrates the relationship between the menus and items better than the prior ui file.
But `submenu` tag is simple and easy to understand.
So, we usually prefer the former ui file style.

The following is a screenshot of the sample program in this section.
Its name is `menu3`.

![menu3](image/menu3.png)

The following is the ui file of the menu in `menu3`.

     1 <?xml version="1.0" encoding="UTF-8"?>
     2 <interface>
     3   <menu id="menubar">
     4     <submenu>
     5       <attribute name="label">File</attribute>
     6       <section>
     7         <item>
     8           <attribute name="label">New</attribute>
     9           <attribute name="action">win.new</attribute>
    10         </item>
    11         <item>
    12           <attribute name="label">Open</attribute>
    13           <attribute name="action">win.open</attribute>
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
    44           <attribute name="action">win.cut</attribute>
    45         </item>
    46         <item>
    47           <attribute name="label">Copy</attribute>
    48           <attribute name="action">win.copy</attribute>
    49         </item>
    50         <item>
    51           <attribute name="label">Paste</attribute>
    52           <attribute name="action">win.paste</attribute>
    53         </item>
    54       </section>
    55       <section>
    56         <item>
    57           <attribute name="label">Select All</attribute>
    58           <attribute name="action">win.selectall</attribute>
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

The ui file is converted to the resource by the resouce compiler `glib-compile-resouces` with xml file below.

    1 <?xml version="1.0" encoding="UTF-8"?>
    2 <gresources>
    3   <gresource prefix="/com/github/ToshioCP/menu3">
    4     <file>menu3.ui</file>
    5   </gresource>
    6 </gresources>

GtkBuilder builds menus from the resource.

    GtkBuilder *builder = gtk_builder_new_from_resource ("/com/github/ToshioCP/menu3/menu3.ui");
    GMenuModel *menubar = G_MENU_MODEL (gtk_builder_get_object (builder, "menubar"));

    gtk_application_set_menubar (GTK_APPLICATION (app), menubar);
    g_object_unref (builder);

It is important that `builder` is unreferred after the GMenuModel `menubar` is set to the application.
If you do it before setting, bad thing will happen -- your computer might freeze.

## Action entry

The coding for building actions and signal handlers is bothersome work as well.
Therefore, it should be automated.
You can implement them easily with GActionEntry structure and `g_action_map_add_action_entries` function.

GActionEntry contains action name, signal handlers, parameter and state.

    typedef struct _GActionEntry GActionEntry;

    struct _GActionEntry
    {
      const gchar *name; /* action name */
      void (* activate) (GSimpleAction *action, GVariant *parameter, gpointer user_data); /* activate handler */
      const gchar *parameter_type; /* the type of the parameter given as a single GVariant type string */
      const gchar *state; /* initial state given in GVariant text format */
      void (* change_state) (GSimpleAction *action, GVariant *value, gpointer user_data); /* change-state handler */
      /*< private >*/
      gsize padding[3];
    };

For example, the actions in the previous section are:

    { "fullscreen", NULL, NULL, "false", fullscreen_changed }
    { "color", color_activated, "s", "red", NULL }
    { "quit", quit_activated, NULL, NULL, NULL },

And `g_action_map_add_action_entries` does all the process instead of the functions you have needed.

    const GActionEntry app_entries[] = {
      { "quit", quit_activated, NULL, NULL, NULL }
    };
    g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries, G_N_ELEMENTS (app_entries), app);

The code above does:

- Build the "quit" action
- Connect the action and the "activate" signal handler `quit_activate`
- Add the action to the action map `app`.

The same goes for the other actions.

    const GActionEntry win_entries[] = {
      { "fullscreen", NULL, NULL, "false", fullscreen_changed },
      { "color", color_activated, "s", "red", NULL }
    };
    g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);

The code above does:

- Build a "fullscreen" action and "color" action.
- Connect the "fullscreen" action and the "change-state" signal handler `fullscreen_changed`
- Its initial state is set to FALSE.
- Connect the "color" action and the "activate" signal handler `color_activate`
- Its parameter type is string and the initial value is "red".
- Add the actions to the action map `win`.

## Example code

The C source code of `menu3` and `meson.build` is as follows.

      1 #include <gtk/gtk.h>
      2 
      3 static void
      4 new_activated (GSimpleAction *action, GVariant *parameter, gpointer win) {
      5 }
      6 
      7 static void
      8 open_activated (GSimpleAction *action, GVariant *parameter, gpointer win) {
      9 }
     10 
     11 static void
     12 save_activated (GSimpleAction *action, GVariant *parameter, gpointer win) {
     13 }
     14 
     15 static void
     16 saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer win) {
     17 }
     18 
     19 static void
     20 close_activated (GSimpleAction *action, GVariant *parameter, gpointer win) {
     21 }
     22 
     23 static void
     24 cut_activated (GSimpleAction *action, GVariant *parameter, gpointer win) {
     25 }
     26 
     27 static void
     28 copy_activated (GSimpleAction *action, GVariant *parameter, gpointer win) {
     29 }
     30 
     31 static void
     32 paste_activated (GSimpleAction *action, GVariant *parameter, gpointer win) {
     33 }
     34 
     35 static void
     36 selectall_activated (GSimpleAction *action, GVariant *parameter, gpointer win) {
     37 }
     38 
     39 static void
     40 fullscreen_changed (GSimpleAction *action, GVariant *state, gpointer win) {
     41   if (g_variant_get_boolean (state))
     42     gtk_window_maximize (GTK_WINDOW (win));
     43   else
     44     gtk_window_unmaximize (GTK_WINDOW (win));
     45   g_simple_action_set_state (action, state);
     46 }
     47 
     48 static void
     49 quit_activated (GSimpleAction *action, GVariant *parameter, gpointer app)
     50 {
     51   g_application_quit (G_APPLICATION(app));
     52 }
     53 
     54 static void
     55 on_activate (GApplication *app, gpointer user_data) {
     56   GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));
     57 
     58   const GActionEntry win_entries[] = {
     59     { "new", new_activated, NULL, NULL, NULL },
     60     { "open", open_activated, NULL, NULL, NULL },
     61     { "save", save_activated, NULL, NULL, NULL },
     62     { "saveas", saveas_activated, NULL, NULL, NULL },
     63     { "close", close_activated, NULL, NULL, NULL },
     64     { "cut", cut_activated, NULL, NULL, NULL },
     65     { "copy", copy_activated, NULL, NULL, NULL },
     66     { "paste", paste_activated, NULL, NULL, NULL },
     67     { "selectall", selectall_activated, NULL, NULL, NULL },
     68     { "fullscreen", NULL, NULL, "false", fullscreen_changed }
     69   };
     70   g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);
     71 
     72   gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (win), TRUE);
     73 
     74   gtk_window_set_title (GTK_WINDOW (win), "menu3");
     75   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
     76   gtk_widget_show (win);
     77 }
     78 
     79 static void
     80 on_startup (GApplication *app, gpointer user_data) {
     81   GtkBuilder *builder = gtk_builder_new_from_resource ("/com/github/ToshioCP/menu3/menu3.ui");
     82   GMenuModel *menubar = G_MENU_MODEL (gtk_builder_get_object (builder, "menubar"));
     83 
     84   gtk_application_set_menubar (GTK_APPLICATION (app), menubar);
     85   g_object_unref (builder);
     86 
     87   const GActionEntry app_entries[] = {
     88     { "quit", quit_activated, NULL, NULL, NULL }
     89   };
     90   g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries, G_N_ELEMENTS (app_entries), app);
     91 }
     92 
     93 int
     94 main (int argc, char **argv) {
     95   GtkApplication *app;
     96   int stat;
     97 
     98   app = gtk_application_new ("com.github.ToshioCP.menu3", G_APPLICATION_FLAGS_NONE);
     99   g_signal_connect (app, "startup", G_CALLBACK (on_startup), NULL);
    100   g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
    101 
    102   stat =g_application_run (G_APPLICATION (app), argc, argv);
    103   g_object_unref (app);
    104   return stat;
    105 }
    106 

meson.build

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


Up: [Readme.md](Readme.md),  Prev: [Section 16](sec16.md)
