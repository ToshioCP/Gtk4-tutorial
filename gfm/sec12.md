Up: [README.md](../README.md),  Prev: [Section 11](sec11.md), Next: [Section 13](sec13.md)

# Signals

## Signals

Each object is encapsulated in Gtk programming.
And it is not recommended to use global variables because they are prone to make the program complicated.
So, we need something to communicate between objects.
There are two ways to do so.

- Instance methods:
Instance methods are functions on instances.
For example, `tb = gtk_text_view_get_buffer (tv)` is an instance method on the instance `tv`.
The caller requests `tv` to give `tb`, which is a GtkTextBuffer instance connected to `tv`.
- Signals:
For example, `activate` signal on GApplication object.
When the application is activated, the signal is emitted.
Then the handler, which has been connected to the signal, is invoked.

The caller of methods or signals are usually out of the object.
One of the difference between these two is that the object is active or passive.
In methods, objects passively respond to the caller.
In signals, objects actively send signals to handlers.

GObject signals are registered, connected and emitted.

1. Signals are registered in the class.
The registration is done usually when the class is initialized.
Signals can have a default handler, which is sometimes called "object method handler".
It is not a user handler connected by `g_signal_connect` family functions.
A default handler is always called on any instance of the class.
1. Signals are connected to handlers by the macro `g_signal_connect` or its family functions.
The connection is usually done out of the object.
One important thing is that signals are connected on a certain instance.
Suppose there exist two GtkButton instances A, B and a function C.
Even if you connected the "clicked" signal on A to C, B and C are *not* connected.
1. When Signals are emitted, the connected handlers are invoked.
Signals are emitted on the instance of the class.

## Signal registration

In TfeTextView, two signals are registered.

- "change-file" signal.
This signal is emitted when `tv->file` is changed.
- "open-response" signal.
The function `tfe_text_view_open` doesn't return the status because it can't get the status from the file chooser dialog.
(Instead, the call back function gets the status.)
This signal is emitted instead of the return value of the function.

A static variable or array is used to store signal ID.

~~~C
enum {
  CHANGE_FILE,
  OPEN_RESPONSE,
  NUMBER_OF_SIGNALS
};

static guint tfe_text_view_signals[NUMBER_OF_SIGNALS];
~~~

Signals are registered in the class initialization function.

~~~C
 1 static void
 2 tfe_text_view_class_init (TfeTextViewClass *class) {
 3   GObjectClass *object_class = G_OBJECT_CLASS (class);
 4 
 5   object_class->dispose = tfe_text_view_dispose;
 6   tfe_text_view_signals[CHANGE_FILE] = g_signal_new ("change-file",
 7                                  G_TYPE_FROM_CLASS (class),
 8                                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
 9                                  0 /* class offset */,
10                                  NULL /* accumulator */,
11                                  NULL /* accumulator data */,
12                                  NULL /* C marshaller */,
13                                  G_TYPE_NONE /* return_type */,
14                                  0     /* n_params */
15                                  );
16   tfe_text_view_signals[OPEN_RESPONSE] = g_signal_new ("open-response",
17                                  G_TYPE_FROM_CLASS (class),
18                                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
19                                  0 /* class offset */,
20                                  NULL /* accumulator */,
21                                  NULL /* accumulator data */,
22                                  NULL /* C marshaller */,
23                                  G_TYPE_NONE /* return_type */,
24                                  1     /* n_params */,
25                                  G_TYPE_INT
26                                  );
27 }
~~~

- 6-15: Registers "change-file" signal.
`g_signal_new` function is used.
The signal "change-file" has no default handler (object method handler) so the offset (the line 9) is set to zero.
You usually don't need a default handler.
If you need it, use `g_signal_new_class_handler` function instead of `g_signal_new`.
See [GObject API Reference](https://docs.gtk.org/gobject/func.signal_new_class_handler.html) for further information.
- The return value of `g_signal_new` is the signal id.
The type of signal id is guint, which is the same as unsigned int.
It is used in the function `g_signal_emit`.
- 16-26: Registers "open-response" signal.
This signal has a parameter.
- 24: Number of the parameters.
"open-response" signal has one parameter.
- 25: The type of the parameter.
`G_TYPE_INT` is a type of integer.
Such fundamental types are described in [GObject reference manual](https://developer-old.gnome.org/gobject/stable/gobject-Type-Information.html).

The handlers are declared as follows.

~~~C
/* "change-file" signal handler */
void
user_function (TfeTextView *tv,
               gpointer user_data)

/* "open-response" signal handler */
void
user_function (TfeTextView *tv,
               TfeTextViewOpenResponseType response-id,
               gpointer user_data)
~~~

- The signal "change-file" doesn't have parameter, so the handler's arguments are a TfeTextView instance and a user data.
- The signal "open-response" signal has one parameter and its arguments are a TfeTextView instance, the signal's parameter and user data.
- The variable `tv` points the instance on which the signal is emitted.
- The last argument `user_data` comes from the fourth argument of `g_signal_connect`.
- The `parameter` (`response-id`) comes from the fourth argument of `g_signal_emit`.

The values of the type `TfeTextViewOpenResponseType` are defined in `tfetextview.h`.

```C
/* "open-response" signal response */
enum TfeTextViewOpenResponseType
{
  TFE_OPEN_RESPONSE_SUCCESS,
  TFE_OPEN_RESPONSE_CANCEL,
  TFE_OPEN_RESPONSE_ERROR
};
```

- The parameter is set to `TFE_OPEN_RESPONSE_SUCCESS` when `tfe_text_view_open` has successfully opened a file and read it.
- The parameter is set to `TFE_OPEN_RESPONSE_CANCEL` when the user has canceled.
- The parameter is set to `TFE_OPEN_RESPONSE_ERROR` when an error has occurred.
 
## Signal connection

A signal and a handler are connected by the function macro `g_signal_connect`.
There are some similar function macros like `g_signal_connect_after`, `g_signal_connect_swapped` and so on.
However, `g_signal_connect` is used most often.
The signals "change-file" and "open-response" are connected to their callback functions out of the TfeTextView object.
Those callback functions are defined by users.

For example, callback functions are defined in `src/tfe6/tfewindow.c` and their names are `file_changed_cb` and `open_response_cb`.
They will be explained later.

~~~C
g_signal_connect (GTK_TEXT_VIEW (tv), "change-file", G_CALLBACK (file_changed_cb), nb);

g_signal_connect (TFE_TEXT_VIEW (tv), "open-response", G_CALLBACK (open_response_cb), nb);
~~~

## Signal emission

A signal is emitted on the instance.
A function `g_signal_emit` is used to emit the signal.
The following lines are extracted from `src/tfetextview/tfetextview.c`.
Each line comes from a different line.

~~~C
g_signal_emit (tv, tfe_text_view_signals[CHANGE_FILE], 0);
g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_SUCCESS);
g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_CANCEL);
g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_ERROR);
~~~

- The first argument is the instance on which the signal is emitted.
- The second argument is the signal id.
- The third argument is the detail of the signal.
The signals "change-file" and "open-response" don't have details and the arguments are zero.
- The signal "change-file" doesn't have parameters, so there's no fourth argument.
- The signal "open-response" has one parameter.
The fourth argument is the parameter.

Up: [README.md](../README.md),  Prev: [Section 11](sec11.md), Next: [Section 13](sec13.md)
