Up: [Readme.md](../Readme.md),  Prev: [Section 14](sec14.md), Next: [Section 16](sec16.md)

# tfe5 source files

## How to compile and execute tfe text editor.

First, source files are shown in the later subsections.
How to download them is written at the end of the [previous section](sec14.md).

The following is the instruction of compilation and execution.

- You need meson and ninja.
- Set necessary environment variables.
If you have installed gtk4 under the instruction in [Section 2](sec2.md), type `. env.sh` to set the environment variables.
- change your current directory to `src/tfe5` directory.
- type `meson _build` for configuration.
- type `ninja -C _build` for compilation.
Then the application `tfe` is build under the `_build` directory.
- type `_build/tfe` to execute it.

Then the window appears.
There are four buttons, `New`, `Open`, `Save` and `Close`.

- Click on `Open` button, then a FileChooserDialog appears.
Choose a file in the list and click on `Open` button.
Then the file is read and a new Notebook Page appears.
- Edit the file and click on `Save` button, then the text is saved to the original file.
- Click `Close`, then the Notebook Page disappears.
- Click `Close` again, then the `Untitled` Notebook Page disappears and at the same time the application quits.

This is a very simple editor.
It is a good practice for you to add more features.

## meson.build

~~~meson
 1 project('tfe', 'c')
 2 
 3 gtkdep = dependency('gtk4')
 4 
 5 gnome=import('gnome')
 6 resources = gnome.compile_resources('resources','tfe.gresource.xml')
 7 
 8 sourcefiles=files('tfeapplication.c', 'tfenotebook.c', '../tfetextview/tfetextview.c')
 9 
10 executable('tfe', sourcefiles, resources, dependencies: gtkdep)
~~~

## tfe.gresource.xml

~~~xml
1 <?xml version="1.0" encoding="UTF-8"?>
2 <gresources>
3   <gresource prefix="/com/github/ToshioCP/tfe">
4     <file>tfe.ui</file>
5   </gresource>
6 </gresources>
~~~

## tfe.ui

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <interface>
 3   <object class="GtkApplicationWindow" id="win">
 4     <property name="title">file editor</property>
 5     <property name="default-width">600</property>
 6     <property name="default-height">400</property>
 7     <child>
 8       <object class="GtkBox" id="boxv">
 9         <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
10         <child>
11           <object class="GtkBox" id="boxh">
12             <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
13             <child>
14               <object class="GtkLabel" id="dmy1">
15                 <property name="width-chars">10</property>
16               </object>
17             </child>
18             <child>
19               <object class="GtkButton" id="btnn">
20                 <property name="label">New</property>
21               </object>
22             </child>
23             <child>
24               <object class="GtkButton" id="btno">
25                 <property name="label">Open</property>
26               </object>
27             </child>
28             <child>
29               <object class="GtkLabel" id="dmy2">
30                 <property name="hexpand">TRUE</property>
31               </object>
32             </child>
33             <child>
34               <object class="GtkButton" id="btns">
35                 <property name="label">Save</property>
36               </object>
37             </child>
38             <child>
39               <object class="GtkButton" id="btnc">
40                 <property name="label">Close</property>
41               </object>
42             </child>
43             <child>
44               <object class="GtkLabel" id="dmy3">
45                 <property name="width-chars">10</property>
46               </object>
47             </child>
48           </object>
49         </child>
50         <child>
51           <object class="GtkNotebook" id="nb">
52             <property name="scrollable">TRUE</property>
53             <property name="hexpand">TRUE</property>
54             <property name="vexpand">TRUE</property>
55           </object>
56         </child>
57       </object>
58     </child>
59   </object>
60 </interface>
61 
~~~

## tfe.h

~~~C
1 #include <gtk/gtk.h>
2 
3 #include "../tfetextview/tfetextview.h"
4 #include "tfenotebook.h"
~~~

## tfeapplication.c

~~~C
 1 #include "tfe.h"
 2 
 3 static void
 4 open_cb (GtkNotebook *nb) {
 5   notebook_page_open (nb);
 6 }
 7 
 8 static void
 9 new_cb (GtkNotebook *nb) {
10   notebook_page_new (nb);
11 }
12 
13 static void
14 save_cb (GtkNotebook *nb) {
15   notebook_page_save (nb);
16 }
17 
18 static void
19 close_cb (GtkNotebook *nb) {
20   notebook_page_close (GTK_NOTEBOOK (nb));
21 }
22 
23 static void
24 tfe_activate (GApplication *application) {
25   GtkApplication *app = GTK_APPLICATION (application);
26   GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
27   GtkWidget *boxv = gtk_window_get_child (GTK_WINDOW (win));
28   GtkNotebook *nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));
29 
30   notebook_page_new (nb);
31   gtk_widget_show (GTK_WIDGET (win));
32 }
33 
34 static void
35 tfe_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
36   GtkApplication *app = GTK_APPLICATION (application);
37   GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
38   GtkWidget *boxv = gtk_window_get_child (GTK_WINDOW (win));
39   GtkNotebook *nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));
40   int i;
41 
42   for (i = 0; i < n_files; i++)
43     notebook_page_new_with_file (nb, files[i]);
44   if (gtk_notebook_get_n_pages (nb) == 0)
45     notebook_page_new (nb);
46   gtk_widget_show (win);
47 }
48 
49 static void
50 tfe_startup (GApplication *application) {
51   GtkApplication *app = GTK_APPLICATION (application);
52   GtkBuilder *build;
53   GtkApplicationWindow *win;
54   GtkNotebook *nb;
55   GtkButton *btno;
56   GtkButton *btnn;
57   GtkButton *btns;
58   GtkButton *btnc;
59 
60   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/tfe.ui");
61   win = GTK_APPLICATION_WINDOW (gtk_builder_get_object (build, "win"));
62   nb = GTK_NOTEBOOK (gtk_builder_get_object (build, "nb"));
63   gtk_window_set_application (GTK_WINDOW (win), app);
64   btno = GTK_BUTTON (gtk_builder_get_object (build, "btno"));
65   btnn = GTK_BUTTON (gtk_builder_get_object (build, "btnn"));
66   btns = GTK_BUTTON (gtk_builder_get_object (build, "btns"));
67   btnc = GTK_BUTTON (gtk_builder_get_object (build, "btnc"));
68   g_signal_connect_swapped (btno, "clicked", G_CALLBACK (open_cb), nb);
69   g_signal_connect_swapped (btnn, "clicked", G_CALLBACK (new_cb), nb);
70   g_signal_connect_swapped (btns, "clicked", G_CALLBACK (save_cb), nb);
71   g_signal_connect_swapped (btnc, "clicked", G_CALLBACK (close_cb), nb);
72   g_object_unref(build);
73 
74 GdkDisplay *display;
75 
76   display = gtk_widget_get_display (GTK_WIDGET (win));
77   GtkCssProvider *provider = gtk_css_provider_new ();
78   gtk_css_provider_load_from_data (provider, "textview {padding: 10px; font-family: monospace; font-size: 12pt;}", -1);
79   gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
80 }
81 
82 int
83 main (int argc, char **argv) {
84   GtkApplication *app;
85   int stat;
86 
87   app = gtk_application_new ("com.github.ToshioCP.tfe", G_APPLICATION_HANDLES_OPEN);
88 
89   g_signal_connect (app, "startup", G_CALLBACK (tfe_startup), NULL);
90   g_signal_connect (app, "activate", G_CALLBACK (tfe_activate), NULL);
91   g_signal_connect (app, "open", G_CALLBACK (tfe_open), NULL);
92 
93   stat =g_application_run (G_APPLICATION (app), argc, argv);
94   g_object_unref (app);
95   return stat;
96 }
97 
~~~

## tfenotebook.h

~~~C
 1 void
 2 notebook_page_save(GtkNotebook *nb);
 3 
 4 void
 5 notebook_page_close (GtkNotebook *nb);
 6 
 7 void
 8 notebook_page_open (GtkNotebook *nb);
 9 
10 void
11 notebook_page_new_with_file (GtkNotebook *nb, GFile *file);
12 
13 void
14 notebook_page_new (GtkNotebook *nb);
15 
~~~

## tfenotebook.c

~~~C
  1 #include "tfe.h"
  2 
  3 /* The returned string should be freed with g_free() when no longer needed. */
  4 static gchar*
  5 get_untitled () {
  6   static int c = -1;
  7   if (++c == 0) 
  8     return g_strdup_printf("Untitled");
  9   else
 10     return g_strdup_printf ("Untitled%u", c);
 11 }
 12 
 13 static TfeTextView *
 14 get_current_textview (GtkNotebook *nb) {
 15   int i;
 16   GtkWidget *scr;
 17   GtkWidget *tv;
 18 
 19   i = gtk_notebook_get_current_page (nb);
 20   scr = gtk_notebook_get_nth_page (nb, i);
 21   tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
 22   return TFE_TEXT_VIEW (tv);
 23 }
 24 
 25 static void
 26 file_changed_cb (TfeTextView *tv, GtkNotebook *nb) {
 27   GtkWidget *scr;
 28   GtkWidget *label;
 29   GFile *file;
 30   char *filename;
 31 
 32   file = tfe_text_view_get_file (tv);
 33   scr = gtk_widget_get_parent (GTK_WIDGET (tv));
 34   if (G_IS_FILE (file)) {
 35     filename = g_file_get_basename (file);
 36     g_object_unref (file);
 37   } else
 38     filename = get_untitled ();
 39   label = gtk_label_new (filename);
 40   gtk_notebook_set_tab_label (nb, scr, label);
 41 }
 42 
 43 void
 44 notebook_page_save (GtkNotebook *nb) {
 45   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
 46 
 47   TfeTextView *tv;
 48 
 49   tv = get_current_textview (nb);
 50   tfe_text_view_save (TFE_TEXT_VIEW (tv));
 51 }
 52 
 53 void
 54 notebook_page_close (GtkNotebook *nb) {
 55   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
 56 
 57   GtkWidget *win;
 58   int i;
 59 
 60   if (gtk_notebook_get_n_pages (nb) == 1) {
 61     win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);
 62     gtk_window_destroy(GTK_WINDOW (win));
 63   } else {
 64     i = gtk_notebook_get_current_page (nb);
 65     gtk_notebook_remove_page (GTK_NOTEBOOK (nb), i);
 66   }
 67 }
 68 
 69 static void
 70 notebook_page_build (GtkNotebook *nb, GtkWidget *tv, char *filename) {
 71   GtkWidget *scr = gtk_scrolled_window_new ();
 72   GtkNotebookPage *nbp;
 73   GtkWidget *lab;
 74   int i;
 75 
 76   gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
 77   gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
 78   lab = gtk_label_new (filename);
 79   i = gtk_notebook_append_page (nb, scr, lab);
 80   nbp = gtk_notebook_get_page (nb, scr);
 81   g_object_set (nbp, "tab-expand", TRUE, NULL);
 82   gtk_notebook_set_current_page (nb, i);
 83   g_signal_connect (GTK_TEXT_VIEW (tv), "change-file", G_CALLBACK (file_changed_cb), nb);
 84 }
 85 
 86 static void
 87 open_response (TfeTextView *tv, int response, GtkNotebook *nb) {
 88   GFile *file;
 89   char *filename;
 90 
 91   if (response != TFE_OPEN_RESPONSE_SUCCESS) {
 92     g_object_ref_sink (tv);
 93     g_object_unref (tv);
 94   }else if (! G_IS_FILE (file = tfe_text_view_get_file (tv))) {
 95     g_object_ref_sink (tv);
 96     g_object_unref (tv);
 97   }else {
 98     filename = g_file_get_basename (file);
 99     g_object_unref (file);
100     notebook_page_build (nb, GTK_WIDGET (tv), filename);
101   }
102 }
103 
104 void
105 notebook_page_open (GtkNotebook *nb) {
106   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
107 
108   GtkWidget *tv;
109 
110   tv = tfe_text_view_new ();
111   g_signal_connect (TFE_TEXT_VIEW (tv), "open-response", G_CALLBACK (open_response), nb);
112   tfe_text_view_open (TFE_TEXT_VIEW (tv), GTK_WINDOW (gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW)));
113 }
114 
115 void
116 notebook_page_new_with_file (GtkNotebook *nb, GFile *file) {
117   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
118   g_return_if_fail(G_IS_FILE (file));
119 
120   GtkWidget *tv;
121   char *filename;
122 
123   if ((tv = tfe_text_view_new_with_file (file)) == NULL)
124     return; /* read error */
125   filename = g_file_get_basename (file);
126   notebook_page_build (nb, tv, filename);
127 }
128 
129 void
130 notebook_page_new (GtkNotebook *nb) {
131   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
132 
133   GtkWidget *tv;
134   char *filename;
135 
136   tv = tfe_text_view_new ();
137   filename = get_untitled ();
138   notebook_page_build (nb, tv, filename);
139 }
140 
~~~

## tfetextview.h

~~~C
 1 #ifndef __TFE_TEXT_VIEW_H__
 2 #define __TFE_TEXT_VIEW_H__
 3 
 4 #include <gtk/gtk.h>
 5 
 6 #define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type ()
 7 G_DECLARE_FINAL_TYPE (TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView)
 8 
 9 /* "open-response" signal response */
10 enum TfeTextViewOpenResponseType
11 {
12   TFE_OPEN_RESPONSE_SUCCESS,
13   TFE_OPEN_RESPONSE_CANCEL,
14   TFE_OPEN_RESPONSE_ERROR
15 };
16 
17 GFile *
18 tfe_text_view_get_file (TfeTextView *tv);
19 
20 void
21 tfe_text_view_open (TfeTextView *tv, GtkWindow *win);
22 
23 void
24 tfe_text_view_save (TfeTextView *tv);
25 
26 void
27 tfe_text_view_saveas (TfeTextView *tv);
28 
29 GtkWidget *
30 tfe_text_view_new_with_file (GFile *file);
31 
32 GtkWidget *
33 tfe_text_view_new (void);
34 
35 #endif /* __TFE_TEXT_VIEW_H__ */
~~~

## tfetextview.c

~~~C
  1 #include <string.h>
  2 #include "tfetextview.h"
  3 
  4 struct _TfeTextView
  5 {
  6   GtkTextView parent;
  7   GFile *file;
  8 };
  9 
 10 G_DEFINE_TYPE (TfeTextView, tfe_text_view, GTK_TYPE_TEXT_VIEW);
 11 
 12 enum {
 13   CHANGE_FILE,
 14   OPEN_RESPONSE,
 15   NUMBER_OF_SIGNALS
 16 };
 17 
 18 static guint tfe_text_view_signals[NUMBER_OF_SIGNALS];
 19 
 20 static void
 21 tfe_text_view_dispose (GObject *gobject) {
 22   TfeTextView *tv = TFE_TEXT_VIEW (gobject);
 23 
 24   if (G_IS_FILE (tv->file))
 25     g_clear_object (&tv->file);
 26 
 27   G_OBJECT_CLASS (tfe_text_view_parent_class)->dispose (gobject);
 28 }
 29 
 30 static void
 31 tfe_text_view_init (TfeTextView *tv) {
 32   tv->file = NULL;
 33 }
 34 
 35 static void
 36 tfe_text_view_class_init (TfeTextViewClass *class) {
 37   GObjectClass *object_class = G_OBJECT_CLASS (class);
 38 
 39   object_class->dispose = tfe_text_view_dispose;
 40   tfe_text_view_signals[CHANGE_FILE] = g_signal_newv ("change-file",
 41                                  G_TYPE_FROM_CLASS (class),
 42                                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
 43                                  NULL /* closure */,
 44                                  NULL /* accumulator */,
 45                                  NULL /* accumulator data */,
 46                                  NULL /* C marshaller */,
 47                                  G_TYPE_NONE /* return_type */,
 48                                  0     /* n_params */,
 49                                  NULL  /* param_types */);
 50   GType param_types[] = {G_TYPE_INT}; 
 51   tfe_text_view_signals[OPEN_RESPONSE] = g_signal_newv ("open-response",
 52                                  G_TYPE_FROM_CLASS (class),
 53                                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
 54                                  NULL /* closure */,
 55                                  NULL /* accumulator */,
 56                                  NULL /* accumulator data */,
 57                                  NULL /* C marshaller */,
 58                                  G_TYPE_NONE /* return_type */,
 59                                  1     /* n_params */,
 60                                  param_types);
 61 }
 62 
 63 GFile *
 64 tfe_text_view_get_file (TfeTextView *tv) {
 65   g_return_val_if_fail (TFE_IS_TEXT_VIEW (tv), NULL);
 66 
 67   if (G_IS_FILE (tv->file))
 68     return g_file_dup (tv->file);
 69   else
 70     return NULL;
 71 }
 72 
 73 static gboolean
 74 save_file (GFile *file, GtkTextBuffer *tb, GtkWindow *win) {
 75   GtkTextIter start_iter;
 76   GtkTextIter end_iter;
 77   gchar *contents;
 78   GtkWidget *message_dialog;
 79   GError *err = NULL;
 80 
 81   /* This function doesn't check G_IS_FILE (file). The caller should check it. */
 82   gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
 83   contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
 84   if (g_file_replace_contents (file, contents, strlen (contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, &err)) {
 85     gtk_text_buffer_set_modified (tb, FALSE);
 86     return TRUE;
 87   } else {
 88     message_dialog = gtk_message_dialog_new (win, GTK_DIALOG_MODAL,
 89                                              GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
 90                                             "%s.\n", err->message);
 91     g_signal_connect (message_dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
 92     gtk_widget_show (message_dialog);
 93     g_error_free (err);
 94     return FALSE;
 95   }
 96 }
 97 
 98 static void
 99 saveas_dialog_response (GtkWidget *dialog, gint response, TfeTextView *tv) {
100   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
101   GFile *file;
102   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
103 
104   if (response == GTK_RESPONSE_ACCEPT) {
105     file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));
106     if (! G_IS_FILE (file))
107       g_warning ("TfeTextView: gtk_file_chooser_get_file returns non GFile object.\n");
108     else if (save_file(file, tb, GTK_WINDOW (win))) {
109       if (G_IS_FILE (tv->file))
110         g_object_unref (tv->file);
111       tv->file = file;
112       g_signal_emit (tv, tfe_text_view_signals[CHANGE_FILE], 0);
113     }
114   }
115   gtk_window_destroy (GTK_WINDOW (dialog));
116 }
117 
118 void
119 tfe_text_view_save (TfeTextView *tv) {
120   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
121 
122   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
123   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
124 
125   if (! gtk_text_buffer_get_modified (tb))
126     return; /* no need to save it */
127   else if (tv->file == NULL)
128     tfe_text_view_saveas (tv);
129   else if (! G_IS_FILE (tv->file))
130     g_error ("TfeTextView: The pointer in this object isn't NULL nor GFile object.\n");
131   else
132     save_file (tv->file, tb, GTK_WINDOW (win));
133 }
134 
135 void
136 tfe_text_view_saveas (TfeTextView *tv) {
137   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
138 
139   GtkWidget *dialog;
140   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
141 
142   dialog = gtk_file_chooser_dialog_new ("Save file", GTK_WINDOW (win), GTK_FILE_CHOOSER_ACTION_SAVE,
143                                       "Cancel", GTK_RESPONSE_CANCEL,
144                                       "Save", GTK_RESPONSE_ACCEPT,
145                                       NULL);
146   g_signal_connect (dialog, "response", G_CALLBACK (saveas_dialog_response), tv);
147   gtk_widget_show (dialog);
148 }
149 
150 GtkWidget *
151 tfe_text_view_new_with_file (GFile *file) {
152   g_return_val_if_fail (G_IS_FILE (file), NULL);
153 
154   GtkWidget *tv;
155   GtkTextBuffer *tb;
156   char *contents;
157   gsize length;
158 
159   if (! g_file_load_contents (file, NULL, &contents, &length, NULL, NULL)) /* read error */
160     return NULL;
161 
162   tv = tfe_text_view_new();
163   tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
164   gtk_text_buffer_set_text (tb, contents, length);
165   g_free (contents);
166   TFE_TEXT_VIEW (tv)->file = g_file_dup (file);
167   return tv;
168 }
169 
170 static void
171 open_dialog_response(GtkWidget *dialog, gint response, TfeTextView *tv) {
172   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
173   GFile *file;
174   char *contents;
175   gsize length;
176   GtkWidget *message_dialog;
177   GError *err = NULL;
178 
179   if (response != GTK_RESPONSE_ACCEPT)
180     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_CANCEL);
181   else if (! G_IS_FILE (file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog)))) {
182     g_warning ("TfeTextView: gtk_file_chooser_get_file returns non GFile object.\n");
183     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_ERROR);
184   } else if (! g_file_load_contents (file, NULL, &contents, &length, NULL, &err)) { /* read error */
185     if (G_IS_FILE (file))
186       g_object_unref (file);
187     message_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog), GTK_DIALOG_MODAL,
188                                              GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
189                                             "%s.\n", err->message);
190     g_signal_connect (message_dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
191     gtk_widget_show (message_dialog);
192     g_error_free (err);
193     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_ERROR);
194   } else {
195     gtk_text_buffer_set_text (tb, contents, length);
196     g_free (contents);
197     if (G_IS_FILE (tv->file))
198       g_object_unref (tv->file);
199     tv->file = file;
200     gtk_text_buffer_set_modified (tb, FALSE);
201     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_SUCCESS);
202     g_signal_emit (tv, tfe_text_view_signals[CHANGE_FILE], 0);
203   }
204   gtk_window_destroy (GTK_WINDOW (dialog));
205 }
206 
207 void
208 tfe_text_view_open (TfeTextView *tv, GtkWindow *win) {
209   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
210   g_return_if_fail (GTK_IS_WINDOW (win));
211 
212   GtkWidget *dialog;
213 
214   dialog = gtk_file_chooser_dialog_new ("Open file", win, GTK_FILE_CHOOSER_ACTION_OPEN,
215                                         "Cancel", GTK_RESPONSE_CANCEL,
216                                         "Open", GTK_RESPONSE_ACCEPT,
217                                         NULL);
218   g_signal_connect (dialog, "response", G_CALLBACK (open_dialog_response), tv);
219   gtk_widget_show (dialog);
220 }
221 
222 GtkWidget *
223 tfe_text_view_new (void) {
224   return GTK_WIDGET (g_object_new (TFE_TYPE_TEXT_VIEW, NULL));
225 }
226 
~~~

## Total number of lines, words and characters

~~~
$ LANG=C wc tfe5/meson.build tfe5/tfeapplication.c tfe5/tfe.gresource.xml tfe5/tfe.h tfe5/tfenotebook.c tfe5/tfenotebook.h tfetextview/tfetextview.c tfetextview/tfetextview.h tfe5/tfe.ui
   10    17   294 tfe5/meson.build
   97   301  3159 tfe5/tfeapplication.c
    6     9   153 tfe5/tfe.gresource.xml
    4     6    87 tfe5/tfe.h
  140   374  3593 tfe5/tfenotebook.c
   15    21   241 tfe5/tfenotebook.h
  226   677  8023 tfetextview/tfetextview.c
   35    60   701 tfetextview/tfetextview.h
   61   100  2073 tfe5/tfe.ui
  594  1565 18324 total
~~~


Up: [Readme.md](../Readme.md),  Prev: [Section 14](sec14.md), Next: [Section 16](sec16.md)
