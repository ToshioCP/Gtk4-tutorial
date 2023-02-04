# GtkSignalListItemFactory

## GtkSignalListItemFactory and GtkBulderListItemFactory

GtkBuilderlistItemFactory is convenient when GtkListView just shows the contents of a list.
Its binding direction is always from an item of a list to a child of GtkListItem.

When it comes to dynamic connection, it's not enough.
For example, you want to edit the contents of a list.
You set a child of GtkListItem to a GtkText instance so a user can edit a text with it.
You need to bind an item in the list with the buffer of the GtkText.
The direction is opposite from the one with GtkBuilderListItemFactory.
It is from the GtkText instance to the item in the list.
You can implement this with GtkSignalListItemFactory, which is more flexible than GtkBuilderListItemFactory.

Two things are shown in this section.

- Binding from a child of a GtkListItem instance to an item of a list.
- Access a child of GtkListItem dynamically.
This direction is the same as the one with GtkBulderListItemFactory.
But GtkBulderListItemFactory uses GtkExpression from the item property of the GtkListItem.
So, it updates its child widget only when the item property changes.
In this example the child reflects the change in the same item in the list dynamically.

This section shows just a part of the source file `listeditor.c`.
If you want to see the whole codes, see `src/listeditor` directory of the [Gtk4 tutorial repository](https://github.com/ToshioCP/Gtk4-tutorial).

## A list editor

The sample program is a list editor and data of the list are strings.
It's the same as a line editor.
It reads a text file line by line.
Each line is an item of the list.
The list is displayed with GtkColumnView.
There are two columns.
The one is a button, which makes the line be a current line.
If the line is the current line, the button is colored with red.
The other is a string which is the contents of the corresponding item of the list.

![List editor](../image/listeditor.png){width=12cm height=9cm}

The source files are located at `src/listeditor` directory.
You can compile end execute it as follows.

- Download the program from the [repository](https://github.com/ToshioCP/Gtk4-tutorial).
- Change your current directory to `src/listeditor`.
- Type the following on your commandline.

~~~
$ meson _build
$ ninja -C _build
$ _build/listeditor
~~~

- Append button: appends a line after the current line, or at the last line if no current line exists.
- Insert button: inserts a line before the current line.
- Remove button: removes a current line.
- Read button: reads a file.
- Write button: writes the contents to a file.
- close button: close the contents.
- quit button: quit the application.
- Button on the select column: makes the line current.
- String column: GtkText. You can edit a string in the field.

The current line number (zero-based) is shown at the left of the tool bar.
The file name is shown at the right of the write button.

## Connect a GtkText instance and an item in the list

The second column (GtkColumnViewColumn) sets its factory property to GtkSignalListItemFactory.
It uses three signals setup, bind and unbind.
The following is their sgnal handlers.

@@@include
listeditor/listeditor.c setup2_cb bind2_cb unbind2_cb
@@@

- 1-6: `setup2_cb` is a setup signal handler on the GtkSignalListItemFactory.
This factory is inserted to the factory property of the second GtkColumnViewColumn.
The handler just creates a GtkText instance and sets the child of `listitem` to it.
The instance will be destroyed automatically when the `listitem` is destroyed.
So, teardown signal handler isn't necessary.
- 8-20: `bind2_cb` is a bind signal handler.
It is called when the `listitem` is bound to an item in the list.
The list items are LeData instances.
LeData is defined in the file `listeditor.c` (the C source file of the list editor).
It is a child class of GObject and has two data.
The one is `listitem` which points a first column GtkListItem instance when they are connected.
Be careful that the GtkListItem instance is *not* the `listitem` in this handler.
If no GtkListItem is connected, it is NULL.
The other is `string` which is a content of the line. 
  - 10-11: `text` is a child of the `listitem` and it is a GtkText instance.
And `buffer` is a GtkTextBuffer instance of the `text`.
  - 12: The LeData instance `data` is an item pointed by the `listitem`.
  - 15-16: Sets the text of `text` to `le_data_look_string (data)`.
le\_data\_look\_string returns the string of the `data` and the ownership of the string is still taken by the `data`.
So, the caller don't need to free the string.
  - 18: `g_object_bind_property` binds a property and another object property.
This line binds the "text" property of the `buffer` (source) and the "string" property of the `data` (destination).
It is a uni-directional binding (`G_BINDING_DEFAULT`).
When a user changes the GtkText text, the same string is immediately put into the `data`. 
The function returns a GBinding instance.
This binding is different from bindings of GtkExpression.
This binding needs the existence of the two properties.
  - 19: GObjec has a table.
The key is a string (or GQuark) and the value is a gpointer (pointer to any type).
The function `g_object_set_data` sets the association from the key to the value.
This line sets the association from "bind" to `bind` instance.
It makes possible for the "unbind" handler to get the `bind` instance.
- 22-28: `unbind2_cb` is a unbind signal handler.
  - 24: Retrieves the `bind` instance from the table in the `listitem` instance.
  - 26: Unbind the binding.
  - 27: Removes the value corresponds to the "bind" key.

This technique is not so complicated.
You can use it when you make a cell editable application.

## Change the cell of GtkColumnView dynamically

Next topic is to change the GtkColumnView (or GtkListView) cells dynamically.
The example changes the color of the buttons, which are children of GtkListItem instances, as the current line position moves.

The line editor has the current position of the list.

- At first, no line is current.
- When a line is appended or inserted, the line is current.
- When the current line is deleted, no line will be current.
- When a button in the first column of GtkColumnView is clicked, the line will be current.
- It is necessary to set the line status (whether current or not) when a GtkListItem is bound to an item in the list.
It is because GtkListItem is recycled.
A GtkListItem was possibly current line before but not current after recycled.
The opposite can also be happen.

The button of the current line is colored with red and otherwise white.

The current line has no relationship to GtkSingleSelection object.
GtkSingleSelection selects a line on the display.
The current line doesn't need to be on the display.
It is possible to be on the line out of the Window (GtkScrolledWindow).
Actually, the program doesn't use GtkSingleSelection.

It is necessary to know the corresponding GtkListItem instance from the item in the list.
It is the opposite direction from `gtk_list_item_get_item` function.
To accomplish this, we set a `listitem` element of LeData to point the corresponding GtkListItem instance.
Therefore, items (LeData) in the list always know the GtkListItem.
If there's no GtkListItem bound to the item, NULL is assigned.

@@@include
listeditor/listeditor.c select_cb setup1_cb bind1_cb unbind1_cb
@@@

- 8-14: `setup1_cb` is a setup signal handler on the GtkSignalListItemFactory.
This factory is inserted to the factory property of the first GtkColumnViewColumn.
It sets the child of `listitem` to a GtkButton instance.
The "clicked" signal on the button is connected to the handler `select_cb`.
When the listitem is destroyed, the child (GtkButton) is also destroyed.
At the same time, the connection of the signal and the handler is also destroyed.
So, you don't need teardown signal handler.
- 1-6: `select_cb` is a "clicked" signal handler.
LeWindow is defined in `listeditor.c`.
It's a child class of GtkApplicationWindow.
The handler just calls the `update_current` function.
The function will be explained later.
- 16-31: `bind1_cb` is a bind signal handler.
It sets the "listitem" element of the item (LeData) to point the `listitem` (GtkListItem instance).
It makes the item possible to find the corresponding GtkListItem instance.
If the item is the current line, the CSS class of the button includes "current" class.
Otherwise it has no CSS class.
This is necessary because the button may be recycled and it has had former CSS class.
The class need to be updated.
- 33-38: `unbind1_cb` is an unbind signal handler.
It removes the `listitem` instance from the "listitem" element of the item.
The element becomes NULL, which tells no GtkListItem is bound.
When referring GtkListItem, it needs to check the "listitem" element whether it points a GtkListItem or not (NULL).
Otherwise bad things will happen.

@@@include
listeditor/listeditor.c update_current
@@@

The function `update_current` does several things.

- It has two parameters.
The first one is `win`, which is an instance of LeWindow class.
It has some elements.
  - win->position: an Integer. it is the current position. If no current line exists, it is -1.
  - win->position_label: GtkLabel. It shows the current position.
- The second parameter is `new`, which is the new current position.
At the beginning of the function, win->position points the old position.
- 10-16: Update the text of GtkLabel.
- 18-26: If the old position (win->position) is not negative, the current line exists.
It gets a GtkListItem instance via the item (LeData) of the list.
And it gets the GtkButton instance which is the child of the GtkListItem.
It clears the "css-classes" property of the button.
- 27: Updates win->position.
- 28-36: If the new position is not negative (It's possible to be negative when the current line has been removed), the current line exists.
It sets the "css-classes" property of the button to `{"current", NULL}`.
It is a NULL-terminated array of strings.
Each string is a CSS class.
Now the button has "current" style class.

The color of buttons are determined by the "background" CSS style.
The following CSS is applied to the default GdkDisplay in advance (in the startup handler of the application).

~~~css
columnview listview row button.current {background: red;}
~~~

The selectors "columnview listview row" is needed before "button" selector.
Otherwise the buttons in the GtkColumnview won't be found.
The button selector has "current" class.
So, the only "current" class button is colored with red.
Other buttons are not colored, which means they are white.

## Gtk\_widget\_dispose\_template function

The function `gtk_widget_dispose_template` clears the template children for the given widget.
This is the opposite of `gtk_widget_init_template()`.
It is a new function of GTK 4.8 version.
If your GTK version is lower than 4.8, you need to modify the program.

## A waring from GtkText

If the list has many items and it needs to be scrolled, a warning message can be issued.

~~~
GtkText - unexpected blinking selection. Removing
~~~

I don't have an idea why this happens.
But if GtkText "focusable" property is FALSE, the warning doesn't happen.
So it probably come from focus and scroll.
If you know the reason, please let me know.

It is a warning message, not a fatal error.
I think we can ignore it.
