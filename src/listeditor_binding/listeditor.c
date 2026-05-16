#include <gtk/gtk.h>

#define LE_TYPE_DATA (le_data_get_type ())
G_DECLARE_FINAL_TYPE (LeData, le_data, LE, DATA, GObject)

enum {
  PROP_0,
  PROP_STRING,
  N_PROPERTIES
};

static GParamSpec *le_data_properties[N_PROPERTIES] = {NULL, };

typedef struct _LeData {
  GObject parent;
  char *string;
} LeData;

G_DEFINE_FINAL_TYPE(LeData, le_data, G_TYPE_OBJECT)

static void
le_data_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec) {
  LeData *self = LE_DATA (object);
  char *s;

  if (property_id == PROP_STRING) {
    s = g_value_dup_string (value);
    if (self->string)
      g_free (self->string); 
    self->string = s; /* The ownership is taken by the LeData instance*/
  } else
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

static void
le_data_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec) {
  LeData *self = LE_DATA (object);

  if (property_id == PROP_STRING)
    g_value_set_string (value, self->string);
  else
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

static void
le_data_init (LeData *self) {
  self->string = NULL;
}

static void
le_data_finalize (GObject *object) {
  LeData *self = LE_DATA (object);

  if (self->string)
    g_free (self->string);
  G_OBJECT_CLASS (le_data_parent_class)->finalize (object);
}

static void
le_data_class_init (LeDataClass *class) {
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);

  gobject_class->finalize = le_data_finalize;
  gobject_class->set_property = le_data_set_property;
  gobject_class->get_property = le_data_get_property;
  le_data_properties[PROP_STRING] = g_param_spec_string ("string", "string", "string", "", G_PARAM_READWRITE);
  g_object_class_install_properties (gobject_class, N_PROPERTIES, le_data_properties);
}

/* getter and setter */

char *
le_data_get_string (LeData *self) {
  return g_strdup (self->string);
}

const char *
le_data_look_string (LeData *self) {
  return self->string;
}

void
le_data_set_string (LeData *self, const char *string) {
  if (self->string)
    g_free (self->string);
  self->string = g_strdup (string);
}

void
le_data_take_string (LeData *self, char *string) {
  if (self->string)
    g_free (self->string);
  self->string = string;
}

LeData *
le_data_new_with_data (const char *string) {
  return LE_DATA (g_object_new (LE_TYPE_DATA, "string", string, NULL));
}

LeData *
le_data_new (void) {
  return LE_DATA (g_object_new (LE_TYPE_DATA, NULL));
}

/* ----- definition of LeWindow ----- */
#define LE_TYPE_WINDOW (le_window_get_type ())
G_DECLARE_FINAL_TYPE (LeWindow, le_window, LE, WINDOW, GtkApplicationWindow)
typedef struct _LeWindow {
  GtkApplicationWindow parent;
  GFile *file;
  GtkWidget *filename_label;
  GtkWidget *position_label;
  GtkWidget *listview;
  GtkSingleSelection *singleselection;
  GListStore *liststore;
} LeWindow;
G_DEFINE_FINAL_TYPE (LeWindow, le_window, GTK_TYPE_APPLICATION_WINDOW)

/* Handler of "selection-changed" signal on GtkSingleSelection */
void
selection_changed_cb (GtkSelectionModel* self, guint position_first, guint n_items, LeWindow *win) {
  GtkSingleSelection *selection = GTK_SINGLE_SELECTION (self);
  guint position;
  char *s;

  position = gtk_single_selection_get_selected (selection);
  if (position == GTK_INVALID_LIST_POSITION)
    s = g_strdup ("");
  else
    s = g_strdup_printf ("%d", position);
  gtk_label_set_text (GTK_LABEL (win->position_label), s);
  g_free (s);
}

/* ----- Button "clicled" signal handlers ----- */
static void
app_cb (GtkButton *btn, LeWindow *win) {
  LeData *data = le_data_new_with_data ("");
  guint position;

  position = gtk_single_selection_get_selected (win->singleselection);
  if (position == GTK_INVALID_LIST_POSITION)
    position = g_list_model_get_n_items (G_LIST_MODEL (win->liststore));
  else
    position++;
  g_list_store_insert (win->liststore, position, data);
  gtk_single_selection_set_selected (win->singleselection, position);
  g_object_unref (data);
}

static void
ins_cb (GtkButton *btn, LeWindow *win) {
  LeData *data = le_data_new_with_data ("");
  guint position;

  position = gtk_single_selection_get_selected (win->singleselection);
  if (position == GTK_INVALID_LIST_POSITION)
    position = 0;
  g_list_store_insert (win->liststore, 0, data);
  gtk_single_selection_set_selected (win->singleselection, position);
  g_object_unref (data);
}

static void
rm_cb (GtkButton *btn, LeWindow *win) {
  int position;

  position = gtk_single_selection_get_selected (win->singleselection);
  if (position != GTK_INVALID_LIST_POSITION)
    g_list_store_remove (win->liststore, position);
}

static void
open_dialog_cb (GObject *source_object, GAsyncResult *res, gpointer user_data) {
  GtkFileDialog *dialog = GTK_FILE_DIALOG (source_object);
  LeWindow *win = LE_WINDOW (user_data);
  GFile *file;
  char *contents;
  GListStore *liststore;
  guint n_items, i;
  LeData *data;
  char *filename_bin, *filename_utf8;
  GFileInputStream *stream;
  GDataInputStream *dstream;
  GError *err = NULL;
  GError *err2 = NULL;

  if ((file = gtk_file_dialog_open_finish (dialog, res, &err)) == NULL) {
    if (err) {
      if (err->code != GTK_DIALOG_ERROR_DISMISSED)
        g_warning ("%s\n", err->message);
      g_error_free (err);
    }
    return;
  }
  stream = g_file_read (file, NULL, &err);
  if (stream == NULL) {
    g_warning ("%s\n", err->message);
    g_error_free (err);
    g_object_unref (file);
    return;
  }
  dstream = g_data_input_stream_new (G_INPUT_STREAM (stream));
  g_object_unref (stream);
  liststore = g_list_store_new (LE_TYPE_DATA);
  while ((contents = g_data_input_stream_read_line_utf8 (dstream, NULL, NULL, &err)) != NULL) {
    data = le_data_new_with_data (contents);
    g_free (contents);
    g_list_store_append (liststore, data);
    g_object_unref (data);
  }
  g_input_stream_close (G_INPUT_STREAM (dstream), NULL, &err2);
  g_object_unref (dstream);
  if (err || err2) {
    if (err) {
      g_warning ("%s\n", err->message);
      g_error_free (err);
    }
    if (err2) {
      g_warning ("%s\n", err2->message);
      g_error_free (err2);
    }
    g_object_unref (file);
    g_object_unref (liststore);
    return;
  }
  /* Now the file has successfully read. */
  if (win->file)
    g_object_unref (win->file);
  win->file = file; /* The ownership of the GFile moves to the window. */
  filename_bin = g_file_get_basename (win->file);
  filename_utf8 = g_filename_display_name (filename_bin);
  gtk_label_set_text (GTK_LABEL (win->filename_label), filename_utf8);
  g_free (filename_bin);
  g_free (filename_utf8);
  /* copy the items in "liststore" into "win->liststore" */
  g_list_store_remove_all (win->liststore);
  n_items = g_list_model_get_n_items (G_LIST_MODEL (liststore));
  for (i = 0; i < n_items; i++) {
    data = g_list_model_get_item(G_LIST_MODEL(liststore), i);
    g_list_store_append(win->liststore, data);
    g_object_unref(data);
  }
  g_object_unref (liststore);
}

static void
read_cb (GtkButton *btn, LeWindow *win) {
  GtkFileDialog *dialog;

  dialog = gtk_file_dialog_new ();
  gtk_file_dialog_open (dialog, GTK_WINDOW (win), NULL, open_dialog_cb, win);
  g_object_unref (dialog);
}

static void
write_data (LeWindow *win) {
  GFileOutputStream *ostream;
  gssize size;
  gsize length;
  guint i, n_items;
  LeData *data;
  const char *s;
  GError *err = NULL;

  if (win->file == NULL) {
    g_warning ("No file to write\n");
    return;
  }
  if ((ostream = g_file_replace (win->file, NULL, TRUE, G_FILE_CREATE_NONE, NULL, &err)) == NULL) {
    if (err) {
      g_warning ("%s\n", err->message);
      g_error_free (err);
    }
    return;
  }
  n_items = g_list_model_get_n_items (G_LIST_MODEL (win->liststore));
  for (i=0; i<n_items; ++i) {
    data = g_list_model_get_item (G_LIST_MODEL (win->liststore), i);
    s = le_data_look_string (data);
    length = (gsize) strlen (s);
    size = g_output_stream_write (G_OUTPUT_STREAM (ostream), s, length, NULL, &err);
    g_object_unref (data);
    if (size < 0) { /* size == -1 when error occurs */
      if (err) {
        g_warning ("%s\n", err->message);
        g_clear_error (&err);
      }
      break;
    }
    size = g_output_stream_write (G_OUTPUT_STREAM (ostream), "\n", 1, NULL, &err);
    if (size < 0) {
      if (err) {
        g_warning ("%s\n", err->message);
        g_clear_error (&err);
      }
      break;
    }
  }
  g_output_stream_close (G_OUTPUT_STREAM (ostream), NULL, &err);
  g_object_unref (ostream);
  if (err) {
    g_warning ("%s\n", err->message);
    g_error_free (err);
  }
}

static void
saveas_dialog_cb (GObject *source_object, GAsyncResult *res, gpointer user_data) {
  GtkFileDialog *dialog = GTK_FILE_DIALOG (source_object);
  LeWindow *win = LE_WINDOW (user_data);
  GFile *file;
  GError *err = NULL;
  char *filename_bin, *filename_utf8;

  if ((file = gtk_file_dialog_save_finish (dialog, res, &err)) == NULL) {
    if (err) {
      if (err->code != GTK_DIALOG_ERROR_DISMISSED)
        g_warning ("%s\n", err->message);
      g_error_free (err);
    }
    return;
  }
  if (win->file)
    g_object_unref (win->file);
  win->file = file; /* the ownership is taken by win */
  filename_bin = g_file_get_basename (win->file);
  filename_utf8 = g_filename_display_name (filename_bin);
  gtk_label_set_text (GTK_LABEL (win->filename_label), filename_utf8);
  g_free (filename_bin);
  g_free (filename_utf8);
  write_data (win);
}

static void
show_saveas_dialog (LeWindow *win) {
  GtkFileDialog *dialog;

  dialog = gtk_file_dialog_new ();
  gtk_file_dialog_save (dialog, GTK_WINDOW (win), NULL, saveas_dialog_cb, win);
  g_object_unref (dialog);
}

static void
write_cb (GtkButton *btn, LeWindow *win) {
  if (g_list_model_get_n_items (G_LIST_MODEL (win->liststore)) == 0)
    return;
  if (win->file)
    write_data (win);
  else
    show_saveas_dialog (win);
}

static void
close_cb (GtkButton *btn, LeWindow *win) {
  if (g_list_model_get_n_items (G_LIST_MODEL (win->liststore)) > 0)
    g_list_store_remove_all (win->liststore);
  if (win->file) {
    g_object_unref (win->file);
    win->file = NULL;
  }
  gtk_label_set_text (GTK_LABEL (win->filename_label), "");
}

static void
quit_cb (GtkButton *btn, LeWindow *win) {
  GtkApplication *app = gtk_window_get_application (GTK_WINDOW (win));
  g_application_quit (G_APPLICATION (app));
}

/* ----- Handlers on GtkSignalListItemFacory ----- */

static void
setup_cb (GtkListItemFactory *factory, GtkListItem *listitem) {
  GtkWidget *text = gtk_text_new ();
  gtk_list_item_set_child (listitem, GTK_WIDGET (text));
  gtk_editable_set_alignment (GTK_EDITABLE (text), 0.0);
  gtk_widget_set_can_target (GTK_WIDGET (text), FALSE);
}

static void
bind_cb (GtkListItemFactory *factory, GtkListItem *listitem) {
  GtkWidget *text = gtk_list_item_get_child (listitem);
  GtkEntryBuffer *buffer = gtk_text_get_buffer (GTK_TEXT (text));
  LeData *data = LE_DATA (gtk_list_item_get_item(listitem));
  GBinding *bind;

  if (data) {
    gtk_editable_set_text (GTK_EDITABLE (text), le_data_look_string (data));
    gtk_editable_set_position (GTK_EDITABLE (text), -1);
  }
  
  bind = g_object_bind_property (buffer, "text", data, "string", G_BINDING_DEFAULT);
  g_object_set_data (G_OBJECT (listitem), "bind", bind);
}

static void
unbind_cb (GtkListItemFactory *factory, GtkListItem *listitem) {
  GBinding *bind = G_BINDING (g_object_get_data (G_OBJECT (listitem), "bind"));

  if (bind)
    g_binding_unbind(bind);
  g_object_set_data (G_OBJECT (listitem), "bind", NULL);
}

static void
teardown_cb (GtkListItemFactory *factory, GtkListItem *listitem) {
  gtk_list_item_set_child (listitem, NULL);
}

/* ----------------------------------------------------*/

static void
adjustment_value_changed_cb (GtkAdjustment *adjustment, gpointer user_data) {
  GtkWidget *win = GTK_WIDGET (user_data);

  gtk_window_set_focus (GTK_WINDOW (win), NULL);
}

static void
le_window_dispose (GObject *object) {
  LeWindow *win = LE_WINDOW (object);

  if (win->file)
    g_clear_object (&win->file);
  /* this function is available since GTK 4.8 */
  gtk_widget_dispose_template (GTK_WIDGET (win), LE_TYPE_WINDOW);
  /* chain to the parent */
  G_OBJECT_CLASS (le_window_parent_class)->dispose (object);
}

static void
le_window_init (LeWindow *win) {
  gtk_widget_init_template (GTK_WIDGET (win));

  win->file =NULL;
}

static void
le_window_class_init (LeWindowClass *class) {
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);

  gobject_class->dispose = le_window_dispose;

  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/listeditor/listeditor.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LeWindow, position_label);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LeWindow, filename_label);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LeWindow, listview);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LeWindow, singleselection);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LeWindow, liststore);
  /* The followint macros are available since GTK 4.8 */
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), app_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), ins_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), rm_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), read_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), write_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), close_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), quit_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), setup_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), bind_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), unbind_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), teardown_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), adjustment_value_changed_cb);
}

GtkWidget *
le_window_new (GtkApplication *app) {
  g_return_val_if_fail (GTK_IS_APPLICATION (app), NULL);

  return GTK_WIDGET (g_object_new (LE_TYPE_WINDOW, "application", app, NULL));
}

/* ----- activate, startup handlers ----- */
static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  gtk_window_present (gtk_application_get_active_window(app));
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkCssProvider *provider;

  le_window_new (app);
  provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_string (provider, "text:focus {border: 1px solid gray;}");
  gtk_style_context_add_provider_for_display (gdk_display_get_default (),
          GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_object_unref (provider);
}

#define APPLICATION_ID "com.github.ToshioCP.listeditor"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);

  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
