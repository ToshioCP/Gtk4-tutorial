Up: [README.md](../README.md),  Prev: [Section 18](sec18.md), Next: [Section 20](sec20.md)

# Ui File for Menu and Action Entries

## Ui File for Menu

You may have thought that building menus was really bothersome.
Yes, the program was complicated and it needs lots of time to code them.
The situation is similar to building widgets.
When we built widgets, using UI files was a good way to avoid such complication.
The same goes for menus.

The UI file for menus has interface and `<menu>` tags.
The file starts and ends with interface tags.

~~~xml
<interface>
  <menu id="menubar">
  </menu>
</interface>
~~~

The `<menu>` tag corresponds to a GMenu object.
The `id` attribute defines the name of the object.
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

The `<item>` tag corresponds to an item in the GMenu which has the same structure as GMenuItem.
The item above has a label attribute.
Its value is "New".
The item also has an action attribute and its value is "win.new".
"win" is a prefix and "new" is an action name.
The `<submenu>` tag corresponds to both GMenuItem and GMenu.
The GMenuItem has a link to GMenu.

The UI file above can be described as follows.

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

The `<link>` tag expresses the link to submenu.
And at the same time it also expresses the submenu itself.
This file illustrates the relationship between the menus and items better than the prior UI file.
But `<submenu>` tags are simple and easy to understand.
So, we usually prefer the former UI style.

For further information, see [GTK 4 API reference -- PopoverMenu](https://docs.gtk.org/gtk4/class.PopoverMenu.html#menu-models).

The following is a screenshot of the sample program `menu3`.
It is located in the directory [/src/menu3](../src/menu3/).

![menu3](/src/images/menu3.png)

The following is the UI file for `menu3`.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <menu id="menubar">
    <submenu>
      <attribute name="label">File</attribute>
      <section>
        <item>
          <attribute name="label">New</attribute>
          <attribute name="action">app.new</attribute>
        </item>
        <item>
          <attribute name="label">Open</attribute>
          <attribute name="action">app.open</attribute>
        </item>
      </section>
      <section>
        <item>
          <attribute name="label">Save</attribute>
          <attribute name="action">win.save</attribute>
        </item>
        <item>
          <attribute name="label">Save As…</attribute>
          <attribute name="action">win.saveas</attribute>
        </item>
      </section>
      <section>
        <item>
          <attribute name="label">Close</attribute>
          <attribute name="action">win.close</attribute>
        </item>
      </section>
      <section>
        <item>
          <attribute name="label">Quit</attribute>
          <attribute name="action">app.quit</attribute>
        </item>
      </section>
    </submenu>
    <submenu>
      <attribute name="label">Edit</attribute>
      <section>
        <item>
          <attribute name="label">Cut</attribute>
          <attribute name="action">app.cut</attribute>
        </item>
        <item>
          <attribute name="label">Copy</attribute>
          <attribute name="action">app.copy</attribute>
        </item>
        <item>
          <attribute name="label">Paste</attribute>
          <attribute name="action">app.paste</attribute>
        </item>
      </section>
      <section>
        <item>
          <attribute name="label">Select All</attribute>
          <attribute name="action">app.selectall</attribute>
        </item>
      </section>
    </submenu>
    <submenu>
      <attribute name="label">View</attribute>
      <section>
        <item>
          <attribute name="label">Maximized</attribute>
          <attribute name="action">win.maximize</attribute>
        </item>
      </section>
    </submenu>
  </menu>
</interface>
```

The UI file is converted to the resource by the resource compiler `glib-compile-resouces` with xml file.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<gresources>
  <gresource prefix="/com/github/ToshioCP/menu3">
    <file>menu3.ui</file>
  </gresource>
</gresources>
```

GtkBuilder builds menus from the resource.

~~~C
GtkBuilder *builder = gtk_builder_new_from_resource ("/com/github/ToshioCP/menu3/menu3.ui");
GMenuModel *menubar = G_MENU_MODEL (gtk_builder_get_object (builder, "menubar"));

gtk_application_set_menubar (GTK_APPLICATION (app), menubar);
g_object_unref (builder);
~~~

The builder instance is freed after the GMenuModel `menubar` is inserted to the application.
If you do it before the insertion, bad thing will happen -- your computer might freeze.
It is because you don't own the `menubar` instance.
The function `gtk_builder_get_object` just returns the pointer to `menubar` and doesn't increase the reference count of `menubar`.
So, if you released `bulder` before `gtk_application_set_menubar`, `builder` would be destroyed and `menubar` as well.

## Action Entry

The coding for building actions and signal handlers is complex work as well.
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
{ "maximize", NULL, NULL, "false", maximize_state_changed }
{ "color", color_activated, "s", "'red'", NULL }
{ "quit", quit_activated, NULL, NULL, NULL },
~~~

- The "maximize" action is stateful, but doesn't have parameters.
So, the third element (parameter type) is NULL.
[GVariant text format](https://docs.gtk.org/glib/gvariant-text-format.html) provides "true" and "false" as boolean GVariant values.
The initial state of the action is false (the fourth element).
It doesn't have activate handler, so the second element is NULL.
Instead, it has change-state handler.
The fifth element `maximize_state_changed` is the handler.
- THe "color" action is stateful and has a parameter.
The parameter type is string.
[GVariant format strings](https://docs.gtk.org/glib/gvariant-format-strings.html) provides string formats to represent GVariant types.
The third element "s" means GVariant string type.
GVariant text format defines that strings are surrounded by single or double quotes.
So, the string red is 'red' or "red".
The fourth element is `"'red'"`, which is a C string format and the string is 'red'.
You can write `"\"red\""` instead.
The second element `color_activated` is the activate handler.
The action doesn't have change-state handler, so the fifth element is NULL.
- The "quit" action is non-stateful and has no parameter.
So, the third and fourth elements are NULL.
The second element `quit_activated` is the activate handler.
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

- Builds the "color" and the "quit" actions
- Connects the action and the "activate" signal handlers (`color_activated` and `quit_activated`).
- Adds the actions to the action map `app`.

The same goes for the other action.

~~~C
const GActionEntry win_entries[] = {
  { "maximize", NULL, NULL, "false", maximize_state_changed }
};
g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries,
                                 G_N_ELEMENTS (win_entries), win);
~~~
The code above performs the following:

- Builds the "maximize" action.
- Connects the action and the signal handler `maximize_state_changed`
- Its initial state is set to false.
- Adds the action to the action map `win`.

## Example

Source files are `menu3.c`, `menu3.ui`, `menu3.gresource.xml` and `meson.build`.
They are in the directory [/src/menu3](../src/menu3/).
The following are `menu3.c` and `meson.build`.

```c
#include <gtk/gtk.h>

static void
new_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
}

static void
open_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
}

static void
save_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
}

static void
saveas_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
}

static void
close_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GtkWindow *win = GTK_WINDOW (user_data);

  gtk_window_destroy (win);
}

static void
cut_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
}

static void
copy_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
}

static void
paste_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
}

static void
selectall_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
}

static void
maximize_state_changed (GSimpleAction *action, GVariant *state, gpointer user_data) {
  GtkWindow *win = GTK_WINDOW (user_data);

  if (g_variant_get_boolean (state))
    gtk_window_maximize (win);
  else
    gtk_window_unmaximize (win);
  g_simple_action_set_state (action, state);
}

static void
quit_activated (GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  GApplication *app = G_APPLICATION (user_data);

  g_application_quit (app);
}

static void
app_activate (GApplication *app) {
  GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));

  const GActionEntry win_entries[] = {
    { "save", save_activated, NULL, NULL, NULL },
    { "saveas", saveas_activated, NULL, NULL, NULL },
    { "close", close_activated, NULL, NULL, NULL },
    { "maximize", NULL, NULL, "false", maximize_state_changed }
  };
  g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), win);

  gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (win), TRUE);

  gtk_window_set_title (GTK_WINDOW (win), "menu3");
  gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_startup (GApplication *app) {
  GtkBuilder *builder = gtk_builder_new_from_resource ("/com/github/ToshioCP/menu3/menu3.ui");
  GMenuModel *menubar = G_MENU_MODEL (gtk_builder_get_object (builder, "menubar"));

  gtk_application_set_menubar (GTK_APPLICATION (app), menubar);
  g_object_unref (builder);

  const GActionEntry app_entries[] = {
    { "new", new_activated, NULL, NULL, NULL },
    { "open", open_activated, NULL, NULL, NULL },
    { "cut", cut_activated, NULL, NULL, NULL },
    { "copy", copy_activated, NULL, NULL, NULL },
    { "paste", paste_activated, NULL, NULL, NULL },
    { "selectall", selectall_activated, NULL, NULL, NULL },
    { "quit", quit_activated, NULL, NULL, NULL }
  };
  g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries, G_N_ELEMENTS (app_entries), app);
}

#define APPLICATION_ID "com.github.ToshioCP.menu3"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);

  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

```

meson.build

```
project('menu3', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','menu3.gresource.xml')

sourcefiles=files('menu3.c')

executable('menu3', sourcefiles, resources, dependencies: gtkdep)
```

Action handlers need to follow the following format.

~~~C
static void
handler (GSimpleAction *action_name, GVariant *parameter, gpointer user_data) { ... ... ... }
~~~

You can't write, for example, "GApplication *app" instead of "gpointer user_data".
Because `g_action_map_add_action_entries` expects that handlers follow the format above.

There are `menu2_ui.c` and `menu2.ui` under the `menu` directory.
They are other examples to show a menu UI file and `g_action_map_add_action_entries`.
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
