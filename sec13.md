Up: [Readme.md](src/Readme.md),  Prev: [Section 12}](src/sec12.src.md), Next: [Section 14[(src/sec14.src.md)# tfe5 source files

The followings are the source files of tfe5.

## meson.buld

     1 project('tfe', 'c')
     2 
     3 gtkdep = dependency('gtk4')
     4 
     5 gnome=import('gnome')
     6 resources = gnome.compile_resources('resources','tfe.gresource.xml')
     7 
     8 sourcefiles=files('tfeapplication.c', 'tfenotebook.c', 'tfetextview.c')
     9 
    10 executable('tfe', sourcefiles, resources, dependencies: gtkdep)

## tfe.gresource.xml

    1 <?xml version="1.0" encoding="UTF-8"?>
    2 <gresources>
    3   <gresource prefix="/com/github/ToshioCP/tfe">
    4     <file>tfe.ui</file>
    5   </gresource>
    6 </gresources>

## tfe.ui

     1 <interface>
     2   <object class="GtkApplicationWindow" id="win">
     3     <property name="title">file editor</property>
     4     <property name="default-width">600</property>
     5     <property name="default-height">400</property>
     6     <child>
     7       <object class="GtkBox" id="boxv">
     8         <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
     9         <child>
    10           <object class="GtkBox" id="boxh">
    11           <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
    12             <child>
    13               <object class="GtkLabel" id="dmy1">
    14               <property name="width-chars">10</property>
    15               </object>
    16             </child>
    17             <child>
    18               <object class="GtkButton" id="btnn">
    19               <property name="label">_New</property>
    20               <property name="use-underline">TRUE</property>
    21               </object>
    22             </child>
    23             <child>
    24               <object class="GtkButton" id="btno">
    25               <property name="label">_Open</property>
    26               <property name="use-underline">TRUE</property>
    27               </object>
    28             </child>
    29             <child>
    30               <object class="GtkLabel" id="dmy2">
    31               <property name="hexpand">TRUE</property>
    32               </object>
    33             </child>
    34             <child>
    35               <object class="GtkButton" id="btns">
    36               <property name="label">_Save</property>
    37               <property name="use-underline">TRUE</property>
    38               </object>
    39             </child>
    40             <child>
    41               <object class="GtkButton" id="btnc">
    42               <property name="label">_Close</property>
    43               <property name="use-underline">TRUE</property>
    44               </object>
    45             </child>
    46             <child>
    47               <object class="GtkLabel" id="dmy3">
    48               <property name="width-chars">10</property>
    49               </object>
    50             </child>
    51           </object>
    52         </child>
    53         <child>
    54           <object class="GtkNotebook" id="nb">
    55             <property name="scrollable">TRUE</property>
    56             <property name="hexpand">TRUE</property>
    57             <property name="vexpand">TRUE</property>
    58           </object>
    59         </child>
    60       </object>
    61     </child>
    62   </object>
    63 </interface>
    64 

## tfe.h

    1 #include <gtk/gtk.h>
    2 
    3 #include "tfetextview.h"
    4 #include "tfenotebook.h"

## tfeapplication.c

      1 #include "tfe.h"
      2 
      3 static void
      4 open_clicked (GtkWidget *btno, GtkNotebook *nb) {
      5   notebook_page_open (nb);
      6 }
      7 
      8 static void
      9 new_clicked (GtkWidget *btnn, GtkNotebook *nb) {
     10   notebook_page_new (nb);
     11 }
     12 
     13 static void
     14 save_clicked (GtkWidget *btns, GtkNotebook *nb) {
     15   notebook_page_save (nb);
     16 }
     17 
     18 static void
     19 close_clicked (GtkWidget *btnc, GtkNotebook *nb) {
     20   GtkWidget *win;
     21   GtkWidget *boxv;
     22   gint i;
     23 
     24   if (gtk_notebook_get_n_pages (nb) == 1) {
     25     boxv = gtk_widget_get_parent (GTK_WIDGET (nb));
     26     win = gtk_widget_get_parent (boxv);
     27     gtk_window_destroy (GTK_WINDOW (win));
     28   } else {
     29     i = gtk_notebook_get_current_page (nb);
     30     gtk_notebook_remove_page (GTK_NOTEBOOK (nb), i);
     31   }
     32 }
     33 
     34 static void
     35 tfe_activate (GApplication *application) {
     36   GtkApplication *app = GTK_APPLICATION (application);
     37   GtkWidget *win;
     38   GtkWidget *boxv;
     39   GtkNotebook *nb;
     40 
     41   win = GTK_WIDGET (gtk_application_get_active_window (app));
     42   boxv = gtk_window_get_child (GTK_WINDOW (win));
     43   nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));
     44 
     45   notebook_page_new (nb);
     46   gtk_widget_show (GTK_WIDGET (win));
     47 }
     48 
     49 static void
     50 tfe_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
     51   GtkApplication *app = GTK_APPLICATION (application);
     52   GtkWidget *win;
     53   GtkWidget *boxv;
     54   GtkNotebook *nb;
     55   int i;
     56 
     57   win = GTK_WIDGET (gtk_application_get_active_window (app));
     58   boxv = gtk_window_get_child (GTK_WINDOW (win));
     59   nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));
     60 
     61   for (i = 0; i < n_files; i++)
     62     notebook_page_new_with_file (nb, files[i]);
     63   if (gtk_notebook_get_n_pages (nb) == 0)
     64     notebook_page_new (nb);
     65   gtk_widget_show (win);
     66 }
     67 
     68 
     69 static void
     70 tfe_startup (GApplication *application) {
     71   GtkApplication *app = GTK_APPLICATION (application);
     72   GtkApplicationWindow *win;
     73   GtkNotebook *nb;
     74   GtkBuilder *build;
     75   GtkButton *btno;
     76   GtkButton *btnn;
     77   GtkButton *btns;
     78   GtkButton *btnc;
     79 
     80   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/tfe.ui");
     81   win = GTK_APPLICATION_WINDOW (gtk_builder_get_object (build, "win"));
     82   nb = GTK_NOTEBOOK (gtk_builder_get_object (build, "nb"));
     83   gtk_window_set_application (GTK_WINDOW (win), app);
     84   btno = GTK_BUTTON (gtk_builder_get_object (build, "btno"));
     85   btnn = GTK_BUTTON (gtk_builder_get_object (build, "btnn"));
     86   btns = GTK_BUTTON (gtk_builder_get_object (build, "btns"));
     87   btnc = GTK_BUTTON (gtk_builder_get_object (build, "btnc"));
     88   g_signal_connect (btno, "clicked", G_CALLBACK (open_clicked), nb);
     89   g_signal_connect (btnn, "clicked", G_CALLBACK (new_clicked), nb);
     90   g_signal_connect (btns, "clicked", G_CALLBACK (save_clicked), nb);
     91   g_signal_connect (btnc, "clicked", G_CALLBACK (close_clicked), nb);
     92   g_object_unref(build);
     93 
     94 GdkDisplay *display;
     95 
     96   display = gtk_widget_get_display (GTK_WIDGET (win));
     97   GtkCssProvider *provider = gtk_css_provider_new ();
     98   gtk_css_provider_load_from_data (provider, "textview {padding: 10px; font-family: monospace; font-size: 12pt;}", -1);
     99   gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    100 }
    101 
    102 int
    103 main (int argc, char **argv) {
    104   GtkApplication *app;
    105   int stat;
    106 
    107   app = gtk_application_new ("com.github.ToshioCP.tfe", G_APPLICATION_HANDLES_OPEN);
    108 
    109   g_signal_connect (app, "startup", G_CALLBACK (tfe_startup), NULL);
    110   g_signal_connect (app, "activate", G_CALLBACK (tfe_activate), NULL);
    111   g_signal_connect (app, "open", G_CALLBACK (tfe_open), NULL);
    112 
    113   stat =g_application_run (G_APPLICATION (app), argc, argv);
    114   g_object_unref (app);
    115   return stat;
    116 }
    117 

### tfenotebook.h

     1 void
     2 notebook_page_save(GtkNotebook *nb);
     3 
     4 void
     5 notebook_page_open (GtkNotebook *nb);
     6 
     7 void
     8 notebook_page_new_with_file (GtkNotebook *nb, GFile *file);
     9 
    10 void
    11 notebook_page_new (GtkNotebook *nb);
    12 

## tfenotebook.c

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
     13 static void
     14 file_changed (TfeTextView *tv, GtkNotebook *nb) {
     15   GFile *file;
     16   char *filename;
     17   GtkWidget *scr;
     18   GtkWidget *label;
     19 
     20   file = tfe_text_view_get_file (tv);
     21   scr = gtk_widget_get_parent (GTK_WIDGET (tv));
     22   if (G_IS_FILE (file))
     23     filename = g_file_get_basename (file);
     24   else
     25     filename = get_untitled ();
     26   label = gtk_label_new (filename);
     27   gtk_notebook_set_tab_label (nb, scr, label);
     28   g_object_unref (file);
     29   g_free (filename);
     30 }
     31 
     32 /* Save the contents in the current page */
     33 void
     34 notebook_page_save(GtkNotebook *nb) {
     35   gint i;
     36   GtkWidget *scr;
     37   GtkWidget *tv;
     38 
     39   i = gtk_notebook_get_current_page (nb);
     40   scr = gtk_notebook_get_nth_page (nb, i);
     41   tv = gtk_scrolled_window_get_child (GTK_SCROLLED_WINDOW (scr));
     42   tfe_text_view_save (TFE_TEXT_VIEW (tv));
     43 }
     44 
     45 static void
     46 notebook_page_build (GtkNotebook *nb, GtkWidget *tv, char *filename) {
     47   GtkWidget *scr;
     48   GtkNotebookPage *nbp;
     49   GtkWidget *lab;
     50   gint i;
     51   scr = gtk_scrolled_window_new ();
     52 
     53   gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
     54   lab = gtk_label_new (filename);
     55   i = gtk_notebook_append_page (nb, scr, lab);
     56   nbp = gtk_notebook_get_page (nb, scr);
     57   g_object_set (nbp, "tab-expand", TRUE, NULL);
     58   gtk_notebook_set_current_page (nb, i);
     59   g_signal_connect (GTK_TEXT_VIEW (tv), "change-file", G_CALLBACK (file_changed), nb);
     60 }
     61 
     62 static void
     63 open_response (TfeTextView *tv, gint response, GtkNotebook *nb) {
     64   GFile *file;
     65   char *filename;
     66 
     67   if (response != TFE_OPEN_RESPONSE_SUCCESS)
     68     g_object_unref (tv);
     69   else if (! G_IS_FILE (file = tfe_text_view_get_file (tv)))
     70     g_object_unref (tv);
     71   else {
     72     filename = g_file_get_basename (file);
     73     g_object_unref (file);
     74     notebook_page_build (nb, GTK_WIDGET (tv), filename);
     75   }
     76 }
     77 
     78 void
     79 notebook_page_open (GtkNotebook *nb) {
     80   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
     81 
     82   GtkWidget *tv;
     83 
     84   tv = tfe_text_view_new ();
     85   g_signal_connect (TFE_TEXT_VIEW (tv), "open-response", G_CALLBACK (open_response), nb);
     86   tfe_text_view_open (TFE_TEXT_VIEW (tv));
     87 }
     88 
     89 void
     90 notebook_page_new_with_file (GtkNotebook *nb, GFile *file) {
     91   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
     92   g_return_if_fail(G_IS_FILE (file));
     93 
     94   GtkWidget *tv;
     95   char *filename;
     96 
     97   if ((tv = tfe_text_view_new_with_file (file)) == NULL)
     98     return; /* read error */
     99   filename = g_file_get_basename (file);
    100   notebook_page_build (nb, tv, filename);
    101 }
    102 
    103 void
    104 notebook_page_new (GtkNotebook *nb) {
    105   g_return_if_fail(GTK_IS_NOTEBOOK (nb));
    106 
    107   GtkWidget *tv;
    108   char *filename;
    109 
    110   tv = tfe_text_view_new ();
    111   filename = get_untitled ();
    112   notebook_page_build (nb, tv, filename);
    113 }
    114 

## tfetextview.h

     1 #define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type ()
     2 G_DECLARE_FINAL_TYPE (TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView)
     3 
     4 /* "open-response" signal response */
     5 enum
     6 {
     7   TFE_OPEN_RESPONSE_SUCCESS,
     8   TFE_OPEN_RESPONSE_CANCEL,
     9   TFE_OPEN_RESPONSE_ERROR
    10 };
    11 
    12 GFile *
    13 tfe_text_view_get_file (TfeTextView *tv);
    14 
    15 void
    16 tfe_text_view_open (TfeTextView *tv);
    17 
    18 void
    19 tfe_text_view_save (TfeTextView *tv);
    20 
    21 void
    22 tfe_text_view_saveas (TfeTextView *tv);
    23 
    24 GtkWidget *
    25 tfe_text_view_new_with_file (GFile *file);
    26 
    27 GtkWidget *
    28 tfe_text_view_new (void);
    29 

## tfetextview.c

      1 #include "tfe.h"
      2 
      3 struct _TfeTextView
      4 {
      5   GtkTextView parent;
      6   GtkTextBuffer *tb;
      7   GFile *file;
      8   gboolean changed;
      9 };
     10 
     11 G_DEFINE_TYPE (TfeTextView, tfe_text_view, GTK_TYPE_TEXT_VIEW);
     12 
     13 enum {
     14   CHANGE_FILE,
     15   OPEN_RESPONSE,
     16   NUMBER_OF_SIGNALS
     17 };
     18 
     19 static guint tfe_text_view_signals[NUMBER_OF_SIGNALS];
     20 
     21 /* Signal handler */
     22 static void
     23 on_changed (GtkTextBuffer *tb, TfeTextView *tv) {
     24   tv->changed=TRUE;
     25 }
     26 
     27 static void
     28 tfe_text_view_dispose (GObject *gobject) {
     29   TfeTextView *tv = TFE_TEXT_VIEW (gobject);
     30 
     31   if (G_IS_FILE (tv->file))
     32     g_clear_object (&tv->file);
     33 
     34   G_OBJECT_CLASS (tfe_text_view_parent_class)->dispose (gobject);
     35 }
     36 
     37 static void
     38 tfe_text_view_init (TfeTextView *tv) {
     39   tv->tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
     40   tv->file = NULL;
     41   tv->changed = FALSE;
     42   gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
     43   g_signal_connect (tv->tb, "changed", G_CALLBACK (on_changed), tv);
     44 }
     45 
     46 static void
     47 tfe_text_view_class_init (TfeTextViewClass *class) {
     48   GObjectClass *object_class = G_OBJECT_CLASS (class);
     49 
     50   object_class->dispose = tfe_text_view_dispose;
     51   tfe_text_view_signals[CHANGE_FILE] = g_signal_newv ("change-file",
     52                                  G_TYPE_FROM_CLASS (class),
     53                                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
     54                                  NULL /* closure */,
     55                                  NULL /* accumulator */,
     56                                  NULL /* accumulator data */,
     57                                  NULL /* C marshaller */,
     58                                  G_TYPE_NONE /* return_type */,
     59                                  0     /* n_params */,
     60                                  NULL  /* param_types */);
     61   GType param_types[] = {G_TYPE_INT}; 
     62   tfe_text_view_signals[OPEN_RESPONSE] = g_signal_newv ("open-response",
     63                                  G_TYPE_FROM_CLASS (class),
     64                                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
     65                                  NULL /* closure */,
     66                                  NULL /* accumulator */,
     67                                  NULL /* accumulator data */,
     68                                  NULL /* C marshaller */,
     69                                  G_TYPE_NONE /* return_type */,
     70                                  1     /* n_params */,
     71                                  param_types);
     72 }
     73 
     74 GFile *
     75 tfe_text_view_get_file (TfeTextView *tv) {
     76   g_return_val_if_fail (TFE_IS_TEXT_VIEW (tv), NULL);
     77 
     78   return g_file_dup (tv->file);
     79 }
     80 
     81 static void
     82 open_dialog_response(GtkWidget *dialog, gint response, TfeTextView *tv) {
     83   GFile *file;
     84   char *contents;
     85   gsize length;
     86   GtkWidget *message_dialog;
     87   GError *err = NULL;
     88 
     89   if (response != GTK_RESPONSE_ACCEPT)
     90     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_CANCEL);
     91   else if (! G_IS_FILE (file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog))))
     92     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_ERROR);
     93   else if (! g_file_load_contents (file, NULL, &contents, &length, NULL, &err)) { /* read error */
     94     if (G_IS_FILE (file))
     95       g_object_unref (file);
     96     message_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog), GTK_DIALOG_MODAL,
     97                                              GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
     98                                             "%s.\n", err->message);
     99     g_signal_connect (message_dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
    100     gtk_widget_show (message_dialog);
    101     g_error_free (err);
    102     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_ERROR);
    103   } else {
    104     gtk_text_buffer_set_text (tv->tb, contents, length);
    105     g_free (contents);
    106     tv->file = file;
    107 /*  tv->changed = FALSE;*/
    108     g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_SUCCESS);
    109   }
    110   gtk_window_destroy (GTK_WINDOW (dialog));
    111 }
    112 
    113 void
    114 tfe_text_view_open (TfeTextView *tv) {
    115   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
    116 
    117   GtkWidget *dialog;
    118 
    119   dialog = gtk_file_chooser_dialog_new ("Open file", NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
    120                                         "Cancel", GTK_RESPONSE_CANCEL,
    121                                         "Open", GTK_RESPONSE_ACCEPT,
    122                                         NULL);
    123   g_signal_connect (dialog, "response", G_CALLBACK (open_dialog_response), tv);
    124   gtk_widget_show (dialog);
    125 }
    126 
    127 static void
    128 saveas_dialog_response (GtkWidget *dialog, gint response, TfeTextView *tv) {
    129   GFile *file;
    130 
    131   if (response == GTK_RESPONSE_ACCEPT) {
    132     file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));
    133     if (G_IS_FILE(file)) {
    134       tv->file = file;
    135       tv->changed = TRUE;
    136       g_signal_emit (tv, tfe_text_view_signals[CHANGE_FILE], 0);
    137       tfe_text_view_save (TFE_TEXT_VIEW (tv));
    138     }
    139   }
    140   gtk_window_destroy (GTK_WINDOW (dialog));
    141 }
    142 
    143 void
    144 tfe_text_view_save (TfeTextView *tv) {
    145   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
    146 
    147   GtkTextIter start_iter;
    148   GtkTextIter end_iter;
    149   gchar *contents;
    150   GtkWidget *message_dialog;
    151   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
    152   GError *err = NULL;
    153 
    154   if (! tv->changed)
    155     return; /* no necessary to save it */
    156   else if (tv->file == NULL)
    157     tfe_text_view_saveas (tv);
    158   else {
    159     gtk_text_buffer_get_bounds (tv->tb, &start_iter, &end_iter);
    160     contents = gtk_text_buffer_get_text (tv->tb, &start_iter, &end_iter, FALSE);
    161     if (g_file_replace_contents (tv->file, contents, strlen (contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, &err))
    162       tv->changed = FALSE;
    163     else {
    164 /* It is possible that tv->file is broken. */
    165 /* It is a good idea to set tv->file to NULL. */
    166       if (G_IS_FILE (tv->file))
    167         g_object_unref (tv->file);
    168       tv->file =NULL;
    169       g_signal_emit (tv, tfe_text_view_signals[CHANGE_FILE], 0);
    170       tv->changed = TRUE;
    171       message_dialog = gtk_message_dialog_new (GTK_WINDOW (win), GTK_DIALOG_MODAL,
    172                                                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
    173                                               "%s.\n", err->message);
    174       g_signal_connect (message_dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
    175       gtk_widget_show (message_dialog);
    176       g_error_free (err);
    177     }
    178   }
    179 }
    180 
    181 void
    182 tfe_text_view_saveas (TfeTextView *tv) {
    183   g_return_if_fail (TFE_IS_TEXT_VIEW (tv));
    184 
    185   GtkWidget *dialog;
    186   GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
    187 
    188   dialog = gtk_file_chooser_dialog_new ("Save file", GTK_WINDOW (win), GTK_FILE_CHOOSER_ACTION_SAVE,
    189                                       "_Cancel", GTK_RESPONSE_CANCEL,
    190                                       "_Save", GTK_RESPONSE_ACCEPT,
    191                                       NULL);
    192   g_signal_connect (dialog, "response", G_CALLBACK (saveas_dialog_response), tv);
    193   gtk_widget_show (dialog);
    194 }
    195 
    196 GtkWidget *
    197 tfe_text_view_new_with_file (GFile *file) {
    198   g_return_val_if_fail (G_IS_FILE (file), NULL);
    199 
    200   GtkWidget *tv;
    201   char *contents;
    202   gsize length;
    203 
    204   if (! g_file_load_contents (file, NULL, &contents, &length, NULL, NULL)) /* read error */
    205     return NULL;
    206 
    207   tv = tfe_text_view_new();
    208   gtk_text_buffer_set_text (TFE_TEXT_VIEW (tv)->tb, contents, length);
    209   g_free (contents);
    210   TFE_TEXT_VIEW (tv)->file = g_file_dup (file);
    211   return tv;
    212 }
    213 
    214 GtkWidget *
    215 tfe_text_view_new (void) {
    216   return gtk_widget_new (TFE_TYPE_TEXT_VIEW, NULL);
    217 }
    218 

## Total number of lines, words and charcters

    $ wc tfe5/meson.build tfe5/tfeapplication.c tfe5/tfe.gresource.xml tfe5/tfe.h tfe5/tfenotebook.c tfe5/tfenotebook.h tfe5/tfetextview.c tfe5/tfetextview.h tfe5/tfe.ui
       10    17   279 tfe5/meson.build
      117   348  3576 tfe5/tfeapplication.c
        6     9   153 tfe5/tfe.gresource.xml
        4     6    72 tfe5/tfe.h
      114   311  2870 tfe5/tfenotebook.c
       12    17   196 tfe5/tfenotebook.h
      218   622  7454 tfe5/tfetextview.c
       29    47   545 tfe5/tfetextview.h
       64   105  2266 tfe5/tfe.ui
      574  1482 17411 合計
Up: [Readme.md](src/Readme.md),  Prev: [Section 12}](src/sec12.src.md), Next: [Section 14[(src/sec14.src.md)