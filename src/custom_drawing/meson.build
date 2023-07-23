project('rect', 'c')

gtkdep = dependency('gtk4')

gnome = import('gnome')
resources = gnome.compile_resources('resources','rect.gresource.xml')

executable(meson.project_name(), 'rect.c', resources, dependencies: gtkdep, export_dynamic: true, install: false)
