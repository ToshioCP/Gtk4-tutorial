Up: [Readme.md](Readme.md),  Prev: [Section 15](sec15.md), Next: [Section 17](sec17.md)

# Menu and action

## Menu

Users often use menus to tell the command to the computer.
It is like this:

![Menu](image/menu.png)

Now let's analyze the menu above.
There are two types of object.

- "File", "Edit", "View", "Cut", "Copy", "Paste" and "Select All".
They are called "menu item" or simply "item".
When the user clicks one of these items, then something will happen.
- Menubar, submenu referenced by "Edit" item and two sections.
They are called "menu".
Menu is an ordered list of items.
They are similar to arrays.

![Menu structure](image/menu_structure.png)

- Menubar is a menu which has three items, which are "File", "Edit" and "View".
- The menu item labeled "Edit" has a link to the submenu which has two items.
These two items don't have labels.
Each item refers to a section.
- The first section is a menu which has three items -- "Cut", "Copy" and "Paste".
- The second section is a menu which has one item -- "Select All".

Menus can build a complicated structure thanks to the links of menu items.

## GMenuModel, GMenu and GMenuItem

GMenuModel is an abstact object which represents a menu.
GMenu is a simple implementation of GMenuModel and a child object of GMenuModel.

    GObjct -- GMenuModel -- GMenu

Because GMenuModel is an abstract object, it doesn't have any functions to generate it.
Therefore, if you want to generate a menu, use `g_menu_new` function to generate GMenu object.
GMenu inherits all the functions of GMenuModel because of the child object.

GMenuItem is an object directly derived from GObject.
GMenuItem and Gmenu (or GMenuModel) don't have a parent-child relationship.

    GObject -- GMenuModel -- GMenu
    GObject -- GMenuItem

Usually, GMenuItem has attributes.
One of the attributes is label.
For example, there is a menu item which has "Edit" label in the first diagram in this section.
"Cut", "Copy", "Paste" and "Select All" are also the lables of menu items.
Other attributes will be explained later.

Some menu items have a link to another GMenu.
There are two types of links, submenu and section.

GMenuItem can be inserted, appended or prepended to GMenu.
When it is inserted, all of the attribute and link values of the item are copied and used to form a new item within the menu.
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
The prefix means that the action belongs to GtkApplication.
If the menu is clicked, then the corresponding action "quit" which belongs to GtkApplication will be activated.
2. `act_quit` is an action.
It has a name "quit".
It belongs to GtkApplication, but it is not obvious in the code above.
The function `g_simple_action_new` generates a stateless action.
So, `act_quit` is stateless.
The meaning of stateless will be explained later.
The argument `NULL` means that the action doesn't have an parameter.
Generally, most of the actions are stateless and have no parameter.
When `act_quit` is activated, it will emit "activate" signal.
3. "activate" signal of the action is connected to the handler `quit_activated`.
So, if the action is activated, the handler will be invoked.

## Simple example

The following is a simple example of menus and actions.

     1 #include <gtk/gtk.h>
     2 
     3 static void
     4 quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
     5 {
     6   g_application_quit (G_APPLICATION(app));
     7 }
     8 
     9 static void
    10 on_activate (GApplication *app, gpointer user_data) {
    11   GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));
    12   gtk_window_set_title (GTK_WINDOW (win), "menu1");
    13   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
    14 
    15   GSimpleAction *act_quit = g_simple_action_new ("quit", NULL);
    16   g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_quit));
    17   g_signal_connect (act_quit, "activate", G_CALLBACK (quit_activated), app);
    18 
    19   GMenu *menubar = g_menu_new ();
    20   GMenuItem *menu_item_menu = g_menu_item_new ("Menu", NULL);
    21   GMenu *menu = g_menu_new ();
    22   GMenuItem *menu_item_quit = g_menu_item_new ("Quit", "app.quit");
    23   g_menu_append_item (menu, menu_item_quit);
    24   g_object_unref (menu_item_quit);
    25   g_menu_item_set_submenu (menu_item_menu, G_MENU_MODEL (menu));
    26   g_menu_append_item (menubar, menu_item_menu);
    27   g_object_unref (menu_item_menu);
    28 
    29   gtk_application_set_menubar (GTK_APPLICATION (app), G_MENU_MODEL (menubar));
    30   gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (win), TRUE);
    31   gtk_window_present (GTK_WINDOW (win));
    32 /*  gtk_widget_show (win); is also OKay instead of gtk_window_present. */
    33 }
    34 
    35 int
    36 main (int argc, char **argv) {
    37   GtkApplication *app;
    38   int stat;
    39 
    40   app = gtk_application_new ("com.github.ToshioCP.menu1", G_APPLICATION_FLAGS_NONE);
    41   g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
    42 
    43   stat =g_application_run (G_APPLICATION (app), argc, argv);
    44   g_object_unref (app);
    45   return stat;
    46 }
    47 

- 3-7: `quit_activated` is a handler of an action `act_quit`.
Handlers of actions have three parameters.
  1. The action object which has emitted the signal.
  2. Parameter.
In this example it is `NULL` because the second argument of `g_simple_action_new` (line 15) is `NULL`.
You don' t need to care about it.
  3. User data.
It is the fourth parameter in the `g_signal_connect` (line 17) that has connected the action and the handler.
- 6: A function `g_application_quit` immediately quits the application.
- 9-33: `on_activate` is a handler of "activate" signal on GtkApplication.
- 11-13: Generate a GtkApplicationWindow and set a pointer to it to `win`. And set the title and default size.
- 15: Generate GSimpleAction `act_quit`.
It is stateless.
The first argument of `g_simple_action_new` is a name of the action and the second argument is a parameter.
If you don't need the parameter, set it `NULL`.
Therefore, `act_quit` has a name "quit" and no parameter.
- 16: Add the action to GtkApplication `app`.
GtkApplication implements an interface GActionMap and GActionGroup.
And GtkApplication can have a group of actions and actions are added by the function `g_action_map_add_action`.
This function is described in GMenuModel section in GIO API reference.
- 17: Connect "activate" signal of the action and the handler `quit_activated`.
- 19-22: Generate GMenu and GMenuItem.
`menubar` and `menu` are GMenu.
`menu_item_menu` and `menu_item_quit` are GMenuItem.
`menu_item_menu` has a label "Menu" and no action.
`menu_item_quit` has a label "Quit".
The second argument "app.quit" is a combination of "app" and "quit".
"app" is a prefix and it means that the action belongs to GtkApplication. "quit" is the name of the action.
Therefore, it points the action which belongs to GtkApplication and has the name "quit" -- it is `act_quit`.
- 23-24: Append `act_quit` to `menu`.
As I mentioned before, all the attribute and link values are copied and used to form a new item within `menu`.
Therefore after the appending, `menu` has a copy of `act_quit` in itself and `act_quit` is no longer needed.
It is freed by `g_object_unref`.
- 25: Set a submenu link to `menu_item_menu`.
And the link points the GMenu `menu`.
- 26-27: Append `menu_item_menu` to `menubar`.
Then free `menu_item_menu`.
GMenu and GMenuItem are connected and finally a menu is made up.
The structure of the menu is shown in the diagram below.
- 29: The menu is set to GtkApplication.
- 30: Set GtkApplicationWindow to show the menubar.
- 31: Show the window.

![menu and action](image/menu1.png)

![Screenshot of menu1](image/menu1_screenshot.png)


Up: [Readme.md](Readme.md),  Prev: [Section 15](sec15.md), Next: [Section 17](sec17.md)
