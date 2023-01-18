project('exp', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','exp.gresource.xml')

sourcefiles=files('exp.c')

executable('exp', sourcefiles, resources, dependencies: gtkdep, export_dynamic: true, install: false)
executable('exp_constant', 'exp_constant.c', dependencies: gtkdep, export_dynamic: true, install: false)
executable('exp_constant_simple', 'exp_constant_simple.c', dependencies: gtkdep, export_dynamic: true, install: false)
executable('exp_property_simple', 'exp_property_simple.c', dependencies: gtkdep, export_dynamic: true, install: false)
executable('closure', 'closure.c', dependencies: gtkdep, export_dynamic: true, install: false)
executable('closure_each', 'closure_each.c', dependencies: gtkdep, export_dynamic: true, install: false)
executable('exp_closure_simple', 'exp_closure_simple.c', dependencies: gtkdep, export_dynamic: true, install: false)
executable('exp_closure_with_error_report', 'exp_closure_with_error_report.c', dependencies: gtkdep, export_dynamic: true, install: false)
executable('exp_bind', 'exp_bind.c', dependencies: gtkdep, export_dynamic: true, install: false)
executable('exp_watch', 'exp_watch.c', dependencies: gtkdep, export_dynamic: true, install: false)
executable('exp_test', 'exp_test.c', dependencies: gtkdep, export_dynamic: true, install: false)
