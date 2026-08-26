// https://quasar.dev/quasar-cli-vite/handling-process-env

/*
 *  Configuration object known at compile time.
 */
export const kCompileTimeConfiguration = {
  APP_ROOT_API: import.meta.env.QCLI_APP_ROOT_API as string | null,
  APP_DEFAULT_API_PORT: import.meta.env.QCLI_APP_DEFAULT_API_PORT,
  DEBUG_MODE: import.meta.env.QCLI_APP_DEBUG_MODE == 'true',
  MY_HTML_APP_VERSION: import.meta.env.QCLI_APP_VERSION ?? '',
  VUE_VERSION: import.meta.env.QCLI_VUE_VERSION ?? '',
};

// export const DEBUG_MODE: boolean = import.meta.env.QCLI_APP_DEBUG_MODE=='true';

// export const VUE_MY_HTML_APP_VERSION: string = import.meta.env.QCLI_APP_VERSION ?? "";
// export const VUE_VERSION: string = import.meta.env.QCLI_VUE_VERSION ?? "";

// console.log(`API_ROOT=` + API_ROOT)
// console.log(`DEBUG_MODE=` + DEBUG_MODE)
// console.log(`VUE_MY_HTML_APP_VERSION=` + VUE_MY_HTML_APP_VERSION)
// console.log(`VUE_VERSION=` + VUE_VERSION)
console.log(
  `kCompileTimeConfiguration: ${JSON.stringify(kCompileTimeConfiguration)}`
);
