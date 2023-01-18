#include <gtk/gtk.h>

struct error_value {
  gboolean e;
  int v;
};

static struct error_value *
calc (GtkLabel *label) { /* this object */
  const char * s;
  int i, j;
  struct error_value *ev;

  ev = g_new (struct error_value, 1);
  s = gtk_label_get_text (label); /* s is owned by the label. */
  if (sscanf (s, "%d+%d", &i, &j) == 2) {
    ev->e = true;
    ev->v = i+j;
  } else
    ev->e = false;
  return ev;
}

int
main (int argc, char **argv) {
  GtkExpression *expression;
  GValue value = G_VALUE_INIT;
  GtkLabel *label;
  struct error_value *ev;

  gtk_init ();
  label = GTK_LABEL (gtk_label_new ("123+456"));
  g_object_ref_sink (label);
  expression = gtk_cclosure_expression_new (G_TYPE_POINTER, NULL, 0, NULL,
                 G_CALLBACK (calc), NULL, NULL);
  if (! gtk_expression_evaluate (expression, label, &value))
    g_print ("The closure expression wasn't evaluated correctly.\n");
  ev = (struct error_value *) g_value_get_pointer (&value);
  if (ev->e)
    g_print ("%d\n", ev->v);
  else
    g_print ("Label format error.\n");
  gtk_expression_unref (expression);
  g_value_unset (&value);
  g_object_unref (label);
  g_free (ev);
  
  return 0;
}
