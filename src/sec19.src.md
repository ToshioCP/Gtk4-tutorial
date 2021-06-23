# Ui file for menu and action entries

## Ui file for menu

You might have thought that building menus is really bothersome.
Yes, the program was complicated and it needs lots of time to code it.
The situation is similar to building widgets.
When we built widgets, using ui file was a good way to avoid such complicated coding.
The same goes for menus.

The ui file for menus has interface, menu tags.
The file starts and ends with interface tag.

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

`item` tag corresponds to item in GMenu which has the same structure as GMenuItem.
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
So, we usually prefer the former ui file style.

The following is a screenshot of the sample program in this section.
Its name is `menu3`.

![menu3](../image/menu3.png){width=6.0cm height=5.055cm}

The following is the ui file of the menu in `menu3`.

@@@include
menu3/menu3.ui
@@@

The ui file is converted to the resource by the resource compiler `glib-compile-resouces` with xml file below.

@@@include
menu3/menu3.gresource.xml
@@@

GtkBuilder builds menus from the resource.

~~~C
GtkBuilder *builder = gtk_builder_new_from_resource ("/com/github/ToshioCP/menu3/menu3.ui");
GMenuModel *menubar = G_MENU_MODEL (gtk_builder_get_object (builder, "menubar"));

gtk_application_set_menubar (GTK_APPLICATION (app), menubar);
g_object_unref (builder);
~~~

It is important that `builder` is unreferred after the GMenuModel `menubar` is inserted to the application.
If you do it before setting, bad thing will happen -- your computer might freeze.

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
  const gchar *name;
  /* activate handler */
  void (* activate) (GSimpleAction *action, GVariant *parameter, gpointer user_data);
  /* the type of the parameter given as a single GVariant type string */
  const gchar *parameter_type;
  /* initial state given in GVariant text format */
  const gchar *state;
  /* change-state handler */
  void (* change_state) (GSimpleAction *action, GVariant *value, gpointer user_data);
  /*< private >*/
  gsize padding[3];
};
~~~
For example, the actions in the previous section are:

~~~C
{ "fullscreen", NULL, NULL, "false", fullscreen_changed }
{ "color", color_activated, "s", "red", NULL }
{ "quit", quit_activated, NULL, NULL, NULL },
~~~

And `g_action_map_add_action_entries` does all the process instead of the functions you have needed.

~~~C
const GActionEntry app_entries[] = {
  { "quit", quit_activated, NULL, NULL, NULL }
};
g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries,
                                 G_N_ELEMENTS (app_entries), app);
~~~

The code above does:

- Builds the "quit" action
- Connects the action and the "activate" signal handler `quit_activated`
- Adds the action to the action map `app`.

The same goes for the other actions.

~~~C
const GActionEntry win_entries[] = {
  { "fullscreen", NULL, NULL, "false", fullscreen_changed },
  { "color", color_activated, "s", "red", NULL }
};
g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries,
                                 G_N_ELEMENTS (win_entries), win);
~~~
The code above does:

- Builds a "fullscreen" action and "color" action.
- Connects the "fullscreen" action and the "change-state" signal handler `fullscreen_changed`
- Its initial state is set to FALSE.
- Connects the "color" action and the "activate" signal handler `color_activated`
- Its parameter type is string and the initial value is "red".
- Adds the actions to the action map `win`.

## Example code

The C source code of `menu3` and `meson.build` is as follows.

@@@include
menu3/menu3.c
@@@

meson.build

@@@include
menu3/meson.build
@@@

