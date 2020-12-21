all: tfe

tfe: tfe.o tfetextview.o resources.o
	gcc -o tfe tfe.o tfetextview.o resources.o `pkg-config --libs gtk4`

tfe.o: tfe.c tfetextview.h
	gcc -c -o tfe.o `pkg-config --cflags gtk4` tfe.c
tfetextview.o: tfetextview.c tfetextview.h
	gcc -c -o tfetextview.o `pkg-config --cflags gtk4` tfetextview.c
resources.o: resources.c
	gcc -c -o resources.o `pkg-config --cflags gtk4` resources.c

resources.c: tfe.gresource.xml tfe.ui
	glib-compile-resources tfe.gresource.xml --target=resources.c --generate-source

.Phony: clean

clean:
	rm -f tfe tfe.o tfetextview.o resources.o resources.c
