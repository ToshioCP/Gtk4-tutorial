# Managing the Application

A good rule of thumb in programming is to split your code into separate files based on what they do.
Following this idea, we should reorganize TFE's `application.c`.
We're going to divide it into three main parts:

- `main`: This function handles the command-line interface.
- `application`: This is the core application object.
It creates and manages things like windows.
- `window`: This object handles the window itself and the widgets inside it.

In this section, we'll focus on the first two parts and save the window management for later.

## Creating an Application Class

When you write a GTK application, you have two main options:

- Use GtkApplication exactly as it is.
- Create your own subclass of GtkApplication.

Creating a subclass is generally the better choice because it lets you use "instance variables".
An instance variable is simply a piece of data attached to a specific object.
For example, remember how we made TfeTextView (a subclass of GtkTextView)?
We gave it an instance variable called `file` to store a GFile object.
If we had a TfeTextView object named `tv`, we could easily access that file using `tv->file`.

Applications usually need to keep track of objects other than just windows.
For example, TFE will eventually need to manage settings using a GSettings object.
The standard way to keep track of these objects is by saving pointers to them as instance variables.
So, let's define our own TfeApplication class as a subclass of GtkApplication.

If you remember how we made TfeTextView, you'll find this process very familiar.

## Writing the Header and C Files.

Let's quickly review how to write the header and C files for our new class.
We'll start with the header file.

@@@include
tfe7/tfeapplication.h
@@@

- 5: We define the GType for TfeApplication using a macro.
There are strict naming rules here:
  - The macro name must be ALL CAPS like this: `PREFIX_TYPE_OBJECT_NAME`.
  - The function name must be all lowercase like this: `prefix_object_name_get_type ()`.
- 6: We use the `G_DECLARE_FINAL_TYPE` macro to set up our class.
  - 1st argument: CamelCase (`PrefixObjectname`) like: `TfeApplication`
  - 2nd argument: lowercase (`prefix_objectname`) like: `tfe_application`
  - 3rd argument: ALL CAPS (`PREFIX`) like: `TFE`
  - 4th argument: ALL CAPS (`OBJECTNAME`) like:  `APPLICATION`
  - 5th argument: CamelCase (`ParentObjectname`) like: `GtkApplication`
- 8-9: The only function we need to make public is the one that creates our application, `tfe_application_new`.

Now, let's look at the C file.
The beginning looks just like what we did for TfeTextView.

@@@if gfm
```C
@@@else
```{.c}
@@@end
#include <gtk/gtk.h>
#include "tfeapplication.h"

struct _TfeApplication {
  GtkApplication parent;
  GSettings *settings;
};

G_DEFINE_FINAL_TYPE (TfeApplication, tfe_application, GTK_TYPE_APPLICATION)
```

- 1: Include the main GTK header.
- 2: You must include your object's own header file.
- 4-7: Here is where we declare the data for our object.
The struct name must be CamelCase, starting with an underscore: `_TfeApplication`.
The very first member must be the parent object's structure.
You can name it whatever you want, but calling it "parent" is the standard practice.
After "parent", you can add your instance variables.
We're adding a pointer for GSettings here because we plan to use it later.
- 9: The `G_DEFINE_FINAL_TYPE` macro.
  - 1st argument: CamelCase. `TfeApplication`
  - 2nd argument: lowercase. `tfe_application`
  - 3rd argument: ALL CAPS (The parent's type macro). `GTK_TYPE_APPLICATION`

These steps are the standard recipe for creating a subclass in GObject.
If you want to know exactly what these macros do behind the scenes, check out the [GObject tutorial](https://toshiocp.github.io/Gobject-tutorial).

## Initializing the Instance and ClassInstance

### Instance Initialization

Right now, we don't actually need to do any special setup when a new instance is created.
However, GTK expects an initialization function to exist, so we still have to write one, even if it's completely empty inside.

### Instance Disposal

@@@include
tfe7/tfeapplication.c tfe_application_dispose
@@@

We already covered how disposal works in detail back in Section 11, so we'll keep it brief here.
For now, our application doesn't need to clean anything up.
But later, when we start using GSettings (which we'll set up during the application's startup), we will need to free that memory here.
So, we've just left a comment as a reminder.
The most important thing to remember about your dispose function is to always call the parent class's dispose function at the end to chain up the cleanup process.

### Class Initialization

#### Overriding the Dispose Method

@@@include
tfe7/tfeapplication.c tfe_application_class_init
@@@

When the class is initialized, we can override class methods.

First, we need to override the dispose method so GTK knows to call our custom cleanup code.
We did the exact same thing for TfeTextView in Section 11, so feel free to look back at that if you need a refresher.

#### Overriding the Startup, Activate, and Open Methods

Take a look at the family tree for our TfeApplication class:

```
GObject - GApplication - GtkApplication - TfeApplication
```

It's actually the GApplication class that defines the "startup", "activate", and "open" signals.
In GTK, there are generally two ways to handle signals:

- Default signal handlers: These are built right into the class that created the signal.
- User-defined signal handlers: These are attached from the outside using functions like `g_signal_connect`.

Let's look at the "startup" signal.
The GApplication class owns the default handler for it.
Inside the class structure, there is a member named `startup` that points to the GApplication startup function.
GTK uses this startup pointer to run the code; it doesn't call the function directly by its name.

When you make a subclass, the beginning of its class structure is an exact copy of the parent's class structure.
For example, the start of the GtkApplication class structure looks just like the GApplication class structure:

@@@if gfm
```C
@@@else
```{.c}
@@@end
struct _GtkApplicationClass
{
  GApplicationClass parent_class; /* This member has the same structure as GApplicationClass */
  ... ... ...
}
```

Because of this, the `startup` member is included in this GApplicationClass area.
As a subclass, we can write our own custom startup function and change this `startup` member so it points to our new function instead.
This is called overriding.

@@@if gfm
```C
@@@else
```{.c}
@@@end
static void
gtk_application_class_init (GtkApplicationClass *class)
{
  ... ... ...
  application_class->startup = gtk_application_startup;
  ... ... ...
}
```

When we override the startup process, it's crucial that we still run the parent class's startup code.
We do this by calling the parent's startup function right at the beginning of our custom function.

@@@if gfm
```C
@@@else
```{.c}
@@@end
static void
gtk_application_startup (GApplication *g_application)
{
  ... ... ...
  G_APPLICATION_CLASS (gtk_application_parent_class)->startup (g_application);
  ... ... ...
}
```

So, to set up our own startup process in TfeApplication, we need to do two things:

- Inside `tfe_application_class_init`, override the startup member so it points to our custom function.
- Inside our custom startup function, call the parent class's (GtkApplication) startup function first.

We follow the exact same steps to override the "activate" and "open" handlers.

@@@if gfm
```C
@@@else
```{.c}
@@@end
static void
tfe_application_class_init (TfeApplicationClass *class) {
  G_OBJECT_CLASS (class)->dispose = tfe_application_dispose;
  G_APPLICATION_CLASS (class)->startup = app_startup;
  G_APPLICATION_CLASS (class)->activate = app_activate;
  G_APPLICATION_CLASS (class)->open = app_open;
}

static void
app_startup (GApplication *application) {
  ... ... ...
  G_APPLICATION_CLASS (tfe_application_parent_class)->startup (application);
  ... ... ...
}
```

#### Looking Ahead: TfeWindow

In the next section, we'll build TfeWindow, a class completely dedicated to managing our application's window.
Since the "activate" and "open" handlers are the ones that actually create the window, building TfeWindow will change how those handlers are written.
Once we have our window class ready, we'll come back to `tfeapplication.c` and show how they connect.

## The Main Function

Finally, let's put our main function in its own file, `main.c`.
The only real difference from before is that we are now creating and running our brand new TfeApplication instead of a plain GtkApplication.

@@@include
tfe7/main.c
@@@
