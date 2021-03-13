# What are the files in this directory?

They are examples to show how to use GFile related objects such as GInputStream and GOutPutStream.

- `meson.build` is used by meson.
- `readline.c` is an example to show how to read lines from a file.
- `copy.c` is an example to show read and write stream functions with Gio objects.
- `gfile_object.c` shows the object implements GFile interface.

## Compilation

Type as follows on your command line.

1. `meson _build`
2. `ninja -C _build`

Then executable files `readline`, `copy` and `gfile_object` are created under `_build` directory.

## Execution

Readline prints the contents of a file.
The file is given to `readline` as an argument.

~~~
$ _build/readline readline.c
/* Samples of GFile related objects */
/* Functions here has "sgf" prefix. Sgf is an acronym of "Sample of GFile". */

/* List a file */
/* This program shows how to read lines from a file. */

#include <gio/gio.h>

void
usage () {
  g_printerr ("Usage: readline file\n");
  exit (1);
}

... ... ...
... ... ...
~~~

Readline can also prints a file through the internet.

~~~
$ _build/readline http://www7b.biglobe.ne.jp/~j87107/SINU/index.html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1.0">

  ... ... ...
  ... ... ...
~~~

This is because GFile can be constructed for local path or uri.

Copy is given two arguments.
The first one is a source filename and the second one is a destination filename.
If the destination file is exist, it will be overwritten.
Before the overwriting, the destination file is backed up.
The backup file name is appended with tilde ('~').

~~~
$ ls
_build  copy.c  meson.build  readline.c  readme.md
$ _build/copy copy.c sample.txt
$ ls
_build  copy.c  meson.build  readline.c  readme.md  sample.txt
$ diff copy.c sample.txt
$ _build/copy readline.c sample.txt
$ ls
_build  copy.c  meson.build  readline.c  readme.md  sample.txt  sample.txt~
$ diff readline.c sample.txt
$ diff copy.c sample.txt~
$
~~~

Files can be on the internet.

~~~
$ _build/copy http://www7b.biglobe.ne.jp/~j87107/SINU/Algebra.pdf algebra.pdf
$ ls
_build  algebra.pdf  copy.c  meson.build  readline.c  readme.md  sample.txt  sample.txt~
~~~

Gfile_object shows the object implements GFile interface.

~~~
$ _build/gfile_object gfile_object.c
GLocalFile
GObject
$ _build/gfile_object http://www7b.biglobe.ne.jp/~j87107/SINU/Algebra.pdf
GDaemonFile
GObject
~~~

`g_file_new_for_path` creates GLocalFile object which implements GFile interface.
And it returns the pointer to the object as `GFile *`.
`g_file_new_for_uri`  creates GDaemonFile object which implements GFile interface.
And it returns the pointer to the object as `GFile *`.

## GFile interface

GFile is an interface.
It is like a pathname.

If you want to read from a file or write to a file, you need to create a stream object.
`copy.c` creates GInputStream and GOutputStream to copy a file.

~~~C
 1 /* Samples of GFile related objects */
 2 /* Copy file program */
 3 /* This function uses 'g_input_stream_read' and 'g_output_stream_write'. */
 4 
 5 #include <gio/gio.h>
 6 
 7 void
 8 usage () {
 9   g_printerr ("Usage: copy file1 file2\n");
10   exit (1);
11 }
12 
13 #define MAXBUF 4096
14 int
15 main (int argc, char **argv) {
16   GFile *file1, *file2;
17   GFileInputStream *istream;
18   GFileOutputStream *ostream;
19   char buf[MAXBUF];
20   gssize size;
21   GError *err = NULL;
22   int status = 0;
23 
24   if (argc != 3)
25     usage();
26   file1 = g_file_new_for_commandline_arg (argv[1]);
27   file2 = g_file_new_for_commandline_arg (argv[2]);
28   if (! (istream = g_file_read (file1, NULL, &err))) {
29     g_warning ("%s\n", err->message);
30     return 1;
31   }
32   if (! (ostream = g_file_replace (file2, NULL, TRUE, G_FILE_CREATE_NONE, NULL, &err))) {
33     g_warning ("%s\n", err->message);
34     return 1;
35   }
36   while ((size = g_input_stream_read (G_INPUT_STREAM (istream), buf, MAXBUF, NULL, &err)) > 0) {
37     if ((size = g_output_stream_write (G_OUTPUT_STREAM (ostream), buf, (gsize) size, NULL, &err)) < 0)
38       break;
39   }
40   if (size < 0) {
41     g_warning ("%s\n", err->message);
42     status = 1;
43   }
44   if (! (g_input_stream_close (G_INPUT_STREAM (istream), NULL, &err))) {
45     g_warning ("%s\n", err->message);
46     status = 1;
47   }
48   if (! (g_output_stream_close (G_OUTPUT_STREAM (ostream), NULL, &err))) {
49     g_warning ("%s\n", err->message);
50     status = 1;
51   }
52   return status;
53 }
54 
~~~

## Stream objects

Stream objects have hierarchy.

~~~
GInputStream
     +--+-- GFilterInputStream
     |  +--+-- GBufferedInputStream
     |  |  +----- GDataInputStream
     |  +----- GConverterInputStream
     +----- GFileInputStream
     +----- GMemoryInputStream
     +----- GUnixInputStream
~~~

GInputStream is the top parent object.
It is an abstract object.
Streams are created with specific objects such as files, memories and unix file descriptors.
For example, the following function creates a GFileInputStream.

~~~C
GFileInputStream *istream =  g_file_read (GFile *file, GCancellable *cancellable, GError **error);
~~~

This stream is a child of GInputStream object, so you can apply any functions of GInputStream for `istream`.
For example,

~~~C
gssize size = g_input_stream_read (G_INPUT_STREAM (istream), void *buffer, gsize count, GCancellable *cancellable, GError **error);
~~~

This function reads data from `istream` and puts them into `buffer`.

GDataInputStream is used often.
It can read structured data such as sized data (byte, int16, int32 and int64) and lines (new line terminated string).
For example, 

~~~C
GDataInputStream *dstream = g_data_input_stream_new (G_INPUT_STREAM (istream));
char *line = g_data_input_stream_read_line_utf8 (dstream, gsize *size, GCancellable *cancellable, GError **error);
~~~

The program above reads a line from `dstream`.

