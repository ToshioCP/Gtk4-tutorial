# GtkSignalListItemFactory

## GtkSignalListItemFactory vs GtkBuilderListItemFactory

GtkBuilderlistItemFactory is convenient when GtkListView just shows the contents of a list model.
Its binding direction is always from an item of the list to a child of GtkListItem.

However, it is insufficient for dynamic connections.
For example, suppose you want to edit the contents of a list.
You can set a GtkText widget as the child of a GtkListItem to allow users to edit text.
You need to update the list item when the text in the GtkText is edited.
The data flow is in the opposite direction compared to GtkBuilderListItemFactory—it goes from the GtkText widget back to the list item.
You can achieve this using GtkSignalListItemFactory, which provides more flexibility.

This section shows just some parts of the source file [listeditor.c](listeditor/listeditor.c).
If you want to see the whole codes, see [src/listeditor](listeditor/) directory of the Gtk4 tutorial repository.

## List Editor

The sample program is a list editor and the data in the list are strings.
It's similar to a line editor.
It reads a text file line by line.
Each line is an item of the list.
The list is displayed with a GtkListView.
The list view contains strings which are the contents of the corresponding items in the list model.

@@@if pdf
![List editor](/images/listeditor.png){width=12cm height=7.2cm}
@@@else
![List editor](/images/listeditor.png)
@@@end

You can compile the source file and execute it as follows.

- Download the program from the [repository](https://github.com/ToshioCP/Gtk4-tutorial).
- Change your current directory to `src/listeditor`.
- Type the following on your commandline.

~~~
$ meson setup _build
$ ninja -C _build
$ _build/listeditor
~~~

The window has buttons:

- **Append button:** appends a data after the current item, or at the end of the model if no current item exists.
- **Insert button:** inserts a row before the current item, or at the top of the model if no current item exists.
- **Remove button:** removes the current item.
- **Read button:** reads a file.
- **Write button:** Writes the contents to the file shown on the toolbar. If no filename is set, a file dialog opens to let you choose one.
- **Close button:** remove all the items from the model.
- **Quit button:** quits the application.

You can click on the content area and select a row.
The corresponding item to the row will be the current item.
The current position (zero-based) is shown at the left of the tool bar.

## Connect a GtkText Instance and an Item in the Model

The ListView sets its factory property to GtkSignalListItemFactory.
Its two signals "setup" and "bind" are connected to the handlers `setup_cb` and `bind_cb` respectively.
The remaining two signals "unbind" and "teardown" aren't used in this program.
The following shows the signal handlers.

@@@include
listeditor/listeditor.c setup_cb bind_cb
@@@

- 5-7: `setup_cb` creates the child widget of the list item.
This widget will be a row of the GtkListView instance.
- 9-10: `setup_cb` connects the "changed" signal on the GtkText instance to the handler `text_changed_cb`.
This handler copies the text in the GtkText to the item when the GtkText is changed.
`setup_cb` also connects the "notify::selected" signal on the GtkListItem to the handler `item_selected_cb`.
The handler make the GtkText grab the keyboard focus to prepare keyboard input.
- These instance and signals created by `setup_cb` will be automatically destroyed when the list item disposes itself.
So, teardown signal handler is not necessary.
- 83-86: `bind_cb` just copy the string in the item in the model to the GtkText.
The macro function `g_signal_block_by_func` avoids unnecessary copy from GtkText to item at the update of the GtkText.
No instances are created in this process, so "unbind" signal handler is not necessary.

- 5-7: `setup_cb` creates the child widget for the list item, which serves as a row in the GtkListView.
- 9-10: `setup_cb` connects the GtkText's "changed" signal to the `text_changed_cb` handler.
This handler updates the item in the model with the new text whenever the GtkText is modified.
Additionally, it connects the "notify::selected" signal of the GtkListItem to `item_selected_cb`, which makes the GtkText grab keyboard focus to prepare for immediate input.
The "notify" signal is inherited from GObject class.
- The instances and signal connections created in `setup_cb` are automatically cleaned up when the list item is disposed.
Therefore, a "teardown" signal handler is unnecessary.
- 19-22: `bind_cb` simply copies the string from the model's item to the GtkText.
The `g_signal_handlers_block_by_func` macro prevents an unnecessary write-back from the GtkText to the item during this update.
Since no new resources are allocated in this step, an "unbind" signal handler is not needed.

The two signal handlers `text_changed_cb` and `item_selected_cb` are shown below.

@@@include
listeditor/listeditor.c text_changed_cb item_selected_cb
@@@

## GtkSingleSelection

In the `liststore.ui` file, the GtkListView's model property is set to a GtkSingleSelection, which in turn wraps the underlying GListModel.

Clicking a row in the list view selects and highlights it, automatically focusing the GtkText widget for immediate editing.
Since GtkSingleSelection handles all the selection logic internally, no additional code is required for this behavior.

## Another Possible Implementation

Alternatively, you can use a GBinding object to bind the GtkText's "text" property to the LeData's "string" property.
To do this, you first need to implement the "string" property in the LeData class.

The code for this version is available in the [src/listeditor_binding/](listeditor_binding/) directory.
The GBinding instance is created in the "bind" handler and unbound in the "unbind" handler.
We use the `g_object_set_data` function to store the binding object within the list item.

This is just one example, and other implementations are possible.
For instance, you could subclass GtkText and store the binding instance directly inside the subclass.

However, these alternatives require more code.
Ultimately, using the "changed" signal remains the simplest and most straightforward approach.
