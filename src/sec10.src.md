# Build system

## What do we need to think about to manage big source files?

We've compiled a small editor so far.
But Some bad signs are beginning to appear.

- We've had only one C source file and put everything into it.
We need to sort it out.
- There are two compilers, `gcc` and `glib-compile-resources`.
We should control them by one building tool. 

These ideas are useful to manage big source files.

## Divide a C source file into two parts.

When you divide C source file into several parts, each file should contain only one thing.
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

@@@include
tfe4/tfetextview.h
@@@

`tfetextview.c`

@@@include
tfe4/tfetextview.c
@@@

`tfe.c`

@@@include
tfe4/tfe.c
@@@

The ui file `tfe.ui` is the same as `tfe3.ui` in the previous section.

`tfe.gresource.xml`

@@@include
tfe4/tfe.gresource.xml
@@@

## Make

Dividing a file makes it easy to maintain source files.
But now we are faced with a new problem.
The building step increases.

- Compiling the ui file `tfe.ui` into `resources.c`.
- Compiling `tfe.c` into `tfe.o` (object file).
- Compiling `tfetextview.c` into `tfetextview.o`.
- Compiling `resources.c` into `resources.o`.
- Linking all the object files into application `tfe`.

Now build tool is necessary to manage it.
Make is one of the build tools.
It was created in 1976.
It is an old and widely used program.

Make analyzes Makefile and executes compilers.
All instructions are written in Makefile.

~~~makefile
sample.o: sample.c
    gcc -o sample.o sample.c
~~~

The sample of Malefile above consists of three elements, `sample.o`, `sample.c` and `gcc -o sample.o sample.c`.

- `sample.o` is called target.
- `sample.c` is prerequisite.
- `gcc -o sample.o sample.c` is recipe.
Recipes follow tab characters, not spaces.
(It is very important. Use tab not space, or make won't work as you expected).

The rule is:

If a prerequisite modified later than a target, then make executes the recipe.

In the example above, if `sample.c` is modified after the generation of `sample.o`, then make executes gcc and compile `sample.c` into `sample.o`.
If the modification time of `sample.c` is older then the generation of `sample.o`, then no compiling is necessary, so make does nothing.

The Makefile for `tfe` is as follows.

@@@include
tfe4/Makefile
@@@

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
- You can write any Ruby code in Rakefile.

Rake has task and file task, which is similar to target, prerequisite and recipe in make.

@@@include
tfe4/Rakefile
@@@

The contents of the `Rakefile` is almost same as the `Makefile` in the previous subsection. 

- 3-6: Defines target file, source file and so on.
- 1, 8: Loads clean library. And defines CLEAN file list.
The files included by CLEAN will be removed when `rake clean` is typed on the command line.
- 10: The default target depends on `targetfile`.
The task `default` is the final goal of tasks.
- 12-14: `targetfile` depends on `objfiles`.
The variable `t` is a task object.
  - `t.name` is a target name
  - `t.prerequisites` is an array of prerequisites.
  - `t.source` is the first element of prerequisites.
- `sh` is a method to give the following string to shell as an argument and executes the shell.
- 16-21: There's a loop by each element of the array of `objfiles`. Each object depends on corresponding source file.
- 23-25: Resource file depends on xml file and ui file.

Rakefile might seem to be difficult for beginners.
But, you can use any Ruby syntax in Rakefile, so it is really flexible.
If you practice Ruby and Rakefile, it will be highly productive tools.

## Meson and ninja

Meson is one of the most popular building tool despite the developing version.
And ninja is similar to make but much faster than make.
Several years ago, most of the C developers used autotools and make.
But now the situation has changed.
Many developers are using meson and ninja now.

To use meson, you first need to write `meson.build` file.

@@@include
tfe4/meson.build
@@@

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

Then the window appears.
And two notebook pages are in the window.
One notebook is `tfe.c` and the other is `tfetextview.c`.

I've shown you three build tools.
I think meson and ninja is the best choice for the present.

We divided a file into some categorized files and used a build tool.
This method is used by many developers.

