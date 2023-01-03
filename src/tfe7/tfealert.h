#ifndef __TFE_ALERT_H__
#define __TFE_ALERT_H__

#include <gtk/gtk.h>

#define TFE_TYPE_ALERT tfe_alert_get_type ()
G_DECLARE_FINAL_TYPE (TfeAlert, tfe_alert, TFE, ALERT, GtkDialog)

void
tfe_alert_set_message (TfeAlert *alert, const char *message);

void
tfe_alert_set_button_label (TfeAlert *alert, const char *label);

GtkWidget *
tfe_alert_new (void);

#endif /* __TFE_ALERT_H__ */

