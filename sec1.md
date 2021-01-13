Up: [Readme.md](Readme.md),  Next: [Section 2](sec2.md)

# Prerequisite and Licence

## Prerequisite

### Tutorial document

This tutorial is about gtk4 libraries.
It is originally used on linux with C compiler, but now it is used more widely, on windows and macOS, with Vala, python and so on.
However, this tutorial describes only _C programs on linux_.

If you want to try the examples in the tutorial, you need:

- PC with linux distribution like ubuntu, debian and so on.
- Gcc
- Gtk4. Gtk included linux distributions is version three at present.
You need to install gtk4 to your computer.
Refer to [gtk4 gitlab repository](https://gitlab.gnome.org/GNOME/gtk).
However, it might make some trouble like, for example, your pc doesn't recognize usb port
if you install them to `/usr/local`.
Therefore, I strongly recommend you not to install gtk4 to `/usr/local` on the computer you usually use.
Instead,

- Install it to another computer only used to try gtk4.
- Install it to your home directory, for example `$HOME/local`, in order to separte gtk4 from your system.

The second choice will be explained in [Section 3](sec3.md).

### Software

This repository inclludes ruby programs.
They are used to generate markdown files, html files, latex files and a pdf file.

You need:

- Linux distribution like ubuntu.
- Ruby programing language.
There are two ways to install it.
One is install the distribution's package.
The other is using rbenv and ruby-build.
If you want to use the latest version of ruby, use rbenv.
- Rake.
It is a gem, which is a library written in ruby.
You can install it as a package of your ditribution or use gem command.

## Licence

Copyright (C) 2020  ToshioCP (Toshio Sekiya)

Gtk4 tutorial repository containes the tutorial document and softwares such as converters, generators and controlers.
All of them make up the 'Gtk4 tutorial' package.
This package is simply called 'Gtk4 tutorial' in the following description.
'Gtk4 tutorial' is free; you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

'Gtk4 tutorial' is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.html) for more details.


Up: [Readme.md](Readme.md),  Next: [Section 2](sec2.md)
