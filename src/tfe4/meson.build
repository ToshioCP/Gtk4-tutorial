project('tfe', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','tfe.gresource.xml')

sourcefiles=files('tfe.c', 'tfetextview.c')

executable('tfe', sourcefiles, resources, dependencies: gtkdep, install: false)
