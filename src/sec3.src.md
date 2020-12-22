# Widgets (1)

## GtkLabel, GtkButton and Gtkbox

### GtkLabel

We made an window and show it on the screen in the previous chapter.
Now we go on to the next topic, widgets in the window.
The simplest widget is GtkLabel.
It is a widget with a string in it.

@@@ lb1.c

Save this program to a file `lb1.c`.
Then compile and run it.

    $ comp lb1
    $ ./a.out

A window with a message "Hello." appears.

![Screenshot of the label](../image/screenshot_lb1.png)

There's only a little change between `pr4.c` and `lb1.c`.
Diff is a good program to know the difference between two files.

$$$
diff misc/pr4.c lb1.c
$$$

This tells us:

- The definition of a variable lab is added.
- The title of the window is changed.
- A label is generated and connected to the window.

The function `gtk_window_set_child (GTK_WINDOW (win), lab)` makes the label `lab` a child widget of the window `win`.
Be careful.
A child widget is different from a child object.
Objects have parent-child relationship and Widgets also have parent-child relationship.
But these two relationships are totally different.
Don't be confused.
In the program `lb1.c`, `lab` is a child widget of `win`.
Child widgets are always located inside its parent widget in the screen.
See the window appeared on the screen.
The window includes the label.

The window `win` dosen't have any parents.
We call such a window top-level window.
One application can have two or more top-level windows.

### GtkButton

Next widget is GtkButton.
It has a label or icon on it.
In this subsection, we will make a button with a label.
When a button is clicked on, it emits a "clicked" signal.
The following program shows how to catch the signal and do something.

@@@ lb2.c

Look at the line 17 to 19.
First, generate a GtkButton widget `btn` with a label "Click me".
Then, set it to the window `win` as a child.
Finally, connect a "clicked" signal of the button to a handler (function) `on_click`.
So, if `btn` is clicked, the function `on_click` is invoked.

Name the program `lb2.c` and save it. 
Now compile and run it.

![Screenshot of the label](../image/screenshot_lb2.png)
 
A window with the button appears.
Click the button (it is a large button, you can click everywhere inside the window), then a string "Clicked." appears on the shell terminal.
It shows the handler was invoked by clicking the button.

It's fairly good for us to make sure that the clicked signal was caught and the handler was invoked.
However, using g_print is out of harmony with GTK which is a GUI library.
So, we will change the handler.
The following code is `lb3.c`.

@@@ lb3.c on_clicked on_activate

And the difference between `lb2.c` and `lb3.c` is as follows.

$$$
diff lb2.c lb3.c
$$$

The change is:

- The function `g_print` in `lb2.c` was deleted and two lines above are inserted instead.
- The label of `btn` is changed from "Click me" to "Quit".
- The fourth argument of `g_signal_connect` is changed from `NULL` to `win`. 

Most important is the fourth argument of `g_signal_connect`.
It is described as "data to pass to handler" in the definition of g\_signal\_connect in GObject API reference.
Therefore, `win` which is a pointer to GtkApplicationWindow is passed to the handler as a second parameter user_data.
Then, the handler cast it to a pointer to GtkWindow and call `gtk_window_destroy` and destroy the top window.
Then, the application quits.

### GtkBox

GtkWindow and GtkApplicationWindow can have only one child.
If you want to add two or more widgets inside a window, you need a container widget.
GtkBox is one of the containers.
It arranges two or more child widgets into a single row or column.
The following procedure shows the way to add two buttons in a window.

- Generate GtkApplicationWindow.
- Generate GtkBox and set it a child of GtkApplicationWindow.
- Generate GtkButton and append it to GtkBox.
- Generate another GtkButton and append it to GtkBox.

After this, the Widgets are connected as following diagram.

![Parent-child relationship](../image/box.png)

Now, code it.

@@@ lb4.c

Look at the function `on_activate`.

After the generation of GtkApplicationWindow, GtkBox is generated.

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_set_homogeneous (GTK_BOX (box), TRUE);

The first argument arranges children vertically.
The second argument is sizes between children.
The next function fills a box with children, giving them equal space.

After that, two buttons `btn1` and `btn2` are generated and the signal handlers are set.
Then, these two buttons are appended to the box.

![Screenshot of the box](../image/screenshot_lb4.png)

The handler corresponds to `btn1` changes its label.
The handler corresponds to `btn2` destroys the top-level window and the application quits.

