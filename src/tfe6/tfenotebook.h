/* tfenotbook.h */

gboolean
has_saved (GtkNotebook *nb);

gboolean
has_saved_all (GtkNotebook *nb);

void
notebook_page_save(GtkNotebook *nb);

void
notebook_page_saveas(GtkNotebook *nb);

void
notebook_page_close (GtkNotebook *nb);

void
notebook_page_open (GtkNotebook *nb);

void
notebook_page_new_with_file (GtkNotebook *nb, GFile *file);

void
notebook_page_new (GtkNotebook *nb);

