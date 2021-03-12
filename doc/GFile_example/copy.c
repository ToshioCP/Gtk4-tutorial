/* Samples of GFile related objects */
/* Copy file program */
/* This function uses 'g_input_stream_read' and 'g_output_stream_write'. */

#include <gio/gio.h>

void
usage () {
  g_printerr ("Usage: copy file1 file2\n");
  exit (1);
}

#define MAXBUF 4096
int
main (int argc, char **argv) {
  GFile *file1, *file2;
  GFileInputStream *istream;
  GFileOutputStream *ostream;
  char buf[MAXBUF];
  gssize size;
  GError *err = NULL;
  int status = 0;

  if (argc != 3)
    usage();
  file1 = g_file_new_for_commandline_arg (argv[1]);
  file2 = g_file_new_for_commandline_arg (argv[2]);
  if (! (istream = g_file_read (file1, NULL, &err))) {
    g_warning ("%s\n", err->message);
    return 1;
  }
  if (! (ostream = g_file_replace (file2, NULL, TRUE, G_FILE_CREATE_NONE, NULL, &err))) {
    g_warning ("%s\n", err->message);
    return 1;
  }
  while ((size = g_input_stream_read (G_INPUT_STREAM (istream), buf, MAXBUF, NULL, &err)) > 0) {
    if ((size = g_output_stream_write (G_OUTPUT_STREAM (ostream), buf, (gsize) size, NULL, &err)) < 0)
      break;
  }
  if (size < 0) {
    g_warning ("%s\n", err->message);
    status = 1;
  }
  if (! (g_input_stream_close (G_INPUT_STREAM (istream), NULL, &err))) {
    g_warning ("%s\n", err->message);
    status = 1;
  }
  if (! (g_output_stream_close (G_OUTPUT_STREAM (ostream), NULL, &err))) {
    g_warning ("%s\n", err->message);
    status = 1;
  }
  return status;
}

