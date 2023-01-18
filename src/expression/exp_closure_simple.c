#include <gtk/gtk.h>

static int
calc (GtkLabel *label) { /* this object */
  const char * s;
  int i, j;

  s = gtk_label_get_text (label); /* s is owned by the label. */
  sscanf (s, "%d+%d", &i, &j);
  return i+j;
}

int
main (int argc, char **argv) {
  GtkExpression *expression;
  GValue value = G_VALUE_INIT;
  GtkLabel *label;

  gtk_init ();
  label = GTK_LABEL (gtk_label_new ("123+456"));
  g_object_ref_sink (label);
  expression = gtk_cclosure_expression_new (G_TYPE_INT, NULL, 0, NULL,
                 G_CALLBACK (calc), NULL, NULL);
  if (gtk_expression_evaluate (expression, label, &value)) /* 'this' object is the label. */
    g_print ("%d\n", g_value_get_int (&value));
  else
    g_print ("The closure expression wasn't evaluated correctly.\n");
  gtk_expression_unref (expression);
  g_value_unset (&value);
  g_object_unref (label);
  
  return 0;
}
