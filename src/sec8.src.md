# Defining a Final Class

## A Very Simple Editor

We made a very simple file viewer in the previous section.
Now we go on to rewrite it and turn it into a very simple editor.
Its source file is `tfe1.c` (text file editor 1) under [tfe](tfe/) directory.

GtkTextView is a multi-line editor.
So, we don't need to write the editor from scratch.
We just add two things to the file viewer:

- Pointers to GFile instances.
- A text-save function.

There are a couple of ways to store the pointers.

- Use global variables
- Make a child class of GtkTextView and its each instance holds a pointer to the GFile instance.

Using global variables is easy to implement.
Define a sufficient size pointer array to GFile.
For example,

~~~C
GFile *f[20];
~~~

The variable `f[i]` corresponds to the file associated with the i-th GtkNotebookPage.

However, there are two problems.
First, the array has a fixed size.
If a user provides too many arguments (more than 20 in the example above), it becomes impossible to store all the GFile instances.
Second, global variables make maintenance difficult.
Our program is small so far, but as a program grows larger, maintaining global variables becomes increasingly challenging.
When you need to check a global variable, you must examine every part of the code that references it.

Making a child class is a good idea in terms of maintenance.
We prefer this approach over using global variables.

Note that we are discussing "child classes," not "child widgets."
These two concepts are entirely different: a "class" is a term within the GObject system, whereas a "widget" refers to the UI hierarchy.
If you are not familiar with GObject, please see:

- [GObject API reference](https://docs.gtk.org/gobject/)
- [GObject tutorial for beginners](https://toshiocp.github.io/Gobject-tutorial/)

A child class inherits everything from the parent and, in addition, extends its performance.
We will define TfeTextView as a child class of GtkTextView.
It has everything that GtkTextView has and adds a pointer to a GFile.

@@@if pdf
![Child object of GtkTextView](/images/child.png){width=9.675cm height=4.89cm}
@@@else
![Child object of GtkTextView](/images/child.png)
@@@end

## How to Define a Child Class of GtkTextView

You need to know GObject system convention.
First, look at the program below.

@@@if gfm
```C
@@@else
```{.c}
@@@end
#define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type ()
G_DECLARE_FINAL_TYPE (TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView)

struct _TfeTextView
{
  GtkTextView parent;
  GFile *file;
};

G_DEFINE_FINAL_TYPE (TfeTextView, tfe_text_view, GTK_TYPE_TEXT_VIEW);

static void
tfe_text_view_init (TfeTextView *tv) {
}

static void
tfe_text_view_class_init (TfeTextViewClass *class) {
}

/* --- Here are some functions --- */

GtkWidget *
tfe_text_view_new (void) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_TEXT_VIEW, NULL));
}
```

- The name "TfeTextView" is composed of two parts: "Tfe" and "TextView."
"Tfe" serves as the prefix (or namespace), while "TextView" represents the object name.
- There are three different identifier patterns.
TfeTextView (camel case), tfe\_text\_view (this is used for functions) and TFE\_TEXT\_VIEW (This is used to cast an object to TfeTextView).
- First, define the `TFE_TYPE_TEXT_VIEW` macro as `tfe_text_view_get_type()`.
Macro names follow the pattern (prefix)\_TYPE\_(object) and are written in uppercase.
The replacement text is always (prefix)\_(object)\_get\_type () and are written in lower case.
This definition must be put before `G_DECLARE_FINAL_TYPE` macro.
- The arguments of `G_DECLARE_FINAL_TYPE` macro are the child class name in camel case, lower case with underscore, prefix (upper case),
object (upper case with underscore) and parent class name (camel case).
The following two C structures are declared in the expansion of the macro.
  - `typedef struct _TfeTextView TfeTextView`
  - `typedef struct {GtkTextViewClass parent_class; } TfeTextViewClass;`
- These declarations tell us that TfeTextView and TfeTextViewClass are C structures.
"TfeTextView" has two meanings, class name and C structure name.
The C structure TfeTextView is called an instance.
Similarly, TfeTextViewClass is called a class.
- Declare the structure `_TfeTextView`.
The underscore is necessary.
The first member is the parent object (C structure).
Notice this is not a pointer but the object itself.
The second member and later are members of the child object.
TfeTextView structure has a pointer to a GFile instance as a member.
- `G_DEFINE_FINAL_TYPE` macro.
The arguments are the child object name in camel case, lower case with underscore and parent object type (prefix)\_TYPE\_(module).
This macro is mainly used to register the new class to the type system.
The type system is a base system of GObject.
Every class has its own type.
The types of GObject, GtkWidget and TfeTextView are `G_TYPE_OBJECT`, `GTK_TYPE_WIDGET` and `TFE_TYPE_TEXT_VIEW` respectively.
For example, `TFE_TYPE_TEXT_VIEW` is a macro and it is expanded to a function `tfe_text_view_get_type()`.
It returns a unique GType identifier representing the type within the GObject.
- The instance init function `tfe_text_view_init` is called when the instance is created.
It is the same as a constructor in other object oriented languages.
- The class init function `tfe_text_view_class_init` is called when the class is created.
- Some public functions can be located between `tfe_text_view_class_init` and `tfe_text_view_new` functions.
Public functions are open to other instances and they can be called anywhere in the program.
They are the same as public method in other object oriented languages.
The variable `tv` contains a pointer to the TfeTextView object (C structure).
It has a member `file` and it is pointed to by `tv->file`.
- TfeTextView instance creation function is `tfe_text_view_new`.
Its name is (prefix)\_(object)\_new.
It uses `g_object_new` function to create the instance.
The arguments are (prefix)\_TYPE\_(object), a list to initialize properties and NULL.
NULL is the end mark of the property list.
No property is initialized here.
And the return value is cast to GtkWidget.

This program shows the overview of how to define a child class.

## Close-request Signal

Imagine that you are using this editor.
First, you run the editor with arguments.
The arguments are filenames.
The editor reads the files and shows the window with the text of files in it.
Then you edit the text.
After you finish editing, you click on the close button of the window and quit the editor.
The editor updates files just before the window closes.

GtkWindow emits the "close-request" signal when the close button is clicked.
We will connect the signal and the handler `before_close`.
(A handler is a C function which is connected to a signal.)
The function `before_close` is called when the signal "close-request" is emitted.

@@@if gfm
```C
@@@else
```{.c}
@@@end
g_signal_connect (win, "close-request", G_CALLBACK (before_close), NULL);
```

The signal "close-request" is defined on the GtkWindow class and it is inherited by the descendant classes.
The first argument `win` is a GtkApplicationWindow, which is the child class of the GtkWindow class.
The macro `g_signal_connect` connects the "close-request" signal to the `before_close` handler.
The handler needs `G_CALLBACK` cast.
The `before_close` function is as follows.

@@@include
tfe/tfe1.c before_close
@@@

The numbers on the left are line numbers.

- 15: The number of note book pages is assigned to `n`.
- 16-29: For loop with regard to the index to each page.
- 17-19: `scr`, `tv` and `file` hold the pointers to the GtkScrolledWindow, TfeTextView and GFile.
The function `tfe_text_view_get_file` returns the pointer to the GFile instance that `tv` holds.
The GFile instance is owned by `tv`.
The function will be shown later.
- 20-22: `tb` holds a pointer to the GtkTextBuffer of the TfeTextView.
The function `gtk_text_buffer_get_bounds` assigns the start and end iterators of the buffer to `start_iter` and `end_iter` respectively.
An iterator represents a specific position within the buffer.
Then, `gtk_text_buffer_get_text` returns the text between `start_iter` and `end_iter`, which encompasses the entire content of the buffer.
- 23-26: The text is saved to the file.
If it fails, error messages are displayed.
The GError instance must be freed and the pointer `err` needs to be NULL for the next run in the loop.
- 27: `contents` are freed.
- 28: GFile is no longer needed. The function `tfe_tet_view_set_file` decrements the reference count of the GFile within `tv` and replaces it with a new GFile or NULL.
Reference counts will be explained in a later section ([section 11](sec11.src.md)).
When the reference count reaches zero, the GFile instance is automatically destroyed.

## Source code of tfe1.c

The following is the whole source code of `tfe1.c`.

@@@include
tfe/tfe1.c
@@@

- 25-33: The function `tfe_text_view_set_file` decreases the reference count of `tv->file` by one (`g_object_unref`).
This means `tv` releases the GFile instance ownership.
Then, the function replaces `tv->file` with the new GFile `f` or NULL.
If it is a GFile, the function increases the reference count of `f` by one and `tv` gets the ownership of `f`.
For reference count, see [Section 11](sec11.src.md) for further information.
- 131: The "close-request" signal is connected to `before_close` handler.
The fourth argument is called "user data" and it will be the second argument of the signal handler.
So, `nb` is given to `before_close` as the second argument.

Now it's time to compile and run.

~~~
$ cd src/tfe
$ comp tfe1
$ ./a.out taketori.txt`.
~~~

Modify the contents and close the window.
Make sure that the file is modified.

Now we got a very simple editor.
It's not smart.
We need more features like open, save, saveas, change font and so on.
We will add them in the following sections.
