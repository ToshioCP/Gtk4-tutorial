#include <gtk/gtk.h>
#include "tfestylemanager.h"
#include "tfepref.h"

struct _TfePref
{
  GtkWindow parent;
  GtkFontDialogButton *font_dialog_btn;
};

G_DEFINE_FINAL_TYPE (TfePref, tfe_pref, GTK_TYPE_WINDOW);

static void
font_dialog_btn_notify_cb (GtkFontDialogButton *btn, GParamSpec *pspec, gpointer user_data) {
  PangoFontDescription *font_desc = gtk_font_dialog_button_get_font_desc (btn);

  tfe_style_manager_set_font (font_desc);
}

static void
tfe_pref_dispose (GObject *gobject) {
  TfePref *pref = TFE_PREF (gobject);

  gtk_widget_dispose_template (GTK_WIDGET (pref), TFE_TYPE_PREF);
  G_OBJECT_CLASS (tfe_pref_parent_class)->dispose (gobject);
}

static void
tfe_pref_init (TfePref *pref) {
  gtk_widget_init_template (GTK_WIDGET (pref));
  g_signal_connect (pref->font_dialog_btn, "notify::font-desc", G_CALLBACK (font_dialog_btn_notify_cb), NULL);
}

static void
tfe_pref_class_init (TfePrefClass *class) {
  G_OBJECT_CLASS (class)->dispose = tfe_pref_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe11/tfepref.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfePref, font_dialog_btn);
}

GtkWidget *
tfe_pref_new (GtkApplication *application) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_PREF, "application", application, NULL));
}
