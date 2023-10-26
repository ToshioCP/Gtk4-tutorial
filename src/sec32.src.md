# GtkColumnView

## GtkColumnView

GtkColumnView is like GtkListView, but it has multiple columns.
Each column is GtkColumnViewColumn.

![Column View](../image/column_view.png){width=11.3cm height=9cm}

- GtkColumnView has "model" property.
The property points a GtkSelectionModel object.
- Each GtkColumnViewColumn has "factory" property.
The property points a GtkListItemFactory (GtkSignalListItemFactory or GtkBuilderListItemFactory).
- The factory connects GtkListItem and items of GtkSelectionModel.
And the factory builds the descendant widgets of GtkColumnView to display the item on the display.
This process is the same as the one in GtkListView.

The following diagram shows how it works.

![ColumnView](../image/column.png){width=12cm height=9cm}

The example in this section is a window that displays information of files in a current directory.
The information is the name, size and last modified datetime of files.
So, there are three columns.

In addition, the example uses GtkSortListModel and GtkSorter to sort the information.

## column.ui

Ui file specifies widgets and list item templates.

@@@include
column/column.ui
@@@

- 3-12: GtkApplicationWindow has a child widget GtkScrolledWindow.
GtkScrolledWindoww has a child widget GtkColumnView.
- 12-18: GtkColumnView has "model" property.
It points GtkSelectionModel interface.
GtkNoSelection class is used as GtkSelectionModel.
And again, it has "model" property.
It points GtkSortListModel.
This list model supports sorting the list.
It will be explained in the later subsection.
And it also has "model" property.
It points GtkDirectoryList.
Therefore, the chain is: GtkColumnView => GtkNoSelection => GtkSortListModel => GtkDirectoryList.
- 18-20: GtkDirectoryList.
It is a list of GFileInfo, which holds information of files under a directory.
It has "attributes" property.
It specifies what attributes is kept in each GFileInfo.
  - "standard::name" is a name of the file.
  - "standard::icon" is a GIcon object of the file
  - "standard::size" is the file size.
  - "time::modified" is the date and time the file was last modified.
- 29-79: The first GtkColumnViewColumn object.
There are four properties, "title", "expand", factory" and "sorter".
- 31: Sets the "title" property to "Name".
This is the title on the header of the column.
- 32: Sets the "expand" property to TRUE to allow the column to expand as much as possible.
(See the image above).
- 33- 69: Sets the "factory" property to GtkBuilderListItemFactory.
The factory has "bytes" property which holds a ui string to define a template to extend GtkListItem class.
The CDATA section (line 36-66) is the ui string to put into the "bytes" property.
The contents are the same as the ui file `factory_list.ui` in the section 30.
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

Therefore, `columnview` determines the way how to sort the list model.
The "sorter" property of GtkColumnView is read-only property and it is a special sorter.
It reflects the user's sorting choice.
If a user clicks the header of a column, then the sorter ("sorter" property) of the column is referenced by "sorter" property of the GtkColumnView.
If the user clicks the header of another column, then the "sorter" property of the GtkColumnView refers to the newly clicked column's "sorter" property.

The binding above makes a indirect connection between the "sorter" property of GtkSortListModel and the "sorter" property of each column.

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

In the ui file above, the GtkExpression is in the line 71 to 76.

~~~xml
<object class="GtkStringSorter">
  <property name="expression">
    <closure type="gchararray" function="get_file_name">
    </closure>
  </property>
</object>
~~~

The GtkExpression calls `get_file_name` function when it is evaluated.

@@@include
column/column.c get_file_name
@@@

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

@@@include
column/column.c get_file_size
@@@

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

@@@include
column/column.c get_file_unixtime_modified
@@@

It gets the modification date and time (GDateTime type) of `info`.
Then it gets a unix time from `dt`.
Unix time, sometimes called unix epoch, is the number of seconds that have elapsed since 00:00:00 UTC on 1 January 1970.
It returns the unix time (gint64 type).

## column.c

`column.c` is as follows.
It is simple and short thanks to `column.ui`.

@@@include
column/column.c
@@@


## Compilation and execution.

All the source files are in [`src/column`](column) directory.
Change your current directory to the directory and type the following.

~~~
$ cd src/colomn
$ meson setup _build
$ ninja -C _build
$ _build/column
~~~

Then, a window appears.

![Column View](../image/column_view.png){width=11.3cm height=9cm}

If you click the header of a column, then the whole lists are sorted by the column.
If you click the header of another column, then the whole lists are sorted by the newly selected column.

GtkColumnView is very useful and it can manage very big GListModel.
It is possible to use it for file list, application list, database frontend and so on.
