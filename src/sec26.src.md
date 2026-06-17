# Gsettings

This section introduces GSettings, a convenient API that allows applications to save and retrieve settings persistently.

## GSettings

We want to save the user's font preferences so they persist even after the application quits.
There are a few ways to achieve this:

- Create a custom configuration file:
For example, we could manually read and write font information to a simple text file like `~/.config/tfe/font_desc.cfg`.
- Use the GSettings API:
While the basic concept is similar to a configuration file, GSettings handles all the heavy lifting.
It safely stores the configuration data in a system-specific backend database (such as dconf on Linux).
This means we don't have to write our own text parsers or worry about file I/O errors.

Using GSettings in your code is simple and highly effective, but its underlying architecture can be a bit tricky to grasp at first.
This subsection will explain the core concepts of GSettings before diving into the actual programming.

### GSettings Schema

A GSettings schema defines a set of keys, their data types, and other metadata.
The `GSettings` object uses this schema to correctly read and write key values to the backend database.

- Schema ID: Every schema must have a unique identifier.
It is formatted as a reverse-DNS string delimited by periods (e.g., `com.github.ToshioCP.tfe`).
While the schema ID and the application ID are technically distinct, it is a common and highly recommended convention to use the exact same string for both.
- Path: A schema typically has a path, which acts as its location directory in the database.
A path must start and end with a slash (`/`), and its internal segments are delimited by slashes.
For example, if a key `font-desc` is defined under the path `/com/github/ToshioCP/tfe/`, its absolute location in the database becomes `/com/github/ToshioCP/tfe/font-desc`.
- Keys and Values: GSettings stores information as key-value pairs.
  - Keys: A key name must begin with a lowercase letter, followed by lowercase letters, digits, or dashes (`-`), and end with a lowercase letter or digit.
  Consecutive dashes are not allowed.
  - Values: Values are stored as `GVariant` types, meaning they can be simple types (integers, doubles, booleans, strings) or complex types (like arrays).
  The exact type for each key must be explicitly defined in the schema.
- Default Value: Every key must have a mandatory default value.
- Summary and Description: You can optionally provide a short summary and a detailed description for each key to document its purpose.

Schemas are written in XML format.
For example:

@@@include
tfe12/com.github.ToshioCP.tfe.gschema.xml
@@@

- 4: The `type` attribute is `"s"`, which represents a `GVariant` string.

For more details on GVariant type strings, see the [GLib API Reference -- GVariant Type Strings](https://docs.gtk.org/glib/struct.VariantType.html#gvariant-type-strings).

Other common type strings include:

- `"b"`: boolean (`gboolean`)
- `"i"`: 32-bit integer (`gint32`)
- `"d"`: double-precision floating point (`double`)

For further information, refer to the official documentation:

- [GLib API Reference -- GVariant Format Strings](https://docs.gtk.org/glib/gvariant-format-strings.html)
- [GLib API Reference -- GVariant Text Format](https://docs.gtk.org/glib/gvariant-text-format.html)
- [GLib API Reference -- GVariant](https://docs.gtk.org/glib/struct.Variant.html)
- [GLib API Reference -- VariantType](https://docs.gtk.org/glib/struct.VariantType.html)

### Gsettings Command

### The gsettings Command

First, let's try the `gsettings` command-line utility.
It is a useful tool for inspecting and modifying GSettings configurations directly from the terminal.

```
$ gsettings help
Usage:
  gsettings --version
  gsettings [--schemadir SCHEMADIR] COMMAND [ARGS?]

Commands:
  help                      Show this information
  list-schemas              List installed schemas
  list-relocatable-schemas  List relocatable schemas
  list-keys                 List keys in a schema
  list-children             List children of a schema
  list-recursively          List keys and values, recursively
  range                     Queries the range of a key
  describe                  Queries the description of a key
  get                       Get the value of a key
  set                       Set the value of a key
  reset                     Reset the value of a key
  reset-recursively         Reset all values in a given schema
  writable                  Check if a key is writable
  monitor                   Watch for changes

Use "gsettings help COMMAND" to get detailed help.
```

To see all available schemas on your system, use the `list-schemas` command:

```
$ gsettings list-schemas
org.gnome.rhythmbox.podcast
ca.desrt.dconf-editor.Demo.Empty
org.gnome.gedit.preferences.ui
org.gnome.evolution-data-server.calendar
org.gnome.rhythmbox.plugins.generic-player

... ...

```

Each line represents a unique schema ID.
Since schemas contain key-value configuration data, you can inspect their contents using the `list-recursively` command.
Let's look at the keys and values for the `org.gnome.calculator` schema:

```
$ gsettings list-recursively org.gnome.calculator
org.gnome.calculator accuracy 9
org.gnome.calculator angle-units 'degrees'
org.gnome.calculator base 10
org.gnome.calculator button-mode 'basic'
org.gnome.calculator number-format 'automatic'
org.gnome.calculator precision 2000
org.gnome.calculator refresh-interval 604800
org.gnome.calculator show-thousands false
org.gnome.calculator show-zeroes false
org.gnome.calculator source-currency ''
org.gnome.calculator source-units 'degree'
org.gnome.calculator target-currency ''
org.gnome.calculator target-units 'radian'
org.gnome.calculator window-position (-1, -1)
org.gnome.calculator word-size 64
```

This schema is used by the GNOME Calculator application.
Let's run the calculator, change its mode, and see how the schema updates.

```
$ gnome-calculator
```

@@@if pdf
![gnome-calculator basic mode](/images/gnome_calculator_basic.png){width=5.34cm height=5.97cm}
@@@else
![gnome-calculator basic mode](/images/gnome_calculator_basic.png)
@@@end

Change the calculator mode to "Advanced" and quit the application.

@@@if pdf
![gnome-calculator advanced mode](/images/gnome_calculator_advanced.png){width=10.74cm height=7.14cm}
@@@else
![gnome-calculator advanced mode](/images/gnome_calculator_advanced.png)
@@@end

Run `gsettings` again and check the value of the `button-mode` key:

```
$ gsettings list-recursively org.gnome.calculator

... ...

org.gnome.calculator button-mode 'advanced'

... ...

```

This demonstrates that GNOME Calculator uses GSettings.
It updated the `button-mode` key to `'advanced'`, and this value persists even after the application is closed.
Consequently, the next time you launch the calculator, it will read this setting and automatically open in Advanced mode.

### The glib-compile-schemas Utility

GSettings schemas are specified in an XML format.
The schema files must have the `.gschema.xml` extension.
The following is the XML schema file for the `tfe` application:

@@@include
tfe12/com.github.ToshioCP.tfe.gschema.xml
@@@

The filename is `com.github.ToshioCP.tfe.gschema.xml`.
Schema XML filenames typically consist of the schema ID followed by the `.gschema.xml` extension.
While you can use a name different from the schema ID, it is unconventional and not recommended.

- 2: The top-level element is `<schemalist>`.
- 3: The `<schema>` tag has `path` and `id` attributes.
The `path` determines where the settings are stored in the conceptual global tree of settings, while the `id` uniquely identifies the schema.
- 4: The `<key>` tag has `name` and `type` attributes.
The `name` is the identifier of the key, and the `type` specifies the data type of the key's value using a GVariant Format String.
- 5: The `<default>` tag sets the initial value of the `font-desc` key to `Sans Regular 12`.
Note that the value must be enclosed in quotes because it is a GVariant string.
- 6-7: The `<summary>` and `<description>` elements describe the key.
Although they are optional, it is recommended to include them in your XML file to document the settings.

These XML files must be compiled using the `glib-compile-schemas` utility.
When executed, it compiles all files with the `.gschema.xml` extension in the specified directory and converts them into a single binary file named `gschemas.compiled`. 

For example, suppose the XML file above is located in the `tfe12` directory:

```
$ glib-compile-schemas tfe12
```

This command generates the `gschemas.compiled` file inside the `tfe12` directory.
When testing your application locally, you must set the `GSETTINGS_SCHEMA_DIR` environment variable so the `GSettings` object can find your newly compiled schema:

```
$ GSETTINGS_SCHEMA_DIR=(path_to_directory_with_gschemas.compiled)
```

At runtime, the `GSettings` object looks for the compiled schema using the following process:

- It searches the `glib-2.0/schemas` subdirectories of all the directories specified in the `XDG_DATA_DIRS` environment variable.
Common directories are `/usr/share/glib-2.0/schemas` and `/usr/local/share/glib-2.0/schemas`.
- If `$HOME/.local/share/glib-2.0/schemas` exists, it is also searched.
- If the `GSETTINGS_SCHEMA_DIR` environment variable is defined, it searches all the directories specified within it.
`GSETTINGS_SCHEMA_DIR` can specify multiple directories delimited by a colon (`:`).

System schema directories typically contain many `.gschema.xml` files.
Therefore, when installing your application, follow these steps to properly install your schemas:

1. Create your `.gschema.xml` file.
2. Copy it to one of the system schema directories mentioned above (for example, `$HOME/.local/share/glib-2.0/schemas`).
3. Run `glib-compile-schemas` on that directory.
This compiles all the schema files in the directory and creates or updates the `gschemas.compiled` binary file.
This binary file acts as a fast index for the schema definitions (such as data types and default values).
Note that this file is not the database itself.
On Linux systems, the database that contains the actual preferences is usually dconf.

### GSettings Object and Binding

Now, let's move on to the next topic: how to use GSettings in your C code.

Before writing the code, ensure your schema file is compiled.
Let's assume the following identifiers for our example:

- GSettings ID: `com.github.ToshioCP.sample`
- GSettings key: `sample_key`
- Class name: `Sample`
- Property to bind: `sample_property`

The example below uses `g_settings_bind`.
To use this function, the GSettings key and the instance property must have the exact same data type.
For this example, we assume `sample_key` and `sample_property` share the same type.

@@@if gfm
```C
@@@else
```{.c}
@@@end
GSettings *settings;
Sample *sample_object;

settings = g_settings_new ("com.github.ToshioCP.sample");
sample_object = sample_new ();
g_settings_bind (settings, "sample_key", sample_object, "sample_property", G_SETTINGS_BIND_DEFAULT);
```

The `g_settings_bind` function creates a bidirectional binding between the GSettings key and the object's property.
If the property value changes, the GSettings database updates automatically, and vice versa.
They are always kept in sync.  

While `g_settings_bind` is simple and convenient, it cannot be used in every situation.
GSettings keys are restricted to GVariant data types.
However, some object properties use complex types that cannot be directly mapped to a GVariant.  

For example, GtkFontDialogButton has a "font-desc" property of type PangoFontDescription.
Since PangoFontDescription is a C structure, it is wrapped in a boxed GValue for the object property system.
GVariant does not natively support these boxed types.

In these cases, you must use `g_settings_bind_with_mapping`.
This function allows you to bind a GVariant-based GSettings key to an object property by providing custom mapping functions that translate the data back and forth.
See the [GIO documentation](https://docs.gtk.org/gio/method.Settings.bind_with_mapping.html) for further details.

@@@if gfm
```C
@@@else
```{.c}
@@@end
void
g_settings_bind_with_mapping (
  GSettings* settings,
  const gchar* key,
  GObject* object,
  const gchar* property,
  GSettingsBindFlags flags, // G_SETTINGS_BIND_DEFAULT is commonly used
  GSettingsBindGetMapping get_mapping, // GSettings => property. See the example below.
  GSettingsBindSetMapping set_mapping, // property => GSettings. See the example below.
  gpointer user_data, // NULL if not needed
  GDestroyNotify destroy // NULL if not needed
)
```

The mapping functions are defined as follows:

@@@if gfm
```C
@@@else
```{.c}
@@@end
gboolean
(* GSettingsBindGetMapping) (
  GValue* value,
  GVariant* variant,
  gpointer user_data
)

GVariant*
(* GSettingsBindSetMapping) (
  const GValue* value,
  const GVariantType* expected_type,
  gpointer user_data
)
```

The following code is extracted from `tfepref.c`:

@@@include
tfe12/tfepref.c get_mapping set_mapping tfe_pref_new
@@@

- 1-7: These lines define the mapping function from GSettings to the object property.
The first argument, `value`, is an empty `GValue` where the property data will be stored.
The second argument, `variant`, is the `GVariant` structure retrieved from GSettings.
- 3: Retrieves the string from the `GVariant` structure.
- 4: Builds a `PangoFontDescription` structure from the string and assigns its address to `font_desc`.
- 5: Places `font_desc` into the `GValue` using `g_value_take_boxed`.
Notice that this function transfers the ownership of `font_desc` to the `GValue`, meaning we do not need to free it manually.
- 6: Returns `TRUE` to indicate that the mapping was successful.
- 9-16: These lines define the mapping function from the object property back to GSettings.
The first argument, `value`, holds the current property data.
The second argument, `expected_type`, specifies the expected `GVariant` type for GSettings.
It is unused in this specific function.
- 11: Retrieves the `PangoFontDescription` structure from the `GValue`.
- 12-13: If the font description is `NULL`, the function returns `NULL`.
This safely cancels the binding operation, meaning the corresponding GSettings value will not be updated.
- 14: Converts the font description back into a string.
- 15: Creates a new `GVariant` from the string using `g_variant_new_take_string`.
This function transfers the ownership of the newly allocated `font_desc_string` to the `GVariant`, which is then returned.
- 18-32: The `tfe_pref_new ()` function creates a new `TfePref` instance and initializes the GSettings binding.
- 25: Obtains the `GSettings` instance from the application.
- 26: Increments the reference count of the `GSettings` instance using `g_object_ref ()` to ensure the `TfePref` instance safely takes its ownership.
- 28-29: Binds the GSettings `"font-desc"` key to the `GtkFontDialogButton`'s `"font-desc"` property, utilizing our custom `get_mapping` and `set_mapping` functions.

## TfeApplication and GSettings

When the application starts, it needs to read the font data from GSettings and apply it to the CSS.
This should be handled inside the `startup` handler.

While the program is running, if the user changes the font in the preference dialog, there are two ways to reflect that change in the CSS:

1. In `tfepref.c`, catch the `notify` signal of the font button's "font-desc" property and call `tfe_style_manager_set_font ()`.
2. In `tfeapplication.c`, catch the "changed::font-desc" signal from GSettings and call `tfe_style_manager_set_font ()`.
The "changed" signal on a GSettings object is emitted when a key has potentially changed, but this signal does not guarantee that the value has actually changed.
You should call one of the g_settings_get() calls to check the new value.
This signal supports detailed connections.
You can connect to the detailed signal "changed::font-desc" in order to only receive callbacks when key "font-desc" changes.
**Note that settings only emits this signal if you have read key at least once while a signal handler was already connected for key.**

We will choose approach 2.
This ensures a consistent rule across our application: "The CSS always reflects GSettings."

The process from clicking the font button to updating the CSS is slightly complex.
Let's review the exact steps:

- The user clicks the `GtkFontDialogButton` and the `GtkFontDialog` appears.
- The user selects a new font.
- The "font-desc" property of the `GtkFontDialogButton` instance changes.
- The value of the "font-desc" key in the GSettings database changes because it is bound to the property.
- The "changed::font-desc" signal on the GSettings instance is emitted.
- The signal handler is called, and the CSS is updated.

In the `startup` handler of `tfeapplication.c`, we will first initialize the style manager, then create the GSettings instance, connect the signal, and finally apply the initial font to the CSS.

@@@if gfm
```C
@@@else
```{.c}
@@@end
  tfe_style_manager_initialize (base_css);

  tfe_app->settings = g_settings_new ("com.github.ToshioCP.tfe");
  g_signal_connect (tfe_app->settings, "changed::font-desc", G_CALLBACK (changed_font_cb), tfe_app);
  changed_font_cb (tfe_app->settings, "font-desc", tfe_app); /* set the initial font */
```

Since the callback function `changed_font_cb` relies on the style manager, it is important that the style manager is initialized beforehand.

The callback function is implemented as follows:

@@@include
tfe12/tfeapplication.c changed_font_cb
@@@

This callback is first called from the startup handler.
At that time, it calls `g_settings_get_string ()` to get the value of the "font-desc" key.
This ensures that the "changed::font-desc" signal will be emitted for future changes. 

Up until now, we assumed that the cleanup for GSettings would be written in the dispose handler.
However, we are going to change this and put the cleanup code in the shutdown handler instead.
Here are the simplified reasons why:

- Symmetry: As a general rule, objects created in the startup handler should be cleaned up in the shutdown handler.
- Application Lifecycle: startup and shutdown run only once during the lifetime of the primary application instance.
Secondary instances do not run them.
Since our GSettings initialization only happens in startup, the cleanup must logically happen in shutdown.
- Execution Order: The GSettings signal handler uses the style manager.
Therefore, we must clean up GSettings (which disconnects the handler) before terminating the style manager.
If we put the cleanup in dispose, this order gets reversed and causes issues.

The shutdown handler now looks like this:

@@@include
tfe12/tfeapplication.c app_shutdown
@@@

At this point, the following parts are no longer needed, so you can delete them from your code:

- The dispose handler.
- The dispose handler override inside the class initialization function.

## Building the Program

Building the program involves several steps:

- Compile the schema file.
- Compile the XML file into a C resource file.
- Compile the C source files.
- Run the executable file.
- (Optional:) If you install the program, copy the executable binary to a directory like `/usr/local/bin`, copy the schema file to a schema directory like `/usr/local/share/glib-2.0/schemas`, and run `glib-compile-schemas` in that directory.

Meson wraps all these commands up for us.
Create the following text and save it as `meson.build`:

@@@include
tfe12/meson.build
@@@

To build and run the program, change your current directory to [src/tfe12](tfe12/) and type the following commands in your terminal:

```
$ meson setup _build
$ ninja -C _build
$ GSETTINGS_SCHEMA_DIR=_build _build/tfe
```

In the last line, the `GSETTINGS_SCHEMA_DIR` environment variable points to the directory where the compiled schema file is located.

Our `meson.build` also supports installing the application to your system.
The executable will be installed in `/usr/local/bin` and the schema file in `/usr/local/share/glib-2.0/schemas`.
Keep in mind that you need administrator privileges to install it:

```
$ sudo ninja -C _build install
```

Once installed, you no longer need to set `GSETTINGS_SCHEMA_DIR`.
You can run the program from anywhere simply by typing its name:

```
$ tfe
```
