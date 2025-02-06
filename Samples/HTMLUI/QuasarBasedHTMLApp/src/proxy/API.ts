import { DateTime } from 'luxon';
import { ref } from "vue";

import { IAbout } from '../models/IAbout';

import { gRuntimeConfiguration } from 'boot/configuration';

import { Logger } from '../utils/Logger';

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

export interface IAppOAuthConfiguration {
  applicationID: string;
  scopes: string[];
  openIdConnectUrl: string;
  redirectUri: string;
  provider: string;
}

export interface IOAuthProviderConfiguration {
  provider: string;
  openid_configuration_uri?: string;
}

export async function getOAuthConfigurations(apiServer: string): Promise<{clients: IAppOAuthConfiguration[], providers: IOAuthProviderConfiguration[]}> {
  try {
    const response: Response = await fetch(`${apiServer}/api/auth/oauth/configurations`, kFetchOptions_);
    throwIfError_(response);
    const data = (await response.json()); // could embellish validation here
    return data;
  } catch (e) {
    Logger.error(e);
    throw e;
  }
}

export async function fetchTokens(apiServer: string, params: object): Promise<object> {
  try {
    const response: Response = await fetch(`${apiServer}/api/auth/oauth/tokens`, {
      ...kFetchOptions_,
      method: "POST",
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(params)
    });
    throwIfError_(response);
    const data = (await response.json()); // could embellish validation here
    return data;
  } catch (e) {
    Logger.error(e);
    throw e;
  }
}

export async function fetchUserInfo(apiServer: string, authToken: string): Promise<object> {
  try {
    const response: Response = await fetch(`${apiServer}/api/auth/oauth/user_info`, {
      ...kFetchOptions_,
      headers: {
        'Authorization': 'Bearer ' + authToken
      }
    });
    throwIfError_(response);
    const data = (await response.json()); // could embellish validation here
    return data;
  } catch (e) {
    Logger.error(e);
    throw e;
  }
}

export async function fetchAboutInfo(apiServer: string): Promise<IAbout> {
  try {
    const response: Response = await fetch(
      `${apiServer}/api/about`,
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

