#include <gtk/gtk.h>

#define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type ()
G_DECLARE_FINAL_TYPE (TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView)

void
tfe_text_view_set_file (TfeTextView *tv, GFile *f);

GFile *
tfe_text_view_get_file (TfeTextView *tv);

GtkWidget *
tfe_text_view_new (void);

