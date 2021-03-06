#include <gtk/gtk.h>

#include "../tfetextview/tfetextview.h"
#include "turtle_lex.h"
#include "turtle_parser.h"

/* The following line defines 'debug' so that debug information is printed during the run time. */
/* However it makes the program slow. */
/* If you don't want to see such information, remove the line. */
/*#define debug 1*/

extern cairo_surface_t *surface;

