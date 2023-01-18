#include <gtk/gtk.h>

int
main (int argc, char **argv) {
  GtkWidget *label;
  GtkExpression *expression;
  GValue value = G_VALUE_INIT;

  gtk_init ();
  label = gtk_label_new ("Hello world.");
  /* Create an expression */
  expression = gtk_property_expression_new (GTK_TYPE_LABEL, NULL, "label");
  /* Evaluate the expression */
  if (gtk_expression_evaluate (expression, label, &value))
    g_print ("The value is %s.\n", g_value_get_string (&value));
  else
    g_print ("The constant expression wasn't evaluated correctly.\n");
  gtk_expression_unref (expression);
  g_value_unset (&value);

  return 0;
}
