import { App, ref, Ref } from 'vue';
import { Router, RouteLocationRaw } from "vue-router";

import { fetchTokens, fetchUserInfo } from '../proxy/API';

import {
    AuthorizationRequest,
    AuthorizationNotifier, AuthorizationRequestHandler,
    AuthorizationServiceConfiguration,
    AuthorizationResponse,
    RedirectRequestHandler,
    FetchRequestor, LocalStorageBackend,
    BaseTokenRequestHandler, TokenRequestHandler,
} from '@openid/appauth';

import { gRuntimeConfiguration } from 'boot/configuration';

export interface OAuthConfig {
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
    configFetcher(): Promise<OAuthConfig | undefined>;

    /**
     *  The router is needed to help support the 'redirect' OAuth requires (so when you call login, it reserves the current route and after
     *  the oauth redirect, routes back to that original saved route).
     */
    router: Router;
}

/**
 *  Information returned from the auth process, about the currently logged in user.
 */
export interface IUserInfo {
    personImageURL?: string;
    personName?: string;
    email?: string;
}

/**
 *  API published by the plugin, on the  app.globalProperties.$auth field and the app.provide name (not sure why both
 *  or which we use when)?
 */
export interface IAuthService {

     /**
     *  readonly ref - soon to be changed to array (and have login take arg saying which config)
     */
     config: Ref<OAuthConfig | undefined>;

    /**
     *   call this method in the oauth redirect to handle the page arguments
     */
    handleRedirectCallback(): Promise<void>;

    /**
     *  Initiate OAuth redirect to provider to 'login' the user
     */
    login(): Promise<void>;

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
    private fBackToAfterRedirectRoute_?: RouteLocationRaw;

    private fNotifier_: AuthorizationNotifier;
    private fAuthorizationHandler_: AuthorizationRequestHandler;

    // state
    private fConfiguration_?: AuthorizationServiceConfiguration;
    private fLastAuthorizationRequest_: AuthorizationRequest | undefined;
    private fLastAuthorizationResponse_: AuthorizationResponse | undefined;
    private fLastAuthorizationCode_: string | undefined;

    private fOptions_: AuthOptions;
    private fOAuthConfig_?: Ref<OAuthConfig | undefined> = ref(undefined);;

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
        await this.fPreserveVarsInPlugin_.removeItem(kAuthPluginLocalstorageNamePrefix_ + 'oauthConfig');
        await this.fPreserveVarsInPlugin_.removeItem(kAuthPluginLocalstorageNamePrefix_ + 'userInfo');
        // await this.fPreserveVarsInPlugin_.removeItem(kAuthPluginLocalstorageNamePrefix_ +'tokensInfo');      // not currently used/stored, but might be useful for refresh
    }

    constructor(options: AuthOptions) {
        this.fOptions_ = options;
        this.fRouter_ = options.router
        this.fNotifier_ = new AuthorizationNotifier();
        this.fAuthorizationHandler_ = new RedirectRequestHandler();
        this.fAuthorizationHandler_.setAuthorizationNotifier(this.fNotifier_);
        // set a listener to listen for authorization responses
        this.fNotifier_.setAuthorizationListener((request, response, error) => {
            console.log('Authorization request complete ', request, response, error);
            if (response) {
                this.fLastAuthorizationRequest_ = request;
                this.preserve_('lastAuthorizationRequest', request);
                this.fLastAuthorizationResponse_ = response;
                this.fLastAuthorizationCode_ = response.code;
                this.preserve_('lastAuthorizationCode', this.fLastAuthorizationCode_);
                console.log(`***WINNER WINNER CHICKIN DINNER: Authorization Code ${response.code}`);
                if (request?.internal && request?.internal['code_verifier']) {
                    console.log(`***AND: code_verifier ${request?.internal['code_verifier']}`);
                }
            }
        });

        // constructor sync, but we want to invoke async get method, and copy out its result
        setTimeout(async () => {
            this.fUser_.value = await this.retrievePreservedT_<IUserInfo>('userInfo');
            this.fLastAuthorizationCode_ = await this.retrievePreserved_('lastAuthorizationCode');
            this.fLastAuthorizationRequest_ = await this.retrievePreservedT_<AuthorizationRequest>('lastAuthorizationRequest');
            this.assureOAuthConfig_();  // so attempts to peek at configuration will show right config
        }, 1);
    }

    private async cleanOutCachedPluginData_() {
        await this.clearPreserved_();
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

    // Handles the oauth 'redirect' callback - squirreling away any needed values, and perhaps
    // taking next steps??? - but also redirecting back to the 'backToAfterRedirect' page (defaults to currnet route as of when you called login)
    public async handleRedirectCallback() {
        console.log('Entering handleRedirectCallback');

        // Crazy michugas - cuz library using expects the value to be in the # part of the URL
        const oauthConfig = await this.assureOAuthConfig_();
        const queryString = window.location.search.substring(1); // includes '?'
        const path = [oauthConfig.redirectUri, queryString].join('#');
        if (queryString) {
            window.location.assign(path);
        }
        else {
            await this.checkForAuthorizationResponse_();
            try {
                await this.makeTokenRequest_();
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

    public async login({ redirectTo } = { redirectTo: undefined }) {
        this.fBackToAfterRedirectRoute_ = redirectTo || this.fRouter_.currentRoute.value;

        await this.cleanOutCachedPluginData_(); // maybe too aggressive - but for now avoids confusing issues with old data

        console.log('In login: Setting this.fBackToAfterRedirectRoute_=', this.fBackToAfterRedirectRoute_);
        await this.preserve_('backToAfterRedirectRoute', this.fBackToAfterRedirectRoute_.path)

        await this.makeAuthCodeRequest_();      // even after this await, still just started process
    }

    public async logout() {
        await this.cleanOutCachedPluginData_();
        this.fUser_.value = undefined;   // this doesn't work but without the .value does!!!
    }
    get config(): Ref<OAuthConfig | undefined> {
        return this.fOAuthConfig_;
    }
    get user(): Ref<IUserInfo | undefined> {
        return this.fUser_;
    }

    // Note - often with OAUTH, you make this request of the OAUTH provider (thats what PKCE is for mainly). But google
    // as of 2025-02-05 - still requires a client secret, only stored in the backend. So we indirect to the backend
    // to perform the actual token request.
    //
    // NOTE - alternatively, we could just grab the ID_token out of the first /auth request, if it provides an ID_Token (google does with the right scopes).
    private async makeTokenRequest_() {
        const oauthConfig = await this.assureOAuthConfig_();
        const code = await this.getLastAuthorizationCode_();
        const lastAuthorizationRequest = await this.getLastAuthorizationRequest_();
        console.log(`do call back to backend: code=${code} and verifier=${lastAuthorizationRequest?.internal && lastAuthorizationRequest?.internal['code_verifier']}`)

        const authorizationCode = code ?? "";
        const applicationId = oauthConfig.clientId;
        const codeVerifier: string | undefined = lastAuthorizationRequest?.internal && lastAuthorizationRequest?.internal['code_verifier'];
        const redirectURL = oauthConfig.redirectUri;
        const provider = oauthConfig.provider;
        const tokensInfo = await fetchTokens(gRuntimeConfiguration.API_ROOT, { authorizationCode, provider, applicationId, redirectURL, codeVerifier });
        // console.log('tokensInfo=', tokensInfo);
        this.preserve_('tokensInfo', tokensInfo);

        const userInfo = await fetchUserInfo(gRuntimeConfiguration.API_ROOT, tokensInfo.access_token);
        // console.log('userInfo=', userInfo);
        this.fUser_.value = userInfo;//tmphack
        this.preserve_('userInfo', this.fUser_.value);
    }

    private async assureOAuthConfig_(): Promise<OAuthConfig> {
        this.fOAuthConfig_.value = await this.retrievePreservedT_<OAuthConfig>('oauthConfig');
        if (!this.fOAuthConfig_.value) {
            this.fOAuthConfig_.value = await this.fOptions_.configFetcher();
            this.preserve_('oauthConfig', this.fOAuthConfig_.value);
        }
        if (this.fOAuthConfig_.value) {
            return this.fOAuthConfig_.value;
        }
        else {
            console.log('assureOAuthConfig_ throwing cuz couldnt fetch oauth config');
            throw new Error('No OAuth configuration, so no oauth supported');
        }
    }

    private async fetchServiceConfiguration_(): Promise<AuthorizationServiceConfiguration> {
        if (!this.fConfiguration_) {
            const oauthConfig = await this.assureOAuthConfig_();
            this.fConfiguration_ = await AuthorizationServiceConfiguration.fetchFromIssuer(oauthConfig.openIdConnectUrl, new FetchRequestor());
            console.log("fetched service configuration", this.fConfiguration_);
        }
        return this.fConfiguration_;
    }

    // Make initial authorization request to get an authorization code
    private async makeAuthCodeRequest_() {
        const oauthConfig: OAuthConfig = await this.assureOAuthConfig_();

        const request = new AuthorizationRequest({
            client_id: oauthConfig.clientId,
            redirect_uri: oauthConfig.redirectUri,
            scope: oauthConfig.scope,
            response_type: AuthorizationRequest.RESPONSE_TYPE_CODE,
            state: undefined,
            // We currently dont use offline/refersh tokens, but might, and not sure why you would want to specify consent prompt...
            // extras: { 'prompt': 'consent', 'access_type': 'offline' }
            extras: { 'access_type': 'offline' }
        });

        const configuration = await this.fetchServiceConfiguration_()
        // make the authorization request (in the web browser - send sending urls to auth server)
        this.fAuthorizationHandler_.performAuthorizationRequest(configuration, request);
    }
}


export default {
    install: (app: App, options: AuthOptions) => {
        const authService = new AuthService(options);
        console.log('Installing auth plugin', authService);
        app.config.globalProperties.$auth = authService;
        app.provide('auth', authService);
    },
};