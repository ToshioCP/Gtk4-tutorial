Up: [README.md](../README.md),  Prev: [Section 9](sec9.md), Next: [Section 11](sec11.md)

# Build system

## Managing big source files

We've compiled a small editor so far.
The program is also small and not complicated yet.
But if it grows bigger, it will be difficult to maintain.
So, we should do the followings now.

- We've had only one C source file and put everything in it.
We need to divide it and sort them out.
- There are two compilers, `gcc` and `glib-compile-resources`.
We should control them by one building tool. 

## Divide a C source file into two parts.

When you divide C source file into several parts, each file should contain one thing.
For example, our source has two things, the definition of TfeTextView and functions related to GtkApplication and GtkApplicationWindow.
It is a good idea to separate them into two files, `tfetextview.c` and `tfe.c`.

- `tfetextview.c` includes the definition and functions of TfeTextView.
- `tfe.c` includes functions like `main`, `app_activate`, `app_open` and so on, which relate to GtkApplication and GtkApplicationWindow

Now we have three source files, `tfetextview.c`, `tfe.c` and `tfe3.ui`.
The `3` of `tfe3.ui` is like a version number.
Managing version with filenames is one possible idea but it also has a problem.
You need to rewrite filename in each version and it affects to contents of source files that refer to filenames.
So, we should take `3` away from the filename.

In `tfe.c` the function `tfe_text_view_new` is invoked to create a TfeTextView instance.
But it is defined in `tfetextview.c`, not `tfe.c`.
The lack of the declaration (not definition) of `tfe_text_view_new` makes error when `tfe.c` is compiled.
The declaration is necessary in `tfe.c`.
Those public information is usually written in header files.
It has `.h` suffix like `tfetextview.h`.
And header files are included by C source files.
For example, `tfetextview.h` is included by `tfe.c`.

The source files are shown below.

`tfetextview.h`

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 #define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type ()
 4 G_DECLARE_FINAL_TYPE (TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView)
 5 
 6 void
 7 tfe_text_view_set_file (TfeTextView *tv, GFile *f);
 8 
 9 GFile *
10 tfe_text_view_get_file (TfeTextView *tv);
11 
12 GtkWidget *
13 tfe_text_view_new (void);
14 
~~~

`tfetextview.c`

~~~C
 1 #include <gtk/gtk.h>
 2 #include "tfetextview.h"
 3 
 4 struct _TfeTextView
 5 {
 6   GtkTextView parent;
 7   GFile *file;
 8 };
 9 
10 G_DEFINE_TYPE (TfeTextView, tfe_text_view, GTK_TYPE_TEXT_VIEW);
11 
12 static void
13 tfe_text_view_init (TfeTextView *tv) {
14 }
15 
16 static void
17 tfe_text_view_class_init (TfeTextViewClass *class) {
18 }
19 
20 void
21 tfe_text_view_set_file (TfeTextView *tv, GFile *f) {
22   tv->file = f;
23 }
24 
25 GFile *
26 tfe_text_view_get_file (TfeTextView *tv) {
27   return tv->file;
28 }
29 
30 GtkWidget *
31 tfe_text_view_new (void) {
32   return GTK_WIDGET (g_object_new (TFE_TYPE_TEXT_VIEW, NULL));
33 }
34 
~~~

`tfe.c`

~~~C
 1 #include <gtk/gtk.h>
 2 #include "tfetextview.h"
 3 
 4 static void
 5 app_activate (GApplication *app) {
 6   g_print ("You need a filename argument.\n");
 7 }
 8 
 9 static void
10 app_open (GApplication *app, GFile ** files, gint n_files, gchar *hint) {
11   GtkWidget *win;
12   GtkWidget *nb;
13   GtkWidget *lab;
14   GtkNotebookPage *nbp;
15   GtkWidget *scr;
16   GtkWidget *tv;
17   GtkTextBuffer *tb;
18   char *contents;
19   gsize length;
20   char *filename;
21   int i;
22   GError *err = NULL;
23   GtkBuilder *build;
24 
25   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/tfe.ui");
26   win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
27   gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
28   nb = GTK_WIDGET (gtk_builder_get_object (build, "nb"));
29   g_object_unref (build);
30   for (i = 0; i < n_files; i++) {
31     if (g_file_load_contents (files[i], NULL, &contents, &length, NULL, &err)) {
32       scr = gtk_scrolled_window_new ();
33       tv = tfe_text_view_new ();
34       tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
35       gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
36       gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
37 
38       tfe_text_view_set_file (TFE_TEXT_VIEW (tv),  g_file_dup (files[i]));
39       gtk_text_buffer_set_text (tb, contents, length);
40       g_free (contents);
41       filename = g_file_get_basename (files[i]);
42       lab = gtk_label_new (filename);
43       gtk_notebook_append_page (GTK_NOTEBOOK (nb), scr, lab);
44       nbp = gtk_notebook_get_page (GTK_NOTEBOOK (nb), scr);
45       g_object_set (nbp, "tab-expand", TRUE, NULL);
46       g_free (filename);
47     } else {
48       g_printerr ("%s.\n", err->message);
49       g_clear_error (&err);
50     }
51   }
52   if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) > 0) {
53     gtk_window_present (GTK_WINDOW (win));
54   } else
55     gtk_window_destroy (GTK_WINDOW (win));
56 }
57 
58 int
59 main (int argc, char **argv) {
60   GtkApplication *app;
61   int stat;
62 
63   app = gtk_application_new ("com.github.ToshioCP.tfe", G_APPLICATION_HANDLES_OPEN);
64   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
65   g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
66   stat =g_application_run (G_APPLICATION (app), argc, argv);
67   g_object_unref (app);
68   return stat;
69 }
70 
~~~

The ui file `tfe.ui` is the same as `tfe3.ui` in the previous section.

`tfe.gresource.xml`

~~~xml
1 <?xml version="1.0" encoding="UTF-8"?>
2 <gresources>
3   <gresource prefix="/com/github/ToshioCP/tfe">
4     <file>tfe.ui</file>
5   </gresource>
6 </gresources>
~~~

Dividing a file makes it easy to maintain.
But now we face a new problem.
The building step increases.

- Compiling the ui file `tfe.ui` into `resources.c`.
- Compiling `tfe.c` into `tfe.o` (object file).
- Compiling `tfetextview.c` into `tfetextview.o`.
- Compiling `resources.c` into `resources.o`.
- Linking all the object files into application `tfe`.

Build tools manage the steps.

## Meson and Ninja

I'll explain Meson and Ninja build tools.

Other possible tools are Make and Autotools.
They are traditional tools but slower than Ninja.
So, many developers use Meson and Ninja lately.
For example, GTK 4 uses them.

You need to create `meson.build` file first.

~~~meson
 1 project('tfe', 'c')
 2 
 3 gtkdep = dependency('gtk4')
 4 
 5 gnome=import('gnome')
 6 resources = gnome.compile_resources('resources','tfe.gresource.xml')
 7 
 8 sourcefiles=files('tfe.c', 'tfetextview.c')
 9 
10 executable('tfe', sourcefiles, resources, dependencies: gtkdep, install: false)
~~~

- 1: The function `project` defines things about the project.
The first argument is the name of the project and the second is the programming language.
- 3: The function `dependency` defines a dependency that is taken by `pkg-config`.
We put `gtk4` as an argument.
- 5: The function `import` imports a module.
In line 5, the gnome module is imported and assigned to the variable `gnome`.
The gnome module provides helper tools to build GTK programs.
- 6: The method `.compile_resources` is of the gnome module and compiles files to resources under the instruction of xml file.
In line 6, the resource filename is `resources`, which means `resources.c` and `resources.h`, and xml file is `tfe.gresource.xml`.
This method generates C source file by default.
- 8: Defines source files.
- 10: Executable function generates a target file by compiling source files.
The first argument is the filename of the target. The following arguments are source files.
The last two arguments have keys and values.
For example, the fourth argument has a key `dependencies` , a delimiter (`:`) and a value `gtkdep`.
This type of parameter is called *keyword parameter* or *kwargs*.
The value `gtkdep` is defined in line 3.
The last argument tells that this project doesn't install the executable file.
So it is just compiled in the build directory.

Now run meson and ninja.

    $ meson setup _build
    $ ninja -C _build

meson has two arguments.

- setup: The first argument is a command of meson.
Setup is the default, so you can leave it out.
But it is recommended to write it explicitly since version 0.64.0.
- The second argument is the name of the build directory.

Then, the executable file `tfe` is generated under the directory `_build`.

    $ _build/tfe tfe.c tfetextview.c

A window appears.
It includes a notebook with two pages.
One is `tfe.c` and the other is `tfetextview.c`.

For further information, see [The Meson Build system](https://mesonbuild.com/).

Up: [README.md](../README.md),  Prev: [Section 9](sec9.md), Next: [Section 11](sec11.md)
