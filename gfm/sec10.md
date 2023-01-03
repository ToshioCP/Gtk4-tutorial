Up: [README.md](../README.md),  Prev: [Section 9](sec9.md), Next: [Section 11](sec11.md)

# Build system

## Managing big source files

We've compiled a small editor so far.
But Some bad signs are beginning to appear.

- We've had only one C source file and put everything in it.
We need to sort it out.
- There are two compilers, `gcc` and `glib-compile-resources`.
We should control them by one building tool. 

These ideas are useful to manage big source files.

## Divide a C source file into two parts.

When you divide C source file into several parts, each file should contain one thing.
For example, our source has two things, the definition of TfeTextView and functions related to GtkApplication and GtkApplicationWindow.
It is a good idea to separate them into two files, `tfetextview.c` and `tfe.c`.

- `tfetextview.c` includes the definition and functions of TfeTextView.
- `tfe.c` includes functions like `main`, `app_activate`, `app_open` and so on, which relate to GtkApplication and GtkApplicationWindow

Now we have three source files, `tfetextview.c`, `tfe.c` and `tfe3.ui`.
The `3` of `tfe3.ui` is like a version number.
Managing version with filenames is one possible idea but it may make bothersome problem.
You need to rewrite filename in each version and it affects to contents of source files that refer to filenames.
So, we should take `3` away from the filename.

In `tfe.c` the function `tfe_text_view_new` is invoked to create a TfeTextView instance.
But it is defined in `tfetextview.c`, not `tfe.c`.
The lack of the declaration (not definition) of `tfe_text_view_new` makes error when `tfe.c` is compiled.
The declaration is necessary in `tfe.c`.
Those public information is usually written in header files.
It has `.h` suffix like `tfetextview.h`
And header files are included by C source files.
For example, `tfetextview.h` is included by `tfe.c`.

All the source files are listed below.

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
22   tv -> file = f;
23 }
24 
25 GFile *
26 tfe_text_view_get_file (TfeTextView *tv) {
27   return tv -> file;
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
22   GtkBuilder *build;
23 
24   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe3/tfe.ui");
25   win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
26   gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
27   nb = GTK_WIDGET (gtk_builder_get_object (build, "nb"));
28   g_object_unref (build);
29   for (i = 0; i < n_files; i++) {
30     if (g_file_load_contents (files[i], NULL, &contents, &length, NULL, NULL)) {
31       scr = gtk_scrolled_window_new ();
32       tv = tfe_text_view_new ();
33       tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
34       gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
35       gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
36 
37       tfe_text_view_set_file (TFE_TEXT_VIEW (tv),  g_file_dup (files[i]));
38       gtk_text_buffer_set_text (tb, contents, length);
39       g_free (contents);
40       filename = g_file_get_basename (files[i]);
41       lab = gtk_label_new (filename);
42       gtk_notebook_append_page (GTK_NOTEBOOK (nb), scr, lab);
43       nbp = gtk_notebook_get_page (GTK_NOTEBOOK (nb), scr);
44       g_object_set (nbp, "tab-expand", TRUE, NULL);
45       g_free (filename);
46     } else if ((filename = g_file_get_path (files[i])) != NULL) {
47         g_print ("No such file: %s.\n", filename);
48         g_free (filename);
49     } else
50         g_print ("No valid file is given\n");
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
3   <gresource prefix="/com/github/ToshioCP/tfe3">
4     <file>tfe.ui</file>
5   </gresource>
6 </gresources>
~~~

Dividing a file makes it easy to maintain source files.
But now we face a new problem.
The building step increases.

- Compiling the ui file `tfe.ui` into `resources.c`.
- Compiling `tfe.c` into `tfe.o` (object file).
- Compiling `tfetextview.c` into `tfetextview.o`.
- Compiling `resources.c` into `resources.o`.
- Linking all the object files into application `tfe`.

Build tools manage the steps.
I'll show you three build tools, Meson and Ninja, Make and Rake.
Meson and Ninja is recommended as a C build tool, but others are also fine.
It's your choice.

## Meson and Ninja

Meson and Ninja is one of the most popular building tool to build C language program.
Many developers use Meson and Ninja lately.
For example, GTK 4 uses them.

You need to make `meson.build` file first.

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
10 executable('tfe', sourcefiles, resources, dependencies: gtkdep)
~~~

- 1: The function `project` defines things about the project.
The first parameter is the name of the project and the second is the programming language.
- 2: `dependency` function defines a dependency that is taken by `pkg-config`.
We put `gtk4` as an argument.
- 5: `import` function imports a module.
In line 5, the gnome module is imported and assigned to the variable `gnome`.
The gnome module provides helper tools to build GTK programs.
- 6: `.compile_resources` is a method of the gnome module and compiles files to resources under the instruction of xml file.
In line 6, the resource filename is `resources`, which means `resources.c` and `resources.h`, and xml file is `tfe.gresource.xml`.
This method generates C source file by default.
- 8: Defines source files.
- 10: Executable function generates a target file by compiling source files.
The first parameter is the filename of the target. The following parameters are source files.
The last parameter is an option `dependencies`.
`gtkdep` is used in the compilation.

Now run meson and ninja.

    $ meson _build
    $ ninja -C _build

Then, the executable file `tfe` is generated under the directory `_build`.

    $ _build/tfe tfe.c tfetextview.c

A window appears.
It includes a notebook with two pages.
One is `tfe.c` and the other is `tfetextview.c`.

For further information, see [The Meson Build system](https://mesonbuild.com/).

## Make

Make is a build tool created in 1976.
It was a standard build tool for C compiling, but lately it is replaced by Meson and Ninja.

Make analyzes Makefile and executes compilers.
All instructions are written in Makefile.

For example,

~~~makefile
sample.o: sample.c
    gcc -o sample.o sample.c
~~~

Malefile above consists of three elements, `sample.o`, `sample.c` and `gcc -o sample.o sample.c`.

- `sample.o` is a target.
- `sample.c` is a prerequisite.
- `gcc -o sample.o sample.c` is a recipe.
Recipes follow tab characters, not spaces.
(It is very important. Use tab, or make won't work as you expected).

The rule is:

If a prerequisite modified later than a target, then make executes the recipe.

In the example above, if `sample.c` is modified after the generation of `sample.o`, then make executes gcc and compile `sample.c` into `sample.o`.
If the modification time of `sample.c` is older then the generation of `sample.o`, then no compiling is necessary, so make does nothing.

The Makefile for `tfe` is as follows.

~~~makefile
 1 all: tfe
 2 
 3 tfe: tfe.o tfetextview.o resources.o
 4 	gcc -o tfe tfe.o tfetextview.o resources.o `pkg-config --libs gtk4`
 5 
 6 tfe.o: tfe.c tfetextview.h
 7 	gcc -c -o tfe.o `pkg-config --cflags gtk4` tfe.c
 8 tfetextview.o: tfetextview.c tfetextview.h
 9 	gcc -c -o tfetextview.o `pkg-config --cflags gtk4` tfetextview.c
10 resources.o: resources.c
11 	gcc -c -o resources.o `pkg-config --cflags gtk4` resources.c
12 
13 resources.c: tfe.gresource.xml tfe.ui
14 	glib-compile-resources tfe.gresource.xml --target=resources.c --generate-source
15 
16 .Phony: clean
17 
18 clean:
19 	rm -f tfe tfe.o tfetextview.o resources.o resources.c
~~~

You just type `make` and everything will be done.

~~~
$ make
gcc -c -o tfe.o `pkg-config --cflags gtk4` tfe.c
gcc -c -o tfetextview.o `pkg-config --cflags gtk4` tfetextview.c
glib-compile-resources tfe.gresource.xml --target=resources.c --generate-source
gcc -c -o resources.o `pkg-config --cflags gtk4` resources.c
gcc -o tfe tfe.o tfetextview.o resources.o `pkg-config --libs gtk4`
~~~

I used only very basic rules to write this Makefile.
There are many more convenient methods to make it more compact.
But it will be long to explain it.
So I want to finish with make and move on to the next topic.

You can download "Gnu Make Manual" from [GNU website](https://www.gnu.org/software/make/manual/).

## Rake

Rake is a similar program to make.
It is written in Ruby language.
If you don't use Ruby, you don't need to read this subsection.
However, Ruby is really sophisticated and recommendable script language.

- Rakefile controls the behavior of `rake`.
- You can write any Ruby code in Rakefile.

Rake has task and file task, which is similar to target, prerequisite and recipe in make.

~~~ruby
 1 require 'rake/clean'
 2 
 3 targetfile = "tfe"
 4 srcfiles = FileList["tfe.c", "tfetextview.c", "resources.c"]
 5 uifile = "tfe.ui"
 6 rscfile = srcfiles[2]
 7 objfiles = srcfiles.ext(".o")
 8 gresource_xml = "tfe.gresource.xml"
 9 
10 CLEAN.include(targetfile, objfiles, rscfile)
11 
12 task default: targetfile
13 
14 file targetfile => objfiles do |t|
15   sh "gcc -o #{t.name} #{t.prerequisites.join(' ')} `pkg-config --libs gtk4`"
16 end
17 
18 objfiles.each do |obj|
19   src = obj.ext(".c")
20   file obj => src do |t|
21     sh "gcc -c -o #{t.name} `pkg-config --cflags gtk4` #{t.source}"
22   end
23 end
24 
25 file rscfile => uifile do |t|
26   sh "glib-compile-resources #{gresource_xml} --target=#{t.name} --generate-source"
27 end
~~~

The contents of the `Rakefile` is almost same as the `Makefile` in the previous subsection. 

- 3-8: Defines target file, source files and so on.
- 1, 10 Requires  rake/clean library. And clean files are added to CLEAN.
The files included by CLEAN will be removed when `rake clean` is typed on the command line.
- 12: The default target depends on `targetfile`.
The task `default` is the final goal of tasks.
- 14-16: `targetfile` depends on `objfiles`.
The variable `t` is a task object.
  - `t.name` is a target name
  - `t.prerequisites` is an array of prerequisites.
  - `t.source` is the first element of prerequisites.
- `sh` is a method to give the following string to shell as an argument and executes the shell.
- 18-23: An each iterator of the array `objfiles`. Each object depends on corresponding source file.
- 25-27: Resource file depends on ui file.

Rakefile might seem to be difficult for beginners.
But, you can use any Ruby syntax in the Rakefile, so it is really flexible.
If you practice Ruby and Rakefile, it will be highly productive tools.

For further information, see [Rake tutorial for beginners](https://toshiocp.github.io/Rake-tutorial-for-beginners-en/LearningRake.html).

Up: [README.md](../README.md),  Prev: [Section 9](sec9.md), Next: [Section 11](sec11.md)
