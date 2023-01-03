Up: [README.md](../README.md),  Prev: [Section 3](sec3.md), Next: [Section 5](sec5.md)

# Widgets (1)

## GtkLabel, GtkButton and GtkBox

### GtkLabel

In the previous section we made a window and displayed it on the screen.
Now we go on to the next topic, where we add widgets to this window.
The simplest widget is GtkLabel.
It is a widget with text in it.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 app_activate (GApplication *app) {
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
15   gtk_window_present (GTK_WINDOW (win));
16 }
17 
18 int
19 main (int argc, char **argv) {
20   GtkApplication *app;
21   int stat;
22 
23   app = gtk_application_new ("com.github.ToshioCP.lb1", G_APPLICATION_DEFAULT_FLAGS);
24   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
25   stat =g_application_run (G_APPLICATION (app), argc, argv);
26   g_object_unref (app);
27   return stat;
28 }
29 
~~~

Save this program to a file `lb1.c`.
Then compile and run it.

    $ comp lb1
    $ ./a.out

A window with a message "Hello." appears.

![Screenshot of the label](../image/screenshot_lb1.png)

There's only a little change between `pr4.c` and `lb1.c`.
A program `diff` is good to know the difference between two files.

~~~
$ cd misc; diff pr4.c lb1.c
4c4
< app_activate (GApplication *app, gpointer user_data) {
---
> app_activate (GApplication *app) {
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
<   app = gtk_application_new ("com.github.ToshioCP.pr4", G_APPLICATION_DEFAULT_FLAGS);
---
>   app = gtk_application_new ("com.github.ToshioCP.lb1", G_APPLICATION_DEFAULT_FLAGS);
~~~

This tells us:

- A signal handler `app_activate` doesn't have `user_data` parameter.
If the fourth argument of `g_signal_connect` is NULL, you can leave out `user_data`.
- The definition of a new variable `lab` is added.
- The title of the window is changed.
- A label is created and connected to the window as a child.

The function `gtk_window_set_child (GTK_WINDOW (win), lab)` makes the label `lab` a child widget of the window `win`.
Be careful.
A child widget is different from a child object.
Objects have parent-child relationships and widgets also have parent-child relationships.
But these two relationships are totally different.
Don't be confused.
In the program `lb1.c`, `lab` is a child widget of `win`.
Child widgets are always located in their parent widget on the screen.
See how the window has appeared on the screen.
The application window includes the label.

The window `win` doesn't have any parents.
We call such a window top-level window.
An application can have more than one top-level window.

### GtkButton

The next widget is GtkButton.
It displays a button on the screen with a label or icon on it.
In this subsection, we will make a button with a label.
When the button is clicked, it emits a "clicked" signal.
The following program shows how to catch the signal and do something.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 click_cb (GtkButton *btn) {
 5   g_print ("Clicked.\n");
 6 }
 7 
 8 static void
 9 app_activate (GApplication *app) {
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
21   gtk_window_present (GTK_WINDOW (win));
22 }
23 
24 int
25 main (int argc, char **argv) {
26   GtkApplication *app;
27   int stat;
28 
29   app = gtk_application_new ("com.github.ToshioCP.lb2", G_APPLICATION_DEFAULT_FLAGS);
30   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
31   stat =g_application_run (G_APPLICATION (app), argc, argv);
32   g_object_unref (app);
33   return stat;
34 }
35 
~~~

Look at the line 17 to 19.
First, it creates a GtkButton instance `btn` with a label "Click me".
Then, adds the button to the window `win` as a child.
Finally, connects a "clicked" signal of the button to the handler `click_cb`.
So, if `btn` is clicked, the function `click_cb` is invoked.
The suffix "cb" means "call back".

Name the program `lb2.c` and save it.
Now compile and run it.

![Screenshot of the label](../image/screenshot_lb2.png)

A window with the button appears.
Click the button (it is a large button, you can click everywhere in the window), then a string "Clicked." appears on the terminal.
It shows the handler was invoked by clicking the button.

It's good that we make sure that the clicked signal was caught and the handler was invoked by using `g_print`.
However, using `g_print` is out of harmony with GTK, which is a GUI library.
So, we will change the handler.
The following code is `lb3.c`.

~~~C
 1 static void
 2 click_cb (GtkButton *btn, GtkWindow *win) {
 3   gtk_window_destroy (win);
 4 }
 5 
 6 static void
 7 app_activate (GApplication *app) {
 8   GtkWidget *win;
 9   GtkWidget *btn;
10 
11   win = gtk_application_window_new (GTK_APPLICATION (app));
12   gtk_window_set_title (GTK_WINDOW (win), "lb3");
13   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
14 
15   btn = gtk_button_new_with_label ("Close");
16   gtk_window_set_child (GTK_WINDOW (win), btn);
17   g_signal_connect (btn, "clicked", G_CALLBACK (click_cb), win);
18 
19   gtk_window_present (GTK_WINDOW (win));
20 }
~~~

And the difference between `lb2.c` and `lb3.c` is as follows.

~~~
$ cd misc; diff lb2.c lb3.c
4,5c4,5
< click_cb (GtkButton *btn) {
<   g_print ("Clicked.\n");
---
> click_cb (GtkButton *btn, GtkWindow *win) {
>   gtk_window_destroy (win);
14c14
<   gtk_window_set_title (GTK_WINDOW (win), "lb2");
---
>   gtk_window_set_title (GTK_WINDOW (win), "lb3");
17c17
<   btn = gtk_button_new_with_label ("Click me");
---
>   btn = gtk_button_new_with_label ("Close");
19c19
<   g_signal_connect (btn, "clicked", G_CALLBACK (click_cb), NULL);
---
>   g_signal_connect (btn, "clicked", G_CALLBACK (click_cb), win);
29c29
<   app = gtk_application_new ("com.github.ToshioCP.lb2", G_APPLICATION_DEFAULT_FLAGS);
---
>   app = gtk_application_new ("com.github.ToshioCP.lb3", G_APPLICATION_DEFAULT_FLAGS);
35d34
< 
~~~

The changes are:

- The function `g_print` in `lb2.c` was deleted and two lines are inserted.
  - `click_cb` has the second parameter, which comes from the fourth argument of the `g_signal_connect` at the line 19.
One thing to be careful is the types are different between the second parameter of `click_cb` and the fourth argument of `g_signal_connect`.
The former is `GtkWindow *` and the latter is `GtkWidget *`.
The compiler doesn't complain because `g_signal_connect` uses gpointer (general type of pointer).
In this program the instance pointed by `win` is a GtkApplicationWindow object.
It is a descendant of GtkWindow and GtkWidget class, so both `GtkWindow *` and `GtkWidget *` are correct types for the instance.
  - `gtk_destroy (win)` destroys the top-level window. Then the application quits.
- The label of `btn` is changed from "Click me" to "Close".
- The fourth argument of `g_signal_connect` is changed from `NULL` to `win`.

The most important change is the fourth argument of the `g_signal_connect`.
This argument is described as "data to pass to handler" in the definition of [`g_signal_connect`](https://docs.gtk.org/gobject/func.signal_connect.html).

### GtkBox

GtkWindow and GtkApplicationWindow can have only one child.
If you want to add two or more widgets in a window, you need a container widget.
GtkBox is one of the containers.
It arranges two or more child widgets into a single row or column.
The following procedure shows the way to add two buttons in a window.

- Create a GtkApplicationWindow instance.
- Create a GtkBox instance and add it to the GtkApplicationWindow as a child.
- Create a GtkButton instance and append it to the GtkBox.
- Create another GtkButton instance and append it to the GtkBox.

After this, the Widgets are connected as the following diagram.

![Parent-child relationship](../image/box.png)

The program `lb4.c` is as follows.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 click1_cb (GtkButton *btn) {
 5   const char *s;
 6 
 7   s = gtk_button_get_label (btn);
 8   if (g_strcmp0 (s, "Hello.") == 0)
 9     gtk_button_set_label (btn, "Good-bye.");
10   else
11     gtk_button_set_label (btn, "Hello.");
12 }
13 
14 static void
15 click2_cb (GtkButton *btn, GtkWindow *win) {
16   gtk_window_destroy (win);
17 }
18 
19 static void
20 app_activate (GApplication *app) {
21   GtkWidget *win;
22   GtkWidget *box;
23   GtkWidget *btn1;
24   GtkWidget *btn2;
25 
26   win = gtk_application_window_new (GTK_APPLICATION (app));
27   gtk_window_set_title (GTK_WINDOW (win), "lb4");
28   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
29 
30   box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
31   gtk_box_set_homogeneous (GTK_BOX (box), TRUE);
32   gtk_window_set_child (GTK_WINDOW (win), box);
33 
34   btn1 = gtk_button_new_with_label ("Hello.");
35   g_signal_connect (btn1, "clicked", G_CALLBACK (click1_cb), NULL);
36 
37   btn2 = gtk_button_new_with_label ("Close");
38   g_signal_connect (btn2, "clicked", G_CALLBACK (click2_cb), win);
39 
40   gtk_box_append (GTK_BOX (box), btn1);
41   gtk_box_append (GTK_BOX (box), btn2);
42 
43   gtk_window_present (GTK_WINDOW (win));
44 }
45 
46 int
47 main (int argc, char **argv) {
48   GtkApplication *app;
49   int stat;
50 
51   app = gtk_application_new ("com.github.ToshioCP.lb4", G_APPLICATION_DEFAULT_FLAGS);
52   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
53   stat =g_application_run (G_APPLICATION (app), argc, argv);
54   g_object_unref (app);
55   return stat;
56 }
~~~

Look at the function `app_activate`.

After the creation of a GtkApplicationWindow instance, a GtkBox instance is created.

~~~C
box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
gtk_box_set_homogeneous (GTK_BOX (box), TRUE);
~~~

The first argument arranges the children of the box vertically.
The orientation constants are defined like this: 

- GTK\_ORIENTATION\_VERTICAL: the children widgets are arranged vertically
- GTK\_ORIENTATION\_HORIZONTAL: the children widgets are arranged horizontally

The second argument is the size between the children.
The unit of the length is pixel.

The next function fills the box with the children, giving them the same space.

After that, two buttons `btn1` and `btn2` are created and the signal handlers are set.
Then, these two buttons are appended to the box.

~~~C
 1 static void
 2 click1_cb (GtkButton *btn) {
 3   const char *s;
 4 
 5   s = gtk_button_get_label (btn);
 6   if (g_strcmp0 (s, "Hello.") == 0)
 7     gtk_button_set_label (btn, "Good-bye.");
 8   else
 9     gtk_button_set_label (btn, "Hello.");
10 }
~~~

The function `gtk_button_get_lable` returns a text from the label.
The string is owned by the button and you can't modify or free it.
The `const` qualifier is necessary for the string `s`.
If you change the string, your compiler will give you a waring.

You always need to be careful with the const qualifier when you see the GTK 4 API reference.

![Screenshot of the box](../image/screenshot_lb4.png)

The handler corresponds to `btn1` toggles its label.
The handler corresponds to `btn2` destroys the top-level window and the application quits.

Up: [README.md](../README.md),  Prev: [Section 3](sec3.md), Next: [Section 5](sec5.md)
