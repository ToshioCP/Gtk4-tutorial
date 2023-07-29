#include <gtk/gtk.h>

static GtkCssProvider *provider = NULL;
static const char *format = "label {padding: 20px;} label#red {background: red;} "
  "label#green {background: green;} label#blue {background: blue;} "
  "label#canvas {color: %s; font-weight: bold; font-size: 72pt;}";

static gboolean
drop_cb (GtkDropTarget* self, const GValue* value, gdouble x, gdouble y, gpointer user_data) {
  char *s;

  s = g_strdup_printf (format, g_value_get_string (value));
  gtk_css_provider_load_from_data (provider, s, -1);
  g_free (s);
  return TRUE;
}

static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWindow *win;

  win = gtk_application_get_active_window (app);
  gtk_window_present (win);
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkBuilder *build;
  GtkWindow *win;
  GtkLabel *src_labels[3];
  int i;
  GtkLabel *canvas;
  GtkDragSource *src;
  GdkContentProvider* content;
  GtkDropTarget *tgt;
  GdkDisplay *display;
  char *s;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/dnd/dnd.ui");
  win = GTK_WINDOW (gtk_builder_get_object (build, "win"));
  src_labels[0] = GTK_LABEL (gtk_builder_get_object (build, "red"));
  src_labels[1] = GTK_LABEL (gtk_builder_get_object (build, "green"));
  src_labels[2] = GTK_LABEL (gtk_builder_get_object (build, "blue"));
  canvas = GTK_LABEL (gtk_builder_get_object (build, "canvas"));
  gtk_window_set_application (win, app);
  g_object_unref (build);

  for (i=0; i<3; ++i) {
    src = gtk_drag_source_new ();
    content = gdk_content_provider_new_typed (G_TYPE_STRING, gtk_widget_get_name (GTK_WIDGET (src_labels[i])));
    gtk_drag_source_set_content (src, content);
    g_object_unref (content);
    gtk_widget_add_controller (GTK_WIDGET (src_labels[i]), GTK_EVENT_CONTROLLER (src)); // The ownership of src is taken by the instance.
  }

  tgt = gtk_drop_target_new (G_TYPE_STRING, GDK_ACTION_COPY);
  g_signal_connect (tgt, "drop", G_CALLBACK (drop_cb), NULL);
  gtk_widget_add_controller (GTK_WIDGET (canvas), GTK_EVENT_CONTROLLER (tgt)); // The ownership of tgt is taken by the instance.

  provider = gtk_css_provider_new ();
  s = g_strdup_printf (format, "black");
  gtk_css_provider_load_from_data (provider, s, -1);
  g_free (s);
  display = gdk_display_get_default ();
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_object_unref (provider); // The provider is still alive because the display owns it.
}

#define APPLICATION_ID "com.github.ToshioCP.dnd"

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
