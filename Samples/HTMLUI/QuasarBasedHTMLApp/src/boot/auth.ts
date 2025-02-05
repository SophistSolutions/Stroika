import { boot } from "quasar/wrappers"

import authPlugin, { OAuthConfig } from '../plugins/auth'
import { gRuntimeConfiguration } from 'boot/configuration';
import { getOAuthConfigurations } from '../proxy/API';

async function configFetcher(): Promise<OAuthConfig|undefined> {
    const oauthConfigs = await getOAuthConfigurations(gRuntimeConfiguration.API_ROOT);
    if (oauthConfigs == null || oauthConfigs.length < 1) {
        console.error("Failed to load OAuth configuration from server, so auth not available");
        return undefined;
    }
    const oauthConfig = oauthConfigs[0];
    console.log("Fetched oauthConfig from Backend:", oauthConfig);
    const clientId = oauthConfig.applicationID;
    const provider = oauthConfig.provider;
    const openIdConnectUrl = 'https://accounts.google.com'; // @todo get this from provider
    const redirectUri = `${window.origin}/oauth/callback`;
    const scope = oauthConfig.scopes.join(" ");
    return {
        provider,
        openIdConnectUrl,
        clientId,
        redirectUri,
        scope
    }
};

export default boot(({ app, router, store }) => {
    app.use(authPlugin, { configFetcher, router });
})
