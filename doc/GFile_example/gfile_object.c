/* Samples of GFile related objects */
/* Copy file program */
/* This function uses 'g_input_stream_read' and 'g_output_stream_write'. */

#include <gio/gio.h>

void
usage () {
  g_printerr ("Usage: gfile_object (file|uri)\n");
  exit (1);
}

int
main (int argc, char **argv) {
  GFile *file;
  GType type;

  if (argc != 2)
    usage();
  file = g_file_new_for_commandline_arg (argv[1]);
  type = G_OBJECT_TYPE (file);
  g_print ("%s\n", g_type_name (type));
  while (type = g_type_parent (type))
    g_print ("%s\n", g_type_name (type));
  return 0;
}

