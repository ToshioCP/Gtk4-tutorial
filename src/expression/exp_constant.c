#include <gtk/gtk.h>

GtkWidget *win;

static void
app_activate (GApplication *application) {
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *label;
  GtkExpression *expression;
  GValue value = G_VALUE_INIT;
  char *s;

  win = gtk_application_window_new (app);
  gtk_window_set_default_size (GTK_WINDOW (win), 600, 400);
  label = gtk_label_new (NULL);
  gtk_window_set_child (GTK_WINDOW (win), label);

  /* Create an expression */
  expression = gtk_constant_expression_new (G_TYPE_INT,100);
  /* Evaluate the expression */
  if (gtk_expression_evaluate (expression, NULL, &value)) {
    s = g_strdup_printf ("%d", g_value_get_int (&value));
    gtk_label_set_text (GTK_LABEL (label), s);
    g_free (s);
  } else
    g_print ("The constant expression wasn't evaluated correctly.\n");
  gtk_expression_unref (expression);
  g_value_unset (&value);
}

#define APPLICATION_ID "com.github.ToshioCP.exp_constant"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
