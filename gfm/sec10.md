Up: [README.md](../README.md),  Prev: [Section 9](sec9.md), Next: [Section 11](sec11.md)

# Build System

## Managing Big Source Files

We've compiled a small editor so far.
The program is not complicated yet, but if it grows bigger, it will be difficult to maintain.
So, we should do the following now.

- We've had only one C source file and put everything in it.
We need to divide and organize it.
- There are two compilers, `gcc` and `glib-compile-resources`.
We should control them with one build tool.

## Divide a C Source File into Two Parts.

When you divide C source file into several parts, each file should contain one thing.
For example, our source has two things, the definition of TfeTextView and functions related to GtkApplication and GtkApplicationWindow.
It is a good idea to separate them into two files, `tfetextview.c` and `tfe.c`.

- `tfetextview.c` includes the definition and functions of TfeTextView.
- `tfe.c` includes functions like `main`, `app_activate`, `app_open` and so on, which relate to GtkApplication and GtkApplicationWindow.

Now we have three source files, `tfetextview.c`, `tfe.c` and `tfe3.ui`.
The `3` of `tfe3.ui` is like a version number.
Managing version with filenames is one possible idea but it also has a problem.
You need to rename files in each version, and this affects the contents of source files that refer to those filenames.
So, we should take `3` away from the filename.

In `tfe.c` the function `tfe_text_view_new` is invoked to create a TfeTextView instance.
But it is defined in `tfetextview.c`, not `tfe.c`.
The absence of a declaration (not definition) of `tfe_text_view_new` will cause a compilation error when `tfe.c` is compiled.
The declaration is necessary in `tfe.c`.
That public information is usually written in header files.
These have a `.h` suffix like `tfetextview.h`.
The header files are included by C source files.
For example, `tfetextview.h` is included by `tfe.c`.

The source files are shown below.

`tfetextview.h`

```c
#include <gtk/gtk.h>

#define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type ()
G_DECLARE_FINAL_TYPE (TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView)

void
tfe_text_view_set_file (TfeTextView *tv, GFile *f);

GFile *
tfe_text_view_get_file (TfeTextView *tv);

GtkWidget *
tfe_text_view_new (void);

```

`tfetextview.c`

```c
#include <gtk/gtk.h>
#include "tfetextview.h"

struct _TfeTextView
{
  GtkTextView parent;
  GFile *file;
};

G_DEFINE_TYPE (TfeTextView, tfe_text_view, GTK_TYPE_TEXT_VIEW);

static void
tfe_text_view_init (TfeTextView *tv) {
}

static void
tfe_text_view_class_init (TfeTextViewClass *class) {
}

void
tfe_text_view_set_file (TfeTextView *tv, GFile *f) {
  tv->file = f;
}

GFile *
tfe_text_view_get_file (TfeTextView *tv) {
  return tv->file;
}

GtkWidget *
tfe_text_view_new (void) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_TEXT_VIEW, NULL));
}

```

`tfe.c`

```c
#include <gtk/gtk.h>
#include "tfetextview.h"

static void
app_activate (GApplication *app) {
  g_print ("You need a filename argument.\n");
}

static void
app_open (GApplication *app, GFile ** files, gint n_files, gchar *hint) {
  GtkWidget *win;
  GtkWidget *nb;
  GtkWidget *lab;
  GtkNotebookPage *nbp;
  GtkWidget *scr;
  GtkWidget *tv;
  GtkTextBuffer *tb;
  char *contents;
  gsize length;
  char *filename;
  int i;
  GError *err = NULL;
  GtkBuilder *build;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/tfe.ui");
  win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
  nb = GTK_WIDGET (gtk_builder_get_object (build, "nb"));
  g_object_unref (build);
  for (i = 0; i < n_files; i++) {
    if (g_file_load_contents (files[i], NULL, &contents, &length, NULL, &err)) {
      scr = gtk_scrolled_window_new ();
      tv = tfe_text_view_new ();
      tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
      gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
      gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);

      tfe_text_view_set_file (TFE_TEXT_VIEW (tv),  g_file_dup (files[i]));
      gtk_text_buffer_set_text (tb, contents, length);
      g_free (contents);
      filename = g_file_get_basename (files[i]);
      lab = gtk_label_new (filename);
      gtk_notebook_append_page (GTK_NOTEBOOK (nb), scr, lab);
      nbp = gtk_notebook_get_page (GTK_NOTEBOOK (nb), scr);
      g_object_set (nbp, "tab-expand", TRUE, NULL);
      g_free (filename);
    } else {
      g_printerr ("%s.\n", err->message);
      g_clear_error (&err);
    }
  }
  if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) > 0) {
    gtk_window_present (GTK_WINDOW (win));
  } else
    gtk_window_destroy (GTK_WINDOW (win));
}

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new ("com.github.ToshioCP.tfe", G_APPLICATION_HANDLES_OPEN);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

```

The UI file `tfe.ui` is the same as `tfe3.ui` in the previous section.

`tfe.gresource.xml`

```xml
<?xml version="1.0" encoding="UTF-8"?>
<gresources>
  <gresource prefix="/com/github/ToshioCP/tfe">
    <file>tfe.ui</file>
  </gresource>
</gresources>
```

Splitting the source code into multiple files makes maintenance easier.
But now we face a new problem.
The number of build steps has increased.

- Compiling the UI file `tfe.ui` into `resources.c`, using `tfe.gresource.xml`.
- Compiling `tfe.c` into `tfe.o` (object file).
- Compiling `tfetextview.c` into `tfetextview.o`.
- Compiling `resources.c` into `resources.o`.
- Linking all the object files into application `tfe`.

Build tools manage the steps.

## Meson and Ninja

I will explain the Meson and Ninja build systems.

Other possible tools are Make and Autotools.
They are traditional tools but slower than Ninja.
Many developers have recently migrated to Meson and Ninja.
For example, GTK 4 uses them.

You need to create `meson.build` file first.

```
project('tfe', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','tfe.gresource.xml')

sourcefiles=files('tfe.c', 'tfetextview.c')

executable('tfe', sourcefiles, resources, dependencies: gtkdep, install: false)
```

- 1: The function `project` defines things about the project.
The first argument is the name of the project and the second is the programming language.
- 3: The function `dependency` defines a dependency that is taken by `pkg-config`.
We put `gtk4` as an argument.
- 5: The function `import` imports a module.
In line 5, the gnome module is imported and assigned to the variable `gnome`.
The gnome module provides helper tools to build GTK programs.
- 6: The method `.compile_resources` is from the gnome module.
It compiles files to resources, as instructed by the xml file.
In line 6, the resource filename is `resources`, which means `resources.c` and `resources.h`, and the xml file is `tfe.gresource.xml`.
This method generates C source file by default.
- 8: Defines source files.
- 10: The `executable` function generates a target file by compiling source files.
The first argument is the filename of the target. The following arguments are source files.
The last two arguments have keys and values.
For example, the fourth argument has a key `dependencies` , a delimiter (`:`) and a value `gtkdep`.
This type of parameter is called *keyword parameter* or *kwargs*.
The value `gtkdep` is defined in line 3.
The last argument tells Meson and Ninja that this project shouldn't install the executable file.
it is only built within the build directory.

Now run meson and ninja.

    $ meson setup _build
    $ ninja -C _build

The meson command takes two arguments.

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
