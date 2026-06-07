#include <glib.h>
#include <pango/pango.h>
#include "../pfd2css.h"

static void
test_pfd2css_one (const char *font, const char *expected)
{
  PangoFontDescription *desc = pango_font_description_from_string (font);
  char *css = pfd2css (desc);
  pango_font_description_free (desc);
  if (strcmp (expected, css) != 0) {
    g_printerr ("CSS didn't match.\n");
    g_printerr ("Expected: %s\n", expected);
    g_printerr ("Actual: %s\n", css);
  }
  g_free (css);
}

int
main (int argc, char *argv[])
{
  test_pfd2css_one ("Monospace 12", "font-family: \"Monospace\"; font-style: normal; font-weight: 400; font-size: 12pt;");
  test_pfd2css_one ("NotoSansCJK-Black Italic Extra-Bold 24", "font-family: \"NotoSansCJK-Black\"; font-style: italic; font-weight: 800; font-size: 24pt;");
  return 0;
}
