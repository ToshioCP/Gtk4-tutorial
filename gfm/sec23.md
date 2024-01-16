Up: [README.md](../README.md),  Prev: [Section 22](sec22.md), Next: [Section 24](sec24.md)

# Pango, CSS and Application

## PangoFontDescription

PangoFontDescription is a C structure for a font.
You can get font family, style, weight and size.
You can also get a string that includes font attributes.
For example, suppose that the PangoFontDescription has a font of "Noto Sans Mono", "Bold", "Italic" and 12 points of size.
Then the string converted from the PangoFontDescription is "Noto Sans Mono Bold Italic 12".

- Font family is  "Noto Sans Mono".
- Font style is "Italic".
- Font weight is "Bold", or 700.
- Font size is 12 pt.

The font in CSS is different from the string from PangoFontDescription.

- `font: bold italic 12pt "Noto Sans Mono"`
- `Noto Sans Mono Bold Italic 12`

So, it may be easier to use each property, i.e. font-family, font-style, font-weight and font-size, to convert a PangoFontDescription data to CSS.

Refer to
[Pango document](https://docs.gtk.org/Pango/index.html)
and [W3C CSS Fonts Module Level 3](https://www.w3.org/TR/css-fonts-3/) for further information.

## Converter from PangoFontDescription to CSS

Two files `pfd2css.h` and `pfd2css.c` include the converter from PangoFontDescription to CSS.

~~~C
 1 #pragma once
 2 
 3 #include <pango/pango.h>
 4 
 5 // Pango font description to CSS style string
 6 // Returned string is owned by the caller. The caller should free it when it becomes useless.
 7 
 8 char*
 9 pfd2css (PangoFontDescription *pango_font_desc);
10 
11 // Each element (family, style, weight and size)
12 
13 const char*
14 pfd2css_family (PangoFontDescription *pango_font_desc);
15 
16 const char*
17 pfd2css_style (PangoFontDescription *pango_font_desc);
18 
19 int
20 pfd2css_weight (PangoFontDescription *pango_font_desc);
21 
22 // Returned string is owned by the caller. The caller should free it when it becomes useless.
23 char *
24 pfd2css_size (PangoFontDescription *pango_font_desc);
~~~

The five functions are public.
The first function is a convenient function to set other four CSS at once.

~~~C
 1 #include <pango/pango.h>
 2 #include "pfd2css.h"
 3 
 4 // Pango font description to CSS style string
 5 // Returned string is owned by caller. The caller should free it when it is useless.
 6 
 7 char*
 8 pfd2css (PangoFontDescription *pango_font_desc) {
 9   char *fontsize;
10 
11   fontsize = pfd2css_size (pango_font_desc);
12   return g_strdup_printf ("font-family: \"%s\"; font-style: %s; font-weight: %d; font-size: %s;",
13               pfd2css_family (pango_font_desc), pfd2css_style (pango_font_desc),
14               pfd2css_weight (pango_font_desc), fontsize);
15   g_free (fontsize); 
16 }
17 
18 // Each element (family, style, weight and size)
19 
20 const char*
21 pfd2css_family (PangoFontDescription *pango_font_desc) {
22   return pango_font_description_get_family (pango_font_desc);
23 }
24 
25 const char*
26 pfd2css_style (PangoFontDescription *pango_font_desc) {
27   PangoStyle pango_style = pango_font_description_get_style (pango_font_desc);
28   switch (pango_style) {
29   case PANGO_STYLE_NORMAL:
30     return "normal";
31   case PANGO_STYLE_ITALIC:
32     return "italic";
33   case PANGO_STYLE_OBLIQUE:
34     return "oblique";
35   default:
36     return "normal";
37   }
38 }
39 
40 int
41 pfd2css_weight (PangoFontDescription *pango_font_desc) {
42   PangoWeight pango_weight = pango_font_description_get_weight (pango_font_desc);
43   switch (pango_weight) {
44   case PANGO_WEIGHT_THIN:
45     return 100;
46   case PANGO_WEIGHT_ULTRALIGHT:
47     return 200;
48   case PANGO_WEIGHT_LIGHT:
49     return 300;
50   case PANGO_WEIGHT_SEMILIGHT:
51     return 350;
52   case PANGO_WEIGHT_BOOK:
53     return 380;
54   case PANGO_WEIGHT_NORMAL:
55     return 400; /* or "normal" */
56   case PANGO_WEIGHT_MEDIUM:
57     return 500;
58   case PANGO_WEIGHT_SEMIBOLD:
59     return 600;
60   case PANGO_WEIGHT_BOLD:
61     return 700; /* or "bold" */
62   case PANGO_WEIGHT_ULTRABOLD:
63     return 800;
64   case PANGO_WEIGHT_HEAVY:
65     return 900;
66   case PANGO_WEIGHT_ULTRAHEAVY:
67     return 900; /* 1000 is available since CSS Fonts level 4 but GTK currently supports level 3. */
68   default:
69     return 400; /* "normal" */
70   }
71 }
72 
73 char *
74 pfd2css_size (PangoFontDescription *pango_font_desc) {
75   if (pango_font_description_get_size_is_absolute (pango_font_desc))
76     return g_strdup_printf ("%dpx", pango_font_description_get_size (pango_font_desc) / PANGO_SCALE);
77   else
78     return g_strdup_printf ("%dpt", pango_font_description_get_size (pango_font_desc) / PANGO_SCALE);
79 }
~~~

- The function `pfd2css_family` returns font family.
- The function `pfd2css_style` returns font style which is one of "normal", "italic" or "oblique".
- The function `pfd2css_weight` returns font weight in integer. See the list below.
- The function `pfd2css_size` returns font size.
  - If the font description size is absolute, it returns the size of device unit, which is pixel. Otherwise the unit is point.
  - The function `pango_font_description_get_size` returns the integer of the size but it is multiplied by `PANGO_SCALE`.
So, you need to divide it by `PANGO_SCALE`.
The `PANGO_SCALE` is currently 1024, but this might be changed in the future.
If the font size is 12pt, the size in pango is `12*PANGO_SCALE=12*1024=12288`.
- The function `pfd2css` returns a string of the font.
For example, if a font "Noto Sans Mono Bold Italic 12" is given,
it returns "font-family: Noto Sans Mono; font-style: italic; font-weight: 700; font-size: 12pt;".

The font weight number is one of:

- 100 - Thin
- 200 - Extra Light (Ultra Light)
- 300 - Light
- 400 - Normal
- 500 - Medium
- 600 - Semi Bold (Demi Bold)
- 700 - Bold
- 800 - Extra Bold (Ultra Bold)
- 900 - Black (Heavy)

## Application object

### TfeApplication class

TfeApplication class is a child of GtkApplication.
It has some instance variables.
The header file defines the type macro and a public function.

~~~C
1 #pragma once
2 
3 #include <gtk/gtk.h>
4 
5 #define TFE_TYPE_APPLICATION tfe_application_get_type ()
6 G_DECLARE_FINAL_TYPE (TfeApplication, tfe_application, TFE, APPLICATION, GtkApplication)
7 
8 TfeApplication *
9 tfe_application_new (const char* application_id, GApplicationFlags flag);
~~~

The following code is extracted from `tfeapplication.c`.
It builds TfeApplication class and instance.

```C
#include <gtk/gtk.h>
#include "tfeapplication.h"

struct _TfeApplication {
  GtkApplication parent;
  TfeWindow *win;
  GSettings *settings;
  GtkCssProvider *provider;
};

G_DEFINE_FINAL_TYPE (TfeApplication, tfe_application, GTK_TYPE_APPLICATION)

static void
tfe_application_dispose (GObject *gobject) {
  TfeApplication *app = TFE_APPLICATION (gobject);

  g_clear_object (&app->settings);
  g_clear_object (&app->provider);
  G_OBJECT_CLASS (tfe_application_parent_class)->dispose (gobject);
}

static void
tfe_application_init (TfeApplication *app) {
  app->settings = g_settings_new ("com.github.ToshioCP.tfe");
  g_signal_connect (app->settings, "changed::font-desc", G_CALLBACK (changed_font_cb), app);
  app->provider = gtk_css_provider_new ();
}

static void
tfe_application_class_init (TfeApplicationClass *class) {
  G_OBJECT_CLASS (class)->dispose = tfe_application_dispose;
  G_APPLICATION_CLASS (class)->startup = app_startup;
  G_APPLICATION_CLASS (class)->activate = app_activate;
  G_APPLICATION_CLASS (class)->open = app_open;
}

TfeApplication *
tfe_application_new (const char* application_id, GApplicationFlags flag) {
  return TFE_APPLICATION (g_object_new (TFE_TYPE_APPLICATION, "application-id", application_id, "flags", flag, NULL));
}
```

- The structure `_TfeApplication` is defined.
It has four members.
One is its parents and the others are kinds of instance variables.
The members are usually initialized in the instance initialization function.
And they are released in the disposal function or freed in the finalization function.
The members are:
  - win: main window instance
  - settings: GSettings instance.it is bound to "font-desc" item in the GSettings
  - provider: a provider for the font of the textview.
- The macro `G_DEFINE_FINAL_TYPE` defines `tfe_application_get_type` function and some other useful things.
- The function `tfe_application_class_init` initializes the TfeApplication class.
It overrides four class methods.
Three class methods `startup`, `activate` and `open` points the default signal handlers.
The overriding changes the default handlers.
You can connect the handlers with `g_signal_connect` macro but the result is different.
The macro connects a user handler to the signal.
The default handler still exists and no change is made to them.
- The function `tfe_application_init` initializes an instance.
  - Creates a new GSettings instance and make `app->settings` point it. Then connects the handler `changed_font_cb` to the "changed::font-desc" signal.
  - Creates a new GtkCssProvider instance and make `app->provider` point it.
- The function `tfe_application_dispose` releases the GSettings and GtkCssProvider instances.
Then, call the parent's dispose handler. It is called "chaining up".
See [GObject document](https://docs.gtk.org/gobject/tutorial.html#chaining-up).

### Startup signal handlers

~~~C
 1 static void
 2 app_startup (GApplication *application) {
 3   TfeApplication *app = TFE_APPLICATION (application);
 4   int i;
 5   GtkCssProvider *provider = gtk_css_provider_new ();
 6   GdkDisplay *display;
 7 
 8   G_APPLICATION_CLASS (tfe_application_parent_class)->startup (application);
 9 
10   app->win = TFE_WINDOW (tfe_window_new (GTK_APPLICATION (app)));
11 
12   gtk_css_provider_load_from_data (provider, "textview {padding: 10px;}", -1);
13   display = gdk_display_get_default ();
14   gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider),
15                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
16   g_object_unref (provider);
17   gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (app->provider),
18                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
19 
20   changed_font_cb (app->settings, "font-desc", app); // Sets the text view font to the font from the gsettings data base.
21 
22 /* ----- accelerator ----- */ 
23   struct {
24     const char *action;
25     const char *accels[2];
26   } action_accels[] = {
27     { "win.open", { "<Control>o", NULL } },
28     { "win.save", { "<Control>s", NULL } },
29     { "win.close", { "<Control>w", NULL } },
30     { "win.new", { "<Control>n", NULL } },
31     { "win.saveas", { "<Shift><Control>s", NULL } },
32     { "win.close-all", { "<Control>q", NULL } },
33   };
34 
35   for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
36     gtk_application_set_accels_for_action(GTK_APPLICATION(app), action_accels[i].action, action_accels[i].accels);
37 }
~~~

The function `app_startup` replace the default signal handlers.
It does five things.

- Calls the parent's startup handler. It is called "chaining up".
The "startup" default handler runs before user handlers.
So the call for the parent's handler must be done at the beginning.
- Creates the main window.
This application has only one top level window.
In that case, it is a good way to create the window in the startup handler, which is called only once.
Activate or open handlers can be called twice or more.
Therefore, if you create a window in the activate or open handler, two or more windows can be created.
- Sets the default display CSS to "textview {padding: 10px;}".
It sets the GtkTextView, or TfeTextView, padding to 10px and makes the text easier to read.
This CSS is fixed and never changed through the application life.
- Adds another CSS provider, which is pointed by `app->provider`, to the default display.
This CSS depends on the GSettings "font-desc" value and it can be changed during the application life time.
And calls `changed_font_cb` to update the font CSS setting.
- Sets application accelerator with the function `gtk_application_set_accels_for_action`.
Accelerators are kinds of short cut key functions.
For example, `Ctrl+O` is an accelerator to activate "open" action.
Accelerators are written in the array `action-accels[]`.
Its element is a structure `struct {const char *action; const char *accels[2];}`.
The member `action` is an action name.
The member `accels` is an array of two pointers.
For example, `{"win.open", { "<Control>o", NULL }}` tells that the accelerator `Ctrl+O` is connected to the "win.open" action.
The second element of `accels` is NULL which is the end mark.
You can define more than one accelerator keys and the list must ends with NULL (zero).
If you want to do so, the array length needs to be three or more.
For example, `{"win.open", { "<Control>o", "<Alt>o", NULL }}` means two accelerators `Ctrl+O` and `Alt+O` is connected to the "win.open" action.
The parser recognizes "\<control\>o", "\<Shift\>\<Alt\>F2", "\<Ctrl\>minus" and so on.
If you want to use symbol key like "\<Ctrl\>-", use "\<Ctrl\>minus" instead.
Such relation between lower case and symbol (character code) is specified in [`gdkkeysyms.h`](https://gitlab.gnome.org/GNOME/gtk/-/blob/master/gdk/gdkkeysyms.h) in the GTK 4 source code.

### Activate and open signal handlers

Two functions `app_activate` and `app_open` replace the default signal handlers.

~~~C
 1 static void
 2 app_activate (GApplication *application) {
 3   TfeApplication *app = TFE_APPLICATION (application);
 4 
 5   tfe_window_notebook_page_new (app->win);
 6   gtk_window_present (GTK_WINDOW (app->win));
 7 }
 8 
 9 static void
10 app_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
11   TfeApplication *app = TFE_APPLICATION (application);
12 
13   tfe_window_notebook_page_new_with_files (app->win, files, n_files);
14   gtk_window_present (GTK_WINDOW (app->win));
15 }
~~~
 
The original default handlers don't do useful works and you don't need to chain up to the parent's default handlers.
They just create notebook pages and show the top level window.

### CSS font setting

~~~C
 1 static void
 2 changed_font_cb (GSettings *settings, char *key, gpointer user_data) {
 3   TfeApplication *app = TFE_APPLICATION (user_data);
 4   char *font, *s, *css;
 5   PangoFontDescription *pango_font_desc;
 6 
 7   if (g_strcmp0(key, "font-desc") != 0)
 8     return;
 9   font = g_settings_get_string (app->settings, "font-desc");
10   pango_font_desc = pango_font_description_from_string (font);
11   g_free (font);
12   s = pfd2css (pango_font_desc); // converts Pango Font Description into CSS style string
13   pango_font_description_free (pango_font_desc);
14   css = g_strdup_printf ("textview {%s}", s);
15   gtk_css_provider_load_from_data (app->provider, css, -1);
16   g_free (s);
17   g_free (css);
18 }
~~~

The function `changed_font_cb` is a handler for "changed::font-desc" signal on the GSettings instance.
The signal name is "changed" and "font-desc" is a key name.
This signal is emitted when the value of the "font-desc" key is changed.
The value is bound to the "font-desc" property of the GtkFontDialogButton instance.
Therefore, the handler `changed_font_cb` is called when the user selects and updates a font through the font dialog.

A string is retrieved from the GSetting database and converts it into a pango font description.
And a CSS string is made by the function `pfd2css` and `g_strdup_printf`.
Then the css provider is set to the string.
The provider has been inserted to the current display in advance.
So, the font is applied to the display.

## Other files

main.c

~~~C
 1 #include <gtk/gtk.h>
 2 #include "tfeapplication.h"
 3 
 4 #define APPLICATION_ID "com.github.ToshioCP.tfe"
 5 
 6 int
 7 main (int argc, char **argv) {
 8   TfeApplication *app;
 9   int stat;
10 
11   app = tfe_application_new (APPLICATION_ID, G_APPLICATION_HANDLES_OPEN);
12   stat =g_application_run (G_APPLICATION (app), argc, argv);
13   g_object_unref (app);
14   return stat;
15 }
16 
~~~

Resource XML file.

~~~xml
1 <?xml version="1.0" encoding="UTF-8"?>
2 <gresources>
3   <gresource prefix="/com/github/ToshioCP/tfe">
4     <file>tfewindow.ui</file>
5     <file>tfepref.ui</file>
6     <file>tfealert.ui</file>
7     <file>menu.ui</file>
8   </gresource>
9 </gresources>
~~~

GSchema XML file

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <schemalist>
 3   <schema path="/com/github/ToshioCP/tfe/" id="com.github.ToshioCP.tfe">
 4     <key name="font-desc" type="s">
 5       <default>'Monospace 12'</default>
 6       <summary>Font</summary>
 7       <description>A font to be used for textview.</description>
 8     </key>
 9   </schema>
10 </schemalist>
~~~

Meson.build

~~~meson
 1 project('tfe', 'c', license : 'GPL-3.0-or-later', meson_version:'>=1.0.1', version: '0.5')
 2 
 3 gtkdep = dependency('gtk4')
 4 
 5 gnome = import('gnome')
 6 resources = gnome.compile_resources('resources','tfe.gresource.xml')
 7 gnome.compile_schemas(depend_files: 'com.github.ToshioCP.tfe.gschema.xml')
 8 
 9 sourcefiles = files('main.c', 'tfeapplication.c', 'tfewindow.c', 'tfepref.c', 'tfealert.c', 'pfd2css.c', '../tfetextview/tfetextview.c')
10 
11 executable(meson.project_name(), sourcefiles, resources, dependencies: gtkdep, export_dynamic: true, install: true)
12 
13 schema_dir = get_option('prefix') / get_option('datadir') / 'glib-2.0/schemas/'
14 install_data('com.github.ToshioCP.tfe.gschema.xml', install_dir: schema_dir)
15 gnome.post_install (glib_compile_schemas: true)
~~~

- The function `project` defines project and initialize meson.
The first argument is the project name and the second is the language name.
The other arguments are keyword arguments.
- The function `dependency` defines the denpendent library.
Tfe depends GTK4.
This is used to create `pkg-config` option in the command line of C compiler to include header files and link libraries.
The returned object `gtkdep` is used as an argument to the `executable` function later.
- The function `import` imports an extension module.
The GNOME module has some convenient methods like `gnome.compile_resources` and `gnome.compile_schemas`.
- The method `gnome.compile_resources` compiles and creates resource files.
The first argument is the resource name without extension and the second is the name of XML file.
The returned value is an array `['resources,c', 'resources.h']`.
- The function `gnome.compile_schemas` compiles the schema files in the current directory.
This just creates `gschemas.compiled` in the build directory.
It is used to test the executable binary in the build directory.
The function doesn't install the schema file.
- The function `files` creates a File Object.
- The function `executable` defines the compilation elements such as target name, source files, dependencies and installation.
The target name is "tfe".
The source files are elements of 'sourcefiles' and `resources'.
It uses GTK4 libraries.
It can be installed.
- The last three lines are post install work.
The variable `schema_dir` is the directory stored the schema file.
If meson runs with `--prefix=$HOME/.local` argument, it is `$HOME/.local/share/glib-2.9/schemas`.
The function `install_data` copies the first argument file into the second argument directory.
The method `gnome.post_install` runs `glib-compile-schemas` and updates `gschemas_compiled` file.

## Compilation and installation.

If you want to install it to your local area, use `--prefix=$HOME/.local` or `--prefix=$HOME` option.
If you want to install it to the system area, no option is needed.
It will be installed under `/user/local` directory.

~~~
$ meson setup --prefix=$HOME/.local _build
$ ninja -C _build
$ ninja -C _build install
~~~

You need root privilege to install it to the system area..

~~~
$ meson setup _build
$ ninja -C _build
$ sudo ninja -C _build install
~~~

Source files are in [src/tfe6](../src/tfe6) directory.

We made a very small text editor.
You can add features to this editor.
When you add a new feature, be careful about the structure of the program.
Maybe you need to divide a file into several files.
It isn't good to put many things into one file.
And it is important to think about the relationship between source files and widget structures.

The source files are in the [Gtk4 tutorial GitHub repository](https://github.com/ToshioCP/Gtk4-tutorial).
Download it and see `src/tfe6` directory.

Note: When the menu button is clicked, error messages are printed.

```
(tfe:31153): Gtk-CRITICAL **: 13:05:40.746: _gtk_css_corner_value_get_x: assertion 'corner->class == &GTK_CSS_VALUE_CORNER' failed
```

I found a [message](https://discourse.gnome.org/t/menu-button-gives-error-messages-with-latest-gtk4/15689) in the GNOME Discourse.
The comment says that GTK 4.10 has a bug and it is fixed in the version 4.10.5.
I haven't check 4.10.5 yet, where the UBUNTU GTK4 is still 4.10.4.

Up: [README.md](../README.md),  Prev: [Section 22](sec22.md), Next: [Section 24](sec24.md)
