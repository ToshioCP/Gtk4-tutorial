#pragma once

#include <gtk/gtk.h>

#define TFE_TYPE_ALERT tfe_alert_get_type ()
G_DECLARE_FINAL_TYPE (TfeAlert, tfe_alert, TFE, ALERT, GtkWindow)

/* "response" signal id */
enum TfeAlertResponseType
{
  TFE_ALERT_RESPONSE_ACCEPT,
  TFE_ALERT_RESPONSE_CANCEL
};

const char *
tfe_alert_get_title (TfeAlert *alert);

const char *
tfe_alert_get_message (TfeAlert *alert);

const char *
tfe_alert_get_button_label (TfeAlert *alert);

void
tfe_alert_set_title (TfeAlert *alert, const char *title);

void
tfe_alert_set_message (TfeAlert *alert, const char *message);

void
tfe_alert_set_button_label (TfeAlert *alert, const char *btn_label);

GtkWidget *
tfe_alert_new (void);

GtkWidget *
tfe_alert_new_with_data (const char *title, const char *message, const char* btn_label);
