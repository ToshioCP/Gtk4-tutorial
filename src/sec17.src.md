# Menu and action

## Menu

Users often use menus to tell the command to the computer.
It is like this:

![Menu](../image/menu.png){width=5.985cm height=5.055cm}

Now let's analyze the menu above.
There are two types of object.

- "File", "Edit", "View", "Cut", "Copy", "Paste" and "Select All".
They are called "menu item" or simply "item".
When the user clicks one of these items, then something will happen.
- Menubar, submenu referenced by "Edit" item and two sections.
They are called "menu".
Menu is an ordered list of items.
They are similar to arrays.

![Menu structure](../image/menu_structure.png){width=10.23cm height=3.57cm}

- Menubar is a menu which has three items, which are "File", "Edit" and "View".
- The menu item labeled "Edit" has a link to the submenu which has two items.
These two items don't have labels.
Each item refers to a section.
- The first section is a menu which has three items -- "Cut", "Copy" and "Paste".
- The second section is a menu which has one item -- "Select All".

Menus can build a complicated structure thanks to the links of menu items.

## GMenuModel, GMenu and GMenuItem

GMenuModel is an abstract object which represents a menu.
GMenu is a simple implementation of GMenuModel and a child object of GMenuModel.

    GObject -- GMenuModel -- GMenu

Because GMenuModel is an abstract object, it isn't instantiatable.
Therefore, it doesn't have any functions to create it.
If you want to create a menu, use `g_menu_new` to create a GMenu instance.
GMenu inherits all the functions of GMenuModel because of the child object.

GMenuItem is an object directly derived from GObject.
GMenuItem and Gmenu (or GMenuModel) don't have a parent-child relationship.

    GObject -- GMenuModel -- GMenu
    GObject -- GMenuItem

GMenuItem has attributes.
One of the attributes is label.
For example, there is a menu item which has "Edit" label in the first diagram in this section.
"Cut", "Copy", "Paste" and "Select All" are also the labels of the menu items.
Other attributes will be explained later.

Some menu items have a link to another GMenu.
There are two types of links, submenu and section.

GMenuItem can be inserted, appended or prepended to GMenu.
When it is inserted, all of the attributes and link values of the item are copied and used to form a new item within the menu.
The GMenuItem itself is not really inserted.
Therefore, after the insertion, GMenuItem is useless and it should be freed.
The same goes for appending or prepending.

The following code shows how to append GMenuItem to GMenu.

    GMenu *menu = g_menu_new ();
    GMenuItem *menu_item_quit = g_menu_item_new ("Quit", "app.quit");
    g_menu_append_item (menu, menu_item_quit);
    g_object_unref (menu_item_quit);

## Menu and action

One of the attributes of menu items is an action.
This attribute points an action object.

There are two action objects, GSimpleAction and GPropertyAction.
GSimpleAction is often used.
And it is used with a menu item.
Only GSimpleAction is described in this section.

An action corresponds to a menu item will be activated when the menu item is clicked.
Then the action emits an activate signal.

1. menu item is clicked.
2. The corresponding action is activated.
3. The action emits a signal.
4. The connected handler is invoked.


The following code is an example.

    static void
    quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app) { ... ... ...}

    GSimpleAction *act_quit = g_simple_action_new ("quit", NULL);
    g_signal_connect (act_quit, "activate", G_CALLBACK (quit_activated), app);
    GMenuItem *menu_item_quit = g_menu_item_new ("Quit", "app.quit");

1. `menu_item_quit` is a menu item.
It has a label "Quit" and is connected to an action "app.quit".
"app" is a prefix and "quit" is the name of an action.
The prefix means that the action belongs to a GtkApplication instance.
If the menu is clicked, then the corresponding action "quit" which belongs to the GtkApplication will be activated.
2. `act_quit` is an action.
It has a name "quit".
It belongs to the GtkApplication, but it is not obvious in the code above.
The function `g_simple_action_new` creates a stateless action.
So, `act_quit` is stateless.
The meaning of stateless will be explained later.
The argument `NULL` means that the action doesn't have an parameter.
Most of the actions are stateless and have no parameter.
When `act_quit` is activated, it will emit "activate" signal.
3. "activate" signal of the action is connected to the handler `quit_activated`.
So, if the action is activated, the handler will be invoked.

## Simple example

The following is a simple example of menus and actions.

@@@include
menu/menu1.c
@@@

- 3-8: `quit_activated` is a handler of the "activate" signal on the  action `act_quit`.
Handlers of the "activate" signal have three parameters.
  1. The action instance on which the signal is emitted.
  2. Parameter.
In this example it is `NULL` because the second argument of `g_simple_action_new` (line 15) is `NULL`.
You don' t need to care about it.
  3. User data.
It is the fourth parameter in the `g_signal_connect` (line 18) that connects the action and the handler.
- 7: A function `g_application_quit` immediately quits the application.
- 10-34: `app_activate` is a handler of "activate" signal on the GtkApplication instance.
- 12-14: Creates a GtkApplicationWindow `win`. And sets the title and the default size.
- 16: Creates GSimpleAction `act_quit`.
It is stateless.
The first argument of `g_simple_action_new` is a name of the action and the second argument is a parameter.
If you don't need the parameter, pass `NULL`.
Therefore, `act_quit` has a name "quit" and no parameter.
- 17: Adds the action to GtkApplication `app`.
GtkApplication implements an interface GActionMap and GActionGroup.
GtkApplication (GActionMap) can have a group of actions and the actions are added with the function `g_action_map_add_action`.
This function is described in [GMenuModel section in GIO API reference](https://developer.gnome.org/gio/stable/GActionMap.html#g-action-map-add-action).
- 18: Connects "activate" signal of the action and the handler `quit_activated`.
- 20-23: Creates GMenu and GMenuItem instances.
`menubar` and `menu` are GMenu.
`menu_item_menu` and `menu_item_quit` are GMenuItem.
`menu_item_menu` has a label "Menu" and no action.
`menu_item_quit` has a label "Quit" and an action "app.quit".
The action "app.quit" is a combination of "app" and "quit".
"app" is a prefix and it means that the action belongs to GtkApplication. "quit" is the name of the action.
Therefore, "app.quit" points the action which belongs to the GtkApplication instance and is named "quit".
- 24-25: Appends `menu_item_quit` to `menu`.
As I mentioned before, all the attributes and links are copied and used to form a new item in `menu`.
Therefore after the appending, `menu_item_quit` is no longer needed.
It is freed by `g_object_unref`.
- 26: Sets the submenu link in `menu_item_menu` to point `menu`.
- 27-28: Appends `menu_item_menu` to `menubar`.
Then frees `menu_item_menu`.
GMenu and GMenuItem are connected and finally a menu is made up.
The structure of the menu is shown in the diagram below.
- 30: The menu is inserted to GtkApplication.
- 31: Sets GtkApplicationWindow to show the menubar.
- 32: Shows the window.

![menu and action](../image/menu1.png){width=12.555cm height=3.285cm}

![Screenshot of menu1](../image/menu1_screenshot.png){width=6.0cm height=5.115cm}

