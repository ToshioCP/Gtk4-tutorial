#pragma once

#include <gtk/gtk.h>

#define TFE_TYPE_PREF tfe_pref_get_type ()
G_DECLARE_FINAL_TYPE (TfePref, tfe_pref, TFE, PREF, GtkWindow)

GtkWidget *
tfe_pref_new (void);
