struct _GtkWidgetClass {
  GInitiallyUnownedClass parent_class;
  /*< public >*/
  guint activate_signal;
  /* basics */
  void (* show)                (GtkWidget        *widget);
  void (* hide)                (GtkWidget        *widget);
  void (* map)                 (GtkWidget        *widget);
  void (* unmap)               (GtkWidget        *widget);
  void (* realize)             (GtkWidget        *widget);
  void (* unrealize)           (GtkWidget        *widget);
  void (* root)                (GtkWidget        *widget);
  void (* unroot)              (GtkWidget        *widget);
  void (* size_allocate)       (GtkWidget           *widget,
                                int                  width,
                                int                  height,
                                int                  baseline);
  void (* state_flags_changed) (GtkWidget        *widget,
                                GtkStateFlags     previous_state_flags);
  void (* direction_changed)   (GtkWidget        *widget,
                                GtkTextDirection  previous_direction);
  void (* grab_notify)         (GtkWidget        *widget,
                                gboolean          was_grabbed);
  /* size requests */
  GtkSizeRequestMode (* get_request_mode)               (GtkWidget      *widget);
  void              (* measure) (GtkWidget      *widget,
                                 GtkOrientation  orientation,
                                 int             for_size,
                                 int            *minimum,
                                 int            *natural,
                                 int            *minimum_baseline,
                                 int            *natural_baseline);
  /* Mnemonics */
  gboolean (* mnemonic_activate)        (GtkWidget           *widget,
                                         gboolean             group_cycling);
  /* explicit focus */
  gboolean (* grab_focus)               (GtkWidget           *widget);
  gboolean (* focus)                    (GtkWidget           *widget,
                                         GtkDirectionType     direction);
  void     (* set_focus_child)          (GtkWidget           *widget,
                                         GtkWidget           *child);
  /* keyboard navigation */
  void     (* move_focus)               (GtkWidget           *widget,
                                         GtkDirectionType     direction);
  gboolean (* keynav_failed)            (GtkWidget           *widget,
                                         GtkDirectionType     direction);
  /* accessibility support
   */
  AtkObject *  (* get_accessible)     (GtkWidget       *widget);
  gboolean     (* query_tooltip)      (GtkWidget  *widget,
                                       gint        x,
                                       gint        y,
                                       gboolean    keyboard_tooltip,
                                       GtkTooltip *tooltip);
  void         (* compute_expand)     (GtkWidget  *widget,
                                       gboolean   *hexpand_p,
                                       gboolean   *vexpand_p);
  void         (* css_changed)                 (GtkWidget            *widget,
                                                GtkCssStyleChange    *change);
  void         (* system_setting_changed)      (GtkWidget            *widget,
                                                GtkSystemSetting      settings);
  void         (* snapshot)                    (GtkWidget            *widget,
                                                GtkSnapshot          *snapshot);
  gboolean     (* contains)                    (GtkWidget *widget,
                                                gdouble    x,
                                                gdouble    y);
  /*< private >*/
  GtkWidgetClassPrivate *priv;
  gpointer padding[8];
};

struct _GtkTextViewClass {
  GtkWidgetClass parent_class;
  /*< public >*/
  void (* move_cursor)           (GtkTextView      *text_view,
                                  GtkMovementStep   step,
                                  gint              count,
                                  gboolean          extend_selection);
  void (* set_anchor)            (GtkTextView      *text_view);
  void (* insert_at_cursor)      (GtkTextView      *text_view,
                                  const gchar      *str);
  void (* delete_from_cursor)    (GtkTextView      *text_view,
                                  GtkDeleteType     type,
                                  gint              count);
  void (* backspace)             (GtkTextView      *text_view);
  void (* cut_clipboard)         (GtkTextView      *text_view);
  void (* copy_clipboard)        (GtkTextView      *text_view);
  void (* paste_clipboard)       (GtkTextView      *text_view);
  void (* toggle_overwrite)      (GtkTextView      *text_view);
  GtkTextBuffer * (* create_buffer) (GtkTextView   *text_view);
  void (* snapshot_layer)        (GtkTextView      *text_view,
			          GtkTextViewLayer  layer,
			          GtkSnapshot      *snapshot);
  gboolean (* extend_selection)  (GtkTextView            *text_view,
                                  GtkTextExtendSelection  granularity,
                                  const GtkTextIter      *location,
                                  GtkTextIter            *start,
                                  GtkTextIter            *end);
  void (* insert_emoji)          (GtkTextView      *text_view);
  /*< private >*/
  gpointer padding[8];
};

/* The following definition is generated by the macro G_DECLARE_FINAL_TYPE */
typedef struct {
  GtkTextView parent_class;
} TfeTextViewClass;

