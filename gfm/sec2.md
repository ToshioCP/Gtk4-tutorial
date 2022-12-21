Up: [README.md](../README.md),  Prev: [Section 1](sec1.md), Next: [Section 3](sec3.md)

# Installing GTK 4 into Linux distributions

This section describes how to install GTK 4 into Linux distributions.

There are two ways to install GTK 4.

- Install it from the distribution packages.
- Build it from the source files.

## Installation from the distribution packages

The first way is the best and easiest way to install it.
I've installed GTK 4 packages (version 4.8.1) in Ubuntu 22.10.

~~~
$ sudo apt install libgtk-4-dev
~~~

It is important to install the developing package (libgtk-4-dev).
It includes C header files.
Otherwise, you can't compile any GTK 4 based programs.

Fedora, Debian, Arch, Gentoo and OpenSUSE also have GTK 4 packages.
Package information for Arch, Debian/Ubuntu and Fedra is described by [Installing GTK from packages](https://www.gtk.org/docs/installations/linux#installing-gtk-from-packages).
The following table shows the distributions which support GTK 4.

|Distribution|          version          | GTK 4 |  GNOME   |
|:----------:|:-------------------------:|:-----:|:--------:|
|   Fedora   |            37             |4.8.2-2| GNOME 43 |
|   Ubuntu   |           22.10           | 4.8.1 |GNOME 43.0|
|   Debian   |     bookworm(testing)     | 4.8.2 | GNOME 43 |
|    Arch    |      rolling release      | 4.8.2 | GNOME 43 |
|   Gentoo   |      rolling release      | 4.8.2 | GNOME 43 |
|  OpenSUSE  |Tumbleweed(rolling release)| 4.8.2 | GNOME 43 |

## Installation from the source file

If you want to install a developing version of GTK 4, you need to build it from the source.
See [Compiling the GTK Libraries](https://docs.gtk.org/gtk4/building.html) secion in the GTK 4 API reference.

Up: [README.md](../README.md),  Prev: [Section 1](sec1.md), Next: [Section 3](sec3.md)
