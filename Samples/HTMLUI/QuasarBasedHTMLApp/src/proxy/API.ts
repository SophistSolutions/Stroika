import { IAbout } from 'src/models/IAbout';
import { gRuntimeConfiguration } from 'boot/configuration';
import { Logger } from 'src/utils/Logger';

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

export async function getOAuthConfigurations(args?:{apiServer?: string}): Promise<{ clients: IAppOAuthConfiguration[], providers: IOAuthProviderConfiguration[] }> {
  const apiServer = args?.apiServer || gRuntimeConfiguration.API_ROOT;
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

export async function fetchTokens(args:{apiServer?: string, params: object}): Promise<object> {
  const apiServer = args.apiServer || gRuntimeConfiguration.API_ROOT;
  try {
    const response: Response = await fetch(`${apiServer}/api/auth/oauth/tokens`, {
      ...kFetchOptions_,
      method: "POST",
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(args.params)
    });
    throwIfError_(response);
    const data = (await response.json()); // could embellish validation here
    return data;
  } catch (e) {
    Logger.error(e);
    throw e;
  }
}

export async function revokeTokens(args: { apiServer?: string, provider: string, refreshToken?: string, accessToken?: string }): Promise<void> {
  const apiServer = args.apiServer || gRuntimeConfiguration.API_ROOT;
  try {
    const response: Response = await fetch(`${apiServer}/api/auth/oauth/tokens/revoke`, {
      ...kFetchOptions_,
      method: "POST",
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ provider: args.provider, refreshToken: args.refreshToken, accessToken: args.accessToken })
    });
    throwIfError_(response);
  } catch (e) {
    Logger.error(e);
    throw e;
  }
}

export async function fetchUserInfo(args:{apiServer?: string, authToken: string}): Promise<object> {
  const apiServer = args.apiServer || gRuntimeConfiguration.API_ROOT;
  try {
    const response: Response = await fetch(`${apiServer}/api/auth/oauth/user_info`, {
      ...kFetchOptions_,
      headers: {
        'Authorization': 'Bearer ' + args.authToken
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

export async function fetchAboutInfo(args?:{apiServer: string}): Promise<IAbout> {
  const apiServer = args?.apiServer || gRuntimeConfiguration.API_ROOT;
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



export default  {fetchAboutInfo, fetchUserInfo, revokeTokens, fetchTokens, getOAuthConfigurations}
