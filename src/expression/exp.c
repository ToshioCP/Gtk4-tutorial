#include <gtk/gtk.h>

GtkWidget *win1;
int width, height;
GtkExpressionWatch *watch_width;
GtkExpressionWatch *watch_height;

/* Notify is called when "default-width" or "default-height" property is changed. */
static void
notify (gpointer user_data) {
  GValue value = G_VALUE_INIT;
  char *title;

  if (watch_width && gtk_expression_watch_evaluate (watch_width, &value))
    width = g_value_get_int (&value);
  g_value_unset (&value);
  if (watch_height && gtk_expression_watch_evaluate (watch_height, &value))
    height = g_value_get_int (&value);
  g_value_unset (&value);
  title = g_strdup_printf ("%d x %d", width, height);
  gtk_window_set_title (GTK_WINDOW (win1), title);
  g_free (title);
}

/* This function is used by closure tag in exp.ui. */
char *
set_title (GtkWidget *win, int width, int height) {
  return g_strdup_printf ("%d x %d", width, height);
}

/* ----- activate, open, startup handlers ----- */
static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *box;
  GtkWidget *label1, *label2, *label3;
  GtkWidget *entry;
  GtkEntryBuffer *buffer;
  GtkBuilder *build;
  GtkExpression *expression, *expression1, *expression2;
  GValue value = G_VALUE_INIT;
  char *s;

  /* Creates GtkApplicationWindow instance. */
  /* The codes below are complecated. It does the same as "win1 = gtk_application_window_new (app);". */
  /* The codes are written just to show how to use GtkExpression. */
  expression = gtk_cclosure_expression_new (GTK_TYPE_APPLICATION_WINDOW, NULL, 0, NULL,
                 G_CALLBACK (gtk_application_window_new), NULL, NULL);
  if (gtk_expression_evaluate (expression, app, &value)) {
    win1 = GTK_WIDGET (g_value_get_object (&value)); /* GtkApplicationWindow */
    g_object_ref (win1);
    g_print ("Got GtkApplicationWindow instance.\n");
  }else
    g_print ("The cclosure expression wasn't evaluated correctly.\n");
  gtk_expression_unref (expression);
  g_value_unset (&value);    /* At the same time, the reference count of win1 is decreased by one. */

  /* Builds a window with components */
  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
  label1 = gtk_label_new (NULL);
  label2 = gtk_label_new (NULL);
  label3 = gtk_label_new (NULL);
  buffer = gtk_entry_buffer_new (NULL, 0);
  entry = gtk_entry_new_with_buffer (buffer);
  gtk_box_append (GTK_BOX (box), label1);
  gtk_box_append (GTK_BOX (box), label2);
  gtk_box_append (GTK_BOX (box), label3);
  gtk_box_append (GTK_BOX (box), entry);
  gtk_window_set_child (GTK_WINDOW (win1), box);

  /* Constant expression */
  expression = gtk_constant_expression_new (G_TYPE_INT,100);
  if (gtk_expression_evaluate (expression, NULL, &value)) {
    s = g_strdup_printf ("%d", g_value_get_int (&value));
    gtk_label_set_text (GTK_LABEL (label1), s);
    g_free (s);
  } else
    g_print ("The constant expression wasn't evaluated correctly.\n");
  gtk_expression_unref (expression);
  g_value_unset (&value);

  /* Property expression and binding*/
  expression1 = gtk_property_expression_new (GTK_TYPE_ENTRY, NULL, "buffer");
  expression2 = gtk_property_expression_new (GTK_TYPE_ENTRY_BUFFER, expression1, "text");
  gtk_expression_bind (expression2, label2, "label", entry);

  /* Constant expression instead of "this" instance */
  expression1 = gtk_constant_expression_new (GTK_TYPE_APPLICATION, app);
  expression2 = gtk_property_expression_new (GTK_TYPE_APPLICATION, expression1, "application-id");
  if (gtk_expression_evaluate (expression2, NULL, &value))
    gtk_label_set_text (GTK_LABEL (label3), g_value_get_string (&value));
  else
    g_print ("The property expression wasn't evaluated correctly.\n");
  gtk_expression_unref (expression1); /* expression 2 is also freed. */
  g_value_unset (&value);

  width = 800;
  height = 600;
  gtk_window_set_default_size (GTK_WINDOW (win1), width, height);
  notify(NULL);

  /* GtkExpressionWatch */
  expression1 = gtk_property_expression_new (GTK_TYPE_WINDOW, NULL, "default-width");
  watch_width = gtk_expression_watch (expression1, win1, notify, NULL, NULL);
  expression2 = gtk_property_expression_new (GTK_TYPE_WINDOW, NULL, "default-height");
  watch_height = gtk_expression_watch (expression2, win1, notify, NULL, NULL);

  gtk_widget_show (win1);

  /* Builds a window with exp.ui resource */
  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/exp/exp.ui");
  GtkWidget *win2 = GTK_WIDGET (gtk_builder_get_object (build, "win2"));
  gtk_window_set_application (GTK_WINDOW (win2), app);
  g_object_unref (build);

  gtk_widget_show (win2);
}

static void
app_startup (GApplication *application) {
}

#define APPLICATION_ID "com.github.ToshioCP.exp"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_FLAGS_NONE);

  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
/*  g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);*/

  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

