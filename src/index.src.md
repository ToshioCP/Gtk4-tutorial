@@@if pdf
# GTK 4 Tutorial for beginners {-}
@@@else
# GTK 4 Tutorial for beginners
@@@end

@@@if gfm
The GitHub Pages site of this tutorial is available.
Click [here](https://toshiocp.github.io/Gtk4-tutorial/).
It is more readable than this repository.

## Contents of This Repository
@@@end

Now that GTK 4 is widely used, "GTK" usually means GTK 4.
Therefore, this tutorial simply says "GTK" except where the version needs to be explicitly mentioned.

This tutorial illustrates how to write C programs with the GTK library.
It is aimed at beginners, so the contents are limited to the basics.
The table of contents is at the end of this README.md.

- Section 3 to 23 describe the basics, with the example of a simple editor `tfe` (Text File Editor).
- Section 24 to 27 describe GtkDrawingArea.
- Section 28 describes Drag and Drop.
- Section 29 to 33 describe the list model and the list view (GtkListView, GtkGridView, and GtkColumnView). It also describes GtkExpression.

The latest version of the tutorial is located at the [GTK4-tutorial GitHub repository](https://github.com/ToshioCP/Gtk4-tutorial).
You can read it directly without downloading.

There's a [GitHub Page](https://toshiocp.github.io/Gtk4-tutorial/) which is the HTML version of the tutorial.

@@@if pdf
## GTK Documentation {-}
@@@else
## GTK Documentation
@@@end

Please refer to the [GTK API Reference](https://docs.gtk.org/gtk4/index.html)
and the [GNOME Developer Documentation Website](https://developer.gnome.org/) for further information.
These websites were opened in August 2021.

If you want to know about GObject and the type system, please refer to the [GObject tutorial](https://github.com/ToshioCP/Gobject-tutorial).
GObject is the base of GTK, so it is important for developers to understand it as well as GTK.

@@@if pdf
## Contribution {-}
@@@else
## Contribution
@@@end

This tutorial is still under development and unstable.
Even though the example codes have been tested on GTK (version 4.10.1), bugs may still exist.
If you find any bugs, errors, or mistakes in the tutorial and C examples, please let me know.
You can post them to [GitHub issues](https://github.com/ToshioCP/Gtk4-tutorial/issues).
You can also post updated files to [pull request](https://github.com/ToshioCP/Gtk4-tutorial/pulls).
When submitting a pull request, please ensure that you only modify the files in the `src` directory.
Don't modify the files under `gfm` or `html` directories.
After modifying some source files, run `bundle exec rake` to create GFM (GitHub Flavoured Markdown) files and run `bundle exec rake html` to create HTML files.
@@@if gfm|html
Please refer to the [Author Guide](author_guide.src.md) for further information.
@@@elif pdf
Please refer to the [Author Guide](#author-guide) for further information.
@@@end

If you have a question, feel free to post it to `issue` of this repository.
Your questions and feedback are welcome and invaluable for improving this tutorial.

@@@if pdf
## How to Get Gtk 4 Tutorial in HTML or PDF Format {-}
@@@else
## How to Get Gtk 4 Tutorial in HTML or PDF Format
@@@end

If you want to get the tutorial in HTML or PDF format, build them with `rake` command, which is a ruby version of make.
Type `bundle exec rake html` for HTML.
Type `bundle exec rake pdf` for PDF.
@@@if gfm|html
The [Author Guide](author_guide.src.md) describes how to write source files.
@@@elif pdf
An appendix [Author Guide](#author-guide) describes how to write source files.
@@@end

@@@if pdf
## License {-}
@@@else
## License
@@@end

@@@if gfm|html
The license of this repository is written in [Section1](sec1.src.md).
@@@elif pdf
The license of this repository is written in [Section1](#prerequisite-and-license).
@@@end

In short:

- GFDL1.3 for documents
- GPL3 for programs
