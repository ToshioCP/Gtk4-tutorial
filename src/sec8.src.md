# Defining a Child object

## A Very Simple Editor

In the previous section we made a very simple file viewer.
Now we go on to rewrite it and turn it into very simple editor.
Its source file is in tfe1.c (text file editor 1).

GtkTextView has a feature for editing  multiple lines. Therefore, we don't need to
write the program from scratch, we just add two things to the file viewer:

- Memory to store a pointer to the GFile instance.
- A function to write the file.

There are a couple of ways to store the details of GFile.

- Use global variables; or
- Make a child object, which can extend the instance memory for the GFile object.

Using global variables is easy to implement.
Define a sufficient size array of pointers to GFile.
For example,

~~~C
GFile *f[20];
~~~

The variable `f[i]` corresponds to the file associated to the i-th GtkNotebookPage.
There are however two problems with this.
The first concerns the size of the array.
If a user gives too many arguments (more than 20 in the example above), it is impossible to store the additional pointers to the GFile instances.
The second is the increasing difficulty for maintenance of the program.
We have a small program so far,
but however, if you continue developing it, the size of the program will grow.
Generally speaking, the bigger the program size, the more difficult it is to keep track of and maintain global variables. Global variables can be used and changed anywhere throughout the entire program.

Making a child object is a good idea in terms of maintenance.
One thing you need to be careful of is the difference between "child object" and "child widget".
Here we are describing a "child object".
A child object includes, and expands on its parent object, as
a child object derives everything from the parent object.

![Child object of GtkTextView](../image/child.png){width=9.675cm height=4.89cm}

We will define TfeTextView as a child object of GtkTextView.
It has everything that GtkTextView has.
Specifically, TfeTextView has a GtkTextbuffer which corresponds to the GtkTextView inside TfeTextView.
The additional important thing is that TfeTextView can also keep an additional pointer to GFile.

In general, this is how GObjects work. Understanding the general theory about Gobject's is difficult,
particularly for beginners.
So, I will just show you the way how to write the code and avoid the theoretical side.
If you want to know about GObject system, refer to the separate tutorial](https://github.com/ToshioCP/Gobject-tutorial).

## How to Define a Child Object of GtkTextView

Let's define the TfeTextView object, which is a child object of GtkTextView.
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

If you are curious about the background theory of this program, that's good,
because knowing the theory is very important if you want to program GTK applications.
Look at [GObject API Reference](https://docs.gtk.org/gobject/).
All you need is described there,
or refer to [GObject tutorial](https://github.com/ToshioCP/Gobject-tutorial).
It's a tough journey especially for beginners so for now, you don't need to know about this difficult theory.
It is enough to just remember the instructions below.

- TfeTextView is divided into two parts.
Tfe and TextView.
Tfe is called the prefix, namespace or module.
TextView is called the object.
- There are three differnet identifier patterns.
TfeTextView (camel case), tfe\_text\_view (this is used to write functions) and TFE\_TEXT\_VIEW (This is used to cast a pointer to point TfeTextView type).
- First, define TFE\_TYPE\_TEXT\_VIEW macro as tfe\_text\_view\_get\_type ().
The name is always (prefix)\_TYPE\_(object) and the letters are upper case.
And the replacement text is always (prefix)\_(object)\_get\_type () and the letters are lower case.
- Next, use G\_DECLARE\_FINAL\_TYPE macro.
The arguments are the child object name in camel case, lower case with underscore, prefix (upper case), object (upper case with underscore) and parent object name (camel case).
- Declare the structure \_TfeTextView.
The underscore is necessary.
The first member is the parent object.
Notice this is not a pointer but the object itself.
The second member and after are members of the child object.
TfeTextView structure has a pointer to a GFile instance as a member.
- Use G\_DEFINE\_TYPE macro.
The arguments are the child object name in camel case, lower case with underscore and parent object type (prefix)\_TYPE\_(module).
- Define instance init function (tfe\_text\_view\_init).
Usually you don't need to do anything.
- Define class init function (tfe\_text\_view\_class\_init).
You don't need to do anything in this object.
- Write function codes you want to add (tfe\_text\_view\_set\_file and tfe\_text\_view\_get\_file).
`tv` is a pointer to the TfeTextView object instance which is a C-structure declared with the tag \_TfeTextView.
So, the structure has a member `file` as a pointer to a GFile instance.
`tv->file = f` is an assignment of `f` to a member `file` of the structure pointed by `tv`.
This is an example how to use the extended memory in a child widget.
- Write a function to create an instance.
Its name is (prefix)\_(object)\_new.
If the parent object function needs parameters, this function also need them.
You sometimes might want to add some parameters.
It's your choice.
Use g\_object\_new function to create the instance.
The arguments are  (prefix)\_TYPE\_(object), a list to initialize properties and NULL.
In this code no property needs to be initialized.
And the return value is casted to GtkWidget.

This program is not perfect.
It has some problems.
It will be modified later.

## Close-request signal

Imagine that you are using this editor.
First, you run the editor with arguments.
The arguments are filenames.
The editor reads the files and shows the window with the text of files in it.
Then you edit the text.
After you finish editing, you exit the editor.
The editor updates files just before the window closes.

GtkWindow emits the "close-request" signal before it closes.
We connect the signal and the handler `before_close`.
A handler is a C function.
When a function is connected to a certain signal, we call it a handler.
The function `before_close` is invoked when the signal "close-request" is emitted.

~~~C
g_signal_connect (win, "close-request", G_CALLBACK (before_close), NULL);
~~~

The argument `win` is a GtkApplicationWindow, in which the signal "close-request" is defined, and `before_close` is the handler.
`G_CALLBACK` cast is necessary for the handler.
The program of `before_close` is as follows.

@@@include
tfe/tfe1.c before_close
@@@

The numbers on the left of items are line numbers in the source code.

- 15: Gets the number of pages `nb` has.
- 16-29: For loop with regard to the index to each pages.
- 17-19: Gets GtkScrolledWindow, TfeTextView and a pointer to GFile.
The pointer was stored when `app_open` handler had run. It will be shown later.
- 20-22: Gets GtkTextBuffer and contents. `start_iter` and `end_iter` are iterators of the buffer.
I don't want to explain them now because it would take a lot of time.
Just remember these lines for the present.
- 23-27: Writes the contents to the file.
If it fails, it outputs an error message.
- 28: Frees `contents`.

## Source code of tfe1.c

The following is the complete source code of `tfe1.c`.

@@@include
tfe/tfe1.c
@@@

- 107: Sets the pointer to GFile into TfeTextView.
`files[i]` is a pointer to GFile structure.
It will be freed by the system. So you need to copy it.
`g_file_dup` duplicates the given GFile structure.
- 123: Connects "close-request" signal and `before_close` handler.
The fourth argument is called user data and it is given to the signal handler.
So, `nb` is given to `before_close` as the second argument.

Now compile and run it.
There's a sample file in the directory `tfe`.
Type `./a.out taketori.txt`.
Modify the contents and close the window.
Make sure that the file is modified.

Now we got a very simple editor.
It's not smart.
We need more features like open, save, saveas, change font and so on.
We will add them in the next section and after.
