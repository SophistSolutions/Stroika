import { App, ref, Ref, computed } from 'vue';
import { Router } from "vue-router";
import {
    AuthorizationRequest,
    AuthorizationNotifier, AuthorizationRequestHandler,
    AuthorizationServiceConfiguration,
    AuthorizationResponse,
    RedirectRequestHandler,
    FetchRequestor, LocalStorageBackend,
    BaseTokenRequestHandler, RevokeTokenRequest
} from '@openid/appauth';
import { DateTime, Duration } from "luxon";

import { fetchTokens, fetchUserInfo, revokeTokens } from '@/proxy/API';

export interface IOAuthProviderConfig {
    provider: string;
    openIdConnectUrl: string;
    clientId: string;
    redirectUri: string;
    scope: string;
}

export interface AuthOptions {
    /**
     * The auth plugin requires information about the auth provider to use - application id, etc. (maybe extend to support a list in the future).
     * 
     * But this is done from the vue 'boot' process (for now - is that best?) - and it doesn't appear it can be constructed asyncrhonously - quite unsure of that.
     * Anyhow - its to accomodate that belief - that we use an async here - so they auth plugin can asyncrhonously complete its initialization.
     */
    configFetcher(): Promise<IOAuthProviderConfig[] | undefined>;

    /**
     *  The router is needed to help support the 'redirect' OAuth requires (so when you call login, it reserves the current route and after
     *  the oauth redirect, routes back to that original saved route).
     */
    router: Router;
}

export interface ITokenInfo {
    accessToken: string;
    idToken?: string;
    refreshToken?: string;
    expiresAt: DateTime;
    scopes?: string[];
}

/**
 *  Information returned from the auth process, about the currently logged in user.
 */
export interface IUserInfo {
    personImageURL?: string;
    personName?: string;
    email?: string;
    // note could add stuff like userRole, etc here...(just change webservice API to report that information)
}

const kStoreExtraStuffInSessionForSpeed_ = false;

// If things are failing, consider turning this on to see better why...
const kDebugLogging_ = false;

const kSupportAuthJSBasedRevocation_ = false;

// Generating refresh tokens works better with this, but I've noticed gmail becomes goofy after you
// do this, so disable it for now
// MAYBE just revoke the auth-token, not the refresh token?
//      --LGP 2025-02-19
const kSupportRefreshTokenRevocation_ = false;


/**
 *  API published by the plugin, on the  app.globalProperties.$auth field and the app.provide name (not sure why both
 *  or which we use when)?
 */
export interface IAuthService {

    /**
     *  readonly ref - an element of configs - which selected during login() (so if not logged in or logging in, will be undefined)
     *  NOTE - if there is a user, this must be valid (not undefined).
     *  HOWEVER, it can be valid, while user is undefined (e.g. during the login process, and if the login process fails).
     */
    activeProvider: Ref<IOAuthProviderConfig | undefined>;

    /**
     *  readonly ref - all available configurations
     */
    availableProviders: Ref<IOAuthProviderConfig[] | undefined>;

    /**
     * if undefined, no auto-refresh (disabled). But otherwise, auto-refresh the token - either with refresh_token or re-login whenever it would
     * have expired (this much before expiry).
     */
    autoRefreshAuth: Ref<Duration | undefined>;

    /**
     * The auth plugin requires information about the auth provider to use - application id, etc. (maybe extend to support a list in the future).
     */
    authorizationHeader: Ref<string | undefined>;

    /**
     *  Returns null if not logged in fully. But if successfully logged in, returns access_token, optional id_token, optional referesh_token, and the, expiresAt value.
     */
    authorizationTokens: Ref<ITokenInfo | undefined>;

    /**
     *   call this method in the oauth redirect to handle the page arguments
     */
    handleRedirectCallback(): Promise<void>;

    /**
     *  Initiate OAuth redirect to provider to 'login';
     *  optional parameters include alternate redirect url, and a 'provider' to use - probably from among availableProviders'
     *  if userProvider not specified, the first availableProviders is chosen (and if no such, an error is raised)
     */
    login(args?: { useProvider?: IOAuthProviderConfig, redirectTo?: string }): Promise<void>;

    /**
     *  Throw away all current 'login' information (so user.value returns null)
     */
    logout(): Promise<void>;

    /**
     *  Returns null if not (yet) fully logged in
     */
    user: Ref<IUserInfo | undefined>;
};

const kAuthPluginLocalstorageNamePrefix_ = "stroika-sample-htmlui-vue-"


/**
 *  Class implementing IAuthService, which is installed in the quasar boot handler, to hook the plugin into each vue instance (not exported, just used in
 *  exported 'install' method)
 */
class AuthService {
    private fPreserveVarsInPlugin_ = new LocalStorageBackend();
    private fRouter_: Router;
    private fBackToAfterRedirectRoute_?: string;

    private fNotifier_: AuthorizationNotifier;
    private fAuthorizationHandler_: AuthorizationRequestHandler;

    // state
    private fActiveOAuthServiceConfiguration_?: AuthorizationServiceConfiguration;  // NOT preserved in localstorage - recomputed from activeProvider
    private fLastAuthorizationRequest_: AuthorizationRequest | undefined;
    private fLastAuthorizationResponse_: AuthorizationResponse | undefined; // so far unused, but I think used in some scenarios (like token request instead of code request)
    private fLastAuthorizationCode_: string | undefined;
    private fTokensInfo_: Ref<ITokenInfo | undefined> = ref(undefined);

    private fOptions_: AuthOptions;
    private fAvailableProviderConfigs_: Ref<IOAuthProviderConfig[] | undefined> = ref(undefined);
    private fActiveProviderConfig_: Ref<IOAuthProviderConfig | undefined> = ref(undefined);

    private fAutoRefreshTokenThisMuchBeforeExpiry_: Ref<Duration | undefined> = ref(Duration.fromDurationLike(5 * 60 * 1000));
    private fAutoRefreshTimeoutCallback_?: NodeJS.Timeout;

    private fUser_: Ref<IUserInfo | undefined> = ref(undefined);


    private async preserve_<T>(name: string, value: T) {
        if (typeof value === "string") {
            await this.fPreserveVarsInPlugin_.setItem(kAuthPluginLocalstorageNamePrefix_ + name, value);
        }
        else {
            await this.preserve_(name, JSON.stringify(value));
        }
    }
    private async retrievePreserved_(name: string): Promise<string | undefined> {
        const o = await this.fPreserveVarsInPlugin_.getItem(kAuthPluginLocalstorageNamePrefix_ + name);
        if (o) {
            return o;
        }
        return undefined;
    }
    private async retrievePreservedT_<T>(name: string): Promise<T | undefined> {
        const o = await this.retrievePreserved_(name);
        if (o) {
            return JSON.parse(o);
        }
        return undefined;
    }
    private async clearPreserved_(): Promise<void> {
        //await this.fPreserveVarsInPlugin_.clear(); removes too much
        await this.fPreserveVarsInPlugin_.removeItem(kAuthPluginLocalstorageNamePrefix_ + 'backToAfterRedirectRoute');
        await this.fPreserveVarsInPlugin_.removeItem(kAuthPluginLocalstorageNamePrefix_ + 'lastAuthorizationCode');
        await this.fPreserveVarsInPlugin_.removeItem(kAuthPluginLocalstorageNamePrefix_ + 'lastAuthorizationRequest');
        if (kStoreExtraStuffInSessionForSpeed_) {
            await this.fPreserveVarsInPlugin_.removeItem(kAuthPluginLocalstorageNamePrefix_ + 'availableProviders');
        }
        await this.fPreserveVarsInPlugin_.removeItem(kAuthPluginLocalstorageNamePrefix_ + 'activeProvider');
        await this.fPreserveVarsInPlugin_.removeItem(kAuthPluginLocalstorageNamePrefix_ + 'userInfo');
        await this.fPreserveVarsInPlugin_.removeItem(kAuthPluginLocalstorageNamePrefix_ + 'tokensInfo');
    }

    private setupAutoRefreshCallback_() {
        if (this.fAutoRefreshTimeoutCallback_) {
            clearTimeout(this.fAutoRefreshTimeoutCallback_);
            this.fAutoRefreshTimeoutCallback_ = undefined;
        }
        if (this.fTokensInfo_.value && this.fTokensInfo_.value.expiresAt) {
            let dur = this.fTokensInfo_.value.expiresAt.diffNow();
            if (this.fAutoRefreshTokenThisMuchBeforeExpiry_.value) {
                dur = dur.minus(this.fAutoRefreshTokenThisMuchBeforeExpiry_.value);
                this.fAutoRefreshTimeoutCallback_ = setTimeout(() => {
                    this.autoRefreshAccessToken_();
                }, dur.as('milliseconds'));
            }
        }
    }

    constructor(options: AuthOptions) {
        this.fOptions_ = options;
        this.fRouter_ = options.router
        this.fNotifier_ = new AuthorizationNotifier();
        this.fAuthorizationHandler_ = new RedirectRequestHandler();
        this.fAuthorizationHandler_.setAuthorizationNotifier(this.fNotifier_);
        // set a listener to listen for authorization responses
        this.fNotifier_.setAuthorizationListener((request, response, error) => {
            if (kDebugLogging_) {
                console.log('Authorization request complete ', request, response, error);
            }
            if (response) {
                this.fLastAuthorizationRequest_ = request;
                this.preserve_('lastAuthorizationRequest', request);
                this.fLastAuthorizationResponse_ = response;
                this.fLastAuthorizationCode_ = response.code;
                this.preserve_('lastAuthorizationCode', this.fLastAuthorizationCode_);
                if (kDebugLogging_) {
                    console.log(`***WINNER WINNER CHICKIN DINNER: Authorization Code ${response.code}`);
                    if (request?.internal && request?.internal['code_verifier']) {
                        console.log(`***AND: code_verifier ${request?.internal['code_verifier']}`);
                    }
                }
            }
        });

        // constructor sync, but we want to invoke async get method, and copy out its result
        setTimeout(async () => {
            this.fUser_.value = await this.retrievePreservedT_<IUserInfo>('userInfo');
            this.fLastAuthorizationCode_ = await this.retrievePreserved_('lastAuthorizationCode');
            this.fLastAuthorizationRequest_ = await this.retrievePreservedT_<AuthorizationRequest>('lastAuthorizationRequest');

            const ti = await this.retrievePreservedT_<ITokenInfo>('tokensInfo');
            if (typeof ti?.expiresAt == "string") {
                ti.expiresAt = DateTime.fromISO(ti.expiresAt);
            }
            this.fTokensInfo_.value = ti;

            try {
                this.fAvailableProviderConfigs_.value = await this.assureOAuthConfigs_();
            }
            catch { }
            this.activeProvider.value = await this.retrievePreservedT_<IOAuthProviderConfig>('activeProvider');
            this.setupAutoRefreshCallback_();
        }, 1);
    }

    private async cleanOutCachedPluginData_() {
        await this.clearPreserved_();
    }

    // If logged in, but logged in with expired token, try to auto-refresh that token
    // if not logged in, do nothing
    //
    // note - we do this with setTimeout instead of on-demand when our async methods accessed, because that works better with
    // reactive elements (which dont callback to us in async calls as needed).
    private async autoRefreshAccessToken_() {
        const ti = this.fTokensInfo_.value;
        console.log('Nearly expired auth token - so trying to refresh it');
        if (ti?.refreshToken) {
            const o = await this.makeTokenRequest_({ refreshToken: ti.refreshToken });
            console.log('Successfully refreshed accessToken with refresh_token', o);
            return;
        }
        // try an actual login again
        await this.login();// best option? - maybe can do better ; think I've done better in the past with extra param saying prev login and msft auth server
    }

    private async getLastAuthorizationCode_(): Promise<string | undefined> {
        if (!this.fLastAuthorizationCode_) {
            // still must check cuz CTOR not async, and no wait there - so this might not have completed by the time needed
            this.fLastAuthorizationCode_ = await this.retrievePreserved_('lastAuthorizationCode');
        }
        return this.fLastAuthorizationCode_;
    }

    private async getLastAuthorizationRequest_(): Promise<AuthorizationRequest | undefined> {
        if (!this.fLastAuthorizationRequest_) {
            // still must check cuz CTOR not async, and no wait there - so this might not have completed by the time needed
            this.fLastAuthorizationRequest_ = await this.retrievePreservedT_<AuthorizationRequest>('lastAuthorizationRequest');
        }
        return this.fLastAuthorizationRequest_;
    }

    private async checkForAuthorizationResponse_() {
        await this.fAuthorizationHandler_.completeAuthorizationRequestIfPossible();
    }

    private async assureActiveProvider_(): Promise<IOAuthProviderConfig> {
        this.activeProvider.value = await this.retrievePreservedT_<IOAuthProviderConfig>('activeProvider');
        if (!this.activeProvider.value) {
            throw new Error("No activeProvider recorded in localstorage");
        }
        return this.activeProvider.value;
    }

    // Handles the oauth 'redirect' callback - squirreling away any needed values, and perhaps
    // taking next steps??? - but also redirecting back to the 'backToAfterRedirect' page (defaults to currnet route as of when you called login)
    public async handleRedirectCallback() {
        if (kDebugLogging_) {
            console.log('Entering handleRedirectCallback');
        }

        const oauthConfig: IOAuthProviderConfig = await this.assureActiveProvider_();
        // Crazy michugas - cuz library using expects the value to be in the # part of the URL
        const queryString = window.location.search.substring(1); // includes '?'
        const path = [oauthConfig.redirectUri, queryString].join('#');
        if (queryString) {
            window.location.assign(path);
        }
        else {
            await this.checkForAuthorizationResponse_();
            try {
                const authorizationCode = await this.getLastAuthorizationCode_();
                const authorizationRequest = await this.getLastAuthorizationRequest_();
                await this.makeTokenRequest_({ authorizationCode, authorizationRequest });
            }
            catch (e) {
                console.log('makeTokenRequest_ failed - seems google doesnt allow PKCE as of 2025-01-27 for web hosted JS apps - but authjs library requires it!', e);
            }
        }
        if (!this.fBackToAfterRedirectRoute_) {
            this.fBackToAfterRedirectRoute_ = await this.retrievePreserved_('backToAfterRedirectRoute') ?? '';
        }
        this.fRouter_.push(this.fBackToAfterRedirectRoute_)
    }

    private fForceConsentPrompt_ = false;

    public async login(args?: { useProvider?: IOAuthProviderConfig, redirectTo?: string }) {
        // Workaround weird issue with google - use prompt: consent to force gen of refresh token
        // Maybe not needed if you use the 'logout' feature (currently in use) - but this appears to cause
        // no problems, and sometimes fixes missing refresh token issues --LGP 2025-02-19
        if (this.fTokensInfo_.value && this.fTokensInfo_.value.refreshToken == undefined) {
            this.fForceConsentPrompt_ = true;
        }
        await this.logout();    // else we could leave some data structures with inconsistent values - partialled logged into one and partially another

        this.fBackToAfterRedirectRoute_ = args?.redirectTo || this.fRouter_.currentRoute.value.path;
        const useProvider = args?.useProvider || (this.availableProviders.value && this.availableProviders.value[0]);
        if (useProvider === undefined) {
            throw new Error("explicit provider required if no availableProviders");
        }
        this.activeProvider.value = useProvider;
        await this.preserve_('activeProvider', useProvider)

        if (kDebugLogging_) {
            console.log('In login: Setting this.fBackToAfterRedirectRoute_=', this.fBackToAfterRedirectRoute_);
        }
        await this.preserve_('backToAfterRedirectRoute', this.fBackToAfterRedirectRoute_)

        const oauthConfig: IOAuthProviderConfig = await this.assureActiveProvider_();
        await this.makeAuthCodeRequest_(oauthConfig);      // even after this await, still just started process
    }

    public async logout() {
        const tokens2Revoke = this.fTokensInfo_.value;
        const oauthConfig: IOAuthProviderConfig | undefined = await (async (): Promise<IOAuthProviderConfig | undefined> => {
            try {
                return await this.assureActiveProvider_();
            }
            catch {
                return undefined;
            }
        })();

        let configuration: AuthorizationServiceConfiguration | undefined;
        if (kSupportAuthJSBasedRevocation_) {
            configuration = await (async (): Promise<AuthorizationServiceConfiguration | undefined> => {
                try {
                    return await this.fetchServiceConfiguration_();
                }
                catch {
                    return undefined;
                }
            })();
        }
        if (kSupportRefreshTokenRevocation_) {
            if (tokens2Revoke && oauthConfig) {
                try {
                    await revokeTokens({ provider: oauthConfig.provider, refreshToken: tokens2Revoke.refreshToken, accessToken: tokens2Revoke.accessToken });
                }
                catch { }
            }
        }

        await this.cleanOutCachedPluginData_();
        this.activeProvider.value = undefined;
        this.fUser_.value = undefined;
        this.fTokensInfo_.value = undefined;
        this.setupAutoRefreshCallback_();

        if (tokens2Revoke && configuration && oauthConfig) {
            console.log('revoking token', tokens2Revoke.refreshToken);
            if (kSupportAuthJSBasedRevocation_) {
                // console.log('configuration=', configuration);
                // revoke the token(s) - really only need todo to refresh token OR the access token since doing for refresh token should automatically do both
                const revocationHandler = new BaseTokenRequestHandler(new FetchRequestor());
                const result = await revocationHandler.performRevokeTokenRequest(configuration, new RevokeTokenRequest({
                    token: tokens2Revoke.refreshToken,
                    client_id: oauthConfig.clientId,
                    // client_secret: oauthConfig.clientSecret
                }));
                const result2 = await revocationHandler.performRevokeTokenRequest(configuration, new RevokeTokenRequest({
                    token: tokens2Revoke.accessToken,
                    client_id: oauthConfig.clientId,
                    client_secret: "THE REASON THIS DOESNT WORK WITH GOOGLE AND WHY WE DO IT IN THE BACKEND"
                }));
            }
        }

    }
    get activeProvider(): Ref<IOAuthProviderConfig | undefined> {
        return this.fActiveProviderConfig_;
    }
    get availableProviders(): Ref<IOAuthProviderConfig[] | undefined> {
        return this.fAvailableProviderConfigs_;
    }
    get autoRefreshAuth(): Ref<Duration | undefined> {
        return this.fAutoRefreshTokenThisMuchBeforeExpiry_;
    }
    get authorizationHeader(): Ref<string | undefined> {
        return computed(() => this.fTokensInfo_.value ? "Bearer " + this.fTokensInfo_.value?.accessToken : undefined);
    }
    get authorizationTokens(): Ref<ITokenInfo | undefined> {
        return this.fTokensInfo_;
    }
    get user(): Ref<IUserInfo | undefined> {
        return this.fUser_;
    }

    // Note - often with OAUTH, you make this request of the OAUTH provider (thats what PKCE is for mainly). But google
    // as of 2025-02-05 - still requires a client secret, only stored in the backend. So we indirect to the backend
    // to perform the actual token request.
    //
    // NOTE - alternatively, we could just grab the ID_token out of the first /auth request, if it provides an ID_Token (google does with the right scopes).
    private async makeTokenRequest_(args: { authorizationCode?: string, authorizationRequest?: AuthorizationRequest, refreshToken?: string }) {
        const oauthConfig: IOAuthProviderConfig = await this.assureActiveProvider_();
        if (kDebugLogging_) {
            console.log(`do call back to backend: authorization_code=${args.authorizationCode} refresh_token=${args.refreshToken}, and verifier=${args.authorizationRequest?.internal && args.authorizationRequest?.internal['code_verifier']}`)
        }
        const applicationId = oauthConfig.clientId;
        const codeVerifier: string | undefined = args.authorizationRequest?.internal && args.authorizationRequest?.internal['code_verifier'];
        const redirectURL = oauthConfig.redirectUri;
        const provider = oauthConfig.provider;
        const tokensInfo = await fetchTokens({ params: { authorizationCode: args.authorizationCode, refreshToken: args.refreshToken, provider, applicationId, redirectURL, codeVerifier } }) as ITokenInfo;
        if (tokensInfo.refreshToken == undefined && args.refreshToken) {
            console.error('refresh_token not returned from server, but we had one, so using the one we had');   // never seen, just worried might happen
            tokensInfo.refreshToken = args.refreshToken;
        }
        this.preserve_('tokensInfo', tokensInfo);
        if (typeof tokensInfo.expiresAt == "string") {
            tokensInfo.expiresAt = DateTime.fromISO(tokensInfo.expiresAt);
        }
        this.fTokensInfo_.value = tokensInfo
        this.setupAutoRefreshCallback_();
        const userInfo = await fetchUserInfo({ auth: this });
        this.fUser_.value = userInfo;
        this.preserve_('userInfo', this.fUser_.value);
        return tokensInfo;
    }

    private async assureOAuthConfigs_(): Promise<IOAuthProviderConfig[]> {
        if (kStoreExtraStuffInSessionForSpeed_) {
            this.fAvailableProviderConfigs_.value = await this.retrievePreservedT_<IOAuthProviderConfig[]>('availableProviders');
        }
        if (!this.fAvailableProviderConfigs_.value) {
            this.fAvailableProviderConfigs_.value = await this.fOptions_.configFetcher();
            if (kStoreExtraStuffInSessionForSpeed_) {
                this.preserve_('availableProviders', this.fAvailableProviderConfigs_.value);
            }
        }
        if (this.fAvailableProviderConfigs_.value) {
            return this.fAvailableProviderConfigs_.value;
        }
        else {
            console.log('assureOAuthConfigs_ throwing cuz couldnt fetch oauth config');
            throw new Error('No OAuth configuration, so no oauth supported');
        }
    }

    private async fetchServiceConfiguration_(): Promise<AuthorizationServiceConfiguration> {
        if (!this.fActiveOAuthServiceConfiguration_) {
            const oauthConfig: IOAuthProviderConfig = await this.assureActiveProvider_();
            let openIDURL = oauthConfig.openIdConnectUrl;   // authJS prefers this name without the /wellknow stuff at the end
            if (openIDURL.endsWith('/.well-known/openid-configuration')) {
                openIDURL = openIDURL.substring(0, openIDURL.length - '/.well-known/openid-configuration'.length);
            }
            this.fActiveOAuthServiceConfiguration_ = await AuthorizationServiceConfiguration.fetchFromIssuer(openIDURL, new FetchRequestor());
            if (kDebugLogging_) {
                console.log("fetched service configuration", this.fActiveOAuthServiceConfiguration_);
            }
        }
        return this.fActiveOAuthServiceConfiguration_;
    }

    // Make initial authorization request to get an authorization code
    private async makeAuthCodeRequest_(oauthConfig: IOAuthProviderConfig) {
        // may need property in 'Provider' saying if we can do RESPONSE_TYPE_CODE or must do RESPONSE_TOKEN_TYPE flow?? --LGP 2025-02-06
        const request = new AuthorizationRequest({
            client_id: oauthConfig.clientId,
            redirect_uri: oauthConfig.redirectUri,
            scope: oauthConfig.scope,
            response_type: AuthorizationRequest.RESPONSE_TYPE_CODE,
            state: undefined,
            extras: { 'access_type': 'offline' }    // offline to get refresh token
        });
        if (this.fForceConsentPrompt_) {
            request.extras['prompt'] = 'consent';   // not sure when we would logically want todo this, but it appears to be needed for google to get refresh token when re-logging in
        }
        const configuration = await this.fetchServiceConfiguration_()
        // make the authorization request (in the web browser - send sending urls to auth server)
        this.fAuthorizationHandler_.performAuthorizationRequest(configuration, request);
    }
}


export default {
    install: (app: App, options: AuthOptions) => {
        const authService = new AuthService(options);
        //console.log('Installing auth plugin', authService);
        app.config.globalProperties.$auth = authService;
        app.provide('auth', authService);
    },
};