# Preparation

## Installing GTK on Linux distributions

This section describes how to install GTK on Linux distributions.

There are two ways to install GTK:

- Install from distribution packages.
- Build from source.

### Installation from distribution packages

This is the easiest way to install.
The following command installs the GTK (version 4) development files on Ubuntu.

~~~
$ sudo apt install libgtk-4-dev
~~~

It is important to install the development package (libgtk-4-dev).
Otherwise, you will not be able to compile any GTK-based programs.

Fedora, Debian, Arch, Gentoo, and OpenSUSE also have GTK packages.
See the website of your distribution for further information.

Package information for Arch, Debian/Ubuntu, and Fedora is described on the GTK website: [Installing GTK from packages](https://www.gtk.org/docs/installations/linux#installing-gtk-from-packages).

### Installation from source

If you want to install the development version of GTK, you need to build it from source.
See the [Compiling the GTK Libraries](https://docs.gtk.org/gtk4/building.html) section of the GTK API reference.

## How to download this repository

There are two ways: zip and git.
Downloading a zip file is the easiest way.
However, if you use git and clone this repository, you can easily update your local repository with the `git pull` command.

### Download a zip file

- Open your browser and navigate to [this repository](https://github.com/ToshioCP/Gtk4-tutorial).
- Click on the green `<> Code` button. A dropdown menu will appear; click `Download ZIP`.
- The repository will be downloaded as a zip file to your computer.
- Unzip the file.

### Clone the repository

- Click on the green `<> Code` button. A dropdown menu will appear. The first section is `Clone` with three tabs.
Click the `HTTPS` tab and click the copy icon to the right of `https://github.com/ToshioCP/Gtk4-tutorial.git`.
- Open a terminal and type `git clone `, then press Ctrl+Shift+V.
The command line will look like `git clone https://github.com/ToshioCP/Gtk4-tutorial.git`.
Press Enter.
- The directory `Gtk4-tutorial` will be created. This directory is a local copy of the repository.

## Examples in the tutorial

Examples are under the `src` directory.
For example, the first example of the tutorial is `pr1.c` and its path is `src/misc/pr1.c`.
Therefore, you do not need to type the examples manually.
