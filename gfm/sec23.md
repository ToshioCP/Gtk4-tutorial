Up: [README.md](../README.md),  Prev: [Section 22](sec22.md), Next: [Section 24](sec24.md)

# Accelerators

## Accelerators

Accelerators are the mechanism used to implement keyboard shortcuts.
For instance, `Ctrl+c` is a common shortcut for copying text.

Because accelerators are incredibly convenient, you should definitely implement them for frequently used features.
Especially in a keyboard-centric application like our Text File Editor (TFE), allowing users to complete tasks without reaching for the mouse is essential for a comfortable user experience.

## Accelerators and Actions

Accelerators are rarely used in isolation; they are usually paired with buttons or menu items.
For example, most text editors have an "Open File" button or menu item, but you can achieve the same result by pressing `Ctrl+o`.

In GTK, the UI elements that trigger events (like buttons, menus, and shortcut keys) are separated from the "actions" that perform the actual work.
Thanks to this architecture, you can connect multiple types of events to a single action, keeping your code clean and straightforward.
Without actions, you would have to write the handling logic for every single event individually, which would lead to messy and complicated code.

Since accelerators function across the entire application, they should be defined in the application source file (e.g., `tfeapplication.c`).
To link an accelerator to an action, we use the `gtk_application_set_accels_for_action ()` function.

```C
void
gtk_application_set_accels_for_action (
  GtkApplication* application,
  const char* detailed_action_name,
  const char* const* accels
)
```

- `application`: The application instance.
- `detailed_action_name`: The name of the action (e.g., `"app.quit"`, `"win.save"`).
- `accels`: An array of strings representing the shortcut keys (the syntax is explained in the next subsection). This array must be `NULL`-terminated.

## Shortcut Key Strings

The GTK parser for shortcut keys can properly interpret strings like `<control>o`, `<Shift><Alt>F2`, and `<Ctrl>minus`.
You can find detailed information about these strings in the [documentation](https://docs.gtk.org/gtk4/func.accelerator_parse.html) for the `gtk_accelerator_parse ()` function.

This syntax is divided into two parts: a modifier key and a character key.
For example, `<control>o` is split into `<control>` and `o`.

- The syntax for modifier keys is quite flexible.
It allows both uppercase and lowercase letters, as well as abbreviations.
For instance, `<control>`, `<CONTROL>`, `<Ctrl>`, and `<Ctl>` all represent the Control key.
- For the character part, the parser calls the `gdk_keyval_from_name ()` function to translate the "key name string" into a key code.
This key name string **must be written in lowercase**.
This is because typing an uppercase 'O' on a keyboard requires pressing the Shift key and the 'o' key together, meaning an uppercase `O` is treated as being equivalent to `<Shift>o`.
Therefore, if you write `<Ctrl>O`, GTK will interpret it as `<Ctrl><Shift>o`.
- Because the character part must be a lowercase alphabet name, you cannot use symbols directly.
To represent the minus (`-`) key, for example, you must spell it out as `minus`.
This means `<Ctrl>-` will not work; you must write `<Ctrl>minus`.
The underlying definitions for these names can be found in the GTK source code within [gdkkeysyms.h](https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkkeysyms.h).
The key string is based on the macro name defined there (such as `GDK_KEY_minus`), simply with the `GDK_KEY_` prefix removed.

## Implementation in the TFE Program

Let's incorporate accelerators into our TFE (Text File Editor) C code.
Since shortcuts should be registered when the application initializes, we will add them to the `"startup"` signal handler.

For example, to link `Ctrl+o` to the `"win.open"` action, you would write:

```C
const char *open_accels[] = {"<Ctrl>o", NULL};
gtk_application_set_accels_for_action(GTK_APPLICATION(app), "win.open", open_accels);
```

When you have multiple shortcut keys, the best practice is to define them in an array and register them using a loop.

```C
  struct {
    const char *action;
    const char *accels[2];
  } action_accels[] = {
    { "win.open", { "<Control>o", NULL } },
    { "win.save", { "<Control>s", NULL } },
    { "win.close", { "<Control>w", NULL } },
    { "win.new", { "<Control>n", NULL } },
    { "win.saveas", { "<Shift><Control>s", NULL } },
    { "app.quit", { "<Control>q", NULL } },
  };

  for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), action_accels[i].action, action_accels[i].accels);
```

- 1-4: This is the structure definition.
It might look a bit complex, but it becomes clearer when you look at the initialization values starting on line 5.
The first member is the action name, like `"win.open"`.
The second member is an array of accelerators (shortcut strings), ending with a `NULL` terminator.
Since we are only registering one shortcut per action here, an array size of 2 (the shortcut plus `NULL`) is sufficient.
If you wanted to assign two shortcuts to a single action, you would need to increase the array size to 3.
The empty brackets `[]` after `action_accels` tell the compiler to calculate the array size automatically based on the initialization data, so we can omit the number.
- 5-10: This is the actual list pairing each action with its corresponding accelerator.
Note that we will add a menu and action for a Preferences dialog later on, but since that isn't a frequently used feature, we are omitting an accelerator for it here.

## Build and Run

The complete source code is located in the [src/tfe9](../src/tfe9/) directory.
Navigate into that directory and type the following commands in your terminal to compile and run the application:

```
$ meson setup _build
$ ninja -C _build
$ _build/tfe
```

Try pressing shortcut keys like `Ctrl+o` to confirm that they are working correctly.

Up: [README.md](../README.md),  Prev: [Section 22](sec22.md), Next: [Section 24](sec24.md)
