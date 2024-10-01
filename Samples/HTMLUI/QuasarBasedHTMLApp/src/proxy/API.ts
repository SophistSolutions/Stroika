import { DateTime } from 'luxon';
import { ref } from "vue";

import { IAbout } from '../models/IAbout';

import { gRuntimeConfiguration } from 'boot/configuration';

import { Logger } from '../utils/Logger';

// On Chrome, CORS frequently fails - not sure why. In chrome debugger, Access-Control-Allow-Origin
// lines not showing up but do in other browsers (edge)
// --LGP 2022-11-24
//const kFetchOptions_: RequestInit = { mode: 'no-cors' };
// I BELIEVE this was fixed by:
//      https://github.com/SophistSolutions/Stroika/commit/cf5a8c8537b0f954729a0911cea51c333ad428de
const kFetchOptions_: RequestInit = {};

function throwIfError_(response: Response): Response {
  if (response.status >= 400 && response.status < 500) {
    throw new Error('Client Request Error from the Server');
  }
  if (response.status >= 500 && response.status < 600) {
    throw new Error(`Server Error ${response.status}`);
  }
  if (!response.ok) {
    throw new Error(
      `Server Error status: ${response.status}, type:${response.type}`
    );
  }
  return response;
}


export async function fetchAboutInfo(apiServer: string): Promise<IAbout> {
  try {
    const response: Response = await fetch(
      `${apiServer}/about`,
      kFetchOptions_
    );
    throwIfError_(response);
    const data = (await response.json()) as IAbout; // could embellish validation here
    return data;
  } catch (e) {
    Logger.error(e);
    throw e;
  }
}

