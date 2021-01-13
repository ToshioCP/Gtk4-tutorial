# Build system

## What do we need to think about to manage big source files?

We've managed to compile a small editor so far.
But Some bad signs are beginning to appear.

- We have only one C source file and put everything into it.
We need to sort it out.
- There are two compilers, `gcc` and `glib-compile-resources`.
We want to control them by one building tool. 

These ideas are useful to manage big source files.

## Divide a C source file into two parts.

When you divide C source file into several parts, each file should contain only one thing.
For example, our source has two things, the definition of TfeTextView and functions related to GtkApplication and GtkApplicationWindow.
It is a good idea to separate them into two files, `tfetextview.c` and `tfe.c`.

- `tfetextview.c` includes the definition and functions of TfeTextView.
- `tfe.c` includes functions like `main`, `on_activate`, `on_open` and so on, which relate to GtkApplication and GtkApplicationWindow

Now we have three source files, `tfetextview.c`, `tfe.c` and `tfe3.ui`.
The `3` of `tfe3.ui` is like a version number.
Managing version with filenames is one possible idea but it may make bothersome problem.
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

All the source files are listed below.

`tfetextview.h`

@@@ tfe4/tfetextview.h

`tfetextview.c`

@@@ tfe4/tfetextview.c

`tfe.c`

@@@ tfe4/tfe.c

`tfe.ui`

@@@ tfe4/tfe.ui

`tfe.gresource.xml`

@@@ tfe4/tfe.gresource.xml

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
- `gcc -o sample.o sample.c` is recipe.
Recipes follow tab characters, not spaces.
(It is very important. Use tab not space, or make won't work as you expected).

The rule is:

If a prerequisite modified later than a target, then make executes the recipe.

In the example above, if `sample.c` is modified after the generation of `sample.o`, then make executes gcc and compile `sample.c` into `sample.o`.
If the modification time of `sample.c` is older then the generation of `sample.o`, then no compiling is necesarry, so make does nothing.

The Makefile for `tfe` is as follows.

@@@ tfe4/Makefile

You only need to type `make`.

    $ make
    gcc -c -o tfe.o `pkg-config --cflags gtk4` tfe.c
    gcc -c -o tfetextview.o `pkg-config --cflags gtk4` tfetextview.c
    glib-compile-resources tfe.gresource.xml --target=resources.c --generate-source
    gcc -c -o resources.o `pkg-config --cflags gtk4` resources.c
    gcc -o tfe tfe.o tfetextview.o resources.o `pkg-config --libs gtk4`

I used only very basic rules to write this Makefile.
There are many more convenient methods to make it more compact.
But it will be long to explain it.
So I want to finish explaining make and move on to the next topic.

## Rake

Rake is a similar program to make.
It is written in Ruby code.
If you don't use Ruby, you don't need to read this subsection.
However, Ruby is really sophisticated and recommendable script language.

- Rakefile controls the behavior of `rake`.
- You can write any ruby code in Rakefile.

Rake has task and file task, which is similar to target, prerequisite and recipe in make.

@@@ tfe4/Rakefile

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
- sh is a method to give the following string to shell as an argument and execute the shell.
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

@@@ tfe4/meson.build

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

Then, the executable file `tfe` is generated under the directory `_build`.

    $ _build/tfe tfe.c tfetextview.c

Then the window appears.

I've shown you three build tools.
I think meson and ninja is the best choice for the present.

We divided a file into some categorized files and used a build tool.
This method is used by many developers.

