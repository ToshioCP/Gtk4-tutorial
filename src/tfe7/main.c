#include <gtk/gtk.h>
#include "tfeapplication.h"

#define APPLICATION_ID "com.github.ToshioCP.tfe"

int
main (int argc, char **argv) {
  TfeApplication *app;
  int stat;

  app = tfe_application_new (APPLICATION_ID, G_APPLICATION_HANDLES_OPEN);
  stat =g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}

