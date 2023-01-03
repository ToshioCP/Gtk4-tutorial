Up: [README.md](../README.md),  Prev: [Section 17](sec17.md), Next: [Section 19](sec19.md)

# Stateful action

Some actions have states.
The typical values of states is boolean or string.
However, other types of states are possible if you want.

Actions which have states are called stateful.

## Stateful action without a parameter

Some menus are called toggle menu.
For example, fullscreen menu has a state which has two values -- fullscreen and non-fullscreen.
The value of the state is changed every time the menu is clicked.
An action corresponds to the fullscreen menu also have a state.
Its value is TRUE or FALSE and it is called boolean value.
TRUE corresponds to fullscreen and FALSE to non-fullscreen.

The following is an example code to implement a fullscreen menu except the signal handler.
The signal handler will be shown later.

~~~C
GSimpleAction *act_fullscreen = g_simple_action_new_stateful ("fullscreen",
                                NULL, g_variant_new_boolean (FALSE));
g_signal_connect (act_fullscreen, "change-state", G_CALLBACK (fullscreen_changed), win);
g_action_map_add_action (G_ACTION_MAP (win), G_ACTION (act_fullscreen));
... ... ...
GMenuItem *menu_item_fullscreen = g_menu_item_new ("Full Screen", "win.fullscreen");
~~~

- `act_fullscreen` is a GSimpleAction instance.
It is created with `g_simple_action_new_stateful`.
The function has three arguments.
The first argument "fullscreen" is the name of the action.
The second argument is a parameter type.
`NULL` means the action doesn't have a parameter.
The third argument is the initial state of the action.
It is a GVariant value.
GVariant will be explained in the next subsection.
The function `g_variant_new_boolean (FALSE)` returns a GVariant value which is the boolean value `FALSE`.
If there are two or more top level windows, each window has its own `act_fullscreen` action.
So, the number of the actions is the same as the number of the windows.
- connects the action `act_fullscreen` and the "change-state" signal handler `fullscreen_changed`.
If the fullscreen menu is clicked, then the corresponding action `act_fullscreen` is activated.
But no handler is connected to the "activate" signal.
Then, the default behavior for boolean-stated actions with a NULL parameter type like `act_fullscreen` is to toggle them via the “change-state” signal.
- The action is added to the GtkWindow `win`.
Therefore, the scope of the action is "win" -- window.
- `menu_item_fullscreen` is a GMenuItem instance.
There are two arguments.
The first argument "Full Screen" is the label of `menu_item_fullscreen`.
The second argument is an action.
The action "win.fullscreen" has a prefix "win" and an action name "fullscreen".
The prefix says that the action belongs to the window.

~~~C
1 static void
2 fullscreen_changed(GSimpleAction *action, GVariant *value, GtkWindow *win) {
3   if (g_variant_get_boolean (value))
4     gtk_window_maximize (win);
5   else
6     gtk_window_unmaximize (win);
7   g_simple_action_set_state (action, value);
8 }
~~~

- The handler `fullscreen_changed` has three parameters.
The first parameter is the action which emits the "change-state" signal.
The second parameter is the value of the new state of the action.
The third parameter is a user data which is set in `g_signal_connect`.
- If the value is boolean type and `TRUE`, then it maximizes the window.
Otherwise unmaximizes.
- Sets the state of the action with `value`.
Note: the second argument was the toggled state value, but at this stage the state of the action has the original value.
So, you need to set the state with the new value by `g_simple_action_set_state`.

You can use "activate" signal instead of "change-state" signal, or both signals.
But the way above is the simplest and the best.

### GVariant

GVarient can contain boolean, string or other type values.
For example, the following program assigns TRUE to `value` whose type is GVariant.

~~~C
GVariant *value = g_variant_new_boolean (TRUE);
~~~

Another example is:

~~~C
GVariant *value2 = g_variant_new_string ("Hello");
~~~

`value2` is a GVariant and it has a string type value "Hello".
GVariant can contain other types like int16, int32, int64, double and so on.

If you want to get the original value, use g\_variant\_get series functions.
For example, you can get the boolean value with g\_variant_get_boolean.

~~~C
gboolean bool = g_variant_get_boolean (value);
~~~

Because `value` has been created as a boolean type GVariant and `TRUE` value, `bool` equals `TRUE`.
In the same way, you can get a string from `value2`

~~~C
const char *str = g_variant_get_string (value2, NULL);
~~~

The second parameter is a pointer to gsize type variable (gsize is defined as unsigned long).
If it isn't NULL, then the length of the string will be set by the function.
If it is NULL, nothing happens.
The returned string `str` can't be changed.

## Stateful action with a parameter

Another example of stateful actions is an action corresponds to color select menus.
For example, there are three menus and each menu has red, green or blue color respectively.
They determine the background color of a GtkLabel widget.
One action is connected to the three menus.
The action has a state whose value is "red", "green" or "blue".
The values are string.
Those colors are given to the signal handler as a parameter.

~~~C
... ... ...
GSimpleAction *act_color = g_simple_action_new_stateful ("color",
                   g_variant_type_new("s"), g_variant_new_string ("red"));
GMenuItem *menu_item_red = g_menu_item_new ("Red", "app.color::red");
GMenuItem *menu_item_green = g_menu_item_new ("Green", "app.color::green");
GMenuItem *menu_item_blue = g_menu_item_new ("Blue", "app.color::blue");
g_signal_connect (act_color, "activate", G_CALLBACK (color_activated), NULL);
... ... ...
~~~

- `act_color` is a GSimpleAction instance.
It is created with `g_simple_action_new_stateful`.
The function has three arguments.
The first argument "color" is the name of the action.
The second argument is a parameter type which is GVariantType.
`g_variant_type_new("s")` creates GVariantType which is a string type (`G_VARIANT_TYPE_STRING`).
The third argument is the initial state of the action.
It is a GVariant.
GVariantType will be explained in the next subsection.
The function `g_variant_new_string ("red")` returns a GVariant value which has the string value "red".
- `menu_item_red` is a GMenuItem instance.
There are two arguments.
The first argument "Red" is the label of `menu_item_red`.
The second argument is a detailed action.
Its prefix is "app", action name is "color" and target is "red".
Target is sent to the action as a parameter.
The same goes for `menu_item_green` and `menu_item_blue`.
- connects the action `act_color` and the "activate" signal handler `color_activated`.
If one of the three menus is clicked, then the action `act_color` is activated with the target (parameter) which is given by the menu.

The following is the "activate" signal handler.

~~~C
static void
color_activated(GSimpleAction *action, GVariant *parameter) {
  char *color = g_strdup_printf ("label.lb {background-color: %s;}",
                                   g_variant_get_string (parameter, NULL));
  gtk_css_provider_load_from_data (provider, color, -1);
  g_free (color);
  g_action_change_state (G_ACTION (action), parameter);
}
~~~

- There are three parameters.
The first parameter is the action which emits the "activate" signal.
The second parameter is the parameter given to the action.
It is a color specified by the menu.
The third parameter is left out because the fourth argument of the `g_signal_connect` is NULL.
- `color` is a CSS string created by `g_strdup_printf`.
The parameter of `g_strdup_printf` is the same as printf C standard function.
`g_variant_get_string` gets the string contained in `parameter`.
You mustn't change or free the string.
- Sets the color of the css provider.
`label.lb` is a selector.
`lable` is a node name of GtkLabel and `lb` is a class.
`label.lb` selects GtkLabel which has lb class.
For example, menus have GtkLabel to display their labels, but they don't have `lb` class.
So, the CSS doesn't change the their background color.
`{background-color %s}` makes the background color the one from `parameter`.
- Frees the string `color`.
- Changes the state by `g_action_change_state`.

Note: If you haven't set an "activate" signal handler, the signal is forwarded to "change-state" signal.
So, you can use "change-state" signal instead of "activate" signal.
See [`src/menu/menu2_change_state.c`](../src/menu/menu2_change_state.c).

### GVariantType

GVariantType gives a type of GVariant.
GVariant can contain many kinds of types.
And the type often needs to be recognized at runtime.

GVariantType is created with a string which expresses a type.

- "b" means boolean type.
- "s" means string type.

The following program is a simple example.
It finally outputs the string "s".

~~~C
1 #include <glib.h>
2 
3 int
4 main (int argc, char **argv) {
5   GVariantType *vtype = g_variant_type_new ("s");
6   const char *type_string = g_variant_type_peek_string (vtype);
7   g_print ("%s\n",type_string);
8 }
~~~

- `g_variant_type_new` creates GVariantType.
It uses a type string "s" which means string.
- `g_variant_type_peek_string` takes a peek at `vtype`.
It is the string "s" given to `vtype` when it was created.
- prints the string to the terminal.

## Example

The following code includes stateful actions above.
This program has menus like this:

![menu2](../image/menu2.png)

- Fullscreen menu toggles the size of the window between maximum and non-maximum.
If the window is maximum size, which is called full screen, then a check mark is put before "fullscreen" label.
- Red, green and blue menu determines the back ground color of any labels.
The menus have radio buttons on the left of the menus.
And the radio button of the selected menu turns on.
- Quit menu quits the application.

The code is as follows.

~~~C
  1 #include <gtk/gtk.h>
  2 
  3 /* The provider below provides application wide CSS data. */
  4 GtkCssProvider *provider;
  5 
  6 static void
  7 fullscreen_changed(GSimpleAction *action, GVariant *value, GtkWindow *win) {
  8   if (g_variant_get_boolean (value))
  9     gtk_window_maximize (win);
 10   else
 11     gtk_window_unmaximize (win);
 12   g_simple_action_set_state (action, value);
 13 }
 14 
 15 static void
 16 color_activated(GSimpleAction *action, GVariant *parameter) {
 17   char *color = g_strdup_printf ("label.lb {background-color: %s;}", g_variant_get_string (parameter, NULL));
 18   /* Change the CSS data in the provider. */
 19   /* Previous data is thrown away. */
 20   gtk_css_provider_load_from_data (provider, color, -1);
 21   g_free (color);
 22   g_action_change_state (G_ACTION (action), parameter);
 23 }
 24 
 25 static void
 26 remove_provider (GApplication *app, GtkCssProvider *provider) {
 27   GdkDisplay *display = gdk_display_get_default();
 28   
 29   gtk_style_context_remove_provider_for_display (display, GTK_STYLE_PROVIDER (provider));
 30   g_object_unref (provider);
 31 }
 32 
 33 static void
 34 app_activate (GApplication *app) {
 35   GtkWindow *win = GTK_WINDOW (gtk_application_window_new (GTK_APPLICATION (app)));
 36   gtk_window_set_title (win, "menu2");
 37   gtk_window_set_default_size (win, 400, 300);
 38 
 39   GtkWidget *lb = gtk_label_new (NULL);
 40   gtk_widget_add_css_class (lb, "lb"); /* the class is used by CSS Selector */
 41   gtk_window_set_child (win, lb);
 42 
 43   GSimpleAction *act_fullscreen
 44     = g_simple_action_new_stateful ("fullscreen", NULL, g_variant_new_boolean (FALSE));
 45   g_signal_connect (act_fullscreen, "change-state", G_CALLBACK (fullscreen_changed), win);
 46   g_action_map_add_action (G_ACTION_MAP (win), G_ACTION (act_fullscreen));
 47 
 48   gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (win), TRUE);
 49 
 50   gtk_window_present (win);
 51 }
 52 
 53 static void
 54 app_startup (GApplication *app) {
 55   GSimpleAction *act_color
 56     = g_simple_action_new_stateful ("color", g_variant_type_new("s"), g_variant_new_string ("red"));
 57   GSimpleAction *act_quit
 58     = g_simple_action_new ("quit", NULL);
 59   g_signal_connect (act_color, "activate", G_CALLBACK (color_activated), NULL);
 60   g_signal_connect_swapped (act_quit, "activate", G_CALLBACK (g_application_quit), app);
 61   g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_color));
 62   g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_quit));
 63 
 64   GMenu *menubar = g_menu_new ();
 65   GMenu *menu = g_menu_new ();
 66   GMenu *section1 = g_menu_new ();
 67   GMenu *section2 = g_menu_new ();
 68   GMenu *section3 = g_menu_new ();
 69   GMenuItem *menu_item_fullscreen = g_menu_item_new ("Full Screen", "win.fullscreen");
 70   GMenuItem *menu_item_red = g_menu_item_new ("Red", "app.color::red");
 71   GMenuItem *menu_item_green = g_menu_item_new ("Green", "app.color::green");
 72   GMenuItem *menu_item_blue = g_menu_item_new ("Blue", "app.color::blue");
 73   GMenuItem *menu_item_quit = g_menu_item_new ("Quit", "app.quit");
 74 
 75   g_menu_append_item (section1, menu_item_fullscreen);
 76   g_menu_append_item (section2, menu_item_red);
 77   g_menu_append_item (section2, menu_item_green);
 78   g_menu_append_item (section2, menu_item_blue);
 79   g_menu_append_item (section3, menu_item_quit);
 80   g_object_unref (menu_item_red);
 81   g_object_unref (menu_item_green);
 82   g_object_unref (menu_item_blue);
 83   g_object_unref (menu_item_fullscreen);
 84   g_object_unref (menu_item_quit);
 85 
 86   g_menu_append_section (menu, NULL, G_MENU_MODEL (section1));
 87   g_menu_append_section (menu, "Color", G_MENU_MODEL (section2));
 88   g_menu_append_section (menu, NULL, G_MENU_MODEL (section3));
 89   g_menu_append_submenu (menubar, "Menu", G_MENU_MODEL (menu));
 90 
 91   gtk_application_set_menubar (GTK_APPLICATION (app), G_MENU_MODEL (menubar));
 92 
 93   provider = gtk_css_provider_new ();
 94   /* Initialize the css data */
 95   gtk_css_provider_load_from_data (provider, "label.lb {background-color: red;}", -1);
 96   /* Add CSS to the default GdkDisplay. */
 97   GdkDisplay *display = gdk_display_get_default ();
 98   gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider),
 99                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
100   g_signal_connect (app, "shutdown", G_CALLBACK (remove_provider), provider);
101 }
102 
103 #define APPLICATION_ID "com.github.ToshioCP.menu2"
104 
105 int
106 main (int argc, char **argv) {
107   GtkApplication *app;
108   int stat;
109 
110   app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
111   g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
112   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
113 
114   stat =g_application_run (G_APPLICATION (app), argc, argv);
115   g_object_unref (app);
116   return stat;
117 }
118 
~~~

- 6-23: Signal handlers connected to the actions.
- 25-31: The handler `remove_provider` is called when the application quits.
It removes the provider from the display and releases the provider.
- 33-51: An activate signal handler.
- 35-37: A new window is created and assigned to `win`.
Its title and default size are set to "menu2" and 400x300 respectively.
- 39-41: A new label is created and assigned to `lb`
The name is "lb", which is used in CSS.
It is added to `win` as a child.
- 43-46: A toggle action is created and assigned to `act_fullscreen`.
It's connected to the signal handler `fullscreen_changed`.
It's added to the window, so the scope is "win".
The action corresponds to the window.
So, if there are two or more windows, the actions are created two or more.
- 48: The function `gtk_application_window_set_show_menubar` adds a menubar to the window.
- 50: The window is shown.
- 53-101: A startup signal handler.
- 55-62: Two actions `act_color` and `act_quit` are created.
These actions exists only one because the startup handler is called once.
They are connected to their handlers and added to the application.
Their scopes are "app".
- 64-89: Menus are built.
- 91: The menubar is added to the application.
- 93-100: A css provider is created, set the data and added to the default display.
The "shutdown" signal on the application is connected to a handler "remove_provider".
So, the provider is removed from the display and freed when the application quits.

## Compile

Change your current directory to `src/menu`.

~~~
$ comp menu2
$./a.out
~~~

Then, you will see a window and the background color of the content is red.
You can change the size to maximum and change again to the original size.
You can change the background color to green or blue.

If you run the application again, another window will appear in the same screen.
Both of the window have the same background color.
Because the `act_color` action has "app" scope and the CSS is applied to the default display shared by the windows.

Up: [README.md](../README.md),  Prev: [Section 17](sec17.md), Next: [Section 19](sec19.md)
