Up: [Readme.md](Readme.md),  Prev: [Section 3](sec3.md), Next: [Section 5](sec5.md)

# Widgets (1)

## GtkLabel, GtkButton and Gtkbox

### GtkLabel

We made an window and show it on the screen in the previous section.
Now we go on to the next topic, widgets in the window.
The simplest widget is GtkLabel.
It is a widget with a string in it.

     1 #include <gtk/gtk.h>
     2 
     3 static void
     4 on_activate (GApplication *app, gpointer user_data) {
     5   GtkWidget *win;
     6   GtkWidget *lab;
     7 
     8   win = gtk_application_window_new (GTK_APPLICATION (app));
     9   gtk_window_set_title (GTK_WINDOW (win), "lb1");
    10   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
    11 
    12   lab = gtk_label_new ("Hello.");
    13   gtk_window_set_child (GTK_WINDOW (win), lab);
    14 
    15   gtk_widget_show (win);
    16 }
    17 
    18 int
    19 main (int argc, char **argv) {
    20   GtkApplication *app;
    21   int stat;
    22 
    23   app = gtk_application_new ("com.github.ToshioCP.lb1", G_APPLICATION_FLAGS_NONE);
    24   g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
    25   stat =g_application_run (G_APPLICATION (app), argc, argv);
    26   g_object_unref (app);
    27   return stat;
    28 }
    29 

Save this program to a file `lb1.c`.
Then compile and run it.

    $ comp lb1
    $ ./a.out

A window with a message "Hello." appears.

![Screenshot of the label](image/screenshot_lb1.png)

There's only a little change between `pr4.c` and `lb1.c`.
Diff is a good program to know the difference between two files.

    $ cd misc; diff pr4.c lb1.c
    5a6
    >   GtkWidget *lab;
    8c9
    <   gtk_window_set_title (GTK_WINDOW (win), "pr4");
    ---
    >   gtk_window_set_title (GTK_WINDOW (win), "lb1");
    9a11,14
    > 
    >   lab = gtk_label_new ("Hello.");
    >   gtk_window_set_child (GTK_WINDOW (win), lab);
    > 
    18c23
    <   app = gtk_application_new ("com.github.ToshioCP.pr4", G_APPLICATION_FLAGS_NONE);
    ---
    >   app = gtk_application_new ("com.github.ToshioCP.lb1", G_APPLICATION_FLAGS_NONE);

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

     1 #include <gtk/gtk.h>
     2 
     3 static void
     4 click_cb (GtkButton *btn, gpointer user_data) {
     5   g_print ("Clicked.\n");
     6 }
     7 
     8 static void
     9 on_activate (GApplication *app, gpointer user_data) {
    10   GtkWidget *win;
    11   GtkWidget *btn;
    12 
    13   win = gtk_application_window_new (GTK_APPLICATION (app));
    14   gtk_window_set_title (GTK_WINDOW (win), "lb2");
    15   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
    16 
    17   btn = gtk_button_new_with_label ("Click me");
    18   gtk_window_set_child (GTK_WINDOW (win), btn);
    19   g_signal_connect (btn, "clicked", G_CALLBACK (click_cb), NULL);
    20 
    21   gtk_widget_show (win);
    22 }
    23 
    24 int
    25 main (int argc, char **argv) {
    26   GtkApplication *app;
    27   int stat;
    28 
    29   app = gtk_application_new ("com.github.ToshioCP.lb2", G_APPLICATION_FLAGS_NONE);
    30   g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
    31   stat =g_application_run (G_APPLICATION (app), argc, argv);
    32   g_object_unref (app);
    33   return stat;
    34 }
    35 

Look at the line 17 to 19.
First, generate a GtkButton widget `btn` with a label "Click me".
Then, set it to the window `win` as a child.
Finally, connect a "clicked" signal of the button to a handler (function) `click_cb`.
So, if `btn` is clicked, the function `click_cb` is invoked.
The suffix cb means "call back".

Name the program `lb2.c` and save it. 
Now compile and run it.

![Screenshot of the label](image/screenshot_lb2.png)
 
A window with the button appears.
Click the button (it is a large button, you can click everywhere inside the window), then a string "Clicked." appears on the shell terminal.
It shows the handler was invoked by clicking the button.

It's fairly good for us to make sure that the clicked signal was caught and the handler was invoked.
However, using g_print is out of harmony with GTK which is a GUI library.
So, we will change the handler.
The following code is `lb3.c`.

     1 static void
     2 click_cb (GtkButton *btn, gpointer user_data) {
     3   GtkWindow *win = GTK_WINDOW (user_data);
     4   gtk_window_destroy (win);
     5 }
     6 
     7 static void
     8 on_activate (GApplication *app, gpointer user_data) {
     9   GtkWidget *win;
    10   GtkWidget *btn;
    11 
    12   win = gtk_application_window_new (GTK_APPLICATION (app));
    13   gtk_window_set_title (GTK_WINDOW (win), "lb3");
    14   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
    15 
    16   btn = gtk_button_new_with_label ("Quit");
    17   gtk_window_set_child (GTK_WINDOW (win), btn);
    18   g_signal_connect (btn, "clicked", G_CALLBACK (click_cb), win);
    19 
    20   gtk_widget_show (win);
    21 }

And the difference between `lb2.c` and `lb3.c` is as follows.

    $ cd misc; diff lb2.c lb3.c
    5c5,6
    <   g_print ("Clicked.\n");
    ---
    >   GtkWindow *win = GTK_WINDOW (user_data);
    >   gtk_window_destroy (win);
    14c15
    <   gtk_window_set_title (GTK_WINDOW (win), "lb2");
    ---
    >   gtk_window_set_title (GTK_WINDOW (win), "lb3");
    17c18
    <   btn = gtk_button_new_with_label ("Click me");
    ---
    >   btn = gtk_button_new_with_label ("Quit");
    19c20
    <   g_signal_connect (btn, "clicked", G_CALLBACK (click_cb), NULL);
    ---
    >   g_signal_connect (btn, "clicked", G_CALLBACK (click_cb), win);
    29c30
    <   app = gtk_application_new ("com.github.ToshioCP.lb2", G_APPLICATION_FLAGS_NONE);
    ---
    >   app = gtk_application_new ("com.github.ToshioCP.lb3", G_APPLICATION_FLAGS_NONE);
    35d35
    < 

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

![Parent-child relationship](image/box.png)

Now, code it.

     1 #include <gtk/gtk.h>
     2 
     3 static void
     4 click1_cb (GtkButton *btn, gpointer user_data) {
     5   const gchar *s;
     6 
     7   s = gtk_button_get_label (btn);
     8   if (g_strcmp0 (s, "Hello.") == 0)
     9     gtk_button_set_label (btn, "Good-bye.");
    10   else
    11     gtk_button_set_label (btn, "Hello.");
    12 }
    13 
    14 static void
    15 click2_cb (GtkButton *btn, gpointer user_data) {
    16   GtkWindow *win = GTK_WINDOW (user_data);
    17   gtk_window_destroy (win);
    18 }
    19 
    20 static void
    21 on_activate (GApplication *app, gpointer user_data) {
    22   GtkWidget *win;
    23   GtkWidget *box;
    24   GtkWidget *btn1;
    25   GtkWidget *btn2;
    26 
    27   win = gtk_application_window_new (GTK_APPLICATION (app));
    28   gtk_window_set_title (GTK_WINDOW (win), "lb4");
    29   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
    30 
    31   box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
    32   gtk_box_set_homogeneous (GTK_BOX (box), TRUE);
    33   gtk_window_set_child (GTK_WINDOW (win), box);
    34 
    35   btn1 = gtk_button_new_with_label ("Hello.");
    36   g_signal_connect (btn1, "clicked", G_CALLBACK (click1_cb), NULL);
    37 
    38   btn2 = gtk_button_new_with_label ("Quit");
    39   g_signal_connect (btn2, "clicked", G_CALLBACK (click2_cb), win);
    40 
    41   gtk_box_append (GTK_BOX (box), btn1);
    42   gtk_box_append (GTK_BOX (box), btn2);
    43 
    44   gtk_widget_show (win);
    45 }
    46 
    47 int
    48 main (int argc, char **argv) {
    49   GtkApplication *app;
    50   int stat;
    51 
    52   app = gtk_application_new ("com.github.ToshioCP.lb4", G_APPLICATION_FLAGS_NONE);
    53   g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
    54   stat =g_application_run (G_APPLICATION (app), argc, argv);
    55   g_object_unref (app);
    56   return stat;
    57 }

Look at the function `on_activate`.

After the generation of GtkApplicationWindow, GtkBox is generated.

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_set_homogeneous (GTK_BOX (box), TRUE);

The first argument arranges children vertically.
The second argument is the size between children.
The next function fills a box with children, giving them equal space.

After that, two buttons `btn1` and `btn2` are generated and the signal handlers are set.
Then, these two buttons are appended to the box.

![Screenshot of the box](image/screenshot_lb4.png)

The handler corresponds to `btn1` changes its label.
The handler corresponds to `btn2` destroys the top-level window and the application quits.


Up: [Readme.md](Readme.md),  Prev: [Section 3](sec3.md), Next: [Section 5](sec5.md)
