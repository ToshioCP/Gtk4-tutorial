/* css.h */
#include <gtk/gtk.h>

void
set_css_for_display (GtkWindow *win, char *css) {
  GdkDisplay *display;

  display = gtk_widget_get_display (GTK_WIDGET (win));
  GtkCssProvider *provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (provider, css, -1);
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void
set_font_for_display (GtkWindow *win, const char *fontfamily, const char *fontstyle, const char *fontweight, int fontsize) {
  char *textview_css;

  textview_css = g_strdup_printf ("textview {padding: 10px; font-family: \"%s\"; font-style: %s; font-weight: %s; font-size: %dpt;}",
                                      fontfamily, fontstyle, fontweight, fontsize);
  set_css_for_display (win, textview_css);
} 

void
set_font_for_display_with_pango_font_desc (GtkWindow *win, PangoFontDescription *pango_font_desc) {
  int pango_style;
  int pango_weight; 
  const char *family;
  const char *style;
  const char *weight;
  int fontsize;

  family = pango_font_description_get_family (pango_font_desc);
  pango_style = pango_font_description_get_style (pango_font_desc);
  switch (pango_style) {
  case PANGO_STYLE_NORMAL:
    style = "normal";
    break;
  case PANGO_STYLE_ITALIC:
    style = "italic";
    break;
  case PANGO_STYLE_OBLIQUE:
    style = "oblique";
    break;
  default:
    style = "normal";
    break;
  }
  pango_weight = pango_font_description_get_weight (pango_font_desc);
  switch (pango_weight) {
  case PANGO_WEIGHT_THIN:
    weight = "100";
    break;
  case PANGO_WEIGHT_ULTRALIGHT:
    weight = "200";
    break;
  case PANGO_WEIGHT_LIGHT:
    weight = "300";
    break;
  case PANGO_WEIGHT_SEMILIGHT:
    weight = "350";
    break;
  case PANGO_WEIGHT_BOOK:
    weight = "380";
    break;
  case PANGO_WEIGHT_NORMAL:
    weight = "400"; /* or "normal" */
    break;
  case PANGO_WEIGHT_MEDIUM:
    weight = "500";
    break;
  case PANGO_WEIGHT_SEMIBOLD:
    weight = "600";
    break;
  case PANGO_WEIGHT_BOLD:
    weight = "700"; /* or "bold" */
    break;
  case PANGO_WEIGHT_ULTRABOLD:
    weight = "800";
    break;
  case PANGO_WEIGHT_HEAVY:
    weight = "900";
    break;
  case PANGO_WEIGHT_ULTRAHEAVY:
    weight = "900"; /* In PangoWeight definition, the weight is 1000. But CSS allows the weight below 900. */
    break;
  default:
    weight = "normal";
    break;
  }
  fontsize = pango_font_description_get_size (pango_font_desc) / PANGO_SCALE;
  set_font_for_display (win, family, style, weight, fontsize);
}
