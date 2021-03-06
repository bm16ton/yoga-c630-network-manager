common_inc = include_directories('.')

nm_polkit_listener = files('nm-polkit-listener.c')

common_deps = [
  libnm_dep,
  libnm_nm_default_dep,
]

common_c_flags = clients_c_flags + ['-DG_LOG_DOMAIN="libnmc"']

sources = files(
  'nm-client-utils.c',
  'nm-secret-agent-simple.c',
  'nm-vpn-helpers.c',
)

libnmc_base = static_library(
  'nmc-base',
  sources: sources,
  dependencies: common_deps,
  c_args: common_c_flags,
)

libnmc_base_dep = declare_dependency(
  include_directories: common_inc,
  dependencies: common_deps,
  link_with: libnmc_base,
)

settings_docs = 'settings-docs.h'

if enable_introspection
  settings_docs_source = custom_target(
    settings_docs,
    input: nm_property_docs,
    output: settings_docs,
    command: [xsltproc, '--output', '@OUTPUT@', join_paths(meson.current_source_dir(), 'settings-docs.xsl'), '@INPUT@'],
  )

  test(
    'check-settings-docs',
    find_program(join_paths(source_root, 'tools', 'check-settings-docs.sh')),
    args: [source_root, build_root, 'clients/common/' + settings_docs],
  )
else
  settings_docs_source = configure_file(
    input: settings_docs + '.in',
    output: '@BASENAME@',
    configuration: configuration_data(),
  )
endif

sources = nm_meta_setting_source + [settings_docs_source] + files(
  'nm-meta-setting-access.c',
  'nm-meta-setting-desc.c',
)

libnmc = static_library(
  'nmc',
  sources: sources,
  dependencies: common_deps + [libnm_libnm_core_aux_dep],
  c_args: c_flags,
  link_depends: settings_docs_source,
)

libnmc_dep = declare_dependency(
  include_directories: common_inc,
  dependencies: common_deps,
  link_with: libnmc,
)

if (enable_introspection or enable_nmtui) and enable_tests
  subdir('tests')
endif
