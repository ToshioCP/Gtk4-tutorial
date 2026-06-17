#pragma once

/* Tfe Style Manager */

void
tfe_style_manager_initialize (const char *base_css);

void
tfe_style_manager_set_font (const PangoFontDescription *desc);

void
tfe_style_manager_terminate (void);
