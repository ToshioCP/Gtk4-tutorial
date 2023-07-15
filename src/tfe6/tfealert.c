#include <gtk/gtk.h>
#include "tfealert.h"

struct _TfeAlert {
  GtkWindow parent;
  GtkLabel *lb_title;
  GtkLabel *lb_message;
  GtkButton *btn_accept;
  GtkButton *btn_cancel;
};

G_DEFINE_FINAL_TYPE (TfeAlert, tfe_alert, GTK_TYPE_WINDOW);

enum {
  RESPONSE,
  NUMBER_OF_SIGNALS
};

static guint tfe_alert_signals[NUMBER_OF_SIGNALS];

static void
cancel_cb (TfeAlert *alert) {
  g_signal_emit (alert, tfe_alert_signals[RESPONSE], 0, TFE_ALERT_RESPONSE_CANCEL);
  gtk_window_destroy (GTK_WINDOW (alert));
}

static void
accept_cb (TfeAlert *alert) {
  g_signal_emit (alert, tfe_alert_signals[RESPONSE], 0, TFE_ALERT_RESPONSE_ACCEPT);
  gtk_window_destroy (GTK_WINDOW (alert));
}

const char *
tfe_alert_get_title (TfeAlert *alert) {
  return gtk_label_get_text (alert->lb_title);
}

const char *
tfe_alert_get_message (TfeAlert *alert) {
    return gtk_label_get_text (alert->lb_message);
}

const char *
tfe_alert_get_button_label (TfeAlert *alert) {
  return gtk_button_get_label (alert->btn_accept);
}

void
tfe_alert_set_title (TfeAlert *alert, const char *title) {
  gtk_label_set_text (alert->lb_title, title);
}

void
tfe_alert_set_message (TfeAlert *alert, const char *message) {
  gtk_label_set_text (alert->lb_message, message);
}

void
tfe_alert_set_button_label (TfeAlert *alert, const char *btn_label) {
  gtk_button_set_label (alert->btn_accept, btn_label);
}

static void
tfe_alert_dispose (GObject *gobject) { // gobject is actually a TfeAlert instance.
  gtk_widget_dispose_template (GTK_WIDGET (gobject), TFE_TYPE_ALERT);
  G_OBJECT_CLASS (tfe_alert_parent_class)->dispose (gobject);
}

static void
tfe_alert_init (TfeAlert *alert) {
  gtk_widget_init_template (GTK_WIDGET (alert));
}

static void
tfe_alert_class_init (TfeAlertClass *class) {
  G_OBJECT_CLASS (class)->dispose = tfe_alert_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfealert.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, lb_title);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, lb_message);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, btn_accept);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, btn_cancel);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), cancel_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), accept_cb);

  tfe_alert_signals[RESPONSE] = g_signal_new ("response",
                                G_TYPE_FROM_CLASS (class),
                                G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                0 /* class offset */,
                                NULL /* accumulator */,
                                NULL /* accumulator data */,
                                NULL /* C marshaller */,
                                G_TYPE_NONE /* return_type */,
                                1     /* n_params */,
                                G_TYPE_INT
                                );
}

GtkWidget *
tfe_alert_new (void) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_ALERT, NULL));
}

GtkWidget *
tfe_alert_new_with_data (const char *title, const char *message, const char* btn_label) {
  GtkWidget *alert = tfe_alert_new ();
  tfe_alert_set_title (TFE_ALERT (alert), title);
  tfe_alert_set_message (TFE_ALERT (alert), message);
  tfe_alert_set_button_label (TFE_ALERT (alert), btn_label);
  return alert;
}
