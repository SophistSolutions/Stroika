import { boot } from 'quasar/wrappers';

import { kCompileTimeConfiguration } from '../config/config';
import { Logger } from '../utils/Logger';

export default boot(async (/* { app, router, ... } */) => {
  let API_ROOT: string | null = null;
  let DEFAULT_API_PORT: number | null = null;
  let successfullyLoadedCFGFile = false;
  let cfgObj: any = null;
  try {
    const r = await fetch('/config.json');
    if (r.ok) {
      const contentType: string | null = r.headers.get('content-type');
      if (contentType && contentType.indexOf('application/json') !== -1) {
        cfgObj = await r.json();
        if (
          cfgObj &&
          cfgObj['API_ROOT'] &&
          typeof cfgObj['API_ROOT'] === 'string'
        ) {
          API_ROOT = cfgObj['API_ROOT'];
        }
        if (
          cfgObj &&
          cfgObj['DEFAULT_API_PORT'] &&
          typeof cfgObj['DEFAULT_API_PORT'] === 'number'
        ) {
          DEFAULT_API_PORT = cfgObj['DEFAULT_API_PORT'];
        }
        successfullyLoadedCFGFile = true;
      }
    }
  } catch (e) {
    Logger.warn(e);
  }
  if (successfullyLoadedCFGFile) {
    console.log(`Successfully loaded /config.json: ${JSON.stringify(cfgObj)}`);
  } else {
    console.log('No /config.json so using kCompileTimeConfiguration');
  }
  if (!API_ROOT) {
    API_ROOT = kCompileTimeConfiguration.APP_ROOT_API;
  }
  if (!API_ROOT) {
    API_ROOT =
      window.location.protocol +
      '//' +
      window.location.hostname +
      ':' +
      (DEFAULT_API_PORT ??
        kCompileTimeConfiguration.APP_DEFAULT_API_PORT ??
        '80');
  }
  gRuntimeConfiguration = {
    API_ROOT: API_ROOT ?? '',
  };
  console.log(
    `gRuntimeConfiguration: ${JSON.stringify(gRuntimeConfiguration)}`
  );
});

/*
 *  Configuration object known at runtime.
 *  The app should access its runtime specified configuration (mainly pointer to API-Server but could be other stuff communicated to app at startup, like
 *  debug flags etc?).
 */
export let gRuntimeConfiguration = {
  API_ROOT: '' as string,
};
