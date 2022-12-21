Up: [README.md](../README.md),  Prev: [Section 5](sec5.md), Next: [Section 7](sec7.md)

# Strings and memory management

GtkTextView and GtkTextBuffer have functions that use string parameters or return a string.
The knowledge of strings and memory management is useful to understand how to use these functions.

## String and memory

A String is an array of characters that is terminated with '\0'.
String is not a C type such as char, int, float or double, but a character array.
It behaves like a string in other languages.
So, the pointer is often called 'a string'.

The following is a sample program.

~~~C
char a[10], *b;

a[0] = 'H';
a[1] = 'e';
a[2] = 'l';
a[3] = 'l';
a[4] = 'o';
a[5] = '\0';

b = a;
/* *b is 'H' */
/* *(++b) is 'e' */
~~~

An array `a` is defined as a `char` type array and its size is ten.
The first five elements are 'H', 'e', 'l', 'l', 'o'.
They are character codes.
For example, 'H' is the same as 0x48 or 72.
The sixth element is '\0', which is the same as zero, and indicates that the sequence of the data ends there.
The array represents the string "Hello".

The size of the array is 10, so 4 bytes aren't used.
But it's OK.
They are just ignored.
(If 'a' is defined out of functions or its class is static, they are assigned with zero.
Otherwise, that is to say, the class is auto or register, they are undefined.)

The variable 'b' is a pointer to a character.
It is assigned with `a`, so `b` points the first element of `a` (character 'H').
The array `a` is immutable.
So `a=a+1` causes syntax error.

On the other hand, 'b' is a pointer type variable, which is mutable.
So, `++b`, which increases `b` by one, is allowed.


If a pointer is NULL, it points nothing.
So, the pointer is not a string.
It is different from empty string.
Empty string is a pointer points `\0`.

There are four cases:

- The string is read only
- The string is in static memory area
- The string is in stack
- The string is in memory allocated from the heap area

## Read only string

A string literal is surrounded by double quotes like this:

~~~C
char *s;
s = "Hello"
~~~

"Hello" is a string literal, and is read only.
So, the following program is illegal.

~~~C
*(s+1) = 'a';
~~~

The result is undefined.
Probably a bad thing will happen, for example, a segmentation fault.

NOTE: The memory of the literal string is allocated when the program is compiled.
It is possible to see the literal strings with `strings` command.

~~~
$ strings src/tvf/a.out
/lib64/ld-linux-x86-64.so.2
cN<5
... ... ...
... ... ...
Once upon a time, there was an old man who was called Taketori-no-Okina. It is a japanese word that means a man whose work is making bamboo baskets.
One day, he went into a mountain and found a shining bamboo. "What a mysterious bamboo it is!," he said. He cut it, then there was a small cute baby girl in it. The girl was shining faintly. He thought this baby girl is a gift from Heaven and took her home.
His wife was surprized at his story. They were very happy because they had no children. 
... ... ...
... ... ...
~~~

It tells us that literal strings are embedded in program binary codes.

## Strings defined as arrays

If a string is defined as an array, it's stored in static memory area or stack.
It depends on the class of the array.
If the array's class is `static`, then it's placed in static memory area.
The allocated memory lives for the life of the program.
This area is writable.

If the array's class is `auto`, it's placed in stack.
If the array is defined inside a function, its default class is `auto`.
The stack area will disappear when the function returns to the caller.
Arrays defined on the stack are writable.

~~~C
static char a[] = {'H', 'e', 'l', 'l', 'o', '\0'};

void
print_strings (void) {
  char b[] = "Hello";

  a[1] = 'a'; /* Because the array is static, it's writable. */
  b[1] = 'a'; /* Because the array is auto, it's writable. */

  printf ("%s\n", a); /* Hallo */
  printf ("%s\n", b); /* Hallo */
}
~~~

The array `a` is defined out of functions.
It is placed in the static memory area even if the `static` class is left out.
The compiler calculates the number of the elements (six) and allocates six bytes in the static memory area.
Then, it copies "Hello" literal string data to the memory.

The array `b` is defined inside the function, so its class is `auto`.
The compiler calculates the number of the elements in the string literal.
It is six because it has '\0' terminator.
The compiler allocates six bytes in the stack and copies "Hello" litaral string to the stack memory.

Both `a` and `b` are writable.

The memory is allocated and freed by the program automatically so you don't need to allocate or free.
The array `a` is alive during the program's life time.
The array `b` is alive when the function is called until the function returns to the caller.

## Strings in the heap area

You can get, use and release memory from the heap area.
The standard C library provides `malloc` to get memory and `free` to put back memory.
GLib provides the functions `g_new` and `g_free`.
They are similar to `malloc` and `free`.

~~~C
g_new (struct_type, n_struct)
~~~

`g_new` is a macro to allocate memory for an array.

- `struct_type` is the type of the element of the array.
- `n_struct` is the size of the array.
- The return value is a pointer to the array.
Its type is a pointer to `struct_type`.

For example,

~~~C
char *s;
s = g_new (char, 10);
/* s points an array of char. The size of the array is 10. */

struct tuple {int x, y;} *t;
t = g_new (struct tuple, 5);
/* t points an array of struct tuple. */
/* The size of the array is 5. */
~~~

`g_free` frees memory.

~~~C
void
g_free (gpointer mem);
~~~

If `mem` is NULL, `g_free` does nothing.
`gpointer` is a type of general pointer.
It is the same as `void *`.
This pointer can be casted to any pointer type.
Conversely, any pointer type can be casted to `gpointer`.

~~~C
g_free (s);
/* Frees the memory allocated to s. */

g_free (t);
/* Frees the memory allocated to t. */
~~~

If the argument doesn't point allocated memory it will cause an error, specifically, a segmentation fault.

Some GLib functions allocate memory.
For example, `g_strdup` allocates memory and copies a string given as an argument.

~~~C
char *s;
s = g_strdup ("Hello");
g_free (s);
~~~

The string literal "Hello" has 6 bytes because the string has '\0' at the end.
`g_strdup` gets 6 bytes from the heap area and copies the string to the memory.
`s` is assigned the start address of the memory.
`g_free` returns the memory to the heap area.

`g_strdup` is described in [GLib API Reference](https://docs.gtk.org/glib/func.strdup.html).
The following is extracted from the reference.

> The returned string should be freed with `g_free()` when no longer needed.

If you forget to free the allocated memory it will remain until the program ends.
Repeated allocation and no freeing cause memory leak.
It is a bug and may bring a serious problem.

## const qualifier

A `const` qualified variable can be assigned to initialize it.
Once it is initialized, it is never allowed to change or free.

~~~C
const int x = 10; /* initialization is OK. */

x = 20; /* This is illegal because x is qualified with const */
~~~

If a function returns `const char*` type, the string can't be changed or freed.
If a function has a `const char *` type parameter, it ensures that the parameter is not changed in the function.

~~~C
// You never change or free the returned string.
const char*
gtk_label_get_text (
  GtkLabel* self
)

// Str keeps itself during the function runs
void
gtk_label_set_text (
  GtkLabel* self,
  const char* str
)
~~~




Up: [README.md](../README.md),  Prev: [Section 5](sec5.md), Next: [Section 7](sec7.md)
