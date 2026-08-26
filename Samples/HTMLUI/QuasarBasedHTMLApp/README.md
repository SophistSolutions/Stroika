# Stroika Sample HTMLUI (html-quasar)

Stroika sample showing how to combine web services and a simple HTML UI (here using Quasar and Vue3).

## Configure

Somewhere have access to the sample's Stroika backend running (either locally build and debug, or point at some other machine running).

Edit .env file as appropriate (perhaps based on .env.development or .env.production).

Note that only variables prefixed `QCLI_` are exposed to client code - that prefix is
@quasar/app-vite's default (`build.env.clientPrefix`). A variable without it is silently
unavailable in the browser, with no build error. Values computed at build time rather than
read from a file go in `build.defineEnv` in quasar.config.ts instead, and are likewise read
through `import.meta.env`.

## Install the dependencies

```bash
npm install
```

### Start the app in development mode (hot-code reloading, error reporting, etc.)

```bash
quasar dev
```

### Lint the files

```bash
npm run lint
```

### Build the app for production

```bash
quasar build
```

### Customize the configuration

See [Configuring quasar.config.ts](https://quasar.dev/quasar-cli-vite/quasar-config-file).
