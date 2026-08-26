// https://quasar.dev/quasar-cli-vite/quasar-config-file

/*
 *  Configuration object known at compile time.
 */
export const kCompileTimeConfiguration = {
  APP_ROOT_API: import.meta.env.QCLI_APP_ROOT_API as string | null,
  APP_DEFAULT_API_PORT: import.meta.env.QCLI_APP_DEFAULT_API_PORT,
  MY_HTML_APP_VERSION: import.meta.env.QCLI_APP_VERSION ?? '',
  VUE_VERSION: import.meta.env.QCLI_VUE_VERSION ?? '',
};

console.log(
  `kCompileTimeConfiguration: ${JSON.stringify(kCompileTimeConfiguration)}`
);
