#### Contents of this Repository

This tutorial illustrates how to write C programs with the Gtk4 library.
It focuses on beginners so the contents are limited to the basics.
The table of contents is at the end of this abstract.

- Section 3 to 21 describes the basics, with the example of a simple editor `tfe` (Text File Editor).
- Section 22 to 24 describes GtkDrawingArea.
- Section 25 to 28 describes the list model and the list view (GtkListView, GtkGridView and GtkColumnView).
It also describes GtkExpression.

The latest version of the tutorial is located at [Gtk4-tutorial github repository](https://github.com/ToshioCP/Gtk4-tutorial).
You can read it from there directly without having to download anything.

#### Gtk4 Documentation

Please refer to [Gtk API Reference](https://docs.gtk.org/gtk4/index.html)
and [Gnome Developer Documentation Website](https://developer.gnome.org/) for further information.

These websites are newly opened lately (Aug/2021).
The old documentation is located at [Gtk Reference Manual](https://developer-old.gnome.org/gtk4/stable/) and [Gnome Developer Center](https://developer-old.gnome.org/).
The new website is in progress at present, so you might need to refer to the old version.

If you want to know about GObject and the type system, please refer to [GObject tutorial](https://github.com/ToshioCP/Gobject-tutorial).
The GObject details are easy to understand and also necessary to know when writing Gtk4 programs.

#### Contribution

This tutorial is under development and unstable.
Even though the codes of the examples have been tested on Gtk4 version 4.0, bugs may still exist.
If you find any bugs, errors or mistakes in the tutorial and C examples, please let me know.
You can post it to [github issues](https://github.com/ToshioCP/Gtk4-tutorial/issues).
You can also post corrected files as a commit to [pull request](https://github.com/ToshioCP/Gtk4-tutorial/pulls).
When you make corrections, correct the source files, which are under the 'src' directory,
then run `rake` to create to create the output file. The GFM files under the 'gfm' directory are automatically updated.

If you have a question, feel free to post it as an issue.
All questions are helpful and will make this tutorial get better.

#### How to get a HTML or PDF version

If you want to get a HTML or PDF version, you can make them with `rake`, which is a ruby version of make.
Type `rake html` for HTML.
Type `rake pdf` for PDF.
@@@if gfm
There is a documentation \("[How to build Gtk4 Tutorial](Readme_for_developers.src.md)"\) that describes how to make them.
@@@elif html
There is a documentation \("[How to build Gtk4 Tutorial](Readme_for_developers.src.md)"\) that describes how to make them.
@@@elif latex
An appendix "How to build Gtk4 Tutorial" describes how to make them.
@@@end
