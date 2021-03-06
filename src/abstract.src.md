This tutorial illustrates how to write C programs with Gtk4 library.
It focuses on beginners so the contents are limited to basic things.
The table of contents are shown at the end of this abstract.

- Section 3 to 20 describes basics and the example is a simple editor `tfe` (Text File Editor).
- Section 21 to 23 describes GtkDrawingArea.
- Section 24 to 27 describes list model and list view (GtkListView, GtkGridView and GtkColumnView).
It also describes GtkExpression.

Please refer to [Gtk reference manual](https://developer.gnome.org/gtk4/stable/index.html)
and [Gnome Developer Center](https://developer.gnome.org/) for further topics.

If you want to know about GObject and type system, refer to [GObject tutorial](https://github.com/ToshioCP/Gobject-tutorial).
It includes the initialization and destruction process of GObject class and instances.
These knowledge is necessary to write gtk4 programs.

This tutorial is under development and unstable.
Even though the codes of the examples have been tested on gtk4 version 4.0, there might exist bugs.
If you find any bugs, errors or mistakes in the tutorial and C examples, please let me know.
You can post it to [github issues](https://github.com/ToshioCP/Gtk4-tutorial/issues).
The latest version of the tutorial is located at [Gtk4-tutorial github repository](https://github.com/ToshioCP/Gtk4-tutorial).
You can read it without download.

If you want to get a html or pdf version, you can make them with `rake`, which is a ruby version of make.
@@@if gfm
There is a documentation \("[How to build Gtk4 Tutorial](Readme_for_developers.src.md)"\) that describes how to make them.
@@@elif html
There is a documentation \("[How to build Gtk4 Tutorial](Readme_for_developers.src.md)"\) that describes how to make them.
@@@elif latex
An appendix "How to build Gtk4 Tutorial" describes how to make them.
@@@end

If you have a question, feel free to post it to the issue.
Any question is helpful to make this tutorial get better.
