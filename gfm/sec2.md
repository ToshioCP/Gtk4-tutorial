Up: [README.md](../README.md),  Prev: [Section 1](sec1.md), Next: [Section 3](sec3.md)

# Preparation

## Installing GTK 4 into Linux distributions

This section describes how to install GTK 4 into Linux distributions.

There are two ways to install GTK 4.

- Install it from the distribution packages.
- Build it from the source files.

### Installation from the distribution packages

This is the easiest way to install.
I've installed GTK 4 packages (version 4.14.2) on Ubuntu 24.04 LTS.

~~~
$ sudo apt install libgtk-4-dev
~~~

It is important to install the development files package (libgtk-4-dev).
Otherwise, you can't compile any GTK 4 based programs.

Fedora, Debian, Arch, Gentoo and OpenSUSE also have GTK 4 packages.
See the website of your distribution for further information.

Package information for Arch, Debian/Ubuntu and Fedora is described in GTK website, [Installing GTK from packages](https://www.gtk.org/docs/installations/linux#installing-gtk-from-packages).

### Installation from the source file

If you want to install a developing version of GTK 4, you need to build it from the source.
See [Compiling the GTK Libraries](https://docs.gtk.org/gtk4/building.html) section in the GTK 4 API reference.

## How to download this repository

There are two ways: zip and git.
Downloading a zip file is the easiest way.
However, if you use git and clone this repository, you can easily update your local repository by `git pull` command.

### Download a zip file

- Run your browser and open [this repository](https://github.com/ToshioCP/Gtk4-tutorial).
- Click on the green button with `<> Code`. Then a popup menu appears. Click on `Download ZIP` menu.
- Then the repository data is downloaded as a zip file into your download folder.
- Unzip the file.

### Clone the repository

- Click on the green button with the label `<> Code`. Then a popup menu appears. The first section is `Clone` with three tabs.
Click `HTTPS` tab and click on the copy icon, which is on the right of `https://github.com/ToshioCP/Gtk4-tutorial.git`.
- Run your terminal and type `git clone `, Ctrl+Shift+V.
Then the line will be `git clone https://github.com/ToshioCP/Gtk4-tutorial.git`.
Press the enter key.
- A directory `Gtk4-tutorial` is created. It is the copy of this repository.

## Examples in the tutorial

Examples are under the `src` directory.
For example, the first example of the tutorial is `pr1.c` and its pathname is `src/misc/pr1.c`.
So you don't need to type the example codes by yourself.

Up: [README.md](../README.md),  Prev: [Section 1](sec1.md), Next: [Section 3](sec3.md)
