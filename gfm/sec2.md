Up: [Readme.md](../Readme.md),  Prev: [Section 1](sec1.md), Next: [Section 3](sec3.md)

# Installation of gtk4 to linux distributions

This section describes how to install gtk4 into linux distributions.
However, I only have an experience to install it to ubuntu 20.10.
Probably you need more than the explanation below.

This tutorial including this section is without any warranty.
If you install gtk4 to your computer, do it at your own risk.

## Prerequisite

- Ubuntu 20.10. Maybe other versions of late years or other distribution might be OK.
- Packages for development such as gcc, meson, ninja, git, wget and so on.
- Dev packages necessary for each software below.

## Installation target

I installed gtk4 under the directory `$HOME/local`.
This is a private user area.

Don't install it to `/usr/local` which is the default.
It is used by ubuntu applications, which are not build on gtk4.
Therefore, the risk is high and probably bad things will happen.
Actually I did it and I needed to reinstall ubuntu.

## Glib installation

Ubuntu includes glib but its version is not high enough to build gtk4.
Glib 2.66.0 or higher is required.
At present (Jan/2021), its latest version is 2.67.2.
I installed 2.67.1 which was the latest version at that time.
Download glib source files from the repository, then decompress and extract files.

    $ wget https://download.gnome.org/sources/glib/2.67/glib-2.67.1.tar.xz
    $ tar -Jxf glib-2.67.1.tar.xz

Some packages are required to build glib.
You can find them if you run meson.

    $ meson --prefix $HOME/local _build

Use apt-get and install the prerequisites.
For example,

    $ sudo apt-get install -y  libpcre2-dev libffi-dev

After that, compile glib.

    $ rm -rf _build
    $ meson --prefix $HOME/local _build
    $ ninja -C _build
    $ ninja -C _build install

Set sevral environment variables so that the glib libraries installed can be used by build tools.
Make a text file below and save it as `env.sh`

    # compiler
    CPPFLAGS="-I$HOME/local/include"
    LDFLAGS="-L$HOME/local/lib"
    PKG_CONFIG_PATH="$HOME/local/lib/pkgconfig:$HOME/local/lib/x86_64-linux-gnu/pkgconfig"
    export CPPFLAGS LDFLAGS PKG_CONFIG_PATH
    # linker
    LD_LIBRARY_PATH="$HOME/local/lib/x86_64-linux-gnu/"
    PATH="$HOME/local/bin:$PATH"
    export LD_LIBRARY_PATH PATH

Then, use . (dot) or source command to include these commands to the current bash.

    $ . env.sh

or

    $ source env.sh

This command carries out the commands in `env.sh` and changes the environment variables above in the corrent shell.

## Pango installation

Download and untar.

    $ wget https://download.gnome.org/sources/pango/1.48/pango-1.48.0.tar.xz
    $ tar -Jxf pango-1.48.0.tar.xz

Try meson and check the required packages.
Install all the prerequisites.
Then, compile and install pango.

    $ meson --prefix $HOME/local _build
    $ ninja -C _build
    $ ninja -C _build install

It installs Pnago-1.0.gir under `$HOME/local/share/gir-1.0`.
If you installed pango without --prefix option, then it would be located at `/usr/local/share/gir-1.0`.
This directory (/usr/local/share) is used by applications.
They find the directory by the environment variable `XDG_DATA_DIRS`.
It is a text file which keep the list of 'share' directoryes like `/usr/share`, `usr/local/share` and so on.
Now `$HOME/local/share` needs to be added to `XDG_DATA_DIRS`, or error will occur in the later compilation.

    $ export XDG_DATA_DIRS=$HOME/local/share:$XDG_DATA_DIRS

## Gdk-pixbuf and gtk-doc installation

Download and untar.

    $ wget https://download.gnome.org/sources/gdk-pixbuf/2.42/gdk-pixbuf-2.42.2.tar.xz
    $ tar -Jxf gdk-pixbuf-2.42.2.tar.xz
    $ wget https://download.gnome.org/sources/gtk-doc/1.33/gtk-doc-1.33.1.tar.xz
    $ tar -Jxf gtk-doc-1.33.1.tar.xz

Same as before, install prerequisite packages, then compile and install them.

The installation of gtk-doc put `gtk-doc.pc` under `$HOME/local/share/pkgconfig`.
This file is used by pkg-config, which is one of the build tools.
The directory needs to be added to the environment variable `PKG_CONFIG_PATH`

    $ export PKG_CONFIG_PATH="$HOME/local/share/pkgconfig:$PKG_CONFIG_PATH"

## Gtk4 installation

If you want the latest development version of gtk4, use git and clone the repository.

    $ git clone https://gitlab.gnome.org/GNOME/gtk.git

If you want a stable version of gtk4, then download it from [Gnome source website](https://download.gnome.org/sources/gtk/4.0/).

Compile and install it.

    $ meson --prefix $HOME/local _build
    $ ninja -C _build
    $ ninja -C _build install

## Modify env.sh

Because environment variables disappear when you log out, you need to add them again.
Modify `env.sh`.

    # compiler
    CPPFLAGS="-I$HOME/local/include"
    LDFLAGS="-L$HOME/local/lib"
    PKG_CONFIG_PATH="$HOME/local/lib/pkgconfig:$HOME/local/lib/x86_64-linux-gnu/pkgconfig:$HOME/local/share/pkgconfig"
    export CPPFLAGS LDFLAGS PKG_CONFIG_PATH
    # linker
    LD_LIBRARY_PATH="$HOME/local/lib/x86_64-linux-gnu/"
    PATH="$HOME/local/bin:$PATH"
    export LD_LIBRARY_PATH PATH
    # gir
    XDG_DATA_DIRS=$HOME/local/share:$XDG_DATA_DIRS
    export XDG_DATA_DIRS

Include this file by . (dot) command before using gtk4 libraries.

You may think you can add them in your `.profile`.
I think the environment variables above are necessary only when you compile gtk4 applications.
And it's not necessary except the case above and it might cause some bad things.
Therefore, I recommend you not to write them to your `.profile`.

## Compiling gtk4 applications

Before you compile gtk4 applications, define environment variables above.

    $ . env.sh

After that you can compile them without anything.
For example, to compile `sample.c`, type the following.

    $ gcc `pkg-config --cflags gtk4` sample.c `pkg-config --libs gtk4`

To know how to compile gtk4 applications, refer to the following sections.


Up: [Readme.md](../Readme.md),  Prev: [Section 1](sec1.md), Next: [Section 3](sec3.md)
