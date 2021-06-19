Up: [Readme.md](../Readme.md),  Prev: [Section 15](sec15.md), Next: [Section 17](sec17.md)

# tfe5 source files

## How to compile and execute the text editor 'tfe'.

First, source files are shown in the later subsections.
How to download them is written at the end of the [previous section](sec15.md).

The following is the instruction of compilation and execution.

- You need meson and ninja.
- Set environment variables if necessary.
If you have installed gtk4 from the source and you preferred the option `--prefix $HOME/local` (see [Section 2](sec2.md)), type `. env.sh` to set the environment variables.

~~~
$ . env.sh
~~~
- change your current directory to `src/tfe5` directory.
- type `meson _build` for configuration.
- type `ninja -C _build` for compilation.
Then the application `tfe` is built under the `_build` directory.
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
24 app_activate (GApplication *application) {
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
35 app_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
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
50 app_startup (GApplication *application) {
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
79   gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
80 }
81 
82 #define APPLICATION_ID "com.github.ToshioCP.tfe"
83 
84 int
85 main (int argc, char **argv) {
86   GtkApplication *app;
87   int stat;
88 
89   app = gtk_application_new (APPLICATION_ID, G_APPLICATION_HANDLES_OPEN);
90 
91   g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
92   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
93   g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
94 
95   stat =g_application_run (G_APPLICATION (app), argc, argv);
96   g_object_unref (app);
97   return stat;
98 }
99 
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
 40   g_free (filename);
 41   gtk_notebook_set_tab_label (nb, scr, label);
 42 }
 43 
 44 void
 45 notebook_page_save (GtkNotebook *nb) {
 46   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
 47 
 48   TfeTextView *tv;
 49 
 50   tv = get_current_textview (nb);
 51   tfe_text_view_save (TFE_TEXT_VIEW (tv));
 52 }
 53 
 54 void
 55 notebook_page_close (GtkNotebook *nb) {
 56   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
 57 
 58   GtkWidget *win;
 59   int i;
 60 
 61   if (gtk_notebook_get_n_pages (nb) == 1) {
 62     win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);
 63     gtk_window_destroy(GTK_WINDOW (win));
 64   } else {
 65     i = gtk_notebook_get_current_page (nb);
 66     gtk_notebook_remove_page (GTK_NOTEBOOK (nb), i);
 67   }
 68 }
 69 
 70 static void
 71 notebook_page_build (GtkNotebook *nb, GtkWidget *tv, char *filename) {
 72   GtkWidget *scr = gtk_scrolled_window_new ();
 73   GtkNotebookPage *nbp;
 74   GtkWidget *lab;
 75   int i;
 76 
 77   gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
 78   gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
 79   lab = gtk_label_new (filename);
 80   i = gtk_notebook_append_page (nb, scr, lab);
 81   nbp = gtk_notebook_get_page (nb, scr);
 82   g_object_set (nbp, "tab-expand", TRUE, NULL);
 83   gtk_notebook_set_current_page (nb, i);
 84   g_signal_connect (GTK_TEXT_VIEW (tv), "change-file", G_CALLBACK (file_changed_cb), nb);
 85 }
 86 
 87 static void
 88 open_response (TfeTextView *tv, int response, GtkNotebook *nb) {
 89   GFile *file;
 90   char *filename;
 91 
 92   if (response != TFE_OPEN_RESPONSE_SUCCESS || ! G_IS_FILE (file = tfe_text_view_get_file (tv))) {
 93     g_object_ref_sink (tv);
 94     g_object_unref (tv);
 95   }else {
 96     filename = g_file_get_basename (file);
 97     g_object_unref (file);
 98     notebook_page_build (nb, GTK_WIDGET (tv), filename);
 99   }
100 }
101 
102 void
103 notebook_page_open (GtkNotebook *nb) {
104   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
105 
106   GtkWidget *tv;
107 
108   if ((tv = tfe_text_view_new ()) == NULL)
109     return;
110   g_signal_connect (TFE_TEXT_VIEW (tv), "open-response", G_CALLBACK (open_response), nb);
111   tfe_text_view_open (TFE_TEXT_VIEW (tv), GTK_WINDOW (gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW)));
112 }
113 
114 void
115 notebook_page_new_with_file (GtkNotebook *nb, GFile *file) {
116   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
117   g_return_if_fail(G_IS_FILE (file));
118 
119   GtkWidget *tv;
120   char *filename;
121 
122   if ((tv = tfe_text_view_new_with_file (file)) == NULL)
123     return; /* read error */
124   filename = g_file_get_basename (file);
125   notebook_page_build (nb, tv, filename);
126 }
127 
128 void
129 notebook_page_new (GtkNotebook *nb) {
130   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
131 
132   GtkWidget *tv;
133   char *filename;
134 
135   if ((tv = tfe_text_view_new ()) == NULL)
136     return;
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
  4 struct _TfeTextView {
  5   GtkTextView parent;
  6   GFile *file;
  7 };
  8 
  9 G_DEFINE_TYPE (TfeTextView, tfe_text_view, GTK_TYPE_TEXT_VIEW);
 10 
 11 enum {
 12   CHANGE_FILE,
 13   OPEN_RESPONSE,
 14   NUMBER_OF_SIGNALS
 15 };
 16 
 17 static guint tfe_text_view_signals[NUMBER_OF_SIGNALS];
 18 
 19 static void
 20 tfe_text_view_dispose (GObject *gobject) {
 21   TfeTextView *tv = TFE_TEXT_VIEW (gobject);
 22 
 23   if (G_IS_FILE (tv->file))
 24     g_clear_object (&tv->file);
 25 
 26   G_OBJECT_CLASS (tfe_text_view_parent_class)->dispose (gobject);
 27 }
 28 
 29 static void
 30 tfe_text_view_init (TfeTextView *tv) {
 31   tv->file = NULL;
 32 }
 33 
 34 static void
 35 tfe_text_view_class_init (TfeTextViewClass *class) {
 36   GObjectClass *object_class = G_OBJECT_CLASS (class);
 37 
 38   object_class->dispose = tfe_text_view_dispose;
 39   tfe_text_view_signals[CHANGE_FILE] = g_signal_new ("change-file",
 40                                  G_TYPE_FROM_CLASS (class),
 41                                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
 42                                  0 /* class offset */,
 43                                  NULL /* accumulator */,
 44                                  NULL /* accumulator data */,
 45                                  NULL /* C marshaller */,
 46                                  G_TYPE_NONE /* return_type */,
 47                                  0     /* n_params */
 48                                  );
 49   tfe_text_view_signals[OPEN_RESPONSE] = g_signal_new ("open-response",
 50                                  G_TYPE_FROM_CLASS (class),
 51                                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
 52                                  0 /* class offset */,
 53                                  NULL /* accumulator */,
 54                                  NULL /* accumulator data */,
 55                                  NULL /* C marshaller */,
 56                                  G_TYPE_NONE /* return_type */,
 57                                  1     /* n_params */,
 58                                  G_TYPE_INT
 59                                  );
 60 }
 61 
 62 GFile *
 63 tfe_text_view_get_file (TfeTextView *tv) {
 64   g_return_val_if_fail (TFE_IS_TEXT_VIEW (tv), NULL);
 65 
 66   if (G_IS_FILE (tv->file))
 67     return g_file_dup (tv->file);
 68   else
 69     return NULL;
 70 }
 71 
 72 static gboolean
 73 save_file (GFile *file, GtkTextBuffer *tb, GtkWindow *win) {
 74   GtkTextIter start_iter;
 75   GtkTextIter end_iter;
 76   gchar *contents;
 77   gboolean stat;
 78   GtkWidget *message_dialog;
 79   GError *err = NULL;
 80 
 81   gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
 82   contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
 83   if (g_file_replace_contents (file, contents, strlen (contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, &err)) {
 84     gtk_text_buffer_set_modified (tb, FALSE);
 85     stat = TRUE;
 86   } else {
 87     message_dialog = gtk_message_dialog_new (win, GTK_DIALOG_MODAL,
 88                                              GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
 89                                             "%s.\n", err->message);
 90     g_signal_connect (message_dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
 91     gtk_widget_show (message_dialog);
 92     g_error_free (err);
 93     stat = FALSE;
 94   }
 95   g_free (contents);
 96   return stat;
 97 }
 98 
 99 static void
100 saveas_dialog_response (GtkWidget *dialog, gint response, TfeTextView *tv) {
101   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
102   GFile *file;
103   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
104 
105   if (response == GTK_RESPONSE_ACCEPT) {
106     file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));
107     if (! G_IS_FILE (file))
108       g_warning ("TfeTextView: gtk_file_chooser_get_file returns non GFile.\n");
109     else if (save_file(file, tb, GTK_WINDOW (win))) {
110       if (G_IS_FILE (tv->file))
111         g_object_unref (tv->file);
112       tv->file = file;
113       g_signal_emit (tv, tfe_text_view_signals[CHANGE_FILE], 0);
114     } else
115       g_object_unref (file);
116   }
117   gtk_window_destroy (GTK_WINDOW (dialog));
118 }
119 
120 void
121 tfe_text_view_save (TfeTextView *tv) {
122   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
123 
124   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
125   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
126 
127   if (! gtk_text_buffer_get_modified (tb))
128     return; /* no need to save it */
129   else if (tv->file == NULL)
130     tfe_text_view_saveas (tv);
131   else if (! G_IS_FILE (tv->file))
132     g_error ("TfeTextView: The pointer tv->file isn't NULL nor GFile.\n");
133   else
134     save_file (tv->file, tb, GTK_WINDOW (win));
135 }
136 
137 void
138 tfe_text_view_saveas (TfeTextView *tv) {
139   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
140 
141   GtkWidget *dialog;
142   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
143 
144   dialog = gtk_file_chooser_dialog_new ("Save file", GTK_WINDOW (win), GTK_FILE_CHOOSER_ACTION_SAVE,
145                                       "Cancel", GTK_RESPONSE_CANCEL,
146                                       "Save", GTK_RESPONSE_ACCEPT,
147                                       NULL);
148   g_signal_connect (dialog, "response", G_CALLBACK (saveas_dialog_response), tv);
149   gtk_widget_show (dialog);
150 }
151 
152 GtkWidget *
153 tfe_text_view_new_with_file (GFile *file) {
154   g_return_val_if_fail (G_IS_FILE (file), NULL);
155 
156   GtkWidget *tv;
157   GtkTextBuffer *tb;
158   char *contents;
159   gsize length;
160 
161   if (! g_file_load_contents (file, NULL, &contents, &length, NULL, NULL)) /* read error */
162     return NULL;
163 
164   if ((tv = tfe_text_view_new()) != NULL) {
165     tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
166     gtk_text_buffer_set_text (tb, contents, length);
167     TFE_TEXT_VIEW (tv)->file = g_file_dup (file);
168     gtk_text_buffer_set_modified (tb, FALSE);
169   }
170   g_free (contents);
171   return tv;
172 }
173 
174 static void
175 open_dialog_response(GtkWidget *dialog, gint response, TfeTextView *tv) {
176   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
177   GFile *file;
178   char *contents;
179   gsize length;
180   GtkWidget *message_dialog;
181   GError *err = NULL;
182 
183   if (response != GTK_RESPONSE_ACCEPT)
184     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_CANCEL);
185   else if (! G_IS_FILE (file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog)))) {
186     g_warning ("TfeTextView: gtk_file_chooser_get_file returns non GFile.\n");
187     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_ERROR);
188   } else if (! g_file_load_contents (file, NULL, &contents, &length, NULL, &err)) { /* read error */
189     g_object_unref (file);
190     message_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog), GTK_DIALOG_MODAL,
191                                              GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
192                                             "%s.\n", err->message);
193     g_signal_connect (message_dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
194     gtk_widget_show (message_dialog);
195     g_error_free (err);
196     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_ERROR);
197   } else {
198     gtk_text_buffer_set_text (tb, contents, length);
199     g_free (contents);
200     if (G_IS_FILE (tv->file))
201       g_object_unref (tv->file);
202     tv->file = file;
203     gtk_text_buffer_set_modified (tb, FALSE);
204     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_SUCCESS);
205     g_signal_emit (tv, tfe_text_view_signals[CHANGE_FILE], 0);
206   }
207   gtk_window_destroy (GTK_WINDOW (dialog));
208 }
209 
210 void
211 tfe_text_view_open (TfeTextView *tv, GtkWindow *win) {
212   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
213   g_return_if_fail (GTK_IS_WINDOW (win));
214 
215   GtkWidget *dialog;
216 
217   dialog = gtk_file_chooser_dialog_new ("Open file", win, GTK_FILE_CHOOSER_ACTION_OPEN,
218                                         "Cancel", GTK_RESPONSE_CANCEL,
219                                         "Open", GTK_RESPONSE_ACCEPT,
220                                         NULL);
221   g_signal_connect (dialog, "response", G_CALLBACK (open_dialog_response), tv);
222   gtk_widget_show (dialog);
223 }
224 
225 GtkWidget *
226 tfe_text_view_new (void) {
227   return GTK_WIDGET (g_object_new (TFE_TYPE_TEXT_VIEW, NULL));
228 }
229 
~~~

## Total number of lines, words and characters

~~~
$ LANG=C wc tfe5/meson.build tfe5/tfeapplication.c tfe5/tfe.gresource.xml tfe5/tfe.h tfe5/tfenotebook.c tfe5/tfenotebook.h tfetextview/tfetextview.c tfetextview/tfetextview.h tfe5/tfe.ui
   10    17   294 tfe5/meson.build
   99   304  3205 tfe5/tfeapplication.c
    6     9   153 tfe5/tfe.gresource.xml
    4     6    87 tfe5/tfe.h
  140   378  3601 tfe5/tfenotebook.c
   15    21   241 tfe5/tfenotebook.h
  229   671  8017 tfetextview/tfetextview.c
   35    60   701 tfetextview/tfetextview.h
   61   100  2073 tfe5/tfe.ui
  599  1566 18372 total
~~~


Up: [Readme.md](../Readme.md),  Prev: [Section 15](sec15.md), Next: [Section 17](sec17.md)
