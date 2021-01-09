#include <glib.h>

int
main (int argc, char **argv) {
  GVariantType *vtype = g_variant_type_new ("s");
  const gchar *type_string = g_variant_type_peek_string (vtype);
  g_print ("%s\n",type_string);
}
