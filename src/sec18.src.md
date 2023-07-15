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

@@@include
menu/menu2.c fullscreen_changed
@@@

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
See [`src/menu/menu2_change_state.c`](menu/menu2_change_state.c).

### GVariantType

GVariantType gives a type of GVariant.
GVariantType is created with a type string.

- "b" means boolean type.
- "s" means string type.

The following program is a simple example.
It finally outputs the string "s".

@@@include
menu/gvarianttype_test.c
@@@

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

![menu2](../image/menu2.png){width=6.03cm height=5.115cm}

- Fullscreen menu toggles the size of the window between maximum and non-maximum.
If the window is maximum size, which is called full screen, then a check mark is put before "fullscreen" label.
- Red, green and blue menu determines the back ground color of the label in the window.
The menus have radio buttons on the left of the menus.
And the radio button of the selected menu turns on.
- Quit menu quits the application.

The code is as follows.

@@@include
menu/menu2.c
@@@

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