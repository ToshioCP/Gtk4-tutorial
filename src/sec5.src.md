# Widgets (2)

## GtkTextView, GtkTextbuffer and GtkScrolledWindow

### GtkTextView and GtkTextBuffer

GtkTextview is a widget for multi-line text editing.
GtkTextBuffer is a text buffer which is connected to GtkTextView.
See the sample program `tfv1.c` below.

@@@include
tfv/tfv1.c
@@@

Look at line 25.
A GtkTextView instance is created and its pointer is assigned to `tv`.
When the GtkTextView instance is created, a GtkTextBuffer instance is also created and connected to the GtkTextView automatically.
"GtkTextBuffer instance" will be referred to simply as "GtkTextBuffer" or "buffer".
In the next line, the pointer to the buffer is got and assigned to `tb`.
Then, the text from line 10 to 20 is assigned to the buffer.

GtkTextView has a wrap mode.
When it is set to `GTK_WRAP_WORD_CHAR`, text wraps in between words, or if that is not enough, also between graphemes.

In line 30, `tv` is added to `win` as a child.

Now compile and run it.

![GtkTextView](../image/screenshot_tfv1.png){width=6.3cm height=5.325cm}

There's an I-beam pointer in the window.
You can add or delete any characters on the GtkTextview.
And your change is kept in the GtkTextBuffer.
If you add more characters than the limit of the window, the height of the window extends.
If the height gets bigger than the height of the display screen, you won't be able to control the size of the window back to the original size.
It's a problem and it shows that there exists a bug in the program.
You can solve it by putting a GtkScrolledWindow between the GtkApplicationWindow and GtkTextView.

### GtkScrolledWindow

What we need to do is:

- Create a GtkScrolledWindow instance and insert it to the GtkApplicationWindow as a child.
- insert the GtkTextView to the GtkScrolledWindow as a child.

Modify `tfv1.c` and save it as `tfv2.c`.
The difference between these two files is very little.

@@@shell
cd tfv; diff tfv1.c tfv2.c
@@@

Though you can modify the source file by this diff output, It's good for you to show `tfv2.c`.

@@@include
tfv/tfv2.c
@@@

Now compile and run it.
This time the window doesn't extend even if you type a lot of characters.
It just scrolls.

