# GtkFontDialogButton and Gsettings

## The preference dialog

If the user clicks on the preference menu, a preference dialog appears.

![Preference dialog](../image/pref_dialog.png){width=4cm height=1.6cm}

It has only one button, which is a GtkFontDialogButton widget.
You can add more widgets on the dialog but this simple dialog isn't so bad for the first example program.

If the button is clicked, a FontDialog appears like this.

![Font dialog](../image/fontdialog.png){width=6.27cm height=7.38cm}

If the user chooses a font and clicks on the select button, the font is changed.

GtkFontDialogButton and GtkFontDialog are available since GTK version 4.10.
They replace GtkFontButton and GtkFontChooserDialog, which are deprecated since 4.10.

## A composite widget

The preference dialog has GtkBox, GtkLabel and GtkFontButton in it and is defined as a composite widget.
The following is the template ui file for TfePref.

@@@include
tfe6/tfepref.ui
@@@

- Template tag specifies a composite widget.
The class attribute specifies the class name, which is "TfePref".
The parent attribute is `GtkWindow`.
Therefore. `TfePref` is a child class of `GtkWindow`.
A parent attribute is optional but it is recommended to write it explicitly.
You can make TfePref as a child of `GtkDialog`, but `GtkDialog` is deprecated since version 4.10.
- There are three properties, title, resizable and modal.
- TfePref has a child widget GtkBox which is horizontal.
The box has two children GtkLabel and GtkFontDialogButton.

## The header file

The file `tfepref.h` defines types and declares a public function.

@@@include
tfe6/tfepref.h
@@@

- 5: Defines the type `TFE_TYPE_PREF`, which is a macro replaced by `tfe_pref_get_type ()`.
- 6: The macro `G_DECLAER_FINAL_TYPE` expands to:
  - The function `tfe_pref_get_type ()` is declared.
  - TfePrep type is defined as a typedef of `struct _TfePrep`.
  - TfePrepClass type is defined as a typedef of `struct {GtkWindowClass *parent;}`.
  - Two functions `TFE_PREF ()` and `TFE_IS_PREF ()` is defined.
- 8-9:The function `tfe_pref_new` is declared. It creates a new TfePref instance.

## The C file for composite widget

The following codes are extracted from the file `tfepref.c`.

@@@if gfm
```C
#include <gtk/gtk.h>
#include "tfepref.h"

struct _TfePref
{
  GtkWindow parent;
  GtkFontDialogButton *font_dialog_btn;
};

G_DEFINE_FINAL_TYPE (TfePref, tfe_pref, GTK_TYPE_WINDOW);

static void
tfe_pref_dispose (GObject *gobject) {
  TfePref *pref = TFE_PREF (gobject);
  gtk_widget_dispose_template (GTK_WIDGET (pref), TFE_TYPE_PREF);
  G_OBJECT_CLASS (tfe_pref_parent_class)->dispose (gobject);
}

static void
tfe_pref_init (TfePref *pref) {
  gtk_widget_init_template (GTK_WIDGET (pref));
}

static void
tfe_pref_class_init (TfePrefClass *class) {
  G_OBJECT_CLASS (class)->dispose = tfe_pref_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfepref.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfePref, font_dialog_btn);
}

GtkWidget *
tfe_pref_new (void) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_PREF, NULL));
}
```
@@@else
```{.C}
#include <gtk/gtk.h>
#include "tfepref.h"

struct _TfePref
{
  GtkWindow parent;
  GtkFontDialogButton *font_dialog_btn;
};

G_DEFINE_FINAL_TYPE (TfePref, tfe_pref, GTK_TYPE_WINDOW);

static void
tfe_pref_dispose (GObject *gobject) {
  TfePref *pref = TFE_PREF (gobject);
  gtk_widget_dispose_template (GTK_WIDGET (pref), TFE_TYPE_PREF);
  G_OBJECT_CLASS (tfe_pref_parent_class)->dispose (gobject);
}

static void
tfe_pref_init (TfePref *pref) {
  gtk_widget_init_template (GTK_WIDGET (pref));
}

static void
tfe_pref_class_init (TfePrefClass *class) {
  G_OBJECT_CLASS (class)->dispose = tfe_pref_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfepref.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfePref, font_dialog_btn);
}

GtkWidget *
tfe_pref_new (void) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_PREF, NULL));
}
```
@@@end

- The structure `_TfePref` has `font_dialog_btn` member.
It points the GtkFontDialogButton object specified in the XML file "tfepref.ui".
The member name `font_dialog_btn` must be the same as the GtkFontDialogButton id attribute in the XML file.
- `G_DEFINE_FINAL_TYPE` macro expands to:
  - The declaration of the functions `tfe_pref_init` and `tfe_pref_class_init`.
They are defined in the following part of the program.
  - The definition of the variable `tfe_pref_parent_class`.
  - The definition of the function `tfe_pref_get_type`.
- The function `tfe_pref_class_init` initializes the TfePref class.
The function `gtk_widget_class_set_template_from_resource` initializes the composite widget template from the XML resource.
The function `gtk_widget_class_bind_template_child` connects the TfePref structure member `font_dialog_btn` and the GtkFontDialogButton in the XML.
The member name and the id attribute value must be the same.
- The function `tfe_pref_init` initializes a newly created instance. The function `gtk_widget_init_template` creates and initializes child widgets.
- The function `tfe_pref_dispose` releases objects. The function `gtk_widget_dispose_template` releases child widgets.

## GtkFontDialogButton and Pango

If the GtkFontDialogButton button is clicked, the GtkFontDialog dialog appears.
A user can choose a font on the dialog.
If the user clicks on the "select" button, the dialog disappears.
And the font information is given to the GtkFontDialogButton instance.
The font data is taken with the method `gtk_font_dialog_button_get_font_desc`.
It returns a pointer to the PangoFontDescription structure.

Pango is a text layout engine.
The [documentation](https://docs.gtk.org/Pango/index.html) is on the internet.

PangoFontDescription is a C structure and it isn't allowed to access directly.
The document is [here](https://docs.gtk.org/Pango/struct.FontDescription.html).
If you want to retrieve the font information, there are several functions.

- `pango_font_description_to_string` returns a string like "Jamrul Bold Italic Semi-Expanded 12".
- `pango_font_description_get_family` returns a font family like "Jamrul".
- `pango_font_description_get_weight` returns a PangoWeight constant like `PANGO_WEIGHT_BOLD`.
- `pango_font_description_get_style` returns a PangoStyle constant like `PANGO_STYLE_ITALIC`.
- `pango_font_description_get_stretch` returns a PangoStretch constant like `PANGO_STRETCH_SEMI_EXPANDED`.
- `pango_font_description_get_size` returns an integer like `12`.
Its unit is point or pixel (device unit).
The function `pango_font_description_get_size_is_absolute` returns TRUE if the unit is absolute that means device unit.
Otherwise the unit is point.

## GSettings

We want to maintain the font data after the application quits.
There are some ways to implement.

- Make a configuration file.
For example, a text file "~/.config/tfe/font_desc.cfg" keeps font information.
- Use GSettings object.
The basic idea of GSettings are similar to configuration file.
Configuration information data is put into a database file.

GSettings is simple and easy to use but a bit hard to understand the concept.
This subsection describes the concept first and then how to program it.

### GSettings schema

GSettings schema describes a set of keys, value types and some other information.
GSettings object uses this schema and it writes/reads the value of a key to/from the right place in the database.

- A schema has an id.
The id must be unique.
We often use the same string as application id, but schema id and application id are different.
You can use different name from application id.
Schema id is a string delimited by periods.
For example, "com.github.ToshioCP.tfe" is a correct schema id.
- A schema usually has a path.
The path is a location in the database.
Each key is stored under the path.
For example, if a key `font-desc` is defined with a path `/com/github/ToshioCP/tfe/`,
the key's location in the database is `/com/github/ToshioCP/tfe/font-desc`.
Path is a string begins with and ends with a slash (`/`).
And it is delimited by slashes.
- GSettings save information as key-value style.
Key is a string begins with a lower case character followed by lower case, digit or dash (`-`) and ends with lower case or digit.
No consecutive dashes are allowed.
Values can be any type.
GSettings stores values as GVariant type, which can be, for example, integer, double, boolean, string or complex types like an array.
The type of values needs to be defined in the schema.
- A default value needs to be set for each key.
- A summery and description can be set for each key optionally.

Schemas are described in an XML format.
For example,

@@@include
tfe6/com.github.ToshioCP.tfe.gschema.xml
@@@

- 4: The type attribute is "s".
It is GVariant type string.
For GVariant type string, see [GLib API Reference -- GVariant Type Strings](https://docs.gtk.org/glib/struct.VariantType.html#gvariant-type-strings).
Other common types are:
  - "b": gboolean
  - "i": gint32.
  - "d": double.

Further information is in:

- [GLib API Reference -- GVariant Format Strings](https://docs.gtk.org/glib/gvariant-format-strings.html)
- [GLib API Reference -- GVariant Text Format](https://docs.gtk.org/glib/gvariant-text.html)
- [GLib API Reference -- GVariant](https://docs.gtk.org/glib/struct.Variant.html)
- [GLib API Reference -- VariantType](https://docs.gtk.org/glib/struct.VariantType.html)

### Gsettings command

First, let's try `gsettings` application.
It is a configuration tool for GSettings.

~~~
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
~~~

List schemas.

~~~
$ gsettings list-schemas
org.gnome.rhythmbox.podcast
ca.desrt.dconf-editor.Demo.Empty
org.gnome.gedit.preferences.ui
org.gnome.evolution-data-server.calendar
org.gnome.rhythmbox.plugins.generic-player

... ...

~~~

Each line is an id of a schema.
Each schema has a key-value configuration data.
You can see them with list-recursively command.
Let's look at the keys and values of `org.gnome.calculator` schema.

~~~
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
~~~

This schema is used by GNOME Calculator.
Run the calculator and change the mode, then check the schema again.

~~~
$ gnome-calculator
~~~

![gnome-calculator basic mode](../image/gnome_calculator_basic.png){width=5.34cm height=5.97cm}


Change the mode to advanced and quit.

![gnome-calculator advanced mode](../image/gnome_calculator_advanced.png){width=10.74cm height=7.14cm}

Run gsettings and check the value of `button-mode`.

~~~
$ gsettings list-recursively org.gnome.calculator

... ...

org.gnome.calculator button-mode 'advanced'

... ...

~~~

Now we know that GNOME Calculator used gsettings and it has set `button-mode` key to "advanced".
The value remains even the calculator quits.
So when the calculator runs again, it will appear as an advanced mode.

### Glib-compile-schemas utility

GSettings schemas are specified with an XML format.
The XML schema files must have the filename extension `.gschema.xml`.
The following is the XML schema file for the application `tfe`.

@@@include
tfe6/com.github.ToshioCP.tfe.gschema.xml
@@@

The filename is "com.github.ToshioCP.tfe.gschema.xml".
Schema XML filenames are usually the schema id followed by ".gschema.xml" suffix.
You can use different name from schema id, but it is not recommended.

- 2: The top level element is `<schemalist>`.
- 3: schema tag has `path` and `id` attributes.
A path determines where the settings are stored in the conceptual global tree of settings.
An id identifies the schema.
- 4: Key tag has two attributes.
Name is the name of the key.
Type is the type of the value of the key and it is a GVariant Format String.
- 5: default value of the key `font-desc` is `Monospace 12`.
- 6: Summery and description elements describes the key.
They are optional, but it is recommended to add them in the XML file.

The XML file is compiled by glib-compile-schemas.
When compiling, `glib-compile-schemas` compiles all the XML files which have ".gschema.xml" file extension in the directory given as an argument.
It converts the XML file into a binary file `gschemas.compiled`.
Suppose the XML file above is under `tfe6` directory.

~~~
$ glib-compile-schemas tfe6
~~~

Then, `gschemas.compiled` is generated under `tfe6`.
When you test your application, set `GSETTINGS_SCHEMA_DIR` environment variable so that GSettings objet can find `gschemas.compiled`.

~~~
$ GSETTINGS_SCHEMA_DIR=(the directory gschemas.compiled is located):$GSETTINGS_SCHEMA_DIR (your application name)
~~~

GSettings object looks for this file by the following process.

- It searches `glib-2.0/schemas` subdirectories of all the directories specified in the environment variable `XDG_DATA_DIRS`.
Common directores are `/usr/share/glib-2.0/schemas` and `/usr/local/share/glib-2.0/schemas`.
- If `$HOME/.local/share/glib-2.0/schemas` exists, it is also searched.
- If `GSETTINGS_SCHEMA_DIR` environment variable is defined, it searches all the directories specified in the variable.
`GSETTINGS_SCHEMA_DIR` can specify multiple directories delimited by colon (:).

The directories above includes more than one `.gschema.xml` file.
Therefore, when you install your application, follow the instruction below to install your schemas.

1. Make `.gschema.xml` file.
2. Copy it to one of the directories above. For example, `$HOME/.local/share/glib-2.0/schemas`.
3. Run `glib-compile-schemas` on the directory.
It compiles all the schema files in the directory and creates or updates the database file `gschemas.compiled`.

### GSettings object and binding

Now, we go on to the next topic, how to program GSettings.

You need to compile your schema file in advance.

Suppose id, key, class name and a property name are:

- GSettings id: com.github.ToshioCP.sample
- GSettings key: sample_key
- The class name: Sample
- The property to bind: sample_property

The example below uses `g_settings_bind`.
If you use it, GSettings key and instance property must have the same the type.
In the example, it is assumed that the type of "sample_key" and "sample_property" are the same.

@@@if gfm
```C
GSettings *settings;
Sample *sample_object;

settings = g_settings_new ("com.github.ToshioCP.sample");
sample_object = sample_new ();
g_settings_bind (settings, "sample_key", sample_object, "sample_property", G_SETTINGS_BIND_DEFAULT);
```
@@@else
```{.C}
GSettings *settings;
Sample *sample_object;

settings = g_settings_new ("com.github.ToshioCP.sample");
sample_object = sample_new ();
g_settings_bind (settings, "sample_key", sample_object, "sample_property", G_SETTINGS_BIND_DEFAULT);
```
@@@end

The function `g_settings_bind` binds the GSettings value and the property of the instance.
If the property value is changed, the GSettings value is also changed, and vice versa.
The two values are always the same.

The function `g_settings_bind` is simple and easy but it isn't always possible.
The type of GSettings are restricted to the type GVariant has.
Some property types are out of GVariant.
For example, GtkFontDialogButton has "font-desc" property and its type is PangoFontDescription.
PangoFontDescription is a C structure and it is wrapped in a boxed type GValue to store in the property.
GVariant doesn't support boxed type.

In that case, another function `g_settings_bind_with_mapping` is used.
It binds GSettings GVariant value and object property via GValue with mapping functions.

@@@if gfm
```C
void
g_settings_bind_with_mapping (
  GSettings* settings,
  const gchar* key,
  GObject* object,
  const gchar* property,
  GSettingsBindFlags flags, // G_SETTINGS_BIND_DEFAULT is commonly used
  GSettingsBindGetMapping get_mapping, // GSettings => property, See the example below
  GSettingsBindSetMapping set_mapping, // property => GSettings, See the example below
  gpointer user_data, // NULL if unnecessary
  GDestroyNotify destroy //NULL if unnecessary
)
```
@@@else
```{.C}
void
g_settings_bind_with_mapping (
  GSettings* settings,
  const gchar* key,
  GObject* object,
  const gchar* property,
  GSettingsBindFlags flags, // G_SETTINGS_BIND_DEFAULT is commonly used
  GSettingsBindGetMapping get_mapping, // GSettings => property, See the example below
  GSettingsBindSetMapping set_mapping, // property => GSettings, See the example below
  gpointer user_data, // NULL if unnecessary
  GDestroyNotify destroy //NULL if unnecessary
)
```
@@@end

The mapping functions are defined like these:

@@@if gfm
```C
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
@@@else
```{.C}
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
@@@end

The following codes are extracted from `tfepref.c`.

@@@include
tfe6/tfepref.c get_mapping set_mapping tfe_pref_init
@@@

- 15-21: This function `tfe_pref_init` initializes the new TfePref instance.
- 18: Creates a new GSettings instance. The id is "com.github.ToshioCP.tfe".
- 19-20: Binds the GSettings "font-desc" and the GtkFontDialogButton property "font-desc". The mapping functions are `get_mapping` and `set_mapping`.
- 1-7: The mapping function from GSettings to the property.
The first argument `value` is a GValue to be stored in the property.
The second argument `variant` is a GVarinat structure that comes from the GSettings value.
- 3: Retrieves a string from the GVariant structure.
- 4: Build a PangoFontDescription structure from the string and assigns its address to `font_desc`.
- 5: Puts `font_desc` into the GValue `value`.
The ownership of `font_desc` moves to `value`.
- 6: Returns TRUE that means the mapping succeeds.
- 9-13: The mapping function from the property to GSettings.
The first argument `value` holds the property data.
The second argument `expected_type` is the type of GVariant that the GSettings value has.
It isn't used in this function.
- 11: Gets the PangoFontDescription structure from `value` and converts it to string.
- 12: The string is inserted to a GVariant structure.
The ownership of the string `font_desc_string` moves to the returned value.

## C file

The following is the full codes of `tfepref.c`

@@@include
tfe6/tfepref.c
@@@

## Test program

There's a test program located at `src/tfe6/test` directory.

@@@include
tfe6/test/test_pref.c
@@@

This program sets its active window to TfePref instance, which is a child object of GtkWindow.

It sets the "changed::font-desc" signal handler in the startup function.
The process from the user's font selection to the handler is:

- The user clicked on the GtkFontDialogButton and GtkFontDialog appears.
- He/she selects a new font.
- The "font-desc" property of the GtkFontDialogButton instance is changed.
- The value of "font-desc" key on the GSettings database is changed since it is bound to the property.
- The "changed::font-desc" signal on the GSettings instance is emitted.
- The handler is called.

The program building is divided into four steps.

- Compile the schema file
- Compile the XML file to a resource (C source file)
- Compile the C files
- Run the executable file

Commands are shown in the next four sub-subsections.
You don't need to try them.
The final sub-subsection shows the meson-ninja way, which is the easiest.

### Compile the schema file

```
$ cd src/tef6/test
$ cp ../com.github.ToshioCP.tfe.gschema.xml com.github.ToshioCP.tfe.gschema.xml
$ glib-compile-schemas .
```

Be careful. The commands `glib-compile-schemas` has an argument ".", which means the current directory.
This results in creating `gschemas.compiled` file.

### Compile the XML file

```
$ glib-compile-resources --sourcedir=.. --generate-source --target=resource.c ../tfe.gresource.xml
```

### Compile the C file

```
$ gcc `pkg-config --cflags gtk4` test_pref.c ../tfepref.c resource.c `pkg-config --libs gtk4`
```

### Run the executable file

```
$ GSETTINGS_SCHEMA_DIR=. ./a.out

Jamrul Italic Semi-Expanded 12 # <= select Jamrul Italic 12
Monospace 12 #<= select Monospace Regular 12
```

### Meson-ninja way

Meson wraps up the commands above.
Create the following text and save it to `meson.build`.

Note: Gtk4-tutorial repository has meson.build file that defines several tests.
So you can try it instead of the following text.

```
project('tfe_pref_test', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','../tfe.gresource.xml', source_dir: '..')
gnome.compile_schemas(build_by_default: true, depend_files: 'com.github.ToshioCP.tfe.gschema.xml')

executable('test_pref', ['test_pref.c', '../tfepref.c'], resources, dependencies: gtkdep, export_dynamic: true, install: false)
```

- Project name is 'tfe\_pref\_test' and it is written in C language.
- It depends on GTK4 library.
- It uses GNOME module. Modules are prepared by Meson.
- GNOME module has `compile_resources` method.
When you call this method, you need the prefix "gnome.".
  - The target filename is resources.
  - The definition XML file is '../tfe.gresource.xml'.
  - The source dir is '..'. All the ui files are located there.
- GNOME module has `compile_schemas` method.
It compiles the schema file 'com.github.ToshioCP.tfe.gschema.xml'.
You need to copy '../com.github.ToshioCP.tfe.gschema.xml' to the current directory in advance.
- It creates an executable file 'test_pref'.
The source files are 'test_pref.c', '../tfepref.c' and `resources`, which is made by `gnome.compile_resources`.
It depends on `gtkdep`, which is GTK4 library.
The symbols are exported and no installation support.

Type like this to build and test the program.

```
$ cd src/tef6/test
$ cp ../com.github.ToshioCP.tfe.gschema.xml com.github.ToshioCP.tfe.gschema.xml
$ meson setup _build
$ ninja -C _build
$ GSETTINGS_SCHEMA_DIR=_build _build/test_pref
```

A window appears and you can choose a font via GtkFontDialog.
If you select a new font, the font string is output through the standard output.
