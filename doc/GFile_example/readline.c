/* Samples of GFile related objects */
/* Functions here has "sgf" prefix. Sgf is an acronym of "Sample of GFile". */

/* List a file */
/* This program shows how to read lines from a file. */

#include <gio/gio.h>
#include <glib/gprintf.h>

void
usage () {
  g_printerr ("Usage: readline file\n");
  exit (1);
}

/* Inputstream objects */
/* GInputStream */
/*      +----- GFileInputStream */
/*      +--+-- GFilterInputStream */
/*         +--+-- GBufferedInputStream */
/*            +----- GDataInputStream */

GDataInputStream *
sgf_open (GFile *file) {
  g_return_val_if_fail (G_IS_FILE (file), NULL);

  GFileInputStream *stream;
  GDataInputStream *dstream;
  GError *err;

  if (! (stream = g_file_read (file, NULL, &err))) {
    g_warning ("%s\n", err->message);
    return NULL;
  }
/* 'dstream' is a different instance from 'stream' instance. */
/* 'dstream' has "base-stream" property, which is a property of GFilterInputStream, and the property points 'stream'. */
/* GBufferedInputStream uses base-stream to read (input) data from its source. */
/* Therefore, 'dstream' reads a line through the IO interface of 'stream'. */
  dstream = g_data_input_stream_new (G_INPUT_STREAM (stream));
  return dstream;
}

char *
sgf_readline (GDataInputStream *dstream) {
  char *contents;
  gsize length;
  GError *err = NULL;

  contents = g_data_input_stream_read_line_utf8 (dstream, &length, NULL, &err);
  if (! contents && err) {
    g_warning ("%s\n", err->message);
    return NULL;
  }
  return contents; /* if contents == NULL, then it is EOF */
}

void
sgf_close (GDataInputStream *dstream) {
  GError *err = NULL;

/* At the same time dstream closes, its base stream (GFileInputStream created by 'sgf_open') is also closed. */
/* Because the default of "close-base-stream" property (of GFilterInputStream which is an ancester of GDataInputStream) is TRUE */
  if (! g_input_stream_close (G_INPUT_STREAM (dstream), NULL, &err))
    g_warning ("%s\n", err->message);
}

/* ----- main ----- */
int
main (int argc, char **argv) {
  GFile *file;
  GDataInputStream *dstream;
  char *line;

  if (argc != 2)
    usage ();
  file = g_file_new_for_commandline_arg (argv[1]);
  if (! (dstream = sgf_open (file)))
    return -1; /* error */
  while (line = sgf_readline (dstream))
    g_printf ("%s\n", line);
  sgf_close (dstream);
  return 0;
}

