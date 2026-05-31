#include <glib.h>

int
main (int argc, char **argv) {
  const GVariantType *vtype = G_VARIANT_TYPE_STRING;
  g_print ("%s\n", g_variant_type_peek_string (vtype));
}