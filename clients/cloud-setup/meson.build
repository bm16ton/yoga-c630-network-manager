name = 'nm-cloud-setup'

if install_systemdunitdir

  nm_cloud_setup_service = configure_file(
    input: 'nm-cloud-setup.service.in',
    output: '@BASENAME@',
    install_dir: systemd_systemdsystemunitdir,
    configuration: data_conf,
  )

  install_data(
    'nm-cloud-setup.timer',
    install_dir: systemd_systemdsystemunitdir,
  )

  install_data(
    '90-nm-cloud-setup.sh',
    install_dir: join_paths(nm_pkglibdir, 'dispatcher.d', 'no-wait.d'),
  )

endif

sources = files(
  'main.c',
  'nm-cloud-setup-utils.c',
  'nm-http-client.c',
  'nmcs-provider-ec2.c',
  'nmcs-provider.c',
)

deps = [
  libnmc_base_dep,
  libnmc_dep,
  libcurl_dep,
]

executable(
  name,
  sources,
  dependencies: deps,
  c_args: clients_c_flags +
          ['-DG_LOG_DOMAIN="@0@"'.format(name)],
  link_with: libnm_systemd_logging_stub,
  link_args: ldflags_linker_script_binary,
  link_depends: linker_script_binary,
  install: true,
  install_dir: nm_libexecdir,
)
