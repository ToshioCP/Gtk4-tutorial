Up: [README.md](../README.md),  Prev: [Section 16](sec16.md), Next: [Section 18](sec18.md)

# Stateful Actions

Actions that maintain a state are called stateful actions.
While these states are typically boolean or string values, other data types can also be used when necessary.

Stateful actions fall into two main categories:

- **Stateful actions without parameters:** Often used for toggle menus.
The state is usually a boolean (TRUE or FALSE), and activating the action simply flips the state to its opposite value.
- **Stateful actions with parameters:** Used when an action can take on one of several distinct states.
For example, a background color action might accept "red", "green", or "blue".
To change the state, the caller must pass the target value as a parameter.

## Action Activation and State Changes

GIO provides two primary functions for working with actions:

- `g_action_activate()`: activates the action.
This function emits the "activate" signal.
- `g_action_change_state()`: requests a state change on the action.
Note that this function itself doesn't change the state. 
This function just emits the "change-state" signal.

### Menus and Shortcuts Always Go Through Activate

Menu items and keyboard shortcuts work by calling `g_action_activate()` internally.
This means the starting point is always the `"activate"` signal --- even for stateful actions.
Menu items do **not** call `g_action_change_state()` directly.

### Aciivate and Change-state Default Behavior

#### Forwarding Activate to Change-state

So how do stateful actions work naturally with menus?
`GSimpleAction` has a helpful default behavior built in.

If **no handler is connected** to the "activate" signal, `GSimpleAction` will automatically forward the activation to "change-state" (since GLib 2.40), under the following conditions:

| Action type | Default behavior |
|---|---|
| Boolean state, no parameter type | Toggles the state via "change-state" |
| State type matches the parameter type (\*) | Forwards directly to "change-state" |

**Note:**

- This forwarding is specific to `GSimpleAction`. It is not a general feature of the `GAction` interface.
- (\*) If the caller's parameter type doesn't match the state type of the action, no signal will be emitted. 

#### change-state's Default Behavior

When "change-state" signal is emitted and no handler is connected, GSimpleAction will automatically call `g_simple_action_set_state()` to apply the requested value.
This means that for simple use cases where no validation is needed, you don't have to write any handler at all --- the state just updates itself.

### A Recommended Pattern: Put Your Logic in change-state

Taking advantage of this default behavior leads to a clean and robust design pattern:

**Leave `"activate"` unhandled, and connect your logic to `"change-state"` instead.**

```c
static void
on_change_state (GSimpleAction *action,
                 GVariant      *value,
                 gpointer       user_data)
{
    /* Optionally validate the requested value here */
    g_simple_action_set_state (action, value);  /* Apply the new state */
    /* Perform tasks related to the state change */ 
}

g_signal_connect (action, "change-state", G_CALLBACK (on_change_state), NULL);
```

With this pattern, activations from menus, keyboard shortcuts, and direct calls to `g_action_change_state()` all flow through the same handler consistently.

- **If you connect your own `"activate"` handler:**
The automatic forwarding to `"change-state"` is disabled.
You become responsible for handling the state update yourself.
- **If you connect your own `"change-state"` handler:**
The default behavior of automatically calling `g_simple_action_set_state()` is also disabled.
If you want the state to actually change, you must call `g_simple_action_set_state()` explicitly from within your handler.
If you don't, the state will remain unchanged --- which is intentional, as it allows you to validate or reject requests before applying them.

## Stateful Action without Parameters

A common example of a stateful action without a parameter is a toggle menu.
Its state, represented by a boolean value (TRUE or FALSE), changes each time the menu is clicked.

The following example demonstrates how to implement a toggle menu that updates a GtkLabel.
The label is initialized to "OFF", and its text alternates between "ON" and "OFF" with each click.

This is a part of the `menu2.c` program, defining the toggle action and the menu item.

```C
GSimpleAction *act_toggle_switch
  = g_simple_action_new_stateful ("toggle-switch", NULL, g_variant_new_boolean (FALSE));
g_signal_connect (act_toggle_switch, "change-state", G_CALLBACK (toggle_switch_changed), label);
g_action_map_add_action (G_ACTION_MAP (win), G_ACTION (act_toggle_switch));
... ... ...
GMenuItem *menu_item_toggle_switch = g_menu_item_new ("Toggle Switch", "win.toggle-switch");
```

`act_toggle_switch` is a GSimpleAction instance created with `g_simple_action_new_stateful`.
This function takes three arguments:

- The first argument, "toggle-switch", is the name of the action.
- The second argument is the parameter type. NULL means this action does not take any parameters.
- The third argument is the initial state of the action.
It is provided as a GVariant value (which will be explained in the next subsection).
Here, `g_variant_new_boolean (FALSE)` sets the initial state to FALSE.

The action's "change-state" signal is connected to the handler `toggle_switch_changed`.
Notice that we pass `label` (the GtkLabel) as the user_data so we can update its text later.
As explained in the previous section, we intentionally leave the "activate" signal unconnected to take advantage of forwarding it to the "change-state" signal .

The action is added to the GtkWindow (`win`) using `g_action_map_add_action`.
Therefore, the scope of this action is `win`.

`menu_item_toggle_switch` is a GMenuItem instance created with two arguments.

- The first argument, "Toggle Switch", is the label displayed on the menu.
- The second argument, "win.toggle-switch", is the action name.
The prefix "win." indicates that the action belongs to the window.

```c
static void
toggle_switch_changed(GSimpleAction *action, GVariant *value, gpointer user_data) {
  GtkWidget *label = GTK_WIDGET (user_data);

  if (g_variant_get_boolean (value))
    gtk_label_set_text (GTK_LABEL (label), "ON");
  else
    gtk_label_set_text (GTK_LABEL (label), "OFF");
  g_simple_action_set_state (action, value);
}
```

This is the signal handler connected to the "change-state" signal of the "toggle-switch" action.

The handler `toggle_switch_changed` has three parameters:

- The first parameter is the action that emits the "change-state" signal.
- The second parameter, value, is the requested new state of the action.
- The third parameter is the `user_data` that was passed to `g_signal_connect`.
In this case, it is the pointer to the GtkLabel.

The handler checks the boolean value of the requested state.
If it is TRUE, it sets the label's text to "ON". Otherwise, it sets the text to "OFF".

Finally, it sets the actual state of the action using `g_simple_action_set_state`.

Note: Before this function is called, the action still holds its original state.
You must explicitly call `g_simple_action_set_state` to update the action to the new state.

As discussed in the previous section, by exclusively handling the "change-state" signal and omitting the handler for the "activate" signal, this single handler gracefully manages both menu clicks and any direct state-change requests.

## GVariant and GVariantType

### GVariant

Before moving on to the "Stateful Actions with Parameters" subsection, we will discuss GVariant, as these parameters are passed as GVariant instances.
This subsection and the next subsection describes GVariant and GVariantType.

GVariant is a fundamental data type in GLib.
Unlike many other classes in GTK, it is not a child of GObject.
It acts as a strongly-typed container that can hold various types of data, such as booleans, strings, and numeric values (e.g., int16, int32, int64, double).

You might wonder why we need a specialized container like GVariant instead of using standard C types.
There are three primary reasons:

- **Serialization and Portability:** GVariant is designed to be easily "serialized" (converted into a stream of bytes).
This makes it perfect for storing data in files, sending data over a network (like D-Bus for inter-process communication), or passing parameters between different programming languages (language bindings).
- **Self-Describing Types:** In C, data types are defined at compile time, not at runtime.
Consequently, a receiver cannot natively determine the data type of a generic pointer at runtime.
In contrast, GVariant packages the data along with its type information, ensuring that the receiver always knows exactly what kind of data they are handling.
- **Flexibility in UI Actions:** In GTK, actions (like "Quit", "Copy", or "Change Color") are often shared across different UI elements (menus, shortcuts, buttons).
Since these UI elements might pass different types of data (e.g., a color name as a string, or a zoom level as a double), GVariant serves as a universal, type-safe wrapper that allows any action to receive any data format safely.

The following code creates a GVariant holding a boolean value of TRUE:

```C
GVariant *value = g_variant_new_boolean (TRUE);
```

Similarly, you can create a GVariant containing a string:

```C
GVariant *value2 = g_variant_new_string ("Hello");
```

To extract the original data from a GVariant, you use the `g_variant_get_*` family of functions.
For instance, you can retrieve the boolean value from our first example like this:

```C
gboolean bool_val = g_variant_get_boolean (value); /* bool_val is now TRUE */
```

In the same way, you can extract the string from value2:

```C
const char *str = g_variant_get_string (value2, NULL);
```

The second parameter of `g_variant_get_string` is a pointer to a `gsize` variable (`gsize` is defined as an `unsigned integer`).
If you pass a valid pointer, the function stores the length of the string in that variable.
If you pass NULL, the length is simply ignored.

Note: The returned string (`str`) is owned by the GVariant instance.
It must not be modified or freed by the caller.

### GVariantType

GVariantType represents the type of a GVariant.
It is initialized using a type string.
Type string examples are:

- "b": Boolean type.
- "s": String type.

See [GLib documentation](https://docs.gtk.org/glib/struct.VariantType.html) for details about type string.

The following program demonstrates how to create, use, and free a GVariantType instance.
This example outputs the string "s".

```c
#include <glib.h>

int
main (int argc, char **argv) {
  GVariantType *vtype = g_variant_type_new ("s");
  const char *type_string = g_variant_type_peek_string (vtype);
  g_print ("%s\n",type_string);
  g_variant_type_free (vtype);
}
```

- `g_variant_type_new` creates a GVariantType structure based on the provided type string (e.g., "s" for a string).
The caller assumes ownership of the returned structure and must release it using `g_variant_type_free` when it is no longer needed.
- `g_variant_type_peek_string` retrieves the type string (in this case, "s") from the GVariantType instance.
The returned string is owned by the GVariantType instance itself, so the caller **must not** modify or free it.
- `g_print` outputs the type string to the terminal.
Note that `vtype` must not be freed before this function is called, as the string pointer depends on the lifetime of the `vtype` instance.
- `g_variant_type_free` frees the GVariantType structure.

Alternatively, for standard types, GLib provides predefined macros.
This is generally preferred as it avoids dynamic allocation.

```c
#include <glib.h>

int
main (int argc, char **argv) {
  const GVariantType *vtype = G_VARIANT_TYPE_STRING;
  g_print ("%s\n", g_variant_type_peek_string (vtype));
}
```

`G_VARIANT_TYPE_STRING` is a macro that provides a pointer to a pre-allocated GVariantType structure representing a string.
Because this is a constant, it is managed by the system; you do not need to (and should not) call `g_variant_type_free` on it.
Using these macros is more efficient and safer as it eliminates the risk of memory leaks or double-free errors.

## Stateful Actions with Parameters

Next example of stateful actions is an action used with color-selection menus.
Imagine three separate menus, each offering the options "red," "green," and "blue".
These menus are used to set the background color of a GtkLabel widget.

In this scenario, a single action can be connected to all three menus.
The action maintains the selected color as its state, using a string value such as 'red', 'green', or 'blue'
When a user makes a selection, the chosen color is passed as a parameter to the signal handler, which then updates the label accordingly.

```C
... ... ...
GSimpleAction *act_color
  = g_simple_action_new_stateful ("color", G_VARIANT_TYPE_STRING, g_variant_new_string ("red"));
GMenuItem *menu_item_red = g_menu_item_new ("Red", "app.color::red");
GMenuItem *menu_item_green = g_menu_item_new ("Green", "app.color::green");
GMenuItem *menu_item_blue = g_menu_item_new ("Blue", "app.color::blue");
GtkCssProvider *provider = gtk_css_provider_new ();
g_signal_connect (act_color, "change-state", G_CALLBACK (color_state_changed), provider);
... ... ...
```

- The variable `act_color` points to a GSimpleAction instance.
It is created with `g_simple_action_new_stateful`.
  - The first argument, "color", is the name of the action.
  - The second argument is the parameter type. `G_VARIANT_TYPE_STRING` represents a string type for GVariant.
  - The third argument is the initial state of the action.
  `g_variant_new_string ("red")` returns a GVariant holding the string "red".
  GVariant uses a floating reference system; `g_simple_action_new_stateful` consumes this floating reference, so you do not need to manually release the GVariant instance.
- The variable `menu_item_red` points to a GMenuItem instance.
The function `g_menu_item_new` takes two arguments: the label ("Red") and a detailed action name ("app.color::red").
The suffix after the `::` is the target, which is passed to the action as a parameter when the menu item is activated.
The same logic applies to `menu_item_green` and `menu_item_blue`.
- The signal handler `color_state_changed` is connected to the "change-state" signal.
This signal is emitted when the action's state is requested to be changed.
Typically, clicking one of the menu items activates the action, and this activation is subsequently forwarded to the "change-state" signal.
We pass `provider` to the handler so that the CSS can be updated accordingly.
We will discuss the details of GtkCssProvider and how it handles CSS later.
- Signal connection: `g_signal_connect` links the "change-state" signal of `act_color` to the `color_state_changed` callback.
When a menu item is selected, the handler is called with the new target value (the color).

```c
static void
color_state_changed(GSimpleAction *action, GVariant *value, gpointer user_data) {
  GtkCssProvider *provider = GTK_CSS_PROVIDER (user_data);
  char *color = g_strdup_printf ("label.lb {background-color: %s; font-size: 72pt; color: white;}", g_variant_get_string (value, NULL));
  /* Change the CSS data in the provider. */
  /* Previous data is thrown away. */
  gtk_css_provider_load_from_string (provider, color);
  g_free (color);
  g_simple_action_set_state (action, value);
}
```

- The handler takes three parameters.
  - action: The GSimpleAction that emitted the "change-state" signal.
  - value: The new state (a GVariant string) passed to the action, representing the selected color.
  - user_data: The data passed from `g_signal_connect`. In this case, it is the GtkCssProvider instance.
- We use `g_strdup_printf` to construct a CSS rule.
`g_variant_get_string` extracts the raw string (e.g., "red") from the GVariant parameter.
This color is then injected into the CSS rule "label.lb { background-color: %s; ... }".
- `gtk_css_provider_load_from_string` updates the provider with the new CSS rule.
This automatically overrides any previous styles loaded into this provider.
See the following "GTK CSS styling" subsection below.
- Finally, `g_simple_action_set_state` updates the action's internal state to the new value.

### GTK CSS Styling

GTK uses a CSS-based styling system that works very similarly to web CSS (CSS is an abbreviation of Cascading Style Sheet).
To style a widget, you typically follow these three steps:

- GtkCssProvider: This acts as a container for your CSS rules.
You load your CSS strings or files into this provider.
- Display-wide Application: Register your GtkCssProvider with the GdkDisplay using `gtk_style_context_add_provider_for_display`.
This applies the CSS rules globally to all widgets on that display.
**GdkDisplay** represents your application's connection to the underlying windowing system.
Because of this, it manages not only the output screens (monitors) but also the input devices (keyboards and mice) you use to interact with the app.
The **"default" display** is simply the primary connection your computer assigned when the app launched.
Standard applications only use this default connection.
We call `gdk_display_get_default()` to access this main connection.
In a web browser, CSS styles a single webpage.
In GTK, adding a CSS provider to the GdkDisplay acts as a "global stylesheet".
It ensures that **your CSS rules are automatically applied to every widget associated with that GdkDisplay**.
- Selectors: Like in HTML, you use selectors to target widgets (e.x. `label.lb`):
  - `label`: Targets the node name of all GtkLabel widgets.
  - `.lb`: Targets any widget that has the "lb" class added (using `gtk_widget_add_css_class`).
  - `label.lb`: Combines both, targeting only GtkLabel widgets that also have the `lb` class.

By updating the GtkCssProvider dynamically via `gtk_css_provider_load_from_string`, you can change the look of your UI in real-time.
Since the provider is registered at the display level, all widgets matching the selectors will automatically reflect the updated styles.

## Example Program for Stateful Actions

This is an example program to demonstrate for the stateful actions. 

```c
#include <gtk/gtk.h>

static void
toggle_switch_changed(GSimpleAction *action, GVariant *value, gpointer user_data) {
  GtkWidget *label = GTK_WIDGET (user_data);

  if (g_variant_get_boolean (value))
    gtk_label_set_text (GTK_LABEL (label), "ON");
  else
    gtk_label_set_text (GTK_LABEL (label), "OFF");
  g_simple_action_set_state (action, value);
}

static void
color_state_changed(GSimpleAction *action, GVariant *value, gpointer user_data) {
  GtkCssProvider *provider = GTK_CSS_PROVIDER (user_data);
  char *color = g_strdup_printf ("label.lb {background-color: %s; font-size: 72pt; color: white;}", g_variant_get_string (value, NULL));
  /* Change the CSS data in the provider. */
  /* Previous data is thrown away. */
  gtk_css_provider_load_from_string (provider, color);
  g_free (color);
  g_simple_action_set_state (action, value);
}

static void
app_shutdown (GApplication *app, GtkCssProvider *provider) {
  gtk_style_context_remove_provider_for_display (gdk_display_get_default(), GTK_STYLE_PROVIDER (provider));
  g_object_unref (provider);
}

static void
app_activate (GApplication *app) {
  GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_title (GTK_WINDOW (win), "menu2");
  gtk_window_set_default_size (GTK_WINDOW (win), 800, 600);

  GtkWidget *label = gtk_label_new ("OFF");
  gtk_widget_add_css_class (label, "lb"); /* the class is used by CSS Selector */
  gtk_window_set_child (GTK_WINDOW (win), label);

  GSimpleAction *act_toggle_switch
    = g_simple_action_new_stateful ("toggle-switch", NULL, g_variant_new_boolean (FALSE));
  g_signal_connect (act_toggle_switch, "change-state", G_CALLBACK (toggle_switch_changed), label);
  g_action_map_add_action (G_ACTION_MAP (win), G_ACTION (act_toggle_switch));
  g_object_unref (act_toggle_switch);
  gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (win), TRUE);

  gtk_window_present (GTK_WINDOW (win));
}

static void
app_startup (GApplication *app) {
  /* The provider below provides application wide CSS data. */
  GtkCssProvider *provider = gtk_css_provider_new ();
  GSimpleAction *act_color
    = g_simple_action_new_stateful ("color", G_VARIANT_TYPE_STRING, g_variant_new_string ("red"));
  GSimpleAction *act_quit
    = g_simple_action_new ("quit", NULL);
  g_signal_connect (act_color, "change-state", G_CALLBACK (color_state_changed), provider);
  g_signal_connect_swapped (act_quit, "activate", G_CALLBACK (g_application_quit), app);
  g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_color));
  g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_quit));
  g_object_unref (act_color);
  g_object_unref (act_quit);

  GMenu *menubar = g_menu_new ();
  GMenu *menu = g_menu_new ();
  GMenu *section1 = g_menu_new ();
  GMenu *section2 = g_menu_new ();
  GMenu *section3 = g_menu_new ();
  GMenuItem *menu_item_toggle_switch = g_menu_item_new ("Toggle Switch", "win.toggle-switch");
  GMenuItem *menu_item_red = g_menu_item_new ("Red", "app.color::red");
  GMenuItem *menu_item_green = g_menu_item_new ("Green", "app.color::green");
  GMenuItem *menu_item_blue = g_menu_item_new ("Blue", "app.color::blue");
  GMenuItem *menu_item_quit = g_menu_item_new ("Quit", "app.quit");

  g_menu_append_item (section1, menu_item_toggle_switch);
  g_menu_append_item (section2, menu_item_red);
  g_menu_append_item (section2, menu_item_green);
  g_menu_append_item (section2, menu_item_blue);
  g_menu_append_item (section3, menu_item_quit);
  g_object_unref (menu_item_red);
  g_object_unref (menu_item_green);
  g_object_unref (menu_item_blue);
  g_object_unref (menu_item_toggle_switch);
  g_object_unref (menu_item_quit);

  g_menu_append_section (menu, NULL, G_MENU_MODEL (section1));
  g_menu_append_section (menu, "Color", G_MENU_MODEL (section2));
  g_menu_append_section (menu, NULL, G_MENU_MODEL (section3));
  g_menu_append_submenu (menubar, "Menu", G_MENU_MODEL (menu));
  g_object_unref (section1);
  g_object_unref (section2);
  g_object_unref (section3);
  g_object_unref (menu);

  gtk_application_set_menubar (GTK_APPLICATION (app), G_MENU_MODEL (menubar));
  g_object_unref (menubar);

  /* Initialize the css data */
  gtk_css_provider_load_from_string (provider, "label.lb {background-color: red; font-size: 72pt; color: white;}");
  /* Add CSS to the default GdkDisplay. */
  gtk_style_context_add_provider_for_display (gdk_display_get_default (),
        GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_signal_connect (app, "shutdown", G_CALLBACK (app_shutdown), provider);
}

#define APPLICATION_ID "com.github.ToshioCP.menu2"

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

- 3-22: Action signal handlers.
The `toggle_switch_changed` handler toggles the label text between "ON" and "OFF" depending on the boolean state.
The `color_state_changed` handler dynamically generates a CSS string to change the background color of the label and loads it into the GtkCssProvider.
In both handlers, `g_simple_action_set_state()` is called at the end to explicitly update the action's state.
- 25-29: The handler `app_shutdown` is called right before the application quits.
It safely cleans up by removing the CSS provider from the default display and unreferencing it.
- 31-49: The activate signal handler.
- 33-35: A new application window is created and assigned to `win`.
Its title and default size are set to "menu2" and 800x600, respectively.
- 37-39: A new label is created with the initial text "OFF" and assigned to `label`.
It is given a CSS class "lb" and added to `win` as its child widget.
- 41-44: A stateful toggle action is created and assigned to `act_toggle_switch` with an initial boolean value of FALSE.
It is connected to the signal handler `toggle_switch_changed` and added to the window.
Because it is added to the window's action map, its scope is "win" (i.e., "win.toggle-switch").
The action object is then unreferenced.
- 46: The function `gtk_application_window_set_show_menubar` is called to ensure the menubar is displayed in the window.
- 48: Shows the window using `gtk_window_present()`.
- 51-106: The startup signal handler.
- 53-64: A new GtkCssProvider is created to handle application-wide CSS.
Two actions, `act_color` (stateful) and `act_quit` (stateless), are created.
Since the startup handler is called only once, there is only one instance of each action.
They are connected to their respective handlers, added to the application, and then unreferenced.
Because they are added to the application's action map, their scope is "app".
- 66-95: The menus are built.
GMenuItem objects are created and appended to distinct sections (section1, section2, section3) to logically group them (e.g., Toggle Switch, Colors, Quit).
Unnecessary references to the menu items and sections are freed.
- 97-98: The fully constructed menubar is applied to the application.
- 100-104: The CSS provider is initialized with default CSS data (setting the label's background to red and font size to 72pt) and added to the default display so it affects all widgets.
- 105: Finally, the "shutdown" signal on the application is connected to the `app_shutdown` handler.

## Compiling and Running

Change your current directory to `src/menu`.

~~~
$ bash comp menu2
$./a.out
~~~

You will see a window with the text "OFF" in the center and a red background.
You can change the text to "ON" and back to "OFF" by clicking the "Toggle Switch" menu item.
You can change the background color by clicking the "Red", "Green", or "Blue" menu items.

If you run the application a second time while the first instance is still running, another window will appear on the screen.
Note that both windows belong to the primary application instance.
The second instance simply emits the "activate" signal to the primary instance and then immediately quits.

```
$ ./a.out & ./a.out
$ 
```

The "ON" and "OFF" states in the two windows are independent of each other because each window has its own GtkLabel instance.

However, both windows share the same GdkDisplay (the overall screen environment).
Since the background color is determined by the CSS applied to the GdkDisplay, both windows will always share the same background color.

**Note for WSL users:**

You may notice that the menu remains on the screen after being clicked, and only disappears on your next click.
This is a known issue specific to WSL.
On a native Linux environment, the menu closes immediately after a selection is made.

![menu2](/src/images/menu2.png)

Up: [README.md](../README.md),  Prev: [Section 16](sec16.md), Next: [Section 18](sec18.md)
