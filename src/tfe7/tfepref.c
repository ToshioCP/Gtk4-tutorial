#include <gtk/gtk.h>
#include "tfepref.h"

struct _TfePref
{
  GtkDialog parent;
  GSettings *settings;
  GtkFontButton *fontbtn;
};

G_DEFINE_TYPE (TfePref, tfe_pref, GTK_TYPE_DIALOG);

static void
tfe_pref_dispose (GObject *gobject) {
  TfePref *pref = TFE_PREF (gobject);

  g_clear_object (&pref->settings);
  G_OBJECT_CLASS (tfe_pref_parent_class)->dispose (gobject);
}

static void
tfe_pref_init (TfePref *pref) {
  gtk_widget_init_template (GTK_WIDGET (pref));
  pref->settings = g_settings_new ("com.github.ToshioCP.tfe");
  g_settings_bind (pref->settings, "font", pref->fontbtn, "font", G_SETTINGS_BIND_DEFAULT);
}

static void
tfe_pref_class_init (TfePrefClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = tfe_pref_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfepref.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfePref, fontbtn);
}

GtkWidget *
tfe_pref_new (void) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_PREF, NULL));
}

