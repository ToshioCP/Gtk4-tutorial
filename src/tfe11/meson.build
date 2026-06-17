project('tfe', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','tfe.gresource.xml')

# Main application
sourcefiles=files('main.c', 'tfeapplication.c', 'tfewindow.c', 'tfetextview.c', 'tfestylemanager.c', 'tfepref.c')
executable('tfe', sourcefiles, resources, dependencies: gtkdep)
