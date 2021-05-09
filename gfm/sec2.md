Up: [Readme.md](../Readme.md),  Prev: [Section 1](sec1.md), Next: [Section 3](sec3.md)

# Installation of gtk4 to Linux distributions

This section describes how to install gtk4 into Linux distributions.

This tutorial including this section is without any warranty.
If you install gtk4 to your computer, do it at your own risk.

This section is written on 9/May/2021.
"At present" means 9/May/2021 in this section.

There are two possible way to install gtk4.

- Install it from the distribution packages.
- Build it from the source file.

The first way is easy to install.
It is a recommended way.
I've installed gtk4 packages in Ubuntu 21.04.

~~~
$ sudo apt-get install libgtk-4-bin libgtk-4-common libgtk-4-dev libgtk-4-doc
~~~

Arch, Debian and Fedora are also possible.
See [Installing GTK from packages](https://www.gtk.org/docs/installations/linux#installing-gtk-from-packages).
If you've installed gtk4 from the packages, you don't need to read the rest of this section.

The rest of this section includes two parts.

1. Building gtk4 from the source.
2. Installing Gnome 40 distribution.
(However, Gnome 40 is not necessary to build gtk4 applications.)

## Installation from the source files

The first part of this section is about the installation from the source files.
If your operating system has gtk4 packages, installing from the packages is the recommended way.
See the previous subsection.
If your operating system doesn't have gtk4 package, you need to build it from the source.

### Prerequisites for gtk4 installation

- Linux operating system. For example, Ubuntu 20.10 or 20.04LTS.
Maybe other distributions might be OK.
- Packages for development such as gcc, meson, ninja, git, wget and so on.
- Dev packages necessary for each software below.

### Installation target

I installed gtk4 under the directory `$HOME/local`.
This is a private user area.

If you want to install it in the system area, `/opt/gtk4` is one of good choices.
[Gtk4 API Reference](https://developer.gnome.org/gtk4/stable/gtk-building.html) gives an installation example to `/opt/gtk4`.

Don't install it to `/usr/local` which is the default.
It is used by Ubuntu applications, which are not build on gtk4.
Therefore, the risk is high and probably bad things will happen.
Actually I did it and I needed to reinstall Ubuntu.

### Installation to Ubuntu 20.10

Most of the necessary libraries are included by Ubuntu 20.10.
Therefore, they can be installed with `apt-get` command.
You don't need to install them from the source tarballs.
You can skip the subsections below about prerequisite library installation (glib, pango, gdk-pixbuf and gtk-doc).

### Glib installation

If your ubuntu is 20.04LTS, you need to install prerequisite libraries from the tarballs.
Check the version of your library and if it is lower than the necessary version, install it from the source.

For example,

~~~
$ pkg-config --modversion glib-2.0
2.64.6
~~~

The necessary version is 2.66.0 or higher.
Therefore, the example above shows that you need to install glib.

The following part of this subsection is written in January 2021.
The glib version was not enough at that time.
The description below is old now, and it probably doesn't fit to your environment.

I installed 2.67.1 which was the latest version at that time (Jan/2021).
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

Set several environment variables so that the glib libraries installed can be used by build tools.
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
    # gsetting
    export GSETTINGS_SCHEMA_DIR=$HOME/local/share/glib-2.0/schemas

Then, use . (dot) or source command to include these commands to the current bash.

    $ . env.sh

or

    $ source env.sh

This command carries out the commands in `env.sh` and changes the environment variables above in the current shell.

### Pango installation

Download and untar.

    $ wget https://download.gnome.org/sources/pango/1.48/pango-1.48.0.tar.xz
    $ tar -Jxf pango-1.48.0.tar.xz

Try meson and check the required packages.
Install all the prerequisites.
Then, compile and install pango.

    $ meson --prefix $HOME/local _build
    $ ninja -C _build
    $ ninja -C _build install

It installs Pango-1.0.gir under `$HOME/local/share/gir-1.0`.
If you installed pango without `--prefix` option, then it would be located at `/usr/local/share/gir-1.0`.
This directory (/usr/local/share) is used by applications.
They find the directory by the environment variable `XDG_DATA_DIRS`.
It is a text file which keep the list of 'share' directories like `/usr/share`, `usr/local/share` and so on.
Now `$HOME/local/share` needs to be added to `XDG_DATA_DIRS`, or error will occur in the later compilation.

    $ export XDG_DATA_DIRS=$HOME/local/share:$XDG_DATA_DIRS

### Gdk-pixbuf and gtk-doc installation

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

### Gtk4 installation

If you want the latest development version of gtk4, use git and clone the repository.

    $ git clone https://gitlab.gnome.org/GNOME/gtk.git

If you want a stable version of gtk4, then download it from [Gnome source website](https://download.gnome.org/sources/gtk/).
The latest version is 4.3 (7/May/2021).

Compile and install it.

    $ meson --prefix $HOME/local _build
    $ ninja -C _build
    $ ninja -C _build install

If you want to know more information, refer to [Gtk4 reference manual](https://developer.gnome.org/gtk4/stable/gtk-building.html).

### Modify env.sh

Because environment variables disappear when you log out, you need to add them again.
Modify `env.sh`.

    # compiler
    CPPFLAGS="-I$HOME/local/include"
    LDFLAGS="-L$HOME/local/lib"
    PKG_CONFIG_PATH="$HOME/local/lib/pkgconfig:$HOME/local/lib/x86_64-linux-gnu/pkgconfig:
    $HOME/local/share/pkgconfig"
    export CPPFLAGS LDFLAGS PKG_CONFIG_PATH
    # linker
    LD_LIBRARY_PATH="$HOME/local/lib/x86_64-linux-gnu/"
    PATH="$HOME/local/bin:$PATH"
    export LD_LIBRARY_PATH PATH
    # gir
    XDG_DATA_DIRS=$HOME/local/share:$XDG_DATA_DIRS
    export XDG_DATA_DIRS
    # gsetting
    export GSETTINGS_SCHEMA_DIR=$HOME/local/share/glib-2.0/schemas
    # girepository-1.0
    export GI_TYPELIB_PATH=$HOME/local/lib/x86_64-linux-gnu/girepository-1.0

Include this file by . (dot) command before using gtk4 libraries.

You may think you can add them in your `.profile`.
But it's a wrong decision.
Never write them to your `.profile`.
The environment variables above are necessary only when you compile and run gtk4 applications.
Otherwise it's not necessary.
If you changed the environment variables above and run gtk3 applications, it probably causes serious damage.

### Compiling gtk4 applications

Before you compile gtk4 applications, define environment variables above.

    $ . env.sh

After that you can compile them without anything.
For example, to compile `sample.c`, type the following.

    $ gcc `pkg-config --cflags gtk4` sample.c `pkg-config --libs gtk4`

To know how to compile gtk4 applications, refer to the section 3 (GtkApplication and GtkApplicationWindow) and after.

## Installing Fedora 34 Beta with gnome-boxes

The second part of this section is about Gnome40.
Gnome 40 is a new version of Gnome desktop system.
And gtk4 is installed in the distribution.
See [Gnome 40 website](https://forty.gnome.org/) first.

*However, Gnome40 is not necessary to compile and run Gtk4 applications.*

There are only three choices at present.

  - Gnome OS
  - Fedora 34 Beta
  - openSUSE

I've tried installing Fedora 34 Beta with gnome-boxes.

There are two ways to install them.

- Install them to a hard disk.
This is a usual installation.
- Install them to a virtual machine.

I've chosen the second way.
I've tried installing Fedora 34 Beta with gnome-boxes.
My OS was Ubuntu 21.04 at that time.
Gnome-boxes creates a virtual machine in Ubuntu and Fedora will be installed to that virtual machine.

The instruction is as follows.

1. Download Fedora 34 Beta iso file.
There is an link at the end of [Gnome 40 website](https://forty.gnome.org/).
2. Install gnome-boxes with apt-get command.
~~~
$ sudo apt-get install gnome-boxes
~~~
3. Run gnome-boxes.
4. Click on `+` button on the top left corner and launch a box creation wizard by clicking `Create a Virtual Machine ...`.
Then a dialog appears.
Click on `Operationg System Image File` and select the iso file you have downloaded.
5. Then, the installer of Fedora is executed.
Follow the instructions by the installer.
At the end of installation, the installer instructs to reboot the system.
Click the right of the title bar and select reboot or shutdown.
6. Your display is back to the initial window of gnome-boxes, but there is a button `Fedora 34 Workstation` on the upper left of the window.
Click on the button then Fedora will be executed.
7. A setup dialog appears.
Setup Fedora according to the wizard.

Now you can use Fedora.
It includes gtk4 libraries already.
But you need to install the gtk4 development package.
Use `dnf` to install `gtk4.x86_64` package.

~~~
$ sudo dnf install gtk4.x86_64
~~~

### Test for compiling a gtk4 application

You can test the gtk4 development package by compiling files which are based on gtk4.
I've tried compiling `tfe` text editor, which is written in section 20.

1. Run Firefox.
2. Open this website \([Gtk4-Tutorial](https://github.com/ToshioCP/Gtk4-tutorial)\).
3. Click on the green button labeled `Code`.
4. Select `Download ZIP` and download the codes from the repository.
5. Unzip the file.
6. Change your current directory to the top directory of the unzipped source files.
7. Compile it.
~~~
$ meson _build
bash: meson: command not found...
Install package 'meson' to provide command 'meson'? [N/y] y

 * Waiting in queue...
The following packages have to be installed:
 meson-0.56.2-2.fc34.noarch    High productivity build system
 ninja-build-1.10.2-2.fc34.x86_64    Small build system with a focus on speed
 vim-filesystem-2:8.2.2787-1.fc34.noarch    VIM filesystem layout
Proceed with changes? [N/y] y

... ...
... ...

The Meson build system
Version: 0.56.2

... ...
... ...

Project name: tfe
Project version: undefined
C compiler for the host machine: cc (gcc 11.0.0 "cc (GCC) 11.0.0 20210210 (Red Hat 11.0.0-0)")
C linker for the host machine: cc ld.bfd 2.35.1-38
Host machine cpu family: x86_64
Host machine cpu: x86_64
Found pkg-config: /usr/bin/pkg-config (1.7.3)
Run-time dependency gtk4 found: YES 4.2.0
Found pkg-config: /usr/bin/pkg-config (1.7.3)
Program glib-compile-resources found: YES (/usr/bin/glib-compile-resources)
Program glib-compile-schemas found: YES (/usr/bin/glib-compile-schemas)
Program glib-compile-schemas found: YES (/usr/bin/glib-compile-schemas)
Build targets in project: 4
 
Found ninja-1.10.2 at /usr/bin/ninja
 
$ ninja -C _build
ninja: Entering directory `_build'
[12/12] Linking target tfe

$ ninja -C _build install
ninja: Entering directory `_build'
[0/1] Installing files.
Installing tfe to /usr/local/bin
Installation failed due to insufficient permissions.
Attempting to use polkit to gain elevated privileges...
Installing tfe to /usr/local/bin
Installing /home/<username>/Gtk4-tutorial-main/src/tfe7/com.github.ToshioCP.tfe.gschema.xml to /usr/local/share/glib-2.0/schemas
Running custom install script '/usr/bin/glib-compile-schemas /usr/local/share/glib-2.0/schemas/'
~~~
8. Execute it.
~~~
$ tfe
~~~

Then, the window of `tfe` text editor appears.
The compilation and execution have succeeded.


Up: [Readme.md](../Readme.md),  Prev: [Section 1](sec1.md), Next: [Section 3](sec3.md)
