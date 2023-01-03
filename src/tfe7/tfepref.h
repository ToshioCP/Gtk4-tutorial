#ifndef __TFE_PREF_H__
#define __TFE_PREF_H__

#include <gtk/gtk.h>

#define TFE_TYPE_PREF tfe_pref_get_type ()
G_DECLARE_FINAL_TYPE (TfePref, tfe_pref, TFE, PREF, GtkDialog)

GtkWidget *
tfe_pref_new (void);

#endif /* __TFE_PREF_H__ */

