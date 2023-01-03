#pragma once

#include <gtk/gtk.h>

#define TFE_TYPE_APPLICATION tfe_application_get_type ()
G_DECLARE_FINAL_TYPE (TfeApplication, tfe_application, TFE, APPLICATION, GtkApplication)

TfeApplication *
tfe_application_new (const char* application_id, GApplicationFlags flags);
