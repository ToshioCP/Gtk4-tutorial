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

@@@include
tfe4/Makefile
@@@

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

@@@include
tfe4/Rakefile
@@@

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
