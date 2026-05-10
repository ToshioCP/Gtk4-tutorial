# GtkBuilder and UI Files

We developed a very simple editor in the previous section.
While it functions by reading and writing files at the start and end of the program, there is room for improvement.
It would be more user-friendly to have "New", "Open", "Save", and "Close" buttons.
This section describes how to incorporate these buttons into the window.
We will focus on building the widgets and omit the file I/O functions for now.

## New, Open, Save and Close Buttons

@@@if pdf
![Screenshot of the file editor](/images/screenshot_tfe2.png){width=9.3cm height=6.825cm}
@@@else
![Screenshot of the file editor](/images/screenshot_tfe2.png)
@@@end

The screenshot above shows the layout.
The source code `tfe2.c` is as follows.

@@@include
tfe/tfe2.c
@@@

This program just builds the widgets.
Other features will be implemented in the later sactions.

The function `app_activate` builds the widgets in the main application window.

- 16-18: Creates a GtkApplicationWindow instance and sets the title and default size.
- 20-21: Creates a vertical GtkBox instance `boxv`as a child of GtkApplicationWindow.
- 23-24: Creates a horizontal GtkBox instance `boxh` and appends it to `boxv` as the first child.
- 26-31: Creates three dummy labels.
The `dmy1` and `dmy3` labels are ten characters wide, while the `dmy2` label has its `hexpand` property set to `TRUE` to fill the available space.
- 32-35: Creates four buttons `New`, `Open`, `Save`, and `Close`.
- 37-43: Appends these label and button instances to `boxh`.
- 45-48: Creates a GtkNotebook instance with `hexpand` and `vexpand` properties set to `TRUE`, allowing it to expand in both directions.
It is then appended to `boxv` as the second child.
- 50-59: Creates, GtkScrolledWindow, GtkTextView, and GtkLabel instances in the for-loop.
The scrolled window `scr` takes `tv` as a child.
The function `gtk_notebook_append_page` appends `scr` as a page and `label` as a page tab to the note book.
The function `gtk_notebook_get_page` retrieves the GtkNotebookPage instance that corresponds to `scr`, a child widget of `nb`.
GtkNotebookPage class does not have a function to set the property "tab-expand", so the general property setting function `g_object_set` is used here.
The property is set to TRUE to expand the tab horizontally.

Building widgets takes 44 lines (= 59-16+1).
We also needed many variables (`boxv`, `boxh`, `dmy1`, ...), and most of them are used only for building the widgets.
Is there a better way to reduce this overhead?

Gtk provides GtkBuilder.
It reads user interface (UI) data and builds a window.
It simplifies this tedious process.

## UI Files

Look at the UI file `tfe3.ui` that defines the widget structure.

@@@include
tfe/tfe3.ui
@@@

This is an XML file.
The extension "ui" means User Interface.

Tags are enclosed in angle brackets, beginning with `<` and ending with `>`.
They generally come in pairs: a start tag and an end tag.
For instance, `<interface>` opens the element, while `</interface>` closes it.

The end tag is identified by a forward slash immediately following the opening bracket, and its name must match the corresponding start tag.
Additionally, certain tags (such as object tags) can include class and id attributes within their start tag to provide extra metadata.

- 1: XML declaration.
This is a special tag and doesn't follow the "start and end tag" rule.
It specifies that the XML version is 1.0 and the encoding is UTF-8.
- 2, 89: The UI file begins and ends with interface tags.
- 3: The object tag defines an instance with the class of `GtkApplicationWindow` and the id of `win`.
The class attribute specifies the type of the object to create, while the id attribute provides a unique name used to retrieve the object within your C code.
- 4-6: Property tags define the values of the properties on the object that is defined with an object tag just before the property definitions.
These three lines define the properties of the top-level window `win`.
  - title: "file editor"
  - default-width: 600
  - default-height: 400
- 7: Child tag means that the following object is a child widget.
For example, line 7 defines the GtkBox as a child widget of the GtkApplicationWindow `win`.
- 8-49: Defines a horizontal box comprising labels and buttons.
- 50-85: Defines the GtkNotebook as a child of the vertical box.
It contains two pairs of children, where each pair consists of a content area (a scrolled window) and a tab (a label).
While a standard `<child>` tag defines the content, a `<child>` tag with `type="tab"` defines the tab itself.
Note that GtkNotebookPage cannot be defined directly in UI files because it is not a widget; it is an auxiliary object derived directly from GObject.

Compare this UI file with the `app_activate` function of `tfe2.c`.
While both methods produce the same window and widget hierarchy, the UI file approach offers superior readability and easier maintenance.

### Gtk4-Builder-Tool

You can verify and optimize your UI files using the `gtk4-builder-tool` utility.

- **Validation:** `gtk4-builder-tool validate <ui_file_name>` checks the file for errors.
If the UI file contains syntax errors or invalid object definitions, the tool will point them out.
- **Simplification:** `gtk4-builder-tool simplify <ui_file_name>` cleans up the UI file and prints the result to the terminal.
  - The `--replace` option: Using this option replaces the original file with the simplified version.
  - Removing Defaults: If a property is set to its default value, the tool removes it to save space.
  For example, since the default orientation for GtkBox widgets is horizontal, that line would be deleted (ex. line 12 in `tfe3.ui`).
  - Value Conversion: Some values are shortened; for instance, "TRUE" and "FALSE" are converted to "1" and "0".

Example:

@@@shell
gtk-builder-tool validate tfe/tfe3.ui
@@@

Since `tfe3.ui` contains no errors, `gtk-builder-tool` produced no output.

**A Note on Maintenance**

While the simplify tool makes the file smaller, it can sometimes make it harder to read.
For example, it converts "TRUE" and "FALSe" into "1" and "0" respectively.
Keeping "TRUE" or "FALSE" is better for maintenance because it is more readable for humans than "1" or "0".

It is always a good practice to validate your UI file before deploying your application.

## GtkBuilder

The file [tfe3.c](tfe/tfe3.c) builds the same widget as `tfe2.c` using `tfe3.ui`.
It is far shorter than `tfe2.c` (39 lines against 73 lines).

@@@shell
wc tfe/tfe2.c tfe/tfe3.c
@@@

The following is the file `tfe3.c`

@@@include
tfe/tfe3.c
@@@

GtkBuilder builds widgets based on a UI file.

- 12: The function `gtk_builder_new_from_file` reads the file `tfe3.ui`.
Then, it builds the widgets and creates a GtkBuilder object.
All the widgets are connected based on the parent-children relationship described in the UI file.
- 13, 14: We can retrieve objects from the builder object using the `gtk_builder_get_object` function.
The top level window, which has an id of "win" in the UI file, is taken and assigned to the variable `win`.
Similarly, The GtkNotbook instance with the id "nb" is assignd to the variable `nb`.
- 15: The function `gtk_window_set_application` adds `win` to the application's list of managed windows.
You can use `gtk_application_add_window` instead of this function.
The result is identical.
- 16: After the window and the application are connected, we no longer need the GtkBuilder instance.
It is released with the `g_object_unref` function.
18-23: Sets the "tab-expand" properties of the GtkNotebookPage instances to TRUE.
Since GtkNotebookPage is not a widget, its properties cannot be set in the UI file; you must set them in your C source code.

### Using UI String

GtkBuilder can build widgets with a string.
Use `gtk_builder_new_from_string` instead of `gtk_builder_new_from_file`.

@@@if gfm
```C
@@@else
```{.c}
@@@end
char *uistring;

uistring =
"<interface>"
  "<object class=\"GtkApplicationWindow\" id=\"win\">"
    "<property name=\"title\">file editor</property>"
    "<property name=\"default-width\">600</property>"
    "<property name=\"default-height\">400</property>"
    "<child>"
      "<object class=\"GtkBox\">"
        "<property name=\"orientation\">GTK_ORIENTATION_VERTICAL</property>"
... ... ...
... ... ...
"</interface>";

build = gtk_builder_new_from_string (uistring, -1);
```

This method has an advantage and disadvantage.
The advantage is that the UI string is written in the source code.
So, no UI file is needed at runtime.
The disadvantage is that defining the UI as a C string can be tedious,
as the xml needs quoting and special characters need escaping.
If you want to use this method, you should write a script that converts UI files into C string literals.

- Replace backslashes with two backslashes.
- Add a backslash before each double quote.
- Add double quotes at the left and right of the string in each line.

Or, if you have `jq` installed, you can use `jq -R < tfe3.ui` to do the quoting and escaping for you.

### GResources

A GResource is similar to a UI string, except that a GResource is a binary data.
The `glib-compile-resources` program compiles UI files into GResources.
It can compile not only text files but also binary files such as images, sounds and so on.
After compilation, it bundles them up into one GResource object.

An xml file is necessary for the resource compiler `glib-compile-resources`.
It describes resource files.

@@@include
tfe/tfe3.gresource.xml
@@@

- 2: "gresources" tag can include multiple "gresource" tags.
However, this xml has only one "gresource" tag.
- 3: A "gresource" tag can haves a prefix attribute.
This tag defines that the prefix for `tfe3.ui` is `/com/github/ToshioCP/tfe3`.
A prefix works like a namespace.
Even if you have two same name resources, GTK can distinguish them with their different prefixes.
- 4: The name of the GRresource is `tfe3.ui`.
The resource will be referred to as "/com/github/ToshioCP/tfe3/tfe3.ui" by GtkBuilder.
The pattern is "prefix" + "name".
If you want to add more files, insert them between line 4 and 5.

Save this xml text to `tfe3.gresource.xml`.
The GResource compiler `glib-compile-resources` shows its usage with the argument `--help`.

@@@shell
glib-compile-resources --help
@@@

Usually, the resources are converted to a C source file.
Two options `--target` and `--generate-source` are added to the command line.
Move your current directory to /src/tfe and run the command below, then a C source file `resources.c` will be generated.

```
$ glib-compile-resources tfe3.gresource.xml --target=resources.c --generate-source
```

Modify `tfe3.c` and save it as `tfe3_r.c`.

~~~C
#include "resources.c"
... ... ...
... ... ...
build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe3/tfe3.ui");
... ... ...
... ... ...
~~~

The function `gtk_builder_new_from_resource` builds widgets from a resource.

Then, compile and run it.

~~~
$ comp tfe3_r
$ ./a.out
~~~

A window appears and it is the same as the screen shown by `tfe3.c`.

Generally, using GResources is the best approach for C programs.
They allow you to use embedded assets in various widgets.
For example, a GtkImage can be created from a resource path, provided that the original image file has been compiled into the bundle using `glib-compile-resources`.

@@@if gfm
```C
@@@else
```{.c}
@@@end
GtkWidget *image;
image = gtk_image_new_from_resource (const char* resource_path)
```