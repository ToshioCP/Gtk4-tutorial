Up: [README.md](../README.md),  Prev: [Section 11](sec11.md), Next: [Section 13](sec13.md)

# Signals

## Signals

Each object is encapsulated in Gtk programming.
And it is not recommended to use global variables because they are prone to make the program complicated.
So, we need something to communicate between objects.
There are two ways to do so.

- Functions.
For example, `tb = gtk_text_view_get_buffer (tv)`.
The caller requests `tv` to give `tb`, which is a GtkTextBuffer instance connected to `tv`.
- Signals.
For example, `activate` signal on GApplication object.
When the application is activated, the signal is emitted.
Then the handler, which has been connected to the signal, is invoked.

The caller of the function or the handler connected to the signal is usually out of the object.
One of the difference between these two is that the object is active or passive.
In functions the object passively responds to the caller.
In signals the object actively sends a signal to the handler.

GObject signals are registered, connected and emitted.

1. Signals are registered with the object type on which they are emitted.
The registration is done usually when the object class is initialized.
2. Signals are connected to handlers by `g_connect_signal` or its family functions.
The connection is usually done out of the object.
3. When Signals are emitted, the connected handlers are invoked.
Signals are emitted on the instance of the object.

## Signal registration

In TfeTextView, two signals are registered.

- "change-file" signal.
This signal is emitted when `tv->file` is changed.
- "open-response" signal.
`tfe_text_view_open` function is not able to return the status because it uses GtkFileChooserDialog.
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

- Because "change-file" signal doesn't have parameter, the handler's parameters are a TfeTextView instance and user data.
- Because "open-response" signal has one parameter, the handler's parameters are a TfeTextView instance, the signal's parameter and user data.
- `tv` is the object instance on which the signal is emitted.
- `user_data` comes from the fourth argument of `g_signal_connect`.
- `parameter` comes from the fourth argument of `g_signal_emit`.

The values of the parameter is defined in `tfetextview.h` because they are public.

~~~C
/* "open-response" signal response */
enum
{
  TFE_OPEN_RESPONSE_SUCCESS,
  TFE_OPEN_RESPONSE_CANCEL,
  TFE_OPEN_RESPONSE_ERROR
};
~~~

- The parameter is set to `TFE_OPEN_RESPONSE_SUCCESS` when `tfe_text_view_open` has successfully opened a file and read it.
- The parameter is set to `TFE_OPEN_RESPONSE_CANCEL` when the user has canceled.
- The parameter is set to `TFE_OPEN_RESPONSE_ERROR` when an error has occurred.
 
## Signal connection

A signal and a handler are connected by the function `g_signal_connect`.
There are some similar functions like `g_signal_connect_after`, `g_signal_connect_swapped` and so on.
However, `g_signal_connect` is the most common.
The signals "change-file" is connected to a callback function out of the TfeTextView object.
In the same way, the signals "open-response" is connected to a callback function out of the TfeTextView object.
Those callback functions are defined by users.

In the program `tfe`, callback functions are defined in `tfenotebook.c`.
And their names are `file_changed` and `open_response`.
They will be explained later.

~~~C
g_signal_connect (GTK_TEXT_VIEW (tv), "change-file", G_CALLBACK (file_changed), nb);

g_signal_connect (TFE_TEXT_VIEW (tv), "open-response", G_CALLBACK (open_response), nb);
~~~

## Signal emission

Signals are emitted on an instance.
The type of the instance is the second argument of `g_signal_new`.
The relationship between the signal and object type is determined when the signal is registered.

A function `g_signal_emit` is used to emit the signal.
The following lines are extracted from `tfetextview.c`.
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
"change-file" signal and "open-response" signal doesn't have details and the argument is zero when no details.
- "change-file" signal doesn't have parameter, so there's no fourth parameter.
- "open-response" signal has one parameter.
The fourth parameter is the parameter.


Up: [README.md](../README.md),  Prev: [Section 11](sec11.md), Next: [Section 13](sec13.md)
