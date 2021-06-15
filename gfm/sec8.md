Up: [Readme.md](../Readme.md),  Prev: [Section 7](sec7.md), Next: [Section 9](sec9.md)

# Define Child object

## Very simple editor

We made a very simple file viewer in the previous section.
Now we go on to rewrite it and make a very simple editor.
Its source file name is tfe1.c (text file editor 1).

GtkTextView originally has a feature of multi line editing.
Therefore, we don't need to rewrite the program from scratch.
We just add two things to the file viewer.

- Static memory is needed to store a pointer to GFile.
- We need to implement file write function.

A couple of ways are possible to get memories to keep GFile.

- Use global variables.
- make a child object so that it can extend the memories for the GFile object.

Using global variables is easy to implement.
Define a sufficient size array of pointers to GFile.
For example,

~~~C
GFile *f[20];
~~~

And `f[i]` corresponds to i-th GtkNotebookPage.
However, there are two problems.
One is the size of the array.
If a user gives arguments more than that, bad thing may happen.
The other is the difficulty of maintenance of the program.
It is a small program so far.
However, if you continue developing it, then its size grows bigger and bigger.
Generally speaking, the bigger the program size, the more difficult to maintain global variables.

Making child object is a good idea in terms of maintenance.
However, one thing you need to be careful is the difference between "child object" and "child widget".
What we are thinking about now is "child object".
A child object includes its parent object.
And a child object derives everything from the parent object.
 
![Child widget of GtkTextView](../image/child.png)

We will define TfeTextView as a child object of GtkTextView.
It has everything that GtkTextView has.
For example, TfeTextView has GtkTextbuffer corresponds to GtkTextView inside TfeTextView.
And important thing is that TfeTextView can have a memory to keep a pointer to GFile.

However, to understand the general theory about Gobject is very hard especially for beginners.
So, I will just show you the way how to write the code and avoid the theoretical side in the next subsection.

## How to define a child widget of GtkTextView

Let's define TfeTextView object which is a child object of GtkTextView.
First, look at the program below.

~~~C
#define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type ()
G_DECLARE_FINAL_TYPE (TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView)

struct _TfeTextView
{
  GtkTextView parent;
  GFile *file;
};

G_DEFINE_TYPE (TfeTextView, tfe_text_view, GTK_TYPE_TEXT_VIEW);

static void
tfe_text_view_init (TfeTextView *tv) {
}

static void
tfe_text_view_class_init (TfeTextViewClass *class) {
}

void
tfe_text_view_set_file (TfeTextView *tv, GFile *f) {
  tv -> file = f;
}

GFile *
tfe_text_view_get_file (TfeTextView *tv) {
  return tv -> file;
}

GtkWidget *
tfe_text_view_new (void) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_TEXT_VIEW, NULL));
}
~~~

If you are curious about the background theory of this program, It's very good for you.
Because knowing the theory is very important for you to program GTK applications.
Look at [GObject API reference](https://developer.gnome.org/gobject/stable/).
All you need is described in it.
However, it's a tough journey especially for beginners.
For now, you don't need to know such difficult theory.
Just remember the instructions below. 

- TfeTextView is divided into two parts.
Tfe and TextView.
Tfe is called prefix, namespace or module.
TextView is called object.
- There are three patterns.
TfeTextView (camel case), tfe\_text\_view (this is used to write functions) and TFE\_TEXT\_VIEW (This is used to write casts).
- First, define TFE\_TYPE\_TEXT\_VIEW as tfe\_text\_view\_get\_type ().
The name is always (prefix)\_TYPE\_(object) and the letters are upper case.
And the replacement text is always (prefix)\_(object)\_get\_type () and the letters are lower case.
- Next, use G\_DECLARE\_FINAL\_TYPE macro.
The arguments are the child object name in camel case, lower case with underscore, prefix (upper case), object (upper case with underscore) and parent object name (camel case).
- Declare the structure \_TfeTextView.
The underscore is necessary.
The first member is the parent object.
Notice this is not a pointer but the object itself.
The second member and after are members of the child object.
TfeTextView structure has a pointer to GFile as a member.
- Use G\_DEFINE\_TYPE macro.
The arguments are the child object name in camel case, lower case with underscore and parent object type (prefix)\_TYPE\_(module).
- Define instance init function (tfe\_text\_view\_init).
Usually you don't need to do anything.
- Define class init function (tfe\_text\_view\_class\_init).
You don't need to do anything in this widget.
- Write function codes you want to add (tfe\_text\_view\_set\_file and tfe\_text\_view\_get\_file).
`tv` is a pointer to TfeTextView object instance which is a C-structure declared with the tag \_TfeTextView.
So, the structure has a member `file` as a pointer to GFile.
`tv->file = f` is an assignment of `f` to a member `file` of the structure pointed by `tv`. 
This is an example how to use the extended memory in a child widget.
- Write object generation function.
Its name is (prefix)\_(object)\_new.
If the parent object function needs parameters, this function also need them.
You sometimes might want to add some parameters.
It's your choice.
Use g\_object\_new function to generate the child widget.
The arguments are  (prefix)\_TYPE\_(object), a list to initialize properties and NULL.
In this code no property needs to be initialized.
And the return value must be casted to GtkWidget.

This program is not perfect.
It has some problems.
But I don't discuss them now.
It will be modified later.

## Close-request signal

Imagine that you use this editor.
First, you run the editor with arguments.
The arguments are filenames.
The editor reads the files and shows its window with the text of files in it.
Then you edit the text.
After you finish editing, you exit the editor.
The editor updates files just before the window closes.

GtkWindow emits "close-request" signal before it closes.
We connect the signal and the handler `before_close`.
A handler is a C function.
When a function is connected to a certain signal, we call it handler.
The function `before_close` is invoked when the signal "close-request" is emitted.

~~~C
g_signal_connect (win, "close-request", G_CALLBACK (before_close), NULL);
~~~

The argument `win` is GtkApplicationWindow, in which the signal "close-request" is defined, and `before_close` is the handler.
`G_CALLBACK` cast is necessary for the handler.
The program of `before_close` is as follows.

~~~C
 1 static gboolean
 2 before_close (GtkWindow *win, GtkWidget *nb) {
 3   GtkWidget *scr;
 4   GtkWidget *tv;
 5   GFile *file;
 6   GtkTextBuffer *tb;
 7   GtkTextIter start_iter;
 8   GtkTextIter end_iter;
 9   char *contents;
10   unsigned int n;
11   unsigned int i;
12 
13   n = gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb));
14   for (i = 0; i < n; ++i) {
15     scr = gtk_notebook_get_nth_page (GTK_NOTEBOOK (nb), i);
16     tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
17     file = tfe_text_view_get_file (TFE_TEXT_VIEW (tv));
18     tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
19     gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
20     contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
21     if (! g_file_replace_contents (file, contents, strlen (contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, NULL))
22       g_print ("ERROR : Can't save %s.", g_file_get_path (file));
23   }
24   return FALSE;
25 }
~~~

The numbers on the left of items are line numbers in the source code.

- 13: Gets the number of pages `nb` has.
- 14-23: For loop with regard to the index to each pages.
- 15-17: Gets GtkScrolledWindow, TfeTextView and a pointer to GFile.
The pointer was stored when `on_open` handler had run. It will be shown later.
- 18-20: Gets GtkTextBuffer and contents. `start_iter` and `end_iter` are iterators of the buffer.
I don't want to explain them now because it would take a lot of time.
Just remember these lines for the present.
- 21: Writes the file.

## Source code of tfe1.c

Now I will show you all the source code of `tfe1`.c.

~~~C
  1 #include <gtk/gtk.h>
  2 
  3 /* Define TfeTextView Widget which is the child object of GtkTextView */
  4 
  5 #define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type ()
  6 G_DECLARE_FINAL_TYPE (TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView)
  7 
  8 struct _TfeTextView
  9 {
 10   GtkTextView parent;
 11   GFile *file;
 12 };
 13 
 14 G_DEFINE_TYPE (TfeTextView, tfe_text_view, GTK_TYPE_TEXT_VIEW);
 15 
 16 static void
 17 tfe_text_view_init (TfeTextView *tv) {
 18 }
 19 
 20 static void
 21 tfe_text_view_class_init (TfeTextViewClass *class) {
 22 }
 23 
 24 void
 25 tfe_text_view_set_file (TfeTextView *tv, GFile *f) {
 26   tv -> file = f;
 27 }
 28 
 29 GFile *
 30 tfe_text_view_get_file (TfeTextView *tv) {
 31   return tv -> file;
 32 }
 33 
 34 GtkWidget *
 35 tfe_text_view_new (void) {
 36   return GTK_WIDGET (g_object_new (TFE_TYPE_TEXT_VIEW, NULL));
 37 }
 38 
 39 /* ---------- end of the definition of TfeTextView ---------- */
 40 
 41 static gboolean
 42 before_close (GtkWindow *win, GtkWidget *nb) {
 43   GtkWidget *scr;
 44   GtkWidget *tv;
 45   GFile *file;
 46   GtkTextBuffer *tb;
 47   GtkTextIter start_iter;
 48   GtkTextIter end_iter;
 49   char *contents;
 50   unsigned int n;
 51   unsigned int i;
 52 
 53   n = gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb));
 54   for (i = 0; i < n; ++i) {
 55     scr = gtk_notebook_get_nth_page (GTK_NOTEBOOK (nb), i);
 56     tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
 57     file = tfe_text_view_get_file (TFE_TEXT_VIEW (tv));
 58     tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
 59     gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
 60     contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
 61     if (! g_file_replace_contents (file, contents, strlen (contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, NULL))
 62       g_print ("ERROR : Can't save %s.", g_file_get_path (file));
 63   }
 64   return FALSE;
 65 }
 66 
 67 static void
 68 on_activate (GApplication *app, gpointer user_data) {
 69   g_print ("You need to give filenames as arguments.\n");
 70 }
 71 
 72 static void
 73 on_open (GApplication *app, GFile ** files, gint n_files, gchar *hint, gpointer user_data) {
 74   GtkWidget *win;
 75   GtkWidget *nb;
 76   GtkWidget *lab;
 77   GtkNotebookPage *nbp;
 78   GtkWidget *scr;
 79   GtkWidget *tv;
 80   GtkTextBuffer *tb;
 81   char *contents;
 82   gsize length;
 83   char *filename;
 84   int i;
 85 
 86   win = gtk_application_window_new (GTK_APPLICATION (app));
 87   gtk_window_set_title (GTK_WINDOW (win), "file editor");
 88   gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
 89   gtk_window_maximize (GTK_WINDOW (win));
 90 
 91   nb = gtk_notebook_new ();
 92   gtk_window_set_child (GTK_WINDOW (win), nb);
 93 
 94   for (i = 0; i < n_files; i++) {
 95     if (g_file_load_contents (files[i], NULL, &contents, &length, NULL, NULL)) {
 96       scr = gtk_scrolled_window_new ();
 97       tv = tfe_text_view_new ();
 98       tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
 99       gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
100       gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
101 
102       tfe_text_view_set_file (TFE_TEXT_VIEW (tv),  g_file_dup (files[i]));
103       gtk_text_buffer_set_text (tb, contents, length);
104       g_free (contents);
105       filename = g_file_get_basename (files[i]);
106       lab = gtk_label_new (filename);
107       gtk_notebook_append_page (GTK_NOTEBOOK (nb), scr, lab);
108       nbp = gtk_notebook_get_page (GTK_NOTEBOOK (nb), scr);
109       g_object_set (nbp, "tab-expand", TRUE, NULL);
110       g_free (filename);
111     } else {
112       filename = g_file_get_path (files[i]);
113       g_print ("No such file: %s.\n", filename);
114       g_free (filename);
115     }
116   }
117   if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) > 0) {
118     g_signal_connect (win, "close-request", G_CALLBACK (before_close), nb);
119     gtk_widget_show (win);
120   } else
121     gtk_window_destroy (GTK_WINDOW (win));
122 }
123 
124 int
125 main (int argc, char **argv) {
126   GtkApplication *app;
127   int stat;
128 
129   app = gtk_application_new ("com.github.ToshioCP.tfe1", G_APPLICATION_HANDLES_OPEN);
130   g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
131   g_signal_connect (app, "open", G_CALLBACK (on_open), NULL);
132   stat =g_application_run (G_APPLICATION (app), argc, argv);
133   g_object_unref (app);
134   return stat;
135 }
136 
~~~

- 102: Sets the pointer to GFile into TfeTextView.
`files[i]` is a pointer to GFile structure.
It will be freed by the system. So you need to copy it.
`g_file_dup` duplicates the given GFile structure.
- 118: Connects "close-request" signal and `before_close` handler.
The fourth argument is called user data and it is given to the signal handler.
So, `nb` is given to `before_close` as the second argument.

Now compile and run it.
Type `./a.out somefile` and make sure that the file is modified.

Now we got a very simple editor.
It's not smart.
We need more features like open, save, saveas, change font and so on.
We will add them in the next section and after.


Up: [Readme.md](../Readme.md),  Prev: [Section 7](sec7.md), Next: [Section 9](sec9.md)
