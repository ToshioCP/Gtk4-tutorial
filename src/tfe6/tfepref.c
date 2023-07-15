#include <gtk/gtk.h>
#include "tfepref.h"

struct _TfePref
{
  GtkWindow parent;
  GSettings *settings;
  GtkFontDialogButton *font_dialog_btn;
};

G_DEFINE_FINAL_TYPE (TfePref, tfe_pref, GTK_TYPE_WINDOW);

static void
tfe_pref_dispose (GObject *gobject) {
  TfePref *pref = TFE_PREF (gobject);

  /* GSetting bindings are automatically removed when the object is finalized, so it isn't necessary to unbind them explicitly.*/
  g_clear_object (&pref->settings);
  gtk_widget_dispose_template (GTK_WIDGET (pref), TFE_TYPE_PREF);
  G_OBJECT_CLASS (tfe_pref_parent_class)->dispose (gobject);
}

/* ---------- get_mapping/set_mapping ---------- */
static gboolean // GSettings => property
get_mapping (GValue* value, GVariant* variant, gpointer user_data) {
  const char *s = g_variant_get_string (variant, NULL);
  PangoFontDescription *font_desc = pango_font_description_from_string (s);
  g_value_take_boxed (value, font_desc);
  return TRUE;
}

static GVariant* // Property => GSettings
set_mapping (const GValue* value, const GVariantType* expected_type, gpointer user_data) {
  char*font_desc_string = pango_font_description_to_string (g_value_get_boxed (value));
  return g_variant_new_take_string (font_desc_string);
}

static void
tfe_pref_init (TfePref *pref) {
  gtk_widget_init_template (GTK_WIDGET (pref));
  pref->settings = g_settings_new ("com.github.ToshioCP.tfe");
  g_settings_bind_with_mapping (pref->settings, "font-desc", pref->font_dialog_btn, "font-desc", G_SETTINGS_BIND_DEFAULT,
      get_mapping, set_mapping, NULL, NULL);
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
