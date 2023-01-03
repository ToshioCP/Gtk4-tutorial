#pragma once

#include <pango/pango.h>

// Pango font description to CSS style string
// Returned string is owned by caller. The caller should free it when it is useless.

char*
pfd2css (PangoFontDescription *pango_font_desc);

// Each element (family, style, weight and size)

const char*
pfd2css_family (PangoFontDescription *pango_font_desc);

const char*
pfd2css_style (PangoFontDescription *pango_font_desc);

int
pfd2css_weight (PangoFontDescription *pango_font_desc);

// Returned string is owned by caller. The caller should free it when it is useless.
char *
pfd2css_size (PangoFontDescription *pango_font_desc);
