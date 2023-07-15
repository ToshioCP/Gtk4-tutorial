Up: [README.md](../README.md),  Prev: [Section 17](sec17.md), Next: [Section 19](sec19.md)

# Stateful action

Some actions have states.
The typical values of states are boolean or string.
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
The function `g_variant_new_boolean (FALSE)` returns a boolean type GVariant value which is `FALSE`.
If there are two or more top level windows, each window has its own `act_fullscreen` action.
So, the number of the actions is the same as the number of the windows.
- The action `act_fullscreen` has "change-state" signal. The signal is connected to a  handler `fullscreen_changed`.
If the fullscreen menu is clicked, then the corresponding action `act_fullscreen` is activated.
But no handler is connected to the "activate" signal.
Then, the default behavior for boolean-stated actions with a NULL parameter type like `act_fullscreen` is to toggle them via the “change-state” signal.
- The action is added to the GtkWindow `win`.
Therefore, the scope of the action is "win".
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
Otherwise it un-maximizes.
- Sets the state of the action with `value`.
Note: At this stage, that means the stage before `g_simple_action_set_state` is called,  the state of the action still has the original value.
So, you need to set the state with the new value by `g_simple_action_set_state`.

You can use "activate" signal instead of "change-state" signal, or both signals.
But the way above is the simplest and the best.

### GVariant

GVariant is a fundamental type.
It isn't a child of GObject.
GVariant can contain boolean, string or other type values.
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

Since `value` has been created as a boolean type GVariant with `TRUE` value, `bool` equals `TRUE`.
In the same way, you can get a string from `value2`

~~~C
const char *str = g_variant_get_string (value2, NULL);
~~~

The second parameter is a pointer to gsize type variable (gsize is defined as unsigned long).
If it isn't NULL, then the pointed value is used as the length by the function.
If it is NULL, nothing happens.
The returned string `str` is owned by the instance and can't be changed or freed by the caller.

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
GVariantType *vtype = g_variant_type_new("s");
GSimpleAction *act_color
  = g_simple_action_new_stateful ("color", vtype, g_variant_new_string ("red"));
g_variant_type_free (vtype);
GMenuItem *menu_item_red = g_menu_item_new ("Red", "app.color::red");
GMenuItem *menu_item_green = g_menu_item_new ("Green", "app.color::green");
GMenuItem *menu_item_blue = g_menu_item_new ("Blue", "app.color::blue");
g_signal_connect (act_color, "activate", G_CALLBACK (color_activated), NULL);
... ... ...
~~~

- GVariantType is a C structure and it keeps a type of GVariant.
It is created with the function `g_variant_type_new`.
The argument of the function is a GVariant type string.
So, `g_variant_type_new("s")` returns a GVariantType structure contains a string type.
The returned value, GVariantType structure, is owned by the caller.
So, you need to free it when it becomes useless. 
- The variable `act_color` points a GSimpleAction instance.
It is created with `g_simple_action_new_stateful`.
The function has three arguments.
The first argument "color" is the name of the action.
The second argument is a parameter type which is GVariantType.
`g_variant_type_new("s")` creates GVariantType which is a string type (`G_VARIANT_TYPE_STRING`).
The third argument is the initial state of the action.
It is a GVariant.
The function `g_variant_new_string ("red")` returns a GVariant value which has the string value "red".
GVariant has a reference count and `g_variant_new_...` series functions returns a GVariant value with a floating reference.
That means the caller doesn't own the value at this point.
And `g_simple_action_new_stateful` function consumes the floating reference so you don't need to care about releasing the GVariant instance.
- The GVariantType structure `vtype` is useless after `g_simple_action_new_stateful`.
It is released with the function `g_variant_type_free`.
- The varable `menu_item_red` points a GMenuItem instance.
The function `g_menu_item_new` has two arguments.
The first argument "Red" is the label of `menu_item_red`.
The second argument is a detailed action.
Its prefix is "app", action name is "color" and target is "red".
Target is sent to the action as a parameter.
The same goes for `menu_item_green` and `menu_item_blue`.
- The function `g_signal_connect` connects the activate signal on the action `act_color` and the handler `color_activated`.
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

- The handler originally has three parameters.
The third parameter is a user data set in the `g_signal_connect` function.
But it is left out because the fourth argument of the `g_signal_connect` has been NULL.
The first parameter is the action which emits the "activate" signal.
The second parameter is the parameter, or target, given to the action.
It is a color specified by the menu.
- The variable `color` is a CSS string created by `g_strdup_printf`.
The arguments of `g_strdup_printf` are the same as printf C standard function.
The function `g_variant_get_string` gets the string contained in `parameter`.
The string is owned by the instance and you mustn't change or free it.
The string `label.lb` is a selector.
It consists of `label`, a node name of GtkLabel, and `lb` which is a class name.
It selects GtkLabel which has `lb` class.
For example, menus have GtkLabel to display their labels, but they don't have `lb` class.
So, the CSS doesn't change their background color.
The string `{background-color %s}` makes the background color `%s` to which the color from `parameter` is assigned.
- Frees the string `color`.
- Changes the state with `g_action_change_state`.

Note: If you haven't set an "activate" signal handler, the signal is forwarded to "change-state" signal.
So, you can use "change-state" signal instead of "activate" signal.
See [`src/menu/menu2_change_state.c`](../src/menu/menu2_change_state.c).

### GVariantType

GVariantType gives a type of GVariant.
GVariantType is created with a type string.

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
8   g_variant_type_free (vtype);
9 }
~~~

- The function `g_variant_type_new` creates a GVariantType structure.
The argument "s" is a type string.
It means string.
The returned structure is owned by the caller.
When it becomes useless, you need to free it with the function `g_variant_type_free`.
- The function `g_variant_type_peek_string` takes a peek at `vtype`.
It is the string "s" given to `vtype` when it was created.
The string is owned by the instance and the caller can't change or free it.
- Prints the string to the terminal.
You can't free `vtype` before `g_print` because the string `type_string` is owned by `vtype`.
- Frees `vtype`.

## Example

The following code includes stateful actions above.
This program has menus like this:

![menu2](../image/menu2.png)

- Fullscreen menu toggles the size of the window between maximum and non-maximum.
If the window is maximum size, which is called full screen, then a check mark is put before "fullscreen" label.
- Red, green and blue menu determines the back ground color of the label in the window.
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
 26 app_shutdown (GApplication *app, GtkCssProvider *provider) {
 27   gtk_style_context_remove_provider_for_display (gdk_display_get_default(), GTK_STYLE_PROVIDER (provider));
 28 }
 29 
 30 static void
 31 app_activate (GApplication *app) {
 32   GtkWindow *win = GTK_WINDOW (gtk_application_window_new (GTK_APPLICATION (app)));
 33   gtk_window_set_title (win, "menu2");
 34   gtk_window_set_default_size (win, 400, 300);
 35 
 36   GtkWidget *lb = gtk_label_new (NULL);
 37   gtk_widget_add_css_class (lb, "lb"); /* the class is used by CSS Selector */
 38   gtk_window_set_child (win, lb);
 39 
 40   GSimpleAction *act_fullscreen
 41     = g_simple_action_new_stateful ("fullscreen", NULL, g_variant_new_boolean (FALSE));
 42   g_signal_connect (act_fullscreen, "change-state", G_CALLBACK (fullscreen_changed), win);
 43   g_action_map_add_action (G_ACTION_MAP (win), G_ACTION (act_fullscreen));
 44 
 45   gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (win), TRUE);
 46 
 47   gtk_window_present (win);
 48 }
 49 
 50 static void
 51 app_startup (GApplication *app) {
 52   GVariantType *vtype = g_variant_type_new("s");
 53   GSimpleAction *act_color
 54     = g_simple_action_new_stateful ("color", vtype, g_variant_new_string ("red"));
 55   g_variant_type_free (vtype);
 56   GSimpleAction *act_quit
 57     = g_simple_action_new ("quit", NULL);
 58   g_signal_connect (act_color, "activate", G_CALLBACK (color_activated), NULL);
 59   g_signal_connect_swapped (act_quit, "activate", G_CALLBACK (g_application_quit), app);
 60   g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_color));
 61   g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_quit));
 62 
 63   GMenu *menubar = g_menu_new ();
 64   GMenu *menu = g_menu_new ();
 65   GMenu *section1 = g_menu_new ();
 66   GMenu *section2 = g_menu_new ();
 67   GMenu *section3 = g_menu_new ();
 68   GMenuItem *menu_item_fullscreen = g_menu_item_new ("Full Screen", "win.fullscreen");
 69   GMenuItem *menu_item_red = g_menu_item_new ("Red", "app.color::red");
 70   GMenuItem *menu_item_green = g_menu_item_new ("Green", "app.color::green");
 71   GMenuItem *menu_item_blue = g_menu_item_new ("Blue", "app.color::blue");
 72   GMenuItem *menu_item_quit = g_menu_item_new ("Quit", "app.quit");
 73 
 74   g_menu_append_item (section1, menu_item_fullscreen);
 75   g_menu_append_item (section2, menu_item_red);
 76   g_menu_append_item (section2, menu_item_green);
 77   g_menu_append_item (section2, menu_item_blue);
 78   g_menu_append_item (section3, menu_item_quit);
 79   g_object_unref (menu_item_red);
 80   g_object_unref (menu_item_green);
 81   g_object_unref (menu_item_blue);
 82   g_object_unref (menu_item_fullscreen);
 83   g_object_unref (menu_item_quit);
 84 
 85   g_menu_append_section (menu, NULL, G_MENU_MODEL (section1));
 86   g_menu_append_section (menu, "Color", G_MENU_MODEL (section2));
 87   g_menu_append_section (menu, NULL, G_MENU_MODEL (section3));
 88   g_menu_append_submenu (menubar, "Menu", G_MENU_MODEL (menu));
 89   g_object_unref (section1);
 90   g_object_unref (section2);
 91   g_object_unref (section3);
 92   g_object_unref (menu);
 93 
 94   gtk_application_set_menubar (GTK_APPLICATION (app), G_MENU_MODEL (menubar));
 95 
 96   provider = gtk_css_provider_new ();
 97   /* Initialize the css data */
 98   gtk_css_provider_load_from_data (provider, "label.lb {background-color: red;}", -1);
 99   /* Add CSS to the default GdkDisplay. */
100   gtk_style_context_add_provider_for_display (gdk_display_get_default (),
101         GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
102   g_signal_connect (app, "shutdown", G_CALLBACK (app_shutdown), provider);
103   g_object_unref (provider); /* release provider, but it's still alive because the display owns it */
104 }
105 
106 #define APPLICATION_ID "com.github.ToshioCP.menu2"
107 
108 int
109 main (int argc, char **argv) {
110   GtkApplication *app;
111   int stat;
112 
113   app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
114   g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
115   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
116 
117   stat =g_application_run (G_APPLICATION (app), argc, argv);
118   g_object_unref (app);
119   return stat;
120 }
~~~

- 6-23: Action signal handlers.
- 25-28: The handler `app_shutdown` is called when the application quits.
It removes the provider from the display.
- 30-48: An activate signal handler.
- 32-34: A new window is created and assigned to `win`.
Its title and default size are set to "menu2" and 400x300 respectively.
- 36-38: A new label is created and assigned to `lb`
The label is given a CSS class "lb".
It is added to `win` as a child.
- 40-43: A toggle action is created and assigned to `act_fullscreen`.
It's connected to the signal handler `fullscreen_changed`.
It's added to the window, so the action scope is "win".
So, if there are two or more windows, the actions are created two or more.
- 45: The function `gtk_application_window_set_show_menubar` adds a menubar to the window.
- 47: Shows the window.
- 50-104: A startup signal handler.
- 52-61: Two actions `act_color` and `act_quit` are created.
These actions exists only one because the startup handler is called once.
They are connected to their handlers and added to the application.
Their scopes are "app".
- 63-92: Menus are built.
- 94: The menubar is added to the application.
- 96-103: A CSS provider is created with the CSS data and added to the default display.
The "shutdown" signal on the application is connected to a handler "app_shutdown".
So, the provider is removed from the display and freed when the application quits.

## Compile

Change your current directory to `src/menu`.

~~~
$ comp menu2
$./a.out
~~~

Then, you will see a window and the background color of the content is red.
You can change the size to maximum and change back to the original size.
You can change the background color to green or blue.

If you run the second application during the first application is running, another window will appear in the same screen.
Both of the window have the same background color.
Because the `act_color` action has "app" scope and the CSS is applied to the default display shared by the windows.

```
$ ./a.out & # Run the first application
[1] 82113
$ ./a.out # Run the second application
$ 
```
Up: [README.md](../README.md),  Prev: [Section 17](sec17.md), Next: [Section 19](sec19.md)
