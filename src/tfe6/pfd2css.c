#include <pango/pango.h>
#include "pfd2css.h"

// Pango font description to CSS style string
// Returned string is owned by caller. The caller should free it when it is useless.

char*
pfd2css (PangoFontDescription *pango_font_desc) {
  char *fontsize;

  fontsize = pfd2css_size (pango_font_desc);
  return g_strdup_printf ("font-family: \"%s\"; font-style: %s; font-weight: %d; font-size: %s;",
              pfd2css_family (pango_font_desc), pfd2css_style (pango_font_desc),
              pfd2css_weight (pango_font_desc), fontsize);
  g_free (fontsize); 
}

// Each element (family, style, weight and size)

const char*
pfd2css_family (PangoFontDescription *pango_font_desc) {
  return pango_font_description_get_family (pango_font_desc);
}

const char*
pfd2css_style (PangoFontDescription *pango_font_desc) {
  PangoStyle pango_style = pango_font_description_get_style (pango_font_desc);
  switch (pango_style) {
  case PANGO_STYLE_NORMAL:
    return "normal";
  case PANGO_STYLE_ITALIC:
    return "italic";
  case PANGO_STYLE_OBLIQUE:
    return "oblique";
  default:
    return "normal";
  }
}

int
pfd2css_weight (PangoFontDescription *pango_font_desc) {
  PangoWeight pango_weight = pango_font_description_get_weight (pango_font_desc);
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
    return 900; /* In PangoWeight definition, the weight is 1000. But CSS allows the weight below 900. */
  default:
    return 400; /* "normal" */
  }
}

char *
pfd2css_size (PangoFontDescription *pango_font_desc) {
  if (pango_font_description_get_size_is_absolute (pango_font_desc))
    return g_strdup_printf ("%dpx", pango_font_description_get_size (pango_font_desc) / PANGO_SCALE);
  else
    return g_strdup_printf ("%dpt", pango_font_description_get_size (pango_font_desc) / PANGO_SCALE);
}
