#pragma once

#include <gtk/gtk.h>

#define TFE_TYPE_WINDOW tfe_window_get_type ()
G_DECLARE_FINAL_TYPE (TfeWindow, tfe_window, TFE, WINDOW, GtkApplicationWindow)

void
tfe_window_append_page (TfeWindow *win, GFile *file);

GtkWidget *
tfe_window_new (GtkApplication *app);
