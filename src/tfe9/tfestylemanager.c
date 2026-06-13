#include <gtk/gtk.h>
#include "tfestylemanager.h"

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
    return 900; /* 1000 is available since CSS Fonts level 4 but GTK currently supports level 3. */
  default:
    return 400; /* "normal" */
  }
}

static char *
font_string_to_css (const char *font_desc_str) {
  PangoFontDescription *desc;
  const char *family;
  double size;
  const char *size_unit;
  int weight;
  PangoStyle style;
  const char *style_str;
  char *css;

  desc = pango_font_description_from_string (font_desc_str);
  family = pango_font_description_get_family (desc); /* The return string is owned by the instance. */
  if ((size = (double) pango_font_description_get_size (desc) / PANGO_SCALE) <= 0)
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
                         family ? family : "sans-serif",
                         size,
                         size_unit,
                         weight,
                         style_str);

  pango_font_description_free (desc);

  return css;
}

/* Public functions*/

static GdkDisplay *display;
static GtkCssProvider *provider_base;
static GtkCssProvider *provider_font;

void
tfe_style_manager_initialize (const char *base_css) {
  if (display != NULL) /* Already initialized */
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
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider_font), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
}

void
tfe_style_manager_set_font (const char *font_name) {
  g_return_if_fail (font_name != NULL);

  char *css_string;

  if (*font_name == '\0')
    return;
  if (provider_font == NULL) /* Not initialized yet */
    return;

  css_string = font_string_to_css (font_name);

  gtk_css_provider_load_from_string (provider_font, css_string);
  g_free (css_string);
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
}
