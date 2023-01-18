#include <glib-object.h>

static int
square (int x) {
  return x*x;
}

int
main (int argc, char **argv) {
  GClosure *closure;
  GValue value0 = G_VALUE_INIT;
  GValue value1 = G_VALUE_INIT;
  int i;

  g_value_init (&value0, G_TYPE_INT);
  g_value_init (&value1, G_TYPE_INT);

  i = 10;
  g_value_set_int (&value0, i);
  closure = g_cclosure_new (G_CALLBACK (square), NULL, NULL);
  g_closure_set_marshal (closure, g_cclosure_marshal_generic);
  g_closure_invoke (closure, &value1, 1, &value0, NULL);
  g_print ("%d*%d = %d\n", i, i, g_value_get_int (&value1));
  g_value_unset (&value0);
  g_value_unset (&value1);

  return 0;
}
