#include <string.h>
#include "tfetextview.h"

struct _TfeTextView {
  GtkTextView parent;
  GFile *file;
};

G_DEFINE_FINAL_TYPE (TfeTextView, tfe_text_view, GTK_TYPE_TEXT_VIEW);

/* 
 * User Error Quark Definition
 * arg 1: Domain name for the error
 * arg 2: Function prefix (a function with _quark suffix will be automatically generated)
 */
G_DEFINE_QUARK (tfe-text-view-error-quark, tfe_text_view_error)

static void
tfe_text_view_dispose (GObject *gobject) {
  TfeTextView *tv = TFE_TEXT_VIEW (gobject);

  g_clear_object (&tv->file);
  G_OBJECT_CLASS (tfe_text_view_parent_class)->dispose (gobject);
}

static void
tfe_text_view_init (TfeTextView *tv) {
  /* The allocated memory is set to zero by the system.*/
  /* Therefore, "tv->file" is automatically initialized to NULL; setting it explicitly is unnecessary here. */
}

static void
tfe_text_view_class_init (TfeTextViewClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = tfe_text_view_dispose;
}

GFile *
tfe_text_view_get_file (TfeTextView *tv) {
  g_return_val_if_fail (TFE_IS_TEXT_VIEW (tv), NULL);

  /* The instance holds the ownership of the returned GFile. */
  return tv->file;
}

void
tfe_text_view_set_file (TfeTextView *tv, GFile *file) {
  g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
  g_return_if_fail (G_IS_FILE (file) || file == NULL);

  GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));

  /* The instance gets the ownership of the new GFile by increment the reference count. */
  g_set_object (&tv->file, file);
  gtk_text_buffer_set_modified (tb, TRUE);
}

gboolean
tfe_text_view_get_modified (TfeTextView *tv) {
  g_return_val_if_fail (TFE_IS_TEXT_VIEW (tv), FALSE);

  GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));

  return gtk_text_buffer_get_modified (tb);
}

gboolean
tfe_text_view_read (TfeTextView *tv, GError **err) {
  g_return_val_if_fail (TFE_IS_TEXT_VIEW (tv), FALSE);
  g_return_val_if_fail (err == NULL || *err == NULL, FALSE);

  GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  gboolean stat;
  char *contents;
  gsize length;

  if (! G_IS_FILE (tv->file)) {
    g_set_error_literal (err, TFE_TEXT_VIEW_ERROR, TFE_TEXT_VIEW_ERROR_NO_FILE, "No file is set in TfeTextView.");
    return FALSE;
  }
  
  stat = g_file_load_contents (tv->file, NULL, &contents, &length, NULL, err);
  if (stat) {
    if (length > G_MAXINT) { /* File is too large to put it in the buffer*/
      g_set_error_literal (err, TFE_TEXT_VIEW_ERROR, TFE_TEXT_VIEW_ERROR_FAILED, "The file is too large.");
      stat = FALSE;
    }
    else if (!g_utf8_validate (contents, length, NULL)) { /* Only valid UTF-8 strings are accepted */
      g_set_error_literal (err, TFE_TEXT_VIEW_ERROR, TFE_TEXT_VIEW_ERROR_FAILED, "The file is not valid UTF-8.");
      stat = FALSE;
    }
    else {
      gtk_text_buffer_set_text (tb, contents, (int) length);
      gtk_text_buffer_set_modified (tb, FALSE);
    }
    g_free (contents);
  }
  return stat;
}

gboolean
tfe_text_view_write (TfeTextView *tv, GError **err) {
  g_return_val_if_fail (TFE_IS_TEXT_VIEW (tv), FALSE);
  g_return_val_if_fail (err == NULL || *err == NULL, FALSE);

  GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
  GtkTextIter start_iter;
  GtkTextIter end_iter;
  char *contents;
  gboolean stat;

  if (! G_IS_FILE (tv->file)) {
    g_set_error_literal (err, TFE_TEXT_VIEW_ERROR, TFE_TEXT_VIEW_ERROR_NO_FILE, "No file is set in TfeTextView.");
    return FALSE;
  }

  gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
  contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
  stat = g_file_replace_contents (tv->file, contents, strlen (contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, err);
  g_free (contents);
  if (stat)
    gtk_text_buffer_set_modified (tb, FALSE);
  return stat;
}

GtkWidget *
tfe_text_view_new (void) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_TEXT_VIEW, "wrap-mode", GTK_WRAP_WORD_CHAR, NULL));
}

/* If file == NULL, the function just returns a new TfeTextView instance. Its file member is NULL and the buffer is empty. */
/* If file is not NULL and the file can be read, the function returns a new TfeTextView instance with the file loaded. */
/* If an error happens during the read operation, the function returns NULL and sets the error. */
GtkWidget *
tfe_text_view_new_with_file (GFile *file, GError **err) {
  g_return_val_if_fail (G_IS_FILE (file) || file == NULL, NULL);
  g_return_val_if_fail (err == NULL || *err == NULL, NULL);

  GtkWidget *tv;

  tv = tfe_text_view_new();
  if (file == NULL)
    return tv;
  tfe_text_view_set_file (TFE_TEXT_VIEW (tv), file);
  if (tfe_text_view_read (TFE_TEXT_VIEW (tv), err))
    return tv;
  /* An error occurs during the read operation */
  g_object_ref_sink (tv);
  g_object_unref (tv);
  return NULL;
}
