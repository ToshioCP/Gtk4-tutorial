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

## Constant expression

A constant expression (GtkConstantExpression) provides constant value or instance when it is evaluated.

~~~C
  GValue value = G_VALUE_INIT;
  expression = gtk_constant_expression_new (G_TYPE_INT,100);
  gtk_expression_evaluate (expression, NULL, &value);
~~~

- GtkExpression uses GValue to hold a value.
GValue is a structure and container to hold a type and value.
It must be initialized with `G_VALUE_INIT`, first.
Be careful that `value` is a structure, not a pointer to a structure.
- Constant expression is created with `gtk_constant_expression_new` function.
The parameter of the function is a type (GType) and a value (or instance).
This expression holds a constant value.
`G_TYPE_INT` is a type that is registered to the type system.
It is integer type.
Some types are shown in the following table.
- `gtk_expression_evaluate` evaluates the expression.
It has three parameters, the expression to evaluate, `this` instance and a pointer to a GValue for being set with the value.
`this` instance isn't necessary for constant expressions.
Therefore, the second argument is NULL.
`gtk_expression_evaluate` returns TRUE if it successfully evaluates the expression.
Otherwise it returns FALSE.
- If it returns TRUE, the GValue `value` is set with the value of the expression.
The type of the value is int.

|GType            |C type|type name |notes                  |
|:----------------|:-----|:---------|:----------------------|
|G\_TYPE\_CHAR    |char  |gchar     |                       |
|G\_TYPE\_BOOLEAN |int   |gboolean  |                       |
|G\_TYPE\_INT     |int   |gint      |                       |
|G\_TYPE\_FLOAT   |float |gfloat    |                       |
|G\_TYPE\_DOUBLE  |double|gdouble   |                       |
|G\_TYPE\_POINTER |void *|gpointer  |general pointer        |
|G\_TYPE\_STRING  |char *|gchararray|null-terminated Cstring|
|G\_TYPE\_OBJECT  |      |GObject   |                       |
|GTK\_TYPE\_WINDOW|      |GtkWindow |                       |


A sample program `exp_constant_simple.c` is in `src/expression` directory.

@@@include
expression/exp_constant_simple.c
@@@

- 9: A constant expression is created. It holds an int value 100. The variable `expression` points the expression.
- 11-14: Evaluates the expression. If it successes, show the value to the stdout. Otherwise show an error message.
- 15-16: Releases the expression and unsets the GValue.

Constant expression is usually used to give a constant value or instance to another expression.

## Property expression

A property expression (GtkPropertyExpression) looks up a property in a GObject instance.
For example, a property expression that refers "label" property in a GtkLabel object is created like this.

@@@if gfm
~~~C
expression = gtk_property_expression_new (GTK_TYPE_LABEL, another_expression, "label");
~~~
@@@else
~~~{.C}
expression = gtk_property_expression_new (GTK_TYPE_LABEL, another_expression, "label");
~~~
@@@end

The second parameter `another_expression` is one of:

- An expression that gives a GtkLabel instance when it is evaluated.
- NULL. When NULL is given, a GtkLabel instance will be given when it is evaluated.
The instance is called `this` object.

For example,

@@@if gfm
~~~C
label = gtk_label_new ("Hello");
another_expression = gtk_constant_expression_new (GTK_TYPE_LABEL, label);
expression = gtk_property_expression_new (GTK_TYPE_LABEL, another_expression, "label");
~~~
@@@else
~~~{.C}
label = gtk_label_new ("Hello");
another_expression = gtk_constant_expression_new (GTK_TYPE_LABEL, label);
expression = gtk_property_expression_new (GTK_TYPE_LABEL, another_expression, "label");
~~~
@@@end

If `expression` is evaluated, the second parameter `another_expression` is evaluated in advance.
The value of `another_expression` is the `label` (GtkLabel instance).
Then, `expression` looks up "label" property of the label and the evaluation results in "Hello".

In the example above, the second argument of `gtk_property_expression_new` is another expression.
But the second argument can be NULL.
If it is NULL, `this` instance is used instead.
`this` is given by `gtk_expression_evaluate` function.

There's a simple program `exp_property_simple.c` in `src/expression` directory.

@@@include
expression/exp_property_simple.c
@@@

- 9-10: `gtk_init` initializes GTK GUI toolkit.
It isn't usually necessary because the GtkApplication default startup handler does the initialization.
A GtkLabel instance is created with the text "Hello world.".
- 12: A property expression is created.
It looks a "label" property of a GtkLabel instance.
But at the creation, no instance is given because the second argument is NULL.
The expression just knows how to take the property from a future-given GtkLabel instance.
- 14-17: The function `gtk_expression_evaluate` evaluates the expression with a 'this' instance `label`.
The result is stored in the GValue `value`.
The function `g_value_get_string` gets a string from the GValue.
But the string is owned by the GValue so you must not free the string.
- 18-19: Releases the expression and unset the GValue.
At the same time the string in the GValue is freed.

If the second argument of `gtk_property_expression_new` isn't NULL, it is another expression.
The expression is owned by a newly created property expression.
So, when the expressions are useless, you just release the last expression.
Then it releases another expression it has.

## Closure expression

A closure expression calls closure when it is evaluated.
A closure is a generic representation of a callback (a pointer to a function).
For information about closure, see [GObject API Reference -- The GObject messaging system](https://docs.gtk.org/gobject/concepts.html#the-gobject-messaging-system).
There are simple closure example files `closure.c` and `closure_each.c` in the `src/expression` directory.

There are two types of closure expressions, GtkCClosureExpression and GtkClosureExpression.
They corresponds to GCClosure and GClosure respectively.
When you program in C language, GtkCClosureExpression and GCClosure are appropriate.

A closure expression is created with `gtk_cclosure_expression_new` function.

@@@if gfm
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
@else
~~~{.C}
GtkExpression *
gtk_cclosure_expression_new (GType value_type,
                             GClosureMarshal marshal,
                             guint n_params,
                             GtkExpression **params,
                             GCallback callback_func,
                             gpointer user_data,
                             GClosureNotify user_destroy);
~~~
@end

- `value_type` is the type of the value when it is evaluated.
- `marshal` is a marshaller.
You can assign NULL.
If it is NULL, then `g_cclosure_marshal_generic ()` is used as a marshaller.
It is a generic marshaller function implemented via libffi.
- `n_params` is the number of parameters.
- `params` points expressions for each parameter of the call back function.
- `callback_func` is a callback function.
It is given arguments `this` and parameters above.
So, if `n_params` is 3, the number of arguments of the function is 4.
(`this` and `params`. See below.)
- `user_data` is user data.
You can add it for the closure.
It is like `user_data` in `g_signal_connect`.
If it is not necessary, assign NULL.
- `user_destroy` is a destroy notify for `user_data`.
It is called to destroy `user_data` when it is no longer needed.
If NULL is assigned to `user_data`, assign NULL to `user_destroy`, too.

Call back functions have the following format.

~~~
C-type
callback (this, param1, param2, ...)
~~~

For example,

@@@if gfm
~~~C
int
callback (GObject *object, int x, const char *s)
~~~
@@@else
~~~{.C}
int
callback (GObject *object, int x, const char *s)
~~~
@@@end

The following is `exp_closure_simple.c` in `src/expression`.

@@@include
expression/exp_closure_simple.c
@@@

- 3-11: A call back function.
The parameter is only one and it is a 'this' object.
It is a GtkLabel and its label is assumed to be "(number)+(number)".
- 8-10: Retrieves two integers from the label and returns the sum of them.
This function has no error report.
If you want to return error report, change the return value type to be a pointer to a structure of gboolean and integer.
One for error and the other for the sum.
The first argument of `gtk_cclosure_expression_new` is `G_TYPE_POINTER`.
There is a sample program `exp_closure_with_error_report` in `src/expression` directory.
- 19: The function `gtk_init`` initializes GTK. It is necessary for GtkLabel.
- 20: A GtkLabel instance is created with "123+456".
- 21: The instance has floating reference. It is changed to an ordinary reference count.
- 22-23: Creates a closure expression. Its return value type is `G_TYPE_INT` and no parameters or 'this' object.
- 24: Evaluates the expression with the label as a 'this' object.
- 25: If the evaluation successes, the sum of "123+456", which is 579, is shown.
- 27: If it fails, an error message appears.
- 28-30: Releases the expression and the label. Unsets the value.

Closure expression is flexible than other type of expression because you can specify your own callback function.

## GtkExpressionWatch

GtkExpressionWatch is a structure, not an object.
It represents a watched GtkExpression.
Two functions create GtkExpressionWatch structure.
They are `gtk_expression_bind` and `gtk_expression_watch`.

### gtk\_expression\_bind function

This function binds the target object's property to the expression.
If the value of the expression changes, the property reflects the value immediately.

@@@if gfm
~~~C
GtkExpressionWatch*
gtk_expression_bind (
  GtkExpression* self,
  GObject* target,
  const char* property,
  GObject* this_
)
~~~
@@@else
~~~{.C}
GtkExpressionWatch*
gtk_expression_bind (
  GtkExpression* self,
  GObject* target,
  const char* property,
  GObject* this_
)
~~~
@@@end

This function takes the ownership of the expression.
So, if you want to own the expression, call `gtk_expression_ref ()` to increase the reference count of the expression.
And you should unref it when it is useless.
If you don't own the expression, you don't care about releasing the expression.

An example `exp_bind.c` and `exp_bind.ui` is in [`src/expression`](expression) directory.

![exp\_bind](../image/exp_bind.png){width=9.2cm height=1.9cm}

It includes a label and a scale.
If you move the slider to the right, the scale value increases and the number on the label also increases.
In the same way, if you move it to the left, the number on the label decreases.
The label is bound to the scale value via an adjustment.

@@@include
expression/exp_bind.ui
@@@

The ui file describes the following parent-child relationship.

~~~
GtkApplicationWindow (win) -- GtkBox -+- GtkLabel (label)
                                      +- GtkScale
~~~

Four GtkScale properties are defined.

- adjustment. GtkAdjustment provides the followings.
  - upper and lower: the range of the scale.
  - value: current value of the scale. It reflects the value of the scale.
  - step increment and page increment: When a user press an arrow key or page up/down key,
the scale moves by the step increment or page increment respectively.
  - page-size: When an adjustment is used with a scale, page-size is zero.
- digits: The number of decimal places that are displayed in the value.
- draw-value: Whether the value is displayed.
- has-origin: Whether the scale has the origin. If it's true, an orange bar appears between the origin and the current point.
- round-digits: The number of digits to round the value to when it changes.
For example, if it is zero, the slider moves to an integer point.

@@@include
expression/exp_bind.c
@@@

The point of the program is:

- 41-42: Two expressions are defined.
One is a property expression and the other is a closure expression.
The property expression looks up the "value" property of the adjustment instance.
The closure expression just converts the double into an integer.
- 43: `gtk_expression_bind` binds the label property of the GtkLabel instance to the integer returned by the closure expression.
It creates a GtkExpressionWatch structure.
The binding works during the watch lives.
When the window is destroyed, the scale and adjustment are also destroyed.
And the watch recognizes the value of the expression changes and tries to change the property of the label.
Obviously, it is not a correct behavior.
The watch should be unwatched before the window is destroyed.
- 37: Connects the "close-request" signal on the window to a handler `close_request_cb`.
This signal is emitted when the close button is clicked.
The handler is called just before the window closes.
It is the right moment to make the GtkExpressionWatch unwatched.
- 10-14: "close-request" signal handler.
The function `gtk_expression_watch_unwatch (watch)` makes the watch stop watching the expression.
It also releases the expression.

If you want to bind a property to an expression, `gtk_expression_bind` is the best choice.
You can do it with `gtk_expression_watch` function, but it is less suitable.

### gtk\_expression\_watch function

@@@if gfm
~~~C
GtkExpressionWatch*
gtk_expression_watch (
  GtkExpression* self,
  GObject* this_,
  GtkExpressionNotify notify,
  gpointer user_data,
  GDestroyNotify user_destroy
)
~~~
@@@else
~~~{.C}
GtkExpressionWatch*
gtk_expression_watch (
  GtkExpression* self,
  GObject* this_,
  GtkExpressionNotify notify,
  gpointer user_data,
  GDestroyNotify user_destroy
)
~~~
@@@end

The function doesn't take the ownership of the expression.
It differs from `gtk_expression_bind`.
So, you need to release the expression when it is useless.
It creates a GtkExpressionWatch structure.
The third parameter `notify` is a callback to invoke when the expression changes.
You can set `user_data` to give it to the callback.
The last parameter is a function to destroy the `user_data` when the watch is unwatched.
Put NULL if you don't need them.

Notify callback has the following format.

@@@if gfm
~~~C
void
notify (
  gpointer user_data
)
~~~
@@@else
~~~{.C}
void
notify (
  gpointer user_data
)
~~~
@@@end

This function is used to do something when the value of the expression changes.
But if you want to bind a property to the value, use `gtk_expression_bind` instead.

There's a sample program `exp_watch.c` in [`src/expression`](expression) directory.
It outputs the width of the window to the standard output.

![exp\_watch](../image/exp_watch.png){width=9.6cm height=6.9cm}

When you resize the window, the width is displayed in the terminal.

@@@include
expression/exp_watch.c
@@@

- 37: A property expression looks up the "default-width" property of the window.
- 38: Create a watch structure for the expression.
The callback `notify` is called every time the value of the expression changes.
The 'this' object is `win`, so the expression returns the default width of the window.
- 6-14: The callback function `notify`.
It uses `gtk_expression_watch_evaluate` to get the value of the expression.
The 'this' object is given in advance (when the watch is created).
It outputs the window width to the standard output.
- 16-21: A handler for the "close-request"signal on the window.
It stops the watch.
In addition, it releases the reference to the expression.
Because `gtk_expression_watch` doesn't take the ownership of the expression, you own it.
So, the release is necessary.

## Gtkexpression in ui files

GtkBuilder supports GtkExpressions.
There are four tags.

- constant tag to create constant expression. Type attribute specifies the type name of the value.
If no type is specified, the type is assumed to be an object.
The content is the value of the expression.
- lookup tag to create property expression. Type attribute specifies the type of the object.
Name attribute specifies the property name.
The content is an expression or object which has the property to look up.
If there's no content, 'this' object is used.
- closure tag to create closure expression. Type attribute specifies the type of the returned value.
Function attribute specifies the callback function.
The contents of the tag are arguments that are expressions.
- binding tag to bind a property to an expression.
It is put in the content of an object tag.
Name attribute specifies the property name of the object.
The content is an expression.

@@@if gfm
~~~xml
<constant type="gchararray">Hello world</constant>
<lookup name="label" type="GtkLabel">label</lookup>
<closure type="gint" function="callback_function"></closure>
<bind name="label">
  <lookup name="default-width">win</lookup>
</bind>
~~~
@@@else
~~~{.xml}
<constant type="gchararray">Hello world</constant>
<lookup name="label" type="GtkLabel">label</lookup>
<closure type="gint" function="callback_function"></closure>
<bind name="label">
  <lookup name="default-width">win</lookup>
</bind>
~~~
@@@end

These tags are usually used for GtkBuilderListItemFactory.

@@@if gfm
~~~xml
<interface>
  <template class="GtkListItem">
    <property name="child">
      <object class="GtkLabel">
        <binding name="label">
          <lookup name="string" type="GtkStringObject">
            <lookup name="item">GtkListItem</lookup>
          </lookup>
        </binding>
      </object>
    </property>
  </template>
</interface>
~~~
@@@else
~~~{.xml}
<interface>
  <template class="GtkListItem">
    <property name="child">
      <object class="GtkLabel">
        <binding name="label">
          <lookup name="string" type="GtkStringObject">
            <lookup name="item">GtkListItem</lookup>
          </lookup>
        </binding>
      </object>
    </property>
  </template>
</interface>
~~~
@@@end

GtkBuilderListItemFactory uses GtkBuilder to extends the GtkListItem with the XML data.

In the xml file above, "GtkListItem" is an instance of the GtkListItem template.
It is the 'this' object given to the expressions.
(The information is in the [GTK Development Blog](https://blog.gtk.org/2020/09/)).

GtkBuilder calls `gtk_expression_bind` function when it finds a binding tag.
The function sets the 'this' object like this:

1. If the binding tag has object attribute, the object will be the 'this' object.
2. If the current object of the GtkBuilder exists, it will be the 'this' object.
That's why a GtkListItem instance is the 'this' object of the XML data for a GtkBuilderListItemFactory.
3. Otherwise, the target object of the binding tag will be the 'this' object.

GTK 4 document doesn't describe information about "this" object when expressions are defined in a ui file.
The information above is found from the GTK 4 source files and it is possible to include mistakes.
If you have accurate information, please let me know.

A sample program `exp.c` and a ui file `exp.ui` is in [`src/expression`](expression) directory.
The ui file includes lookup, closure and bind tags.
No constant tag is included.
However, constant tags are not used so often.

![exp.c](../image/exp.png){width=10.3cm height=7.6cm}

If you resize the window, the size is shown at the title of the window.
If you type characters in the entry, the same characters appear on the label.

The ui file is as follows.

@@@include
expression/exp.ui
@@@

- 4-9: The title property of the main window is bound to a closure expression.
Its callback function `set_title` is defined in the C source file.
It returns a string because the type attribute of the tag is "gchararray".
Two parameters are given to the function.
They are width and height of the window.
Lookup tags don't have contents, so 'this' object is used to look up the properties.
The 'this' object is `win`, which is the target of the binding (`win` includes the binding tag).
- 17-21: The "label" property of the GtkLabel instance is bound to the "text" property of `buffer`,
which is the buffer of GtkEntry defined in line 25.
If a user types characters in the entry, the same characters appear on the label.

The C source file is as follows.

@@@include
expression/exp.c
@@@

- 4-6: The callback function.
It returns a string (w)x(h), where the w and h are the width and height of the window.
String duplication is necessary.

The C source file is very simple because almost everything is done in the ui file.

## Conversion between GValues

If you bind different type properties, type conversion is automatically done.
Suppose a label property (string) is bound to default-width property (int).

@@@if gfm
~~~xml
<object class="GtkLabel">
  <binding name="label">
    <lookup name="default-width">
      win
    </lookup>
  </binding>
</object>
~~~
@@@else
~~~{.xml}
<object class="GtkLabel">
  <binding name="label">
    <lookup name="default-width">
      win
    </lookup>
  </binding>
</object>
~~~
@@@end

The expression created by the lookup tag returns a int type GValue.
On the other hand "label" property holds a string type GValue.
When a GValue is copied to another GValue, the type is automatically converted if possible.
If the current width is 100, an int `100` is converted to a string `"100"`.

If you use `g_object_get` and `g_object_set` to copy properties, the value is automatically converted. 

## Meson.build

The source files are in `src/expression` directory.
You can build all the files at once.

~~~
$ cd src/expression
$ meson setup _build
$ ninja -C _build
~~~

For example, if you want to run "exp", which is the executable file from "exp.c", type `_build/exp`.
You can run other programs as well.

The file `meson.build` is as follows.

@@@include
expression/meson.build
@@@
