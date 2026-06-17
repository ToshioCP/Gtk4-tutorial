Up: [README.md](../README.md),  Prev: [Section 23](sec23.md), Next: [Section 25](sec25.md)

# CSS and Pango

GTK uses CSS (Cascading Style Sheets) to style widgets, just like web browsers do.
GTK supports the [W3C CSS Fonts Module Level 3](https://www.w3.org/TR/css-fonts-3/), but not necessarily every feature in the specification.
Refer to the official documentation for details:

* [CSS in GTK](https://docs.gtk.org/gtk4/css-overview.html)
* [GTK CSS Properties](https://docs.gtk.org/gtk4/css-properties.html)

We briefly used CSS in Section 17 to change a label's background color, focusing mostly on CSS providers.
Now, let's look closer at how CSS itself works in GTK.

## CSS Basics

### CSS Syntax

CSS uses very simple rules.
It combines three main elements: "Where" (Selector), "What" (Property), and "How" (Value).

```
selector {
  property: value;
  another-property: another-value;
}
```

- selector: The target widget you want to style.
- property: The attribute you want to change (like color, margin, or font).
- value: The specific setting for that property (like `red` or `10px`).

```css
button {
  color: white;             /* Text color */
  background-color: blue;   /* Background color */
  font-weight: bold;        /* Text weight */
}
```

### Types of Selectors

GTK CSS uses three main selectors to target widgets:

#### Node

Targets the widget type itself, applying the style to all widgets of that kind.
You can find these node names in the CSS section of the official GTK documentation for each widget.
For example, "window" is the node for GtkWindow, "button" for GtkButton, and "textview" for GtkTextView.

```css
/* Makes the background of ALL text views light gray */
textview {
  background-color: lightgray;
}
```

#### Class

Targets a specific group of widgets.
You define a class by adding a dot (`.`) before the class name.
For example, if you have four stacked labels, you could assign an `.upper` class to the top two and a `.lower` class to the bottom two.

```css
/* Applies only to labels with the "upper" class */
label.upper {
  background-color: green;
}
```

#### ID

Targets one unique widget in your entire application.
You define an ID by adding a hash (`#`) before the name.

```css
/* Applies ONLY to the specific widget named "my-main-button" */
#my-main-button {
  font-size: 20pt;
}
```

### Combining Selectors

In a real app, you might want to style a "button inside a header bar" differently from a normal button.
By **separating selectors with a space**, you can target "something inside something else" (called a descendant selector).

```css
/* Applies ONLY to buttons inside a header bar */
headerbar button {
  color: red;
}

/* Applies ONLY to buttons inside a box with the "upper" class */
box.upper button {
  font-weight: bold;
}
```

### The Box Model

GTK CSS uses the standard "Box Model" for sizing.
From the inside out, it consists of:

1. Content: The actual item, like text or an icon.
2. Padding: The inner spacing between the content and the border.
3. Border: The frame surrounding the widget (even if invisible).
4. Margin: The outer spacing that pushes neighboring widgets away.

```css
button {
  padding: 8px 16px;     /* Inner spacing: 8px top/bottom, 16px left/right */
  border: 2px solid red; /* Border: 2px solid red line */
  margin: 10px;          /* Outer spacing: 10px gap all around */
}
```

## Widgets and CSS

To apply styles, you must assign a node, class, or ID to a specific widget using C functions:

- Node: Fixed by the widget type (e.g., `GtkLabel` node is "label"). No function is needed.
- Class: Manage the widget classes with `gtk_widget_add_css_class()`, `gtk_widget_set_css_classes()`, or `gtk_widget_remove_css_class()`.
- ID: Corresponds to the widget's name, which you set using `gtk_widget_set_name()`.

## Applying CSS

We covered applying CSS in Section 17, but here is a quick recap:

- Load your CSS into a `GtkCssProvider` using `gtk_css_provider_load_from_string()`.
- Apply that provider to the default `GdkDisplay` using `gtk_style_context_add_provider_for_display()`.
- A display can have multiple providers.
If styles conflict, the provider with the higher priority wins.
The `GTK_STYLE_PROVIDER_PRIORITY_APPLICATION` priority is appropriate for application style providers.
(Tip: You can check the exact priority rankings in the `gtkstyleprovider.h` in the [GTK source files](https://gitlab.gnome.org/GNOME/gtk)).

## TFE Example: Applying Base CSS

Let's look at how TFE (Text File Editor) applies CSS. TFE uses two separate providers:

- Base CSS: Loaded at startup.
Right now, it just adds a 10px padding to the GtkTextView to make it look better.
- Font CSS: Allows users to change fonts dynamically via a font button (we will build this in the next section).

Let's apply the Base CSS in our startup handler.

```C
#include "tfestylemanager.h"

  ... ... ...

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  int i;
  const char *base_css = "textview {padding: 10px;}";

  G_APPLICATION_CLASS (tfe_application_parent_class)->startup (application);

  tfe_style_manager_initialize (base_css); /* initialize the style manager helper functions */

  ... ... ...

}

static void
app_shutdown (GApplication *application) {
  tfe_style_manager_terminate ();

 G_APPLICATION_CLASS (tfe_application_parent_class)->shutdown (application);
}

```

The files `tfestylemanager.h` and `tfestylemanager.c` contain our CSS helper functions.

- `tfe_style_manager_initialize()` initializes the library and loads the Base CSS at startup.
- `tfe_style_manager_terminate()` cleans it up during the application's dispose phase.

The two functions are called in the startup and shutdown handlers respectively.

Here is the implementation from `tfestylemanager.c`:

```C
static gboolean initialized = FALSE;
static GdkDisplay *display;
static GtkCssProvider *provider_base;
static GtkCssProvider *provider_font;

void
tfe_style_manager_initialize (const char *base_css) {
  if (initialized)
    return;
    
  display = gdk_display_get_default (); /* This is a singleton and should not be freed */
  if (display == NULL) {
    g_warning ("Failed to get default display");
    return;
  }
  
  if (base_css != NULL) {
    provider_base = gtk_css_provider_new ();
    gtk_css_provider_load_from_string (provider_base, base_css); /* Recommended for GTK 4.12+ */
    gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider_base), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  }

  provider_font = gtk_css_provider_new ();
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider_font), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  initialized = TRUE;
}

void
tfe_style_manager_terminate (void) {
  if (provider_base)
    gtk_style_context_remove_provider_for_display (display, GTK_STYLE_PROVIDER (provider_base));
  if (provider_font)
    gtk_style_context_remove_provider_for_display (display, GTK_STYLE_PROVIDER (provider_font));
    
  g_clear_object (&provider_base);
  g_clear_object (&provider_font);
  display = NULL;
  initialized = FALSE;
}
```

Here is what the initialization does:

1. Grabs the default `GdkDisplay`.
2. Loads the Base CSS into `provider_base` and applies it to the display.
3. Applies an *empty* `provider_font` to the display.
We can safely attach this empty provider now and inject the Font CSS into it later using `tfe_style_manager_set_font()`.

Add `tfestylemanager.c` to your `meson.build` file:

```
project('tfe', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','tfe.gresource.xml')

# Main application
sourcefiles=files('main.c', 'tfeapplication.c', 'tfewindow.c', 'tfetextview.c', 'tfestylemanager.c')
executable('tfe', sourcefiles, resources, dependencies: gtkdep)
```

THe source files are in the [src/tfe10](../src/tfe10/) directory.

Compile and run the app.
You should now see a 10px padding around your text view.

### Pango

Pango is the library GTK uses under the hood to handle text rendering and layout.

When we add a font dialog later, the user's selected font will be returned as a `PangoFontDescription` structure.
We need to convert this Pango format into CSS, which requires a bit of manual work since GTK doesn't do it automatically.

Here are the key functions to extract font data from Pango:

- `pango_font_description_get_family()`: Gets the font family (e.g., "sans-serif").
- `pango_font_description_get_size()`: Gets the font size. (Note: Divide this by the `PANGO_SCALE` constant to get standard points or pixels.)
- `pango_font_description_get_size_is_absolute()`: Returns `TRUE` for pixels (px) and `FALSE` for points (pt).
- `pango_font_description_get_weight()`: Returns the weight (e.g., `PANGO_WEIGHT_NORMAL`).
- `pango_font_description_get_style()`: Returns the style (e.g., `PANGO_STYLE_ITALIC`).

You can also easily convert a font description to and from a readable string using `pango_font_description_from_string()` and `pango_font_description_to_string()`.

The string format looks like this:

```
[FAMILY-LIST] [STYLE-OPTIONS] [SIZE] [VARIATIONS] [FEATURES]
```

You don't need to fill out every option.
A simple string like `"sans-serif Bold 12"` works perfectly.

### Pango to CSS Conversion

To make our font button work, we need to bridge the gap between Pango and CSS.
The helper function `font_desc_to_css()` does exactly this.

```c
static int
pango_weight2int (PangoWeight pango_weight) {
  switch (pango_weight) {
  case PANGO_WEIGHT_THIN:
    return 100;
  case PANGO_WEIGHT_ULTRALIGHT:
    return 200;
  case PANGO_WEIGHT_LIGHT:
    return 300;
  case PANGO_WEIGHT_SEMILIGHT:
    return 350;
  case PANGO_WEIGHT_BOOK:
    return 380;
  case PANGO_WEIGHT_NORMAL:
    return 400; /* or "normal" */
  case PANGO_WEIGHT_MEDIUM:
    return 500;
  case PANGO_WEIGHT_SEMIBOLD:
    return 600;
  case PANGO_WEIGHT_BOLD:
    return 700; /* or "bold" */
  case PANGO_WEIGHT_ULTRABOLD:
    return 800;
  case PANGO_WEIGHT_HEAVY:
    return 900;
  case PANGO_WEIGHT_ULTRAHEAVY:
    /* CSS Fonts Level 4 defines 1000, but GTK only supports Level 3 (max 900). */
    return 900;
  default:
    return 400; /* "normal" */
  }
}

static char *
font_desc_to_css (const PangoFontDescription *desc) {
  const char *family;
  double size;
  const char *size_unit;
  int weight;
  PangoStyle style;
  const char *style_str;
  char *css;

  family = pango_font_description_get_family (desc); /* The return string is owned by the instance. */
  size = (double) pango_font_description_get_size (desc) / PANGO_SCALE;
  if (size <= 0)
    size = 11.0; /* Default font size */
  size_unit = pango_font_description_get_size_is_absolute (desc) ? "px" : "pt";
  weight = pango_weight2int (pango_font_description_get_weight (desc));
  style = pango_font_description_get_style (desc);

  switch (style) {
    case PANGO_STYLE_ITALIC:
      style_str = "italic";
      break;
    case PANGO_STYLE_OBLIQUE:
      style_str = "oblique";
      break;
    case PANGO_STYLE_NORMAL:
    default:
      style_str = "normal";
      break;
  }

  css = g_strdup_printf ("textview {font-family: \"%s\"; font-size: %.1f%s; font-weight: %d; font-style: %s;}\n",
                         family ? family : "sans-serif", /* family can be NULL */
                         size,
                         size_unit,
                         weight,
                         style_str);

  return css;
}
```

This function turns a PangoFontDescription (which is a C structure) into proper CSS.
For example, if the description contains "sans-serif", "Bold", and "12pt", the CSS string will be:

```CSS
textview {font-family: "sans-serif"; font-size: 12pt; font-weight: 700; font-style: normal;}
```

Finally, here is our public interface, `tfe_style_manager_set_font()`:

```c
void
tfe_style_manager_set_font (const PangoFontDescription *desc) {
  g_return_if_fail (initialized);
  g_return_if_fail (desc != NULL);

  char *css_string;

  css_string = font_desc_to_css (desc);

  gtk_css_provider_load_from_string (provider_font, css_string);
  g_free (css_string);
}
```

This function creates the CSS string and feeds it directly into the `provider_font` (GtkCssProvider instance).
Since the provider is already linked to the display, the screen updates instantly.

Writing these conversion functions from scratch takes time because you have to cross-reference Pango and CSS documentation.
Thankfully, our helper library handles the heavy lifting.

In the next section, we'll use these helpers to build the actual font selection feature.

Up: [README.md](../README.md),  Prev: [Section 23](sec23.md), Next: [Section 25](sec25.md)
