import { boot } from "quasar/wrappers"

import authPlugin, { IOAuthProviderConfig } from 'src/plugins/auth'
import { gRuntimeConfiguration } from 'boot/configuration';
import { getOAuthConfigurations } from 'src/proxy/API';

async function configFetcher(): Promise<IOAuthProviderConfig[] | undefined> {
    const oauthConfig = await getOAuthConfigurations(gRuntimeConfiguration.API_ROOT);
    const clientConfigs = oauthConfig.clients;
    if (clientConfigs == null || clientConfigs.length < 1) {
        console.error("Failed to load OAuth configuration from server, so auth not available");
        return undefined;
    }
    return clientConfigs.map(
        (oc) => {
            return {
                provider: oc.provider,
                clientId: oc.applicationID,
                openIdConnectUrl: oauthConfig.providers.find((p) => p.provider == oc.provider)?.openid_configuration_uri ?? "",
                redirectUri: `${window.origin}/oauth/callback`,
                scope: oc.scopes.join(" ")
            }
        });
};

export default boot(({ app, router, store }) => {
    app.use(authPlugin, { configFetcher, router });
})
