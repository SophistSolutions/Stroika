import { boot } from "quasar/wrappers"

import authPlugin, { IOAuthProviderConfig } from '@/plugins/auth'
import { getOAuthConfigurations } from '@/proxy/API';

async function configFetcher(): Promise<IOAuthProviderConfig[] | undefined> {
    const oauthConfig = await getOAuthConfigurations();
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

export default boot(({ app, router }) => {
    app.use(authPlugin, { configFetcher, router });
})
