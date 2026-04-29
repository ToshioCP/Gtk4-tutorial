Up: [README.md](../README.md),  Prev: [Section 31](sec31.md), Next: [Section 33](sec33.md)

# GtkColumnView

## GtkColumnView

GtkColumnView is like GtkListView, but it has multiple columns.
Each column is GtkColumnViewColumn.

![Column View](/src/images/column_view.png)

- GtkColumnView has "model" property.
The property points to a GtkSelectionModel object.
- Each GtkColumnViewColumn has the "factory" property.
The property points to a GtkListItemFactory (GtkSignalListItemFactory or GtkBuilderListItemFactory).
- The factory connects GtkListItem and items of GtkSelectionModel.
And the factory builds the descendant widgets of GtkColumnView to display the items on the screen.
This process is the same as the one in GtkListView.

The following diagram shows how it works.

![ColumnView](/src/images/column.png)

The example in this section is a window that displays information of files in the current directory.
The information is the name, size and last modified datetime of files.
So, there are three columns.

In addition, the example uses GtkSortListModel and GtkSorter to sort the information.

## column.ui

UI file specifies widgets and list item templates.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <object class="GtkApplicationWindow" id="win">
    <property name="title">file list</property>
    <property name="default-width">800</property>
    <property name="default-height">600</property>
    <child>
      <object class="GtkScrolledWindow">
        <property name="hexpand">TRUE</property>
        <property name="vexpand">TRUE</property>
        <child>
          <object class="GtkColumnView" id="columnview">
            <property name="model">
              <object class="GtkNoSelection">
                <property name="model">
                  <object class="GtkSortListModel">
                    <property name="model">
                      <object class="GtkDirectoryList" id="directorylist">
                        <property name="attributes">standard::name,standard::icon,standard::size,time::modified</property>
                      </object>
                    </property>
                    <binding name="sorter">
                      <lookup name="sorter">columnview</lookup>
                    </binding>
                  </object>
                </property>
              </object>
            </property>
            <child>
              <object class="GtkColumnViewColumn">
                <property name="title">Name</property>
                <property name="expand">TRUE</property>
                <property name="factory">
                  <object class="GtkBuilderListItemFactory">
                    <property name="bytes"><![CDATA[
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <template class="GtkListItem">
    <property name="child">
      <object class="GtkBox">
        <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
        <property name="spacing">20</property>
        <child>
          <object class="GtkImage">
            <binding name="gicon">
              <closure type="GIcon" function="get_icon_factory">
                <lookup name="item">GtkListItem</lookup>
              </closure>
            </binding>
          </object>
        </child>
        <child>
          <object class="GtkLabel">
            <property name="hexpand">TRUE</property>
            <property name="xalign">0</property>
            <binding name="label">
              <closure type="gchararray" function="get_file_name_factory">
                <lookup name="item">GtkListItem</lookup>
              </closure>
            </binding>
          </object>
        </child>
      </object>
    </property>
  </template>
</interface>
                    ]]></property>
                  </object>
                </property>
                <property name="sorter">
                  <object class="GtkStringSorter">
                    <property name="expression">
                      <closure type="gchararray" function="get_file_name">
                      </closure>
                    </property>
                  </object>
                </property>
              </object>
            </child>
            <child>
              <object class="GtkColumnViewColumn">
                <property name="title">Size</property>
                <property name="factory">
                  <object class="GtkBuilderListItemFactory">
                    <property name="bytes"><![CDATA[
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <template class="GtkListItem">
    <property name="child">
      <object class="GtkLabel">
        <property name="hexpand">TRUE</property>
        <property name="xalign">0</property>
        <binding name="label">
          <closure type="gint64" function="get_file_size_factory">
            <lookup name="item">GtkListItem</lookup>
          </closure>
        </binding>
      </object>
    </property>
  </template>
</interface>
                    ]]></property>
                  </object>
                </property>
                <property name="sorter">
                  <object class="GtkNumericSorter">
                    <property name="expression">
                      <closure type="gint64" function="get_file_size">
                      </closure>
                    </property>
                    <property name="sort-order">GTK_SORT_ASCENDING</property>
                  </object>
                </property>
              </object>
            </child>
            <child>
              <object class="GtkColumnViewColumn">
                <property name="title">Date modified</property>
                <property name="factory">
                  <object class="GtkBuilderListItemFactory">
                    <property name="bytes"><![CDATA[
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <template class="GtkListItem">
    <property name="child">
      <object class="GtkLabel">
        <property name="hexpand">TRUE</property>
        <property name="xalign">0</property>
        <binding name="label">
          <closure type="gchararray" function="get_file_time_modified_factory">
            <lookup name="item">GtkListItem</lookup>
          </closure>
        </binding>
      </object>
    </property>
  </template>
</interface>
                    ]]></property>
                  </object>
                </property>
                <property name="sorter">
                  <object class="GtkNumericSorter">
                    <property name="expression">
                      <closure type="gint64" function="get_file_unixtime_modified">
                      </closure>
                    </property>
                    <property name="sort-order">GTK_SORT_ASCENDING</property>
                  </object>
                </property>
              </object>
            </child>
          </object>
        </child>
      </object>
    </child>
  </object>
</interface>
```

- 3-12: GtkApplicationWindow has a child widget GtkScrolledWindow.
GtkScrolledWindow has a child widget GtkColumnView.
- 12-18: GtkColumnView has "model" property.
It points to GtkSelectionModel interface.
GtkNoSelection class is used as GtkSelectionModel.
Similarly, it also has a "model" property.
It points to GtkSortListModel.
This list model supports sorting the list.
It will be explained in the later subsection.
And it also has "model" property.
It points to GtkDirectoryList.
Therefore, the chain is: GtkColumnView => GtkNoSelection => GtkSortListModel => GtkDirectoryList.
- 18-20: GtkDirectoryList.
It is a list of GFileInfo, which holds information of files under a directory.
It has "attributes" property.
It specifies which attributes are kept in each GFileInfo.
  - "standard::name" is the name of the file.
  - "standard::icon" is the GIcon object of the file
  - "standard::size" is the file size.
  - "time::modified" is the date and time the file was last modified.
- 29-79: The first GtkColumnViewColumn object.
There are four properties, "title", "expand", factory" and "sorter".
- 31: Sets the "title" property to "Name".
This is the title on the header of the column.
- 32: Sets the "expand" property to TRUE to allow the column to expand as much as possible.
(See the image above).
- 33- 69: Sets the "factory" property to GtkBuilderListItemFactory.
The factory has "bytes" property which holds a UI string to define a template to extend GtkListItem class.
The CDATA section (line 36-66) is the UI string to put into the "bytes" property.
The contents are the same as the UI file `factory_list.ui` in Section 30.
- 70-77: Sets the "sorter" property to GtkStringSorter object.
This object provides a sorter that compares strings.
It has "expression" property.
A closure tag with a string type function `get_file_name` is used here.
The function will be explained later.
- 80-115: The second GtkColumnViewColumn object.
Its sorter property is set to GtkNumericSorter.
- 116-151: The third GtkColumnViewColumn object.
Its sorter property is set to GtkNumericSorter.

## GtkSortListModel and GtkSorter

GtkSortListModel is a list model that sorts its elements according to a GtkSorter instance assigned to the "sorter" property.
The property is bound to "sorter" property of GtkColumnView in line 22 to 24.

~~~xml
<object class="GtkSortListModel" id="sortlist">
... ... ...
  <binding name="sorter">
    <lookup name="sorter">columnview</lookup>
  </binding>
~~~

Therefore, `columnview` determines how to sort the list model.
The "sorter" property of GtkColumnView is a read-only GtkSorter and it always refers to the current column's "sorter" property.
When a user clicks a column header, GtkColumnView updates its own "sorter" property to point to that column's sorter.

The binding above makes an indirect connection between the "sorter" property of GtkSortListModel and the "sorter" property of each column.

GtkSorter compares two items (GObject or its descendant).
GtkSorter has several child objects.

- GtkStringSorter compares strings taken from the items.
- GtkNumericSorter compares numbers taken from the items.
- GtkCustomSorter uses a callback to compare.
- GtkMultiSorter combines multiple sorters.

The example uses GtkStringSorter and GtkNumericSorter.

GtkStringSorter uses GtkExpression to get the strings from the items (objects).
The GtkExpression is stored in the "expression" property of the GtkStringSorter.
When GtkStringSorter compares two items, it evaluates the expression by calling `gtk_expression_evaluate` function.
It assigns each item to the second argument ('this' object) of the function.

In the UI file above, the GtkExpression is in the line 71 to 76.

~~~xml
<object class="GtkStringSorter">
  <property name="expression">
    <closure type="gchararray" function="get_file_name">
    </closure>
  </property>
</object>
~~~

The GtkExpression calls `get_file_name` function when it is evaluated.

```c
char *
get_file_name (GFileInfo *info) {
  return G_IS_FILE_INFO (info) ? g_strdup (g_file_info_get_name (info)) : NULL;
}
```

The function is given the item (GFileInfo) of the GtkSortListModel as an argument (`this` object).
But you need to be careful that it can be NULL while the list item is being recycled.
So, `G_IS_FILE_INFO (info)` is always necessary in callback functions.
The function retrieves a filename from `info`.
The string is owned by `info` so it is necessary to duplicate.
And it returns the copied string.

GtkNumericSorter compares numbers.
It is used in the line 106 to 112 and line 142 to 148.
The lines from 106 to 112 is:

~~~xml
<object class="GtkNumericSorter">
  <property name="expression">
    <closure type="gint64" function="get_file_size">
    </closure>
  </property>
  <property name="sort-order">GTK_SORT_ASCENDING</property>
</object>
~~~

The closure tag specifies a callback function `get_file_size`.

```c
goffset
get_file_size (GFileInfo *info) {
  return G_IS_FILE_INFO (info) ? g_file_info_get_size (info): -1;
}
```

It just returns the size of `info`.
The type of the size is `goffset`.
The type `goffset` is the same as `gint64`.

The lines from 142 to 148 is:

~~~xml
<object class="GtkNumericSorter" id="sorter_datetime_modified">
  <property name="expression">
    <closure type="gint64" function="get_file_unixtime_modified">
    </closure>
  </property>
  <property name="sort-order">GTK_SORT_ASCENDING</property>
</object>
~~~

The closure tag specifies a callback function `get_file_unixtime_modified`.

```c
gint64
get_file_unixtime_modified (GFileInfo *info) {
  GDateTime *dt;

  dt = G_IS_FILE_INFO (info) ? g_file_info_get_modification_date_time (info) : NULL;
  return dt ? g_date_time_to_unix (dt) : -1;
}
```

It gets the modification date and time (GDateTime type) of `info`.
Then it gets a Unix time from `dt`.
Unix time, sometimes called Unix epoch, is the number of seconds that have elapsed since 00:00:00 UTC on 1 January 1970.
It returns the Unix time as a gint64.

## column.c

`column.c` is as follows.
The code is simple and short thanks to `column.ui`.

```c
#include <gtk/gtk.h>

/* functions (closures) for GtkBuilderListItemFactory */
GIcon *
get_icon_factory (GtkListItem *item, GFileInfo *info) {
  GIcon *icon;

   /* g_file_info_get_icon can return NULL */
  icon = G_IS_FILE_INFO (info) ? g_file_info_get_icon (info) : NULL;
  return icon ? g_object_ref (icon) : NULL;
}

char *
get_file_name_factory (GtkListItem *item, GFileInfo *info) {
  return G_IS_FILE_INFO (info) ? g_strdup (g_file_info_get_name (info)) : NULL;
}

/* goffset is defined as gint64 */
/* It is used for file offsets. */
goffset
get_file_size_factory (GtkListItem *item, GFileInfo *info) {
  return G_IS_FILE_INFO (info) ? g_file_info_get_size (info) : -1;
}

char *
get_file_time_modified_factory (GtkListItem *item, GFileInfo *info) {
  GDateTime *dt;

   /* g_file_info_get_modification_date_time can return NULL */
  dt = G_IS_FILE_INFO (info) ? g_file_info_get_modification_date_time (info) : NULL;
  return dt ? g_date_time_format (dt, "%F") : NULL;
}

/* Functions (closures) for GtkSorter */
char *
get_file_name (GFileInfo *info) {
  return G_IS_FILE_INFO (info) ? g_strdup (g_file_info_get_name (info)) : NULL;
}

goffset
get_file_size (GFileInfo *info) {
  return G_IS_FILE_INFO (info) ? g_file_info_get_size (info): -1;
}

gint64
get_file_unixtime_modified (GFileInfo *info) {
  GDateTime *dt;

  dt = G_IS_FILE_INFO (info) ? g_file_info_get_modification_date_time (info) : NULL;
  return dt ? g_date_time_to_unix (dt) : -1;
}

static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  gtk_window_present (gtk_application_get_active_window(app));
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GFile *file;
  GtkBuilder *build = gtk_builder_new_from_resource ("/com/github/ToshioCP/column/column.ui");
  GtkWidget *win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
  GtkDirectoryList *directorylist = GTK_DIRECTORY_LIST (gtk_builder_get_object (build, "directorylist"));
  g_object_unref (build);

  gtk_window_set_application (GTK_WINDOW (win), app);

  file = g_file_new_for_path (".");
  gtk_directory_list_set_file (directorylist, file);
  g_object_unref (file);
}

#define APPLICATION_ID "com.github.ToshioCP.columnview"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);

  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

```


## Compilation and Execution.

All the source files are in [src/column](../src/column/) directory.
Change your current directory to the directory and type the following.

~~~
$ cd src/column
$ meson setup _build
$ ninja -C _build
$ _build/column
~~~

Then, a window appears.

![Column View](/src/images/column_view.png)

If you click the header of a column, then the entire list is sorted by the column.
If you click the header of another column, then the list is sorted by the newly selected column.

GtkColumnView is very useful and it can manage very big GListModel.
It is possible to use it for file list, application list, database frontend and so on.

Up: [README.md](../README.md),  Prev: [Section 31](sec31.md), Next: [Section 33](sec33.md)
