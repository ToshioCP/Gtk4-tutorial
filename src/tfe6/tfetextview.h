#pragma once

#include <gtk/gtk.h>

#define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type ()
G_DECLARE_FINAL_TYPE (TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView)

/* User Error Definition */
#define TFE_TEXT_VIEW_ERROR (tfe_text_view_error_quark ())
GQuark tfe_text_view_error_quark (void);
typedef enum {
  TFE_TEXT_VIEW_ERROR_NO_FILE,   /* No file is set */
  TFE_TEXT_VIEW_ERROR_FAILED,   /* Other invalid state */
} TfeTextViewError;

GFile *
tfe_text_view_get_file (TfeTextView *tv);

void
tfe_text_view_set_file (TfeTextView *tv, GFile *file);

gboolean
tfe_text_view_read (TfeTextView *tv, GError **err);

gboolean
tfe_text_view_write (TfeTextView *tv, GError **err);

GtkWidget *
tfe_text_view_new (void);

GtkWidget *
tfe_text_view_new_with_file (GFile *file, GError **err);
