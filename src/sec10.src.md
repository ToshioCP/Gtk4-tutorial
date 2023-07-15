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

@@@include
tfe4/meson.build
@@@

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
