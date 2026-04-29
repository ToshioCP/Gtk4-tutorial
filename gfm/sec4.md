Up: [README.md](../README.md),  Prev: [Section 3](sec3.md), Next: [Section 5](sec5.md)

# Widgets (1)

## GtkLabel, GtkButton and GtkBox

### GtkLabel

We made a window and showed it on the screen in the previous section.
Now we go on to the next topic: widgets.
The simplest widget is GtkLabel.
It is a widget with text in it.

```c
#include <gtk/gtk.h>

static void
app_activate (GApplication *app) {
  GtkWidget *win;
  GtkWidget *lab;

  win = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_title (GTK_WINDOW (win), "lb1");
  gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);

  lab = gtk_label_new ("Hello.");
  gtk_window_set_child (GTK_WINDOW (win), lab);

  gtk_window_present (GTK_WINDOW (win));
}

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new ("com.github.ToshioCP.lb1", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

```

Save this program to a file `lb1.c`.
(You can use [/src/misc/lb1.c](../src/misc/lb1.c) if you've downloaded this repository.)
Then compile and run it.

    $ cd src/misc
    $ comp lb1
    $ ./a.out

A window with a message "Hello." appears.

![Screenshot of the label](/src/images/screenshot_lb1.png)

There are only a few changes between `pr4.c` and `lb1.c`.
A program `diff` is useful to know the difference.

```
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
$
```

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

```c
#include <gtk/gtk.h>

static void
click_cb (GtkButton *btn) {
  g_print ("Clicked.\n");
}

static void
app_activate (GApplication *app) {
  GtkWidget *win;
  GtkWidget *btn;

  win = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_title (GTK_WINDOW (win), "lb2");
  gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);

  btn = gtk_button_new_with_label ("Click me");
  gtk_window_set_child (GTK_WINDOW (win), btn);
  g_signal_connect (btn, "clicked", G_CALLBACK (click_cb), NULL);

  gtk_window_present (GTK_WINDOW (win));
}

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new ("com.github.ToshioCP.lb2", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

```

Look at the line 17 to 19.
First, it creates a GtkButton instance `btn` with a label "Click me".
Then, adds the button to the window `win` as a child.
Finally, connects the "clicked" signal of the button to the handler `click_cb`.
So, if `btn` is clicked, the function `click_cb` is invoked.
The suffix "cb" means "call back".

Name the program `lb2.c` and save it.
Now compile and run it.

![Screenshot of the label](/src/images/screenshot_lb2.png)

A window with the button appears.
Click the button (it is a large button, you can click everywhere in the window), then a string "Clicked." appears on the terminal.
It shows the handler was invoked by clicking the button.

It's good that we've made sure that the clicked signal was caught and the handler was invoked by using `g_print`.
However, using `g_print` doesn't fit well with GTK, which is a GUI library.
So, we will change the handler.
The following code is extracted from `lb3.c`.

```c
static void
click_cb (GtkButton *btn, GtkWindow *win) {
  gtk_window_destroy (win);
}

static void
app_activate (GApplication *app) {
  GtkWidget *win;
  GtkWidget *btn;

  win = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_title (GTK_WINDOW (win), "lb3");
  gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);

  btn = gtk_button_new_with_label ("Close");
  gtk_window_set_child (GTK_WINDOW (win), btn);
  g_signal_connect (btn, "clicked", G_CALLBACK (click_cb), win);

  gtk_window_present (GTK_WINDOW (win));
}
```

And the difference between `lb2.c` and `lb3.c` is as follows.

```
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
$
```

The changes are:

- The function `g_print` in `lb2.c` was deleted and two lines are inserted.
  - `click_cb` has the second parameter, which comes from the fourth argument of the `g_signal_connect` at line 17.
One thing to be careful is the types are different between the second parameter of `click_cb` and the fourth argument of `g_signal_connect`.
The former is `GtkWindow *` and the latter is `GtkWidget *`.
The compiler doesn't complain because `g_signal_connect` uses gpointer (general type of pointer).
In this program the instance pointed to by `win` is a GtkApplicationWindow object.
It is a descendant of GtkWindow and GtkWidget class, so both `GtkWindow *` and `GtkWidget *` are correct types of the instance.
  - `gtk_window_destroy (win)` destroys the top-level window. Then the application quits.
- The label of `btn` is changed from "Click me" to "Close".
- The fourth argument of `g_signal_connect` is changed from `NULL` to `win`.

The most important change is the fourth argument of the `g_signal_connect`.
This argument is described as "data to pass to the handler" in the definition of [g\_signal\_connect](https://docs.gtk.org/gobject/func.signal_connect.html).

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

After this, the widgets are connected as shown in the following diagram.

![Parent-child relationship](/src/images/box.png)

The program `lb4.c` is as follows.

```c
#include <gtk/gtk.h>

static void
click1_cb (GtkButton *btn) {
  const char *s;

  s = gtk_button_get_label (btn);
  if (g_strcmp0 (s, "Hello.") == 0)
    gtk_button_set_label (btn, "Good-bye.");
  else
    gtk_button_set_label (btn, "Hello.");
}

static void
click2_cb (GtkButton *btn, GtkWindow *win) {
  gtk_window_destroy (win);
}

static void
app_activate (GApplication *app) {
  GtkWidget *win;
  GtkWidget *box;
  GtkWidget *btn1;
  GtkWidget *btn2;

  win = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_title (GTK_WINDOW (win), "lb4");
  gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_set_homogeneous (GTK_BOX (box), TRUE);
  gtk_window_set_child (GTK_WINDOW (win), box);

  btn1 = gtk_button_new_with_label ("Hello.");
  g_signal_connect (btn1, "clicked", G_CALLBACK (click1_cb), NULL);

  btn2 = gtk_button_new_with_label ("Close");
  g_signal_connect (btn2, "clicked", G_CALLBACK (click2_cb), win);

  gtk_box_append (GTK_BOX (box), btn1);
  gtk_box_append (GTK_BOX (box), btn2);

  gtk_window_present (GTK_WINDOW (win));
}

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new ("com.github.ToshioCP.lb4", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
```

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

The second argument is the size of the space between the children.
The unit of the length is pixel.

The next function `gtk_box_set_homogeneous` fills the box with the children, giving them the same space.

After that, two buttons `btn1` and `btn2` are created and the signal handlers are set.
Then, these two buttons are appended to the box.

```c
static void
click1_cb (GtkButton *btn) {
  const char *s;

  s = gtk_button_get_label (btn);
  if (g_strcmp0 (s, "Hello.") == 0)
    gtk_button_set_label (btn, "Good-bye.");
  else
    gtk_button_set_label (btn, "Hello.");
}
```

The function `gtk_button_get_label` returns the text from the label.
The string is owned by the button and you can't modify or free it.
The `const` qualifier is necessary for the string `s`.
If you change the string, your compiler will give you a warning.

You always need to be careful with the const qualifier when you see the GTK 4 API reference.

![Screenshot of the box](/src/images/screenshot_lb4.png)

The handler corresponding to `btn1` toggles its label.
The handler corresponding to `btn2` destroys the top-level window and the application quits.

Up: [README.md](../README.md),  Prev: [Section 3](sec3.md), Next: [Section 5](sec5.md)
