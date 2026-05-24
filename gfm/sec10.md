Up: [README.md](../README.md),  Prev: [Section 9](sec9.md), Next: [Section 11](sec11.md)

# Build System

## Managing Big Source Files

We've compiled a small editor so far.
The program is not complicated yet, but if it grows bigger, it will be difficult to maintain.
So, we should do the following now.

- We've had only one C source file and put everything in it.
We need to divide it into several files.
- There are two compilers, `gcc` and `glib-compile-resources`.
We should control them with one build tool.

## Divide a C Source File into Two Parts.

When you divide C source file into several parts, each file should contain one thing.
For example, our C source has two parts, one is for TfeTextView and the other is for application, including GtkApplication and GtkApplicationWindow.
So, we divide it into:

- `tfetextview.h` and `tfetextview.c`
- `tfeapplication.c`

### Header Files

In `tfeapplication.c`, the function `tfe_text_view_new` is called to create a TfeTextView instance.
However, since this function is defined in `tfetextview.c`, using it in `tfeapplication.c` without a prior declaration will cause a compilation error.
To prevent this, the function must be declared.
In C, such declarations (along with variables and macros) are typically written in header files.
For example, by placing the declaration of `tfe_text_view_new` in `tfetextview.h` and including this header file in both `tfeapplication.c` and `tfetextview.c`, we can successfully avoid compilation errors.

### compilation process

In addition, we have some other files.

- `tfe.ui`: a UI file.
- `tfe.gresource.xml`: a resource description file

Now we face another problem.
The number of build steps has increased.

- Compiling the UI file `tfe.ui` into `resources.c`, using `tfe.gresource.xml` by resource compiler.
- Compiling `tfeapplication.c` into `tfe.o` (object file).
- Compiling `tfetextview.c` into `tfetextview.o`.
- Compiling `resources.c` into `resources.o`.
- Linking all the object files into application `tfe`.

How can we manage the build process above?
It is time to use Build tools to manage the steps.

## Meson and Ninja

Meson and Ninja are one of the most popular build systems.
Many developers, includeing GTK developers, use them.

First, You need to create `meson.build` file.

```
project('tfe', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','tfe.gresource.xml')

sourcefiles=files('tfeapplication.c', 'tfetextview.c')

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
- 10: The executable function generates a target file by compiling the source files.
The first argument is the filename of the target, and the following arguments are the source files.
The last two arguments are provided as "key-value" pairs.
For example, the fourth argument consists of the key `dependencies` and the value `gtkdep`, separated by a colon (:).
This type of argument is called a **keyword argument** (or **kwargs**).
The value `gtkdep` is defined on line 3.
The final argument specifies that this executable will not be installed to a system directory (such as `/usr/local/bin`).
Instead, it is only built and kept within the build directory.

Now, build the project by running Meson and Ninja.
The whole programs are in [src/tfe4](../src/tfe4/) directory.
Change your current directory to `src/tfe4` and type the following on your command line.

    $ meson setup _build
    $ ninja -C _build

Here, the meson command takes two arguments:

- `setup`: The primary command for Meson.
Although it is the default action, explicitly specifying `setup` has been recommended since Meson version 0.64.0.
- `_build`: The name of the build directory.

Once the build is complete, the executable file tfe will be generated inside the `_build` directory.
You can run it using the following command:

    $ _build/tfe tfeapplication.c tfetextview.c

A window with two pages appears.
One is `tfeapplication.c` and the other is `tfetextview.c`.

This application has four buttons but their signal handlers are not implemented yet.
So, you can just display the files.
Even you can edit the files on the screen, the files will not be updated.

For further information, see [The Meson Build system](https://mesonbuild.com/).

Up: [README.md](../README.md),  Prev: [Section 9](sec9.md), Next: [Section 11](sec11.md)
