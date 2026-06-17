Up: [README.md](../README.md),  Prev: [Section 15](sec15.md), Next: [Section 17](sec17.md)

# Menus and Actions

## Menus

Users often use menus to give a command to the application.
It is like this:

![Menu](/src/images/menu.png)

There are two types of objects.

- "File", "Edit", "View", "Cut", "Copy", "Paste" and "Select All".
They are called "menu item" or simply "item".
When the user clicks one of these items, then something will happen.
- Menubar, submenu referenced by "Edit" item and two sections.
They are called "menu".
Menu is an ordered list of items.
They are similar to arrays.

![Menu structure](/src/images/menu_structure.png)

- Menubar has three items, which are "File", "Edit" and "View".
- The menu item labeled "Edit" has a link to the submenu which has two items.
These two items don't have labels.
Each item refers to a section.
- The first section has three items -- "Cut", "Copy" and "Paste".
- The second section has one item -- "Select All".

Menus can build a complicated structure thanks to the links of menu items.

## GMenuModel, GMenu and GMenuItem

GMenuModel is an abstract object which represents a menu.
GMenu is a simple implementation of GMenuModel and a child object of GMenuModel.

    GObject -- GMenuModel -- GMenu

Because GMenuModel is an abstract object, it isn't instantiatable.
Therefore, it doesn't have any functions to create its instance.
If you want to create a menu, use `g_menu_new` to create a GMenu instance.
GMenu inherits all the GMenuModel functions.

GMenuItem is an object directly derived from GObject.
GMenuItem and Gmenu (or GMenuModel) don't have a parent-child relationship.

    GObject -- GMenuModel -- GMenu
    GObject -- GMenuItem

GMenuItem has attributes.
One of the attributes is label.
For example, One of the menu item in the diagram above has "Edit" label.
"Cut", "Copy", "Paste" and "Select All" are also the labels of the menu items.
Other attributes will be explained later.

Some menu items have a link to another GMenu.
There are two types of links, submenu and section.

GMenuItem can be inserted, appended or prepended to GMenu.
When it is inserted, all of the attributes and link values are copied and stored in the menu.
The GMenuItem itself is not really inserted.
Therefore, after the insertion, GMenuItem is useless and it should be freed.
The same goes for appending or prepending.

The following code shows how to append GMenuItem to GMenu.

    GMenu *menu = g_menu_new ();
    GMenuItem *menu_item_quit = g_menu_item_new ("Quit", "app.quit");
    g_menu_append_item (menu, menu_item_quit);
    g_object_unref (menu_item_quit);

## Menu and Action

One of the menu item attributes is an action and this attribute points to an action object.

There are two action objects, GSimpleAction and GPropertyAction.
GSimpleAction is often used.
And it is used with a menu item.
This section only covers GSimpleAction.

An action that corresponds to a menu item will be activated when the menu item is clicked.
Then the action emits an activate signal.

1. menu item is clicked.
2. The corresponding action is activated.
3. The action emits a signal.
4. The connected handler is invoked.


The following code is an example.

```C
static void
quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app) { ... ... ...}

GSimpleAction *act_quit = g_simple_action_new ("quit", NULL);
g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_quit));
g_signal_connect (act_quit, "activate", G_CALLBACK (quit_activated), app);
GMenuItem *menu_item_quit = g_menu_item_new ("Quit", "app.quit");
```

- `act_quit` represents an action named "quit".
The function `g_simple_action_new` creates a stateless action, meaning `act_quit` does not maintain any internal state.
Stateless actions are like simple push buttons: when clicked, they perform an operation (like "Quit", "Copy", or "Paste") and nothing more.
Most menu items are connected to stateless actions that take no parameters, which is exactly what the NULL argument specifies here.
(Stateful actions, on the other hand, will be explained in the next section.)
- The action `act_quit` is added to the GtkApplication instance using `g_action_map_add_action`.
This means the action's scope is application-wide, which is precisely what the "app" prefix in "app.quit" signifies.
- Finally, the "activate" signal of the action is connected to the callback handler `quit_activated`.
- The variable `menu_item_quit` points to a menu item (though we often simply say it is the menu item).
It has the label "Quit" and is connected to the action "app.quit".
Here, "app" is a prefix indicating that the action belongs to the GtkApplication instance, and "quit" is the name of the action.
Therefore, "app.quit" refers to the `act_quit` action.

If the menu is clicked, the corresponding action "quit" will be activated and emits an "activate" signal.
Then, the handler `quit_activated` is called.

## Menu Bar

While modern applications often use menu buttons instead of traditional menu bars, the classic style is still widely used.

An application typically defines only one menu model.
If an application has multiple windows with menu bars, they display exactly the same menus because every window creates its own menu bar instance based on the same shared menu model.

An application's menu bar typically remains unchanged once set.
Therefore, the ideal place to configure it is in the startup handler, as this handler is called only once in the primary application instance.

It is helpful for readers to understand how application instances behave.

- When an application runs for the first time, this instance is called the primary instance.
- It registers itself with the system and, upon success, emits the "startup" signal.
- After that, an "activate" or "open" signal is emitted.
- If the application is launched again while the primary instance is running, this new instance is called a remote instance.
- A remote instance does not emit the "startup" signal.
- Instead of emitting "activate" or "open" signals locally, it forwards these events to the primary instance and then quits.

Therefore, the "activate" or "open" handlers can be called multiple times, whereas the "startup" handler is called exactly once.
This is why the menu bar should be set in the startup handler.

```C
static void
app_startup (GApplication *app) {
... ... ...
  gtk_application_set_menubar (GTK_APPLICATION (app), G_MENU_MODEL (menubar));
... ... ...
}
```

## Simple Example

The following is a simple example of menus and actions.
The source file `menu1.c` is located in the [/src/menu](../src/menu/) directory.

```c
#include <gtk/gtk.h>

static void
quit_activated(GSimpleAction *action, GVariant *parameter, GApplication *application) {
  g_application_quit (application);
}

static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (win), "menu1");
  gtk_window_set_default_size (GTK_WINDOW (win), 800, 600);
  gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (win), TRUE);
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);

  GSimpleAction *act_quit = g_simple_action_new ("quit", NULL);
  g_signal_connect (act_quit, "activate", G_CALLBACK (quit_activated), application);
  g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_quit));
  g_object_unref (act_quit);

  GMenu *menubar = g_menu_new ();
  GMenuItem *menu_item_menu = g_menu_item_new ("Menu", NULL);
  GMenu *menu = g_menu_new ();
  GMenuItem *menu_item_quit = g_menu_item_new ("Quit", "app.quit");
  g_menu_append_item (menu, menu_item_quit);
  g_object_unref (menu_item_quit);
  g_menu_item_set_submenu (menu_item_menu, G_MENU_MODEL (menu));
  g_object_unref (menu);
  g_menu_append_item (menubar, menu_item_menu);
  g_object_unref (menu_item_menu);

  gtk_application_set_menubar (GTK_APPLICATION (app), G_MENU_MODEL (menubar));
  g_object_unref (menubar);
}

#define APPLICATION_ID "com.github.ToshioCP.menu1"

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

- 3–6: `quit_activated` is the handler for the "activate" signal on the `act_quit` action.
The "activate" signal handler takes three parameters:
  - Action: The action instance that emitted the signal.
  - Parameter: The parameter passed to the action.
  In this example, it is NULL because the second argument of `g_simple_action_new` (line 22) is NULL, so we can safely ignore it.
  - User Data: The user data, which is provided as the fourth argument to `g_signal_connect` (line 23) when connecting the signal to the handler.
- 5: The `g_application_quit` function immediately quits the application.
- 8–16: `app_activate` is the handler for the application's "activate" signal.
- 11–13: Creates a GtkApplicationWindow named `win`, and sets its title and default size.
- 14: Instructs the GtkApplicationWindow to display the menu bar.
- 15: Shows the window on the screen.
- 18–40: `app_startup` is the handler for the application's "startup" signal.
- 22: Creates a stateless GSimpleAction named `act_quit`.
The first argument of `g_simple_action_new` is the action's name, and the second is its parameter type.
By passing NULL, we specify that the "quit" action takes no parameters.
- 23: Connects the "activate" signal of the action to the `quit_activated` handler.
- 24: Adds the action to the GtkApplication instance `app`.
Because GtkApplication implements the GActionMap and GActionGroup interfaces, it can hold a group of actions using `g_action_map_add_action`
(see the [Gio API Reference](https://docs.gtk.org/gio/method.ActionMap.add_action.html)).
Since this action belongs to the application, its scope is "app".
Therefore, it is referred to as "app.quit" when a prefix is required.
- 25: Frees the initial reference to `act_quit`.
Because `g_action_map_add_action` acquires its own reference to the action when it is added to the application, our local reference is no longer needed.
- 27–30: Creates the GMenu and GMenuItem instances.
`menubar` and `menu` are GMenu objects, while `menu_item_menu` and `menu_item_quit` are GMenuItem objects.
`menu_item_menu` has the label "Menu" but no action, whereas `menu_item_quit` has the label "Quit" and is linked to the "app.quit" action.
- 31–32: Appends `menu_item_quit` to `menu`.
As discussed previously, the menu model copies all attributes and links to form a new item internally.
Thus, the original `menu_item_quit instance` is no longer needed and is safely freed using `g_object_unref`.
- 33–34: Sets `menu` as a submenu of `menu_item_menu`.
Once linked, the original `menu` is no longer needed and is freed.
- 35–36: Appends `menu_item_menu` to the `menubar` and then frees `menu_item_menu`.
Through this process, the entire menu structure is built and anchored to the `menubar` variable.
(The resulting menu structure is shown in the diagram below.)
- 38–39: The completed menubar model is set into the GtkApplication.
Finally, the initial reference to `menubar` is freed, completing the startup initialization safely without memory leaks.

![menu and action](/src/images/menu1.png)

## Compiling and Running

Change your current directory to `src/menu`.
Use `comp` to compile `menu1.c`.

```
$ bash comp menu1
$ ./a.out
```

A window will appear. Click "Menu" on the menu bar to open the drop-down menu.
Click "Quit" to exit the application.

![Screenshot of menu1](/src/images/menu1_screenshot.png)

## Testing Primary and Remote Application Instances

Let's try running the application twice simultaneously to see how multiple instances behave.
You can do this elegantly by executing both commands on a single line using `&`.

```
$ ./a.out & ./a.out
```

Two identical windows will appear at once.
Here is what happens behind the scenes:

- The First Command (./a.out &): This runs the application in the background and creates the primary instance.
It executes the startup and activate handlers, displaying the first window.
- The Second Command (./a.out): This runs immediately after in the foreground, creating a remote instance.
Because the primary instance is already running, the remote instance does not emit the startup signal.
Instead, it forwards its activate signal to the primary instance and immediately quits.
- The Result: The primary instance receives the forwarded signal and executes its activate handler a second time, creating the second window.

Because both windows belong to the single primary instance and are generated from the same shared menu model, they look exactly the same.
Finally, if you click the "Quit" item on either window's menu, the primary instance quits, and both windows will close simultaneously.

![menu1 -- two windows](/src/images/menu1_two_windows.png)

The reason a second window appeared in our test is specifically because of how our `activate` handler is written.
Currently, it calls `gtk_application_window_new` every time it is invoked. 

This behavior is entirely up to you as a developer.
Depending on your application's design, you can change what happens during subsequent executions:

- **Single Window Design:** If you want your application to only ever have one window, you should create the window in the `startup` handler.
Your `activate` handler would then only be responsible for presenting it (e.g., using `gtk_window_present`).
With this design, a second execution will simply bring the existing window to the front instead of creating a new one.
- **Tabbed Interface Design:** Alternatively, you can design the `activate` handler to add a new notebook page (tab).
For example, a text file editor (like `tfe`) might open a new blank tab in the existing window rather than spawning a second window.
In practice, this is often a much better user experience.

In real-world desktop environments, users frequently launch applications multiple times, such as by double-clicking an icon repeatedly.
Therefore, it is crucial to carefully consider how you divide your logic between the `startup` and `activate` (or `open`) handlers to ensure your application behaves exactly as intended.

Up: [README.md](../README.md),  Prev: [Section 15](sec15.md), Next: [Section 17](sec17.md)
