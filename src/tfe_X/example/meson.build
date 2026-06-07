project('example_tfe', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','../tfe.gresource.xml', source_dir: '..')
# gnome.compile_schemas(build_by_default: true, depend_files: 'com.github.ToshioCP.tfe.gschema.xml')

executable('ex_alert', ['ex_alert.c', '../tfealert.c'], resources, dependencies: gtkdep, export_dynamic: true, install: false)
