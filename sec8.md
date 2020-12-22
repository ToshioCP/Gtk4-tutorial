Up: [Readme.md](Readme.md),  Prev: [Section 7](sec7.md), Next: [Section 9](sec9.md)
# Build system

## What do we need to think about building?

We've managed to compile a small editor so far.
But Some bad signs are beginning to appear.

- We have only one C source file and put everything into it.
We need to sort it out.
- There are two compilers, `gcc` and `glib-compile-resources`.
We want to control them by one building tool. 

## Divide a C source file into two parts.

When you divide C source file into several parts, each file should contain only one thing.
For example, our source has two things, the definition of TfeTextView and functions related to GtkApplication and GtkApplicationWindow.
It is a good idea to separate them into two files, `tfetextview.c` and `tfe.c`.

- `tfetextview.c` includes the definition and functions of TfeTextView.
- `tfe.c` includes functions like `main`, `on_activate`, `on_open` and so on, which relate to GtkApplication and GtkApplicationWindow

Now we have three source files, `tfetextview.c`, `tfe.c` and `tfe3.ui`.
The `3` of `tfe3.ui` is like a version number.
Managing version with filenames is one possible idea but it may make bothersome complicated problem.
You need to rewrite filename in each version and it affects to contents of sourcefiles that refer to filenames.
So, we should take `3` away from the filename.

In `tfe.c` the function `tfe_text_view_new` is invoked to generate TfeTextView.
But it is defined in `tfetextview.c`, not `tfe.c`.
The lack of the declaration (not definition) of `tfe_text_view_new` makes error when `tfe.c` is compiled.
The declaration is necessary in `tfe.c`.
Those public information is usually written in header files.
It has `.h` suffix like `tfetextview.h`
And header files are included by C source files.
For example, `tfetextview.h` is included by `tfe.c`.

`tfetextview.h`

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

`tfetextview.c`

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
    32   return gtk_widget_new (TFE_TYPE_TEXT_VIEW, NULL);
    33 }
    34 

`tfe.c`

     1 #include <gtk/gtk.h>
     2 #include "tfetextview.h"
     3 
     4 static void
     5 on_activate (GApplication *app, gpointer user_data) {
     6   g_print ("You need a filename argument.\n");
     7 }
     8 
     9 static void
    10 on_open (GApplication *app, GFile ** files, gint n_files, gchar *hint, gpointer user_data) {
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
    31       scr = gtk_scrolled_window_new (NULL, NULL);
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
    46     } else {
    47       filename = g_file_get_path (files[i]);
    48       g_print ("No such file: %s.\n", filename);
    49       g_free (filename);
    50     }
    51   }
    52   if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) > 0) {
    53     gtk_widget_show (win);
    54   } else
    55     gtk_window_destroy (GTK_WINDOW (win));
    56 }
    57 
    58 int
    59 main (int argc, char **argv) {
    60   GtkApplication *app;
    61   int stat;
    62 
    63   app = gtk_application_new ("com.github.ToshioCP.tfe3", G_APPLICATION_HANDLES_OPEN);
    64   g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
    65   g_signal_connect (app, "open", G_CALLBACK (on_open), NULL);
    66   stat =g_application_run (G_APPLICATION (app), argc, argv);
    67   g_object_unref (app);
    68   return stat;
    69 }
    70 

`tfe.ui`

     1 <interface>
     2   <object class="GtkApplicationWindow" id="win">
     3     <property name="title">file editor</property>
     4     <property name="default-width">600</property>
     5     <property name="default-height">400</property>
     6     <child>
     7       <object class="GtkBox" id="boxv">
     8         <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
     9         <child>
    10           <object class="GtkBox" id="boxh">
    11           <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
    12             <child>
    13               <object class="GtkLabel" id="dmy1">
    14               <property name="width-chars">10</property>
    15               </object>
    16             </child>
    17             <child>
    18               <object class="GtkButton" id="btnn">
    19               <property name="label">New</property>
    20               </object>
    21             </child>
    22             <child>
    23               <object class="GtkButton" id="btno">
    24               <property name="label">Open</property>
    25               </object>
    26             </child>
    27             <child>
    28               <object class="GtkLabel" id="dmy2">
    29               <property name="hexpand">TRUE</property>
    30               </object>
    31             </child>
    32             <child>
    33               <object class="GtkButton" id="btns">
    34               <property name="label">Save</property>
    35               </object>
    36             </child>
    37             <child>
    38               <object class="GtkButton" id="btnc">
    39               <property name="label">Close</property>
    40               </object>
    41             </child>
    42             <child>
    43               <object class="GtkLabel" id="dmy3">
    44               <property name="width-chars">10</property>
    45               </object>
    46             </child>
    47           </object>
    48         </child>
    49         <child>
    50           <object class="GtkNotebook" id="nb">
    51             <property name="hexpand">TRUE</property>
    52             <property name="vexpand">TRUE</property>
    53           </object>
    54         </child>
    55       </object>
    56     </child>
    57   </object>
    58 </interface>
    59 

`tfe.gresource.xml`

    1 <?xml version="1.0" encoding="UTF-8"?>
    2 <gresources>
    3   <gresource prefix="/com/github/ToshioCP/tfe3">
    4     <file>tfe.ui</file>
    5   </gresource>
    6 </gresources>

## Make

Dividing a file makes it easy to maintain source files.
But now we are faced with a new problem.
The building step increases.

- Compile the ui file `tfe.ui` into `resources.c`.
- Compile `tfe.c` into `tfe.o` (object file).
- Compile `tfetextview.c` into `tfetextview.o`.
- Compile `resources.c` into `resources.o`.
- Link all the object files into application `tfe`.

Now build tool is necessary to manage it.
Make is one of the build tools.
It was originally created in 1976.
So it is an old and widely used program.

Make analyzes Makefile and executes compilers.
All instructions are written in Makefile.

    sample.o: sample.c
        gcc -o sample.o sample.c

The sample of Malefile above consists of three elements, `sample.o`, `sample.c` and `gcc -0 sample.o sample.c`.

- `sample.o` is called target.
- `sample.c` is prerequisite.
- `gcc -0 sample.o sample.c` is recipe.
Recipes follow tab characters, not spaces.
(It is very important. Use tab not space, or make won't work as you expected).

The rule is:

If a prerequisite modified later than a target, then make executes the recipe.

In the example above, if `sample.c` is modified after the generation of `sample.o`, then make executes gcc and compile `sample.c` into `sample.o`.
If the modification time of `sample.c` is older then the generation of `sample.o`, then no compiling is necesarry, so make does nothing.

The Makefile for `tfe` is as follows.

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

Only you need is to type `make`.

    $ make
    gcc -c -o tfe.o `pkg-config --cflags gtk4` tfe.c
    gcc -c -o tfetextview.o `pkg-config --cflags gtk4` tfetextview.c
    glib-compile-resources tfe.gresource.xml --target=resources.c --generate-source
    gcc -c -o resources.o `pkg-config --cflags gtk4` resources.c
    gcc -o tfe tfe.o tfetextview.o resources.o `pkg-config --libs gtk4`

I used only very basic rules to write this Makefile.
There are many more convenient methods to make it more compact.
But it needs long story to explain.
So I want to finish the explanation about make.

## Rake

Rake is a similar program to make.
It is written in Ruby code.
If you don't use Ruby, you don't need to read this subsection.
However, Ruby is really sophisticated and recommendable script language.

- Rakefile controls the behavior of `rake`.
- You can write any ruby code in Rakefile.

Rake has task and file task, which is similar to target, prerequisite and recipe in make.

     1 require 'rake/clean'
     2 
     3 targetfile = "tfe"
     4 srcfiles = FileList["tfe.c", "tfetextview.c", "resources.c"]
     5 rscfile = srcfiles[2]
     6 objfiles = srcfiles.gsub(/.c$/, '.o')
     7 
     8 CLEAN.include(targetfile, objfiles, rscfile)
     9 
    10 task default: targetfile
    11 
    12 file targetfile => objfiles do |t|
    13   sh "gcc -o #{t.name} #{t.prerequisites.join(' ')} `pkg-config --libs gtk4`"
    14 end
    15 
    16 objfiles.each do |obj|
    17   src = obj.gsub(/.o$/,'.c')
    18   file obj => src do |t|
    19     sh "gcc -c -o #{t.name} `pkg-config --cflags gtk4` #{t.source} "
    20   end
    21 end
    22 
    23 file rscfile => ["tfe.gresource.xml", "tfe.ui"] do |t|
    24   sh "glib-compile-resources #{t.prerequisites[0]} --target=#{t.name} --generate-source"
    25 end

What `Rakefile` describes is almost same as `Makefile` in the previous subsection. 

- 3-6: define target file, source file and so on.
- 1, 8: Load clean library. And define CLEAN file list.
The files included by CLEAN will be removed when `rake clean` is typed on the command line.
- 10: default target depends on targetfile.
default is the final goal of tasks.
- 12-14: targetfile depends on objfiles.
The variable `t` is a task object.
  - t.name is a target name
  - t.prerequisites is an array of prerequisits.
  - t.source is the first element of prerequisites.
- sh is a method to give the following string to shell as an argument and execute.
- 16-21: Loop by each element of the array of objfiles. Each object depends on corresponding source file.
- 23-25: resouce file depends on xml file and ui file.

Rakefile might seem to be difficult for beginners.
But, you can use any ruby syntax in Rakefile, so it is really flexible.
If you practice Ruby and Rakefile, it will be highly productive tools.

## Meson and ninja

Meson is one of the most popular building tool despite the developing version.
And ninja is similar to make but much faster than make.
Several years ago, most of the C developers used autotools and make.
But now the situation has changed.
Many developers are using meson and ninja now.

To use meson, you first need to write `meson.build` file.

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

- 1: The function `project` defines things about the project.
The first parameter is the name of the project and the second is the programing language.
- 2: `dependency` function defines a dependency that is taken by `pkg-config`.
We put `gtk4` as an argument.
- 5: `import` function inports a module.
In line 5, gnome module is imported and assignd to the variable `gnome`.
gnome module provides helper tools to build GTK programs.
- 6: `.compile_resources` is a method of gnome module and compile files to resources under the instruction of xml file.
In line 6, the resource filename is `resources`, which means `resources.c` and `resources.h`, and xml file is `tfe.gresource.xml`.
This method generates C source file by default.
- 8: define source files.
- 10: executable function generates a target file by building source files.
The first parameter is the filename of the target. The following parameters are source files.
The last parameter has a option `dependencies`.
In line 10 it is `gtkdep` which is defined in line 3.

Now run meson and ninja.

    $ meson _build
    $ ninja -C _build

Then, the executable file `tfe` has been generated under the directory `_build`.

    $ _build/tfe tfe.c tfetextview.c

Then the window appears.

I show you three build tools.
I think meson and ninja is the best choice for the present.

We divided a file into some categorized files and used a build tool.
This method is used by many developers.

Up: [Readme.md](Readme.md),  Prev: [Section 7](sec7.md), Next: [Section 9](sec9.md)
