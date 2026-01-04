import { IAbout } from 'src/models/IAbout';
import { gRuntimeConfiguration } from 'boot/configuration';
import { Logger } from 'src/utils/Logger';
import { IAuthService } from "src/plugins/auth";

const kFetchOptions_: RequestInit = {};


/*
 *  Some patterns in API access functions.
 *
 *    - take arguments as single args object, so you can use optional, named parameters. This is a convenient
 *      way to make many things optional but use named params to pick out the ones you are providing/overriding
 * 
 *    - optional apiServer argument - but defaults to configured one
 *    - optional auth (plugin api) object - and if provided, auth-headers injected (if already authorized - doesn't force auth)
 *    - export individual functions and single 'default' - so either style of import usage works out depending on client needs
 * 
 *    - _ in private (unuexporeted) names
 *
 */


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

function buildDefaultFetchOptions_(args: { apiServer?: string, auth?: IAuthService }): RequestInit {
  const options = { ...kFetchOptions_, }
  if (args?.auth?.authorizationHeader?.value) {
    if (!options.headers) {
      options.headers={};
    }
    options.headers['Authorization'] = args.auth.authorizationHeader.value;
  }
  return options;
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

export async function getOAuthConfigurations(args?: { apiServer?: string }): Promise<{ clients: IAppOAuthConfiguration[], providers: IOAuthProviderConfiguration[] }> {
  const apiServer = args?.apiServer || gRuntimeConfiguration.API_ROOT;
  const fetchOptions = buildDefaultFetchOptions_(args);
  try {
    const response: Response = await fetch(`${apiServer}/api/auth/oauth/configurations`, fetchOptions);
    throwIfError_(response);
    const data = (await response.json()); // could embellish validation here
    return data;
  } catch (e) {
    Logger.error(e);
    throw e;
  }
}

export async function fetchTokens(args: { apiServer?: string, params: object }): Promise<object> {
  const apiServer = args.apiServer || gRuntimeConfiguration.API_ROOT;
  const fetchOptions = buildDefaultFetchOptions_(args);
  try {
    const response: Response = await fetch(`${apiServer}/api/auth/oauth/tokens`, {
      ...fetchOptions,
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
  const fetchOptions = buildDefaultFetchOptions_(args);
  try {
    const response: Response = await fetch(`${apiServer}/api/auth/oauth/tokens/revoke`, {
      ...fetchOptions,
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

export async function fetchUserInfo(args: { apiServer?: string, auth: IAuthService }): Promise<object> {
  const apiServer = args.apiServer || gRuntimeConfiguration.API_ROOT;
  const fetchOptions = buildDefaultFetchOptions_(args);
  try {
    const response: Response = await fetch(`${apiServer}/api/auth/oauth/user_info`, {
      ...fetchOptions,
    });
    throwIfError_(response);
    const data = (await response.json()); // could embellish validation here
    return data;
  } catch (e) {
    Logger.error(e);
    throw e;
  }
}

export async function fetchAboutInfo(args?: { apiServer: string, auth?: IAuthService }): Promise<IAbout> {
  const apiServer = args?.apiServer || gRuntimeConfiguration.API_ROOT;
  const fetchOptions = buildDefaultFetchOptions_(args);
  try {
    const response: Response = await fetch(
      `${apiServer}/api/about`,
      fetchOptions
    );
    throwIfError_(response);
    const data = (await response.json()) as IAbout; // could embellish validation here
    return data;
  } catch (e) {
    Logger.error(e);
    throw e;
  }
}


export default { fetchAboutInfo, fetchUserInfo, revokeTokens, fetchTokens, getOAuthConfigurations }
