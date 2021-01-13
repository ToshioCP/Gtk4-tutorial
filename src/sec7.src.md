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
- make a child widget object and extend the memories allocated to the widget.

Using global variables is easy to implement.
Define a sufficient size array of pointers to GFile.
For example,

    GFile *f[20];

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
 
![Child widget of GtkTwxtView](../image/child.png){width=9.675cm height=4.89cm}

We will define TfeTextView as a child object of GtkTextView.
It has everything that GtkTextView has.
For example, TfeTextView has GtkTextbuffer correspods to GtkTextView inside TfeTextView.
And important thing is that TfeTextView can have a memory to keep a pointer to GFile.

However, to understand the general theory about gobjects is very hard especially for beginners.
So, I will just show you the way how to write the code and avoid the theoretical side in the next section.

## How to define a child widget of GtkTextView


Let's define TfeTextView object which is a child object of GtkTextView.
First, look at the program below.

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

If you are curious about the background theory of this program, It's very good for you.
Because to know the theory is very important for you to program GTK applications.
Look at GObject API reference.
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
`tv` is a pointer to TfeTextView object instance which is a C-struture declared with the tag \_TfeTextView.
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
It has some problem.
But I don't discuss it now.
It will be modified later.

## Close-request signal

After editing a file, `tfe1.c` writes files just before the window closes.
GtkWindow emits "close-request" signal before it closes.
We connect the signal and the handler `before_close`.
A handler is a C function.
When a function is connected to a certain signal, we call it handler.
The function `before_close` is invoked when the signal "close-request" is emittd.

    g_signal_connect (win, "close-request", G_CALLBACK (before_close), NULL);

The argument win is GtkApplicationWindow, in which the signal "close-request" is defined, and before\_close is the handler.
`G_CALLBACK` cast is necessary for the handler.
The program of before\_close is as follows.

@@@ tfe/tfe1.c before_close

The numbers on the left of items are line numbers in the source code.

- 13: Get the number of pages `nb` has.
- 14-23: For loop with regard to the index to each pages.
- 15-17: Get GtkScrolledWindow, TfeTextView and a pointer to GFile. The pointer was stored when `on_open` handler had run. It will be shown later.
- 18-20: Get GtkTextBuffer and contents. start\_iter and end\_iter is iterators of the buffer. I don't want to explain them now because it would take a lot of time. Just remember these lines for the present.
- 21: Write the file.

## Source code of tfe1.c

Now I will show you all the source code of `tfe1`.c.

@@@ tfe/tfe1.c

- 102: set the pointer to GFile into TfeTextView.
`files[i]` is a pointer to GFile structure.
It will be freed by the system. So you need to copy it.
`g_file_dup` duplicate the given GFile structure.
- 118: connect "close-request" signal and `before_close` handler.
The fourth argument is called user data and it is given to the signal handler.
So, `nb` is given to `before_close` as the second argument.

Now compile and run it.
Type `./a.out somefile` and make sure that the file is modified.

Now we got a very simple editor.
It's not smart.
We need more features like open, save, saveas, change font and so on.
We will add them in the next section and after.

