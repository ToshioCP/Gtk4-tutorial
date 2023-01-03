#include <gtk/gtk.h>
#include "tfealert.h"

struct _TfeAlert
{
  GtkDialog parent;
  GtkLabel *lb_alert;
  GtkButton *btn_accept;
};

G_DEFINE_TYPE (TfeAlert, tfe_alert, GTK_TYPE_DIALOG);

void
tfe_alert_set_message (TfeAlert *alert, const char *message) {
  gtk_label_set_text (alert->lb_alert, message);
}

void
tfe_alert_set_button_label (TfeAlert *alert, const char *label) {
  gtk_button_set_label (alert->btn_accept, label);
}

static void
tfe_alert_init (TfeAlert *alert) {
  gtk_widget_init_template (GTK_WIDGET (alert));
}

static void
tfe_alert_class_init (TfeAlertClass *class) {
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfealert.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, lb_alert);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, btn_accept);
}

GtkWidget *
tfe_alert_new (void) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_ALERT, NULL));
}

