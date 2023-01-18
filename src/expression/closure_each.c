#include <glib-object.h>

// Ruby "each" method on an array is like this.
//
// [1,2,3].each do |x|
//   print "#{x}\n"
// end
// #=> 1
// #=> 2
// #=> 3
//
// The following program is similar to the program above.
// It is implemented with a closure. (print => closure)
// The result of the program is:
// 2
// 0
// 2
// 3
// 1
// 11

void
each (int array[], int size, GClosure *closure) {
  int i;
  GValue value = G_VALUE_INIT;
 
  for (i=0; i<size; ++i) {
    g_value_init (&value, G_TYPE_INT);
    g_value_set_int (&value, array[i]);
    g_closure_invoke (closure, NULL, 1, &value, NULL);
    g_value_unset (&value);
  }
}

static void
int_print (int x) {
  g_print ("%d\n", x);
}

int
main (int argc, char **argv) {
  GClosure *closure;
  int a[] = {2,0,2,3,1,11};

  closure = g_cclosure_new (G_CALLBACK (int_print), NULL, NULL);
  g_closure_set_marshal (closure, g_cclosure_marshal_generic);
  each (a, sizeof(a)/sizeof(int), closure);

  return 0;
}
