Up: [Readme.md](../Readme.md),  Prev: [Section 27](sec27.md), Next: [Section 29](sec29.md)

# GtkExpression

GtkExpression is a fundamental type.
It is not a descendant of GObject.
GtkExpression provides a way to describe references to values.
GtkExpression needs to be evaluated to obtain a value.

It is similar to arithmetic calculation.

~~~
1 + 2 = 3
~~~

`1+2` is an expression.
It shows the way how to calculate.
`3` is the value comes from the expression.
Evaluation is to calculate the expression and get the value.

GtkExpression is a way to get a value.
Evaluation is like a calculation.
A value is got by evaluating the expression.

First, I want to show you the C file of the example for GtkExpression.
Its name is `exp.c` and located under [src/expression](../src/expression) directory.
You don't need to understand the details now, just look at it.
It will be explained in the next subsection.

~~~C
  1 #include <gtk/gtk.h>
  2 
  3 GtkWidget *win1;
  4 int width, height;
  5 GtkExpressionWatch *watch_width;
  6 GtkExpressionWatch *watch_height;
  7 
  8 /* Notify is called when "default-width" or "default-height" property is changed. */
  9 static void
 10 notify (gpointer user_data) {
 11   GValue value = G_VALUE_INIT;
 12   char *title;
 13 
 14   if (watch_width && gtk_expression_watch_evaluate (watch_width, &value))
 15     width = g_value_get_int (&value);
 16   g_value_unset (&value);
 17   if (watch_height && gtk_expression_watch_evaluate (watch_height, &value))
 18     height = g_value_get_int (&value);
 19   g_value_unset (&value);
 20   title = g_strdup_printf ("%d x %d", width, height);
 21   gtk_window_set_title (GTK_WINDOW (win1), title);
 22   g_free (title);
 23 }
 24 
 25 /* This function is used by closure tag in exp.ui. */
 26 char *
 27 set_title (GtkWidget *win, int width, int height) {
 28   return g_strdup_printf ("%d x %d", width, height);
 29 }
 30 
 31 /* ----- activate, open, startup handlers ----- */
 32 static void
 33 app_activate (GApplication *application) {
 34   GtkApplication *app = GTK_APPLICATION (application);
 35   GtkWidget *box;
 36   GtkWidget *label1, *label2, *label3;
 37   GtkWidget *entry;
 38   GtkEntryBuffer *buffer;
 39   GtkBuilder *build;
 40   GtkExpression *expression, *expression1, *expression2;
 41   GValue value = G_VALUE_INIT;
 42   char *s;
 43 
 44   /* Creates GtkApplicationWindow instance. */
 45   /* The codes below are complecated. It does the same as "win1 = gtk_application_window_new (app);". */
 46   /* The codes are written just to show how to use GtkExpression. */
 47   expression = gtk_cclosure_expression_new (GTK_TYPE_APPLICATION_WINDOW, NULL, 0, NULL,
 48                  G_CALLBACK (gtk_application_window_new), NULL, NULL);
 49   if (gtk_expression_evaluate (expression, app, &value)) {
 50     win1 = GTK_WIDGET (g_value_get_object (&value)); /* GtkApplicationWindow */
 51     g_object_ref (win1);
 52     g_print ("Got GtkApplicationWindow instance.\n");
 53   }else
 54     g_print ("The cclosure expression wasn't evaluated correctly.\n");
 55   gtk_expression_unref (expression);
 56   g_value_unset (&value);    /* At the same time, the reference count of win1 is decreased by one. */
 57 
 58   /* Builds a window with components */
 59   box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
 60   label1 = gtk_label_new (NULL);
 61   label2 = gtk_label_new (NULL);
 62   label3 = gtk_label_new (NULL);
 63   buffer = gtk_entry_buffer_new (NULL, 0);
 64   entry = gtk_entry_new_with_buffer (buffer);
 65   gtk_box_append (GTK_BOX (box), label1);
 66   gtk_box_append (GTK_BOX (box), label2);
 67   gtk_box_append (GTK_BOX (box), label3);
 68   gtk_box_append (GTK_BOX (box), entry);
 69   gtk_window_set_child (GTK_WINDOW (win1), box);
 70 
 71   /* Constant expression */
 72   expression = gtk_constant_expression_new (G_TYPE_INT,100);
 73   if (gtk_expression_evaluate (expression, NULL, &value)) {
 74     s = g_strdup_printf ("%d", g_value_get_int (&value));
 75     gtk_label_set_text (GTK_LABEL (label1), s);
 76     g_free (s);
 77   } else
 78     g_print ("The constant expression wasn't evaluated correctly.\n");
 79   gtk_expression_unref (expression);
 80   g_value_unset (&value);
 81 
 82   /* Property expression and binding*/
 83   expression1 = gtk_property_expression_new (GTK_TYPE_ENTRY, NULL, "buffer");
 84   expression2 = gtk_property_expression_new (GTK_TYPE_ENTRY_BUFFER, expression1, "text");
 85   gtk_expression_bind (expression2, label2, "label", entry);
 86 
 87   /* Constant expression instead of "this" instance */
 88   expression1 = gtk_constant_expression_new (GTK_TYPE_APPLICATION, app);
 89   expression2 = gtk_property_expression_new (GTK_TYPE_APPLICATION, expression1, "application-id");
 90   if (gtk_expression_evaluate (expression2, NULL, &value))
 91     gtk_label_set_text (GTK_LABEL (label3), g_value_get_string (&value));
 92   else
 93     g_print ("The property expression wasn't evaluated correctly.\n");
 94   gtk_expression_unref (expression1); /* expression 2 is also freed. */
 95   g_value_unset (&value);
 96 
 97   width = 800;
 98   height = 600;
 99   gtk_window_set_default_size (GTK_WINDOW (win1), width, height);
100   notify(NULL);
101 
102   /* GtkExpressionWatch */
103   expression1 = gtk_property_expression_new (GTK_TYPE_WINDOW, NULL, "default-width");
104   watch_width = gtk_expression_watch (expression1, win1, notify, NULL, NULL);
105   expression2 = gtk_property_expression_new (GTK_TYPE_WINDOW, NULL, "default-height");
106   watch_height = gtk_expression_watch (expression2, win1, notify, NULL, NULL);
107 
108   gtk_widget_show (win1);
109 
110   /* Builds a window with exp.ui resource */
111   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/exp/exp.ui");
112   GtkWidget *win2 = GTK_WIDGET (gtk_builder_get_object (build, "win2"));
113   gtk_window_set_application (GTK_WINDOW (win2), app);
114   g_object_unref (build);
115 
116   gtk_widget_show (win2);
117 }
118 
119 static void
120 app_startup (GApplication *application) {
121 }
122 
123 #define APPLICATION_ID "com.github.ToshioCP.exp"
124 
125 int
126 main (int argc, char **argv) {
127   GtkApplication *app;
128   int stat;
129 
130   app = gtk_application_new (APPLICATION_ID, G_APPLICATION_FLAGS_NONE);
131 
132   g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
133   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
134 /*  g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);*/
135 
136   stat =g_application_run (G_APPLICATION (app), argc, argv);
137   g_object_unref (app);
138   return stat;
139 }
140 
~~~

`exp.c` consists of five functions.

- `notify`
- `set_title`
- `app_activate`. This is a handler of "activate" signal on GtkApplication instance.
- `app_startup`. This is a handler of "startup"signal. But nothing is done in this function.
- `main`.

The function `app_activate` is an actual main body in `exp.c`.

## Constant expression

Constant expression provides constant value or instance when it is evaluated.

- 72-80: A constant expression.
It is extracted and put into here.

~~~C
  expression = gtk_constant_expression_new (G_TYPE_INT,100);
  if (gtk_expression_evaluate (expression, NULL, &value)) {
    s = g_strdup_printf ("%d", g_value_get_int (&value));
    gtk_label_set_text (GTK_LABEL (label1), s);
    g_free (s);
  } else
    g_print ("The constant expression wasn't evaluated correctly.\n");
  gtk_expression_unref (expression);
  g_value_unset (&value);
~~~

- Constant expression is created with `gtk_constant_expression_new` function.
The parameter of the function is a type (GType) and a value (or instance).
- `gtk_expression_evaluate` evaluates the expression.
It has three parameters, the expression to evaluate, `this` instance and GValue for being set with the value.
`this` instance isn't necessary for constant expressions.
Therefore the second argument is NULL.
`gtk_expression_evaluate` returns TRUE if it successfully evaluates the expression.
Otherwise it returns FALSE.
- If it returns TRUE, the GValue `value` is set with the value of the expression.
The type of the value is int.
`g_strdup_printf` converts the value to a string `s`.
- GtkLabel `label1` is set with `s`.
The string `s` needs to be freed.
- If the evaluation fails a message is outputted to stderr.
- The expression and GValue are freed.

Constant expression is usually used to give a constant value or instance to another expression.

## Property expression

Property expression looks up a property in a GObject object.
For example, a property expression that refers "label" property in GtkLabel object is created like this.

~~~C
expression = gtk_property_expression_new (GTK_TYPE_LABEL, another_expression, "label");
~~~

`another_expression` is expected to give a GtkLabel instance when it is evaluated.
For example,

~~~C
label = gtk_label_new ("Hello");
another_expression = gtk_constant_expression_new (GTK_TYPE_LABEL, label);
expression = gtk_property_expression_new (GTK_TYPE_LABEL, another_expression, "label");
~~~

If `expression` is evaluated, the second parameter `another_expression` is evaluated in advance.
The value of `another_expression` is `label` (GtkLabel instance).
Then, `expression` looks up "label" property of `label` and the evaluation result is "Hello".

In the example above, the second argument of `gtk_property_expression_new` is another expression.
But the second argument can be NULL.
If it is NULL, `this` instance is used instead.
`this` is given by `gtk_expression_evaluate` function at the evaluation.

Now look at `exp.c`.
The lines from 83 to 85 is extracted here.

~~~C
  expression1 = gtk_property_expression_new (GTK_TYPE_ENTRY, NULL, "buffer");
  expression2 = gtk_property_expression_new (GTK_TYPE_ENTRY_BUFFER, expression1, "text");
  gtk_expression_bind (expression2, label2, "label", entry);
~~~

- `expression1` looks up "buffer" property of `this` object, which is `GTK_TYPE_ENTRY` type.
- `expression2` looks up "text" property of GtkEntryBuffer object given by `epression1`.
- `gtk_expression_bind` binds a property to a value given by the expression.
In this program, it binds a "label" property in `label2` to the value evaluated with `expresion2` with `entry` as `this` object.
The evaluation process is as follows.
  1. `expression2` is evaluated. But it includes `expression1` so `expression1` is evaluated in advance.
  2. Because the second argument of `expression1` is NULL, `this` object is used.
`this` is given by `gtk_expression_bind`.
It is `entry` (GtkEntry instance).
`expression1` looks up "buffer" property in `entry`.
It is a GtkEntryBuffer instance `buffer`.
(See line 64 in `exp.c`.)
  3. Then, `expression2` looks up "text" property in `buffer`.
It is a text held in `entry`.
  4. The text is assigned to "label" property in `label2`.
- `gtk_expression_bind` creates a GtkExpressionWatch.
(But it isn't assigned to a variable in the program above.
If you want to keep the GtkExpressionWatch instance, assign it to a variable.)

~~~C
  GtkExpressionWatch *watch;
  watch = gtk_expression_bind (expression2, label2, "label", entry);
~~~

- Whenever the value from `expression2` changes, it evaluates `expression2` and set "label" property in `label2`.
So, the change of the text in `entry` makes the "label" property reflect it immediately.

## Closure expression

Closure expression calls closure when it is evaluated.
A closure is a generic representation of a callback (a pointer to a function).
For information about closure, see [GObject API Reference, The GObject messaging system](https://docs.gtk.org/gobject/concepts.html#the-gobject-messaging-system).
A closure expression is created with `gtk_cclosure_expression_new` function.

~~~C
GtkExpression *
gtk_cclosure_expression_new (GType value_type,
                             GClosureMarshal marshal,
                             guint n_params,
                             GtkExpression **params,
                             GCallback callback_func,
                             gpointer user_data,
                             GClosureNotify user_destroy);
~~~

- `value_type` is the type of the value when it is evaluated.
- `marshal` is a marshaller.
You can assign NULL.
If it is NULL, then `g_cclosure_marshal_generic ()` is used as a marshaller.
It is a generic marshaller function implemented via libffi.
- `n_params` is the number of parameters.
- `params` points expressions for each parameter of the call back function.
- `callback_func` is a callback function.
- `user_data` is user data.
You can add it for the closure.
It is like `user_data` in `g_signal_connect`.
If it is not necessary, assign NULL.
- `user_destroy` is a destroy notify for `user_data`.
It is called to destroy `user_data` when it is no longer needed.
If NULL is assigned to `user_data`, assign NULL to `user_destroy`, too.

The following is extracted from `exp.c`.
It is from line 47 to line 56.

~~~C
expression = gtk_cclosure_expression_new (GTK_TYPE_APPLICATION_WINDOW, NULL, 0, NULL,
               G_CALLBACK (gtk_application_window_new), NULL, NULL);
if (gtk_expression_evaluate (expression, app, &value)) {
  win1 = GTK_WIDGET (g_value_get_object (&value)); /* GtkApplicationWindow */
  g_object_ref (win1);
  g_print ("Got GtkApplicationWindow object.\n");
}else
  g_print ("The cclosure expression wasn't evaluated correctly.\n");
gtk_expression_unref (expression);
g_value_unset (&value);    /* At the same time, the reference count of win1 is decreased by one. */
~~~

The callback function is `gtk_application_window_new`.
This function has one parameter which is an instance of GtkApplication.
And it returns newly created GtkApplicationWindow instance.
So, the first argument is `GTK_TYPE_APPLICATION_WINDOW` which is the type of the return value.
The second argument is NULL so general marshaller `g_cclosure_marshal_generic ()` will be used.
I think assigning NULL works in most cases when you program in C language.

The arguments given to the call back function are `this` object and parameters which are the fourth argument of `gtk_cclosure_expression_new`.
So, the number of arguments is `n_params + 1`.
Because `gtk_application_window_new` has one parameter, so `n_params` is zero and `**params` is NULL.
No user data is necessary, so `user_data` and `user_destroy` are NULL.

`gtk_expression_evaluate` evaluates the expression.
`this` instance will be the first argument for `gtk_application_window_new`, so it is `app`.

If the evaluation succeeds, the GValue `value` holds a newly created GtkApplicationWindow instance.
It is assigned to `win1`.
The GValue will be unset when it is no longer used.
And when it is unset, the GtkApplicationWindow instance will be released and its reference count will be decreased by one.
It is necessary to increase the reference count by one in advance to keep the instance.
`gtk_expression_unref` frees `expression` and `value` is unset.

As a result, we got a GtkApplicationWindow instance `win1`.
We can do the same by:

~~~C
win1 = gtk_application_window_new (app);
~~~

The example is more complicated and not practical than this one line code.
The aim of the example is just to show how closure expression works.

Closure expression is flexible than other type of expression because you can specify your own callback function.

## GtkExpressionWatch

GtkExpressionWatch watches an expression and if the value of the expression changes it calls its notify handler.

The example uses GtkExpressionWatch in the line 103 to 106.

~~~C
expression1 = gtk_property_expression_new (GTK_TYPE_WINDOW, NULL, "default-width");
watch_width = gtk_expression_watch (expression1, win1, notify, NULL, NULL);
expression2 = gtk_property_expression_new (GTK_TYPE_WINDOW, NULL, "default-height");
watch_height = gtk_expression_watch (expression2, win1, notify, NULL, NULL);
~~~

The expressions above refer to "default-width" and "default-height" properties of GtkWindow.
The variable `watch_width` watches `expression1`.
The second argument `win1` is `this` instance for `expression1`.
So, `watch_width` watches the value of "default-width" property of `win1`.
If the value changes, it calls `notify` handler.
The fourth and fifth arguments are NULL because no user data is necessary.

The variable `watch_height` connects `notify` handler to `expression2`.
So, `notiry` is also called when "default-height" changes.

The handler `norify` is as follows.

~~~C
 1 static void
 2 notify (gpointer user_data) {
 3   GValue value = G_VALUE_INIT;
 4   char *title;
 5 
 6   if (watch_width && gtk_expression_watch_evaluate (watch_width, &value))
 7     width = g_value_get_int (&value);
 8   g_value_unset (&value);
 9   if (watch_height && gtk_expression_watch_evaluate (watch_height, &value))
10     height = g_value_get_int (&value);
11   g_value_unset (&value);
12   title = g_strdup_printf ("%d x %d", width, height);
13   gtk_window_set_title (GTK_WINDOW (win1), title);
14   g_free (title);
15 }
~~~

- 6-11: Evaluates `expression1` and `expression2` with `expression_watch_evaluate` function.
- 12: Creates a string `title`.
It contains the width and height, for example, "800 x 600".
- 13: Sets the title of `win1` with the string `title`.

The title of the window reflects the size of the window.

## exp.ui

`exp.c` builds a GtkWindow instance `win2` with `exp.ui`.
The ui file `exp.ui` includes tags to create GtkExpressions.
The tags are:

- constant tag to create constant expression
- lookup tag to create property expression
- closure tag to create closure expression
- binding tag to bind a property to an expression

The window `win2` behaves like `win1`.
Because similar expressions are built with the ui file.

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <interface>
 3   <object class="GtkWindow" id="win2">
 4     <binding name="title">
 5       <closure type="gchararray" function="set_title">
 6         <lookup name="default-width" type="GtkWindow"></lookup>
 7         <lookup name="default-height" type="GtkWindow"></lookup>
 8       </closure>
 9     </binding>
10     <property name="default-width">600</property>
11     <property name="default-height">400</property>
12     <child>
13       <object class="GtkBox">
14         <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
15         <child>
16           <object class="GtkLabel">
17             <binding name="label">
18               <constant type="gint">100</constant>
19             </binding>
20           </object>
21         </child>
22         <child>
23           <object class="GtkLabel">
24             <binding name="label">
25               <lookup name="text">
26                 <lookup name="buffer">
27                   entry
28                 </lookup>
29               </lookup>
30             </binding>
31           </object>
32         </child>
33         <child>
34           <object class="GtkLabel">
35             <binding name="label">
36               <lookup name="application-id">
37                 <lookup name="application">win2</lookup>
38               </lookup>
39             </binding>
40           </object>
41         </child>
42         <child>
43           <object class="GtkEntry" id="entry">
44             <property name="buffer">
45               <object class="GtkEntryBuffer"></object>
46             </property>
47           </object>
48         </child>
49       </object>
50     </child>
51   </object>
52 </interface>
~~~

### Constant tag

A constant tag corresponds to a constant expression.

- 18: Constant tag.
The constant expression is created with the tag.
It returns 100, the type is "gint", when it is evaluated.
The type "gint" is a name of `G_TYPE_INT` type.
Similarly, the types which is registered to the type system has type and name.
For example, "gchararray" is a name of `G_TYPE_STRING` type.
You need to use the name of types for the `type`attribute.
See [GObject tutorial](https://github.com/ToshioCP/Gobject-tutorial/blob/main/gfm/sec5.md#gvalue).
- 17-19: Binding tag corresponds to `gtk_expression_bind` function.
`name` attribute specifies the "label" property of the GtkLabel object just before the binding tag.
The expression returns a int type GValue.
On the other hand "label" property holds a string type GValue.
When a GValue is copied to another GValue, the type is automatically converted if possible.
In this case, an int `100` is converted to a string `"100"`.

These binding and constant tag works.
But they are not good.
A property tag is more straightforward.

~~~xml
<object class="GtkLabel">
  <property name="label">100</property>
</object>
~~~

This example just shows the way how to use constant tag.
Constant tag is mainly used to give a constant argument to a closure.

### Lookup tag

A lookup tag corresponds to a property expression.
Line 23 to 31 is copied here.

~~~xml
          <object class="GtkLabel">
            <binding name="label">
              <lookup name="text">
                <lookup name="buffer">
                  entry
                </lookup>
              </lookup>
            </binding>
          </object>
~~~

- binding tag binds a "label" property in GtkLabel to an expression.
The expression is defined with a lookup tag.
- The lookup tag defines a property expression looks up a "text" property in the instance which is defined in the next expression.
The next expression is created with the lookup tag.
The expression looks up the `buffer` property of the `entry` instance.
The `entry` instance is defined in the line 43.
It is a GtkEntry `entry`.
A lookup tag takes an instance in some ways to look up for a property.
  - If it has no contents, it takes `this` instance when it is evaluated.
  - If it has a content of a tag for an expression, which is constant, lookup or closure tag, the value of the expression will be the instance to look up when it is evaluated.
  - If it has a content of an id of an object, then the instance of the object will be taken as the instance to lookup.

As a result, the label of the GtkLabel instance are bound to the text in the field of GtkEntry.
If a user input a text in the field in the GtkEntry, GtkLabel displays the same text.

Another lookup tag is in the lines from 34 to 40.

~~~xml
          <object class="GtkLabel">
            <binding name="label">
              <lookup name="application-id">
                <lookup name="application">win2</lookup>
              </lookup>
            </binding>
          </object>
~~~

- Two expressions are nested.
- A lookup tag looks up "application-id" property of the next expression.
- The next lookup tag looks up "application" property of `win2` instance.

As a result, the "label" property in the GtkLabel instance is bound to the "application-id" property.
The nested tag makes a chain like:

~~~
"label" <= "application-id" <= "application" <= `win2`
~~~

By the way, the application of `win2` is set after the objects in ui file are built.
Look at `exp.c`.
`gtk_window_set_application` is called after `gtk_build_new_from_resource`.

~~~C
build = gtk_builder_new_from_resource ("/com/github/ToshioCP/exp/exp.ui");
GtkWidget *win2 = GTK_WIDGET (gtk_builder_get_object (build, "win2"));
gtk_window_set_application (GTK_WINDOW (win2), app);
~~~

Therefore, before the call for `gtk_window_set_application`, the "application" property of `win2` is *not* set.
So, the evaluation of `<lookup name="application">win2</lookup>` fails.
And the evaluation of `<lookup name="application-id">` also fails.
A function `gtk_expression_bind ()`, which corresponds to `binding` tag, doesn't update the target property if the expression fails.
So, the "label" property isn't updated at the first evaluation.

Note that an evaluation can fail.
The care is especially necessary when you write a callback for a closure tag which has contents of expressions like lookup tags.
The expressions are given to the callback as an argument.
If an expression fails the argument will be NULL.
You need to check if the argument exactly points the instance that is expected by the callback.

### Closure tag

The lines from 3 to 9 include a closure tag.

~~~xml
  <object class="GtkWindow" id="win2">
    <binding name="title">
      <closure type="gchararray" function="set_title">
        <lookup name="default-width" type="GtkWindow"></lookup>
        <lookup name="default-height" type="GtkWindow"></lookup>
      </closure>
    </binding>
~~~

- A binding tag corresponds to a `gtk_expression_bind` function.
`name` attribute specifies the "title" property of `win2`.
Binding tag gives `win2` as the `this` instance to the expressions, which are the contents of the binding tag.
So, closure tag and lookup tags use `win2` as the `this` object when they are evaluated.
- A closure tag corresponds to a closure expression.
Its callback function is `set_title` and it returns "gchararray" type, which is "an array of characters" i.e. a string.
The contents of the closure tag are assigned to parameters of the function.
So, `set_title` has three parameters, `win2` (`this` instance), default width and default height.
- Lookup tags correspond to property expressions.
They lookup "default-width" and "default-height" properties of `win2` (`this` instance).
- Binding tab creates GtkExpressionWatch automatically, so "title" property reflects the changes of "default-width" and "default-height" properties.

`set_title` function in `exp.c` is as follows.

~~~C
1 char *
2 set_title (GtkWidget *win, int width, int height) {
3   return g_strdup_printf ("%d x %d", width, height);
4 }
~~~

It just creates a string, for example, "800 x 600", and returns it.

You've probably been noticed that ui file is easier and clearer than the corresponding C program.
One of the most useful case of GtkExpression is building GtkListItem instance with GtkBuilderListItemFatory.
Such case has already been described in the prior two sections.

It will be used in the next section to build GtkListItem in GtkColumnView, which is the most useful view object for GListModel.

## Compilation and execution

All the sources are in [src/expression](../src/expression) directory.
Change your current directory to the directory and run meson and ninja.
Then, execute the application.

~~~
$ meson _build
$ ninja -C _build
$ build/exp
~~~

Then, two windows appear.

![Expression](../image/expression.png)

If you put some text in the field of the entry, then the same text appears in the second GtkLabel.
Because the "label" property of the second GtkLabel instance is bound to the text in the GtkEntryBuffer.

If you resize the window, then the size appears in the title bar because the "title" property is bound to "default-width" and "default-height" properties.


Up: [Readme.md](../Readme.md),  Prev: [Section 27](sec27.md), Next: [Section 29](sec29.md)
