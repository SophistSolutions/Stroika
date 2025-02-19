<script setup lang="ts">
import { defineComponent, computed, watch, ref, Ref, onMounted, getCurrentInstance, onUnmounted } from "vue";
import { QSelectOption } from 'quasar';

import { useRouter, useRoute } from "vue-router";
import { useConfigurationStore } from "../stores/Configuration-Store";
import { IAuthService, IOAuthProviderConfig } from "../plugins/auth";

defineComponent({
    components: {},
});

function capitalize_(s: string) {
    if (s.length > 0) {
        return s.charAt(0).toUpperCase() + s.slice(1);
    }
    return s;
}

const route = useRoute();
const router = useRouter();
const configurationStore = useConfigurationStore();
let auth: IAuthService = getCurrentInstance()?.appContext.config.globalProperties.$auth;

const availableProviders: Ref<IOAuthProviderConfig[] | undefined> = auth.availableProviders;
const availableProviderOptions = computed(() => availableProviders.value?.map((i) => { return { 'label': capitalize_(i.provider), 'value': i.provider }; }));
const activeProvider: Ref<IOAuthProviderConfig | undefined> = auth.activeProvider;

const loginWith: Ref<QSelectOption> = ref(availableProviderOptions.value?.length ? availableProviderOptions.value[0] : { 'label': "Unavailable", 'value': "unavailable" });
// availableProviders not immediately available always, and not sure how to specify loginWith to have its default react this way
// other than watch --LGP 2025-02-06
watch(availableProviders, () => {
    if (loginWith.value.value == "unavailable" && availableProviderOptions.value?.length) {
        loginWith.value = availableProviderOptions.value[0];
    }
})

let iterervalTimer: NodeJS.Timeout | undefined = undefined;

onMounted(() => {
    console.log("in /user page on-mounted auth=", auth);

    recomputeExpiresIn_()
    iterervalTimer = setInterval(function () {
        recomputeExpiresIn_()
    }, 30 * 1000);
});

onUnmounted(() => {
    console.log("in /user page on-mounted auth=", auth);
    if (iterervalTimer) {
        clearInterval(iterervalTimer);
    }
});

const isAuthenticated = computed(() => auth.user.value != null);

const personName = computed(() => auth.user.value?.personName);
const personEMail = computed(() => auth.user.value?.email);
const personImageURL = computed(() => auth.user.value?.personImageURL);

async function login() {
    const useProvider: IOAuthProviderConfig | undefined = availableProviders.value?.find((i) => { if (loginWith.value.value == i.provider) { return i; } });
    try {
        await auth.login({ useProvider });
    }
    catch (e) {
        alert(`Error during login: ${e?.message}`);
    }
}

/*
 *  Somewhat confusing limitation of vue reactivity - not working well with Luxons 'time' - or noticing that time going by
 *  changes these relative values. So we must update manually with setInterval
 */
const expiresIn = ref("");
function recomputeExpiresIn_() {
    expiresIn.value = auth.authorizationTokens.value?.expires_at?.toRelative();
}
watch(auth.authorizationTokens, () => {
    recomputeExpiresIn_();
});

function logout() {
    auth.logout();
}
</script>

<template>
    <q-page padding>
        <div class="row text-h4 text-center q-mb-md">
            <div class="col">HTMLUI - Auth Test</div>
        </div>
        <div class="justify-center row">
            <q-card class="col-11 pageCard">
                <q-card-section>
                    <div class="row">Configuration (availableProviders - from Backend webservices and its
                        configuration):</div>
                    <pre v-if="auth.availableProviders.value">{{
                        JSON.stringify(auth.availableProviders.value, null, 2)
                    }}</pre>
                    <div v-if="!auth.availableProviders.value">
                        <div class="row" style="color:red">No OAuth configuration.</div>
                        <div class="row">See /api/v1/auth/oauth/configurations (backend api)</div>
                        <div class="row">
                            See C:/ProgramData/Stroika-Sample-HTMLUI/Server.json (or
                            /var/opt/Stroika-Sample-HTMLUI on unix)
                        </div>
                        TRY something like:
                        <pre class="row">
    "Auth" : {
        "OAuth2" : [
            {
                "provider" : "google",
                "applicationID" : "57741917417-6jci8aese97ppaakqniqf8e1vidn0u7q.apps.googleusercontent.com",
                "redirectURLs" : [
                    "http://localhost/oauth/callback",
                    "https://localhost/oauth/callback",
                    "http://localhost:9000/oauth/callback",
                    "http://localhost:9001/oauth/callback",
                ],
                "scopes" : [ "email", "openid", "profile" ],
                "clientSecret" : "ITS A SECRET"
            }
        ]
    }
            </pre>
                    </div>
                </q-card-section>
            </q-card>

            <q-card class="col-11 pageCard" v-if="activeProvider">
                <q-card-section>
                    <div class="row">Configuration (activeProvider - if logged in, will be valid, but can be valid even
                        if not logged in):</div>
                    <div class="row">
                        <pre v-if="activeProvider">{{
                            JSON.stringify(activeProvider, null, 2)
                        }}</pre>
                    </div>
                </q-card-section>
            </q-card>

            <q-card class="col-11 pageCard">
                <q-card-section>
                    <div class="row">Login/Logout:</div>
                    <div class="q-gutter-y-sm">
                        <div class="row" v-if="isAuthenticated">
                            <div class="col-1"></div>
                            <div class="col-1">
                                User Info:
                            </div>
                            <div class="col">
                                You are logged in as "{{ personName }}" (email: {{ personEMail }})
                                <img v-if="personImageURL" :src="personImageURL" style="height: .25in; width: .25in;" />
                            </div>
                        </div>
                        <div class="row" v-if="auth.authorizationTokens.value">
                            <div class="col-1"></div>
                            <div class="col-1">
                                Access Token:
                            </div>
                            <div class="col">
                                <pre>{{ auth.authorizationTokens.value.access_token }}</pre>
                            </div>
                        </div>
                        <div class="row" v-if="auth.authorizationTokens.value">
                            <div class="col-1"></div>
                            <div class="col-1">ExpiresAt</div>
                            <div class="col">{{ auth.authorizationTokens.value.expires_at }} ({{
                                expiresIn }})</div>
                        </div>
                        <div class="row" v-if="auth.authorizationTokens.value?.id_token">
                            <div class="col-1"></div>
                            <div class="col-1">ID Token:</div>
                            <div class="col">
                                <pre>{{ auth.authorizationTokens.value.id_token }}</pre>
                            </div>
                        </div>
                        <div class="row" v-if="auth.authorizationTokens.value?.refresh_token">
                            <div class="col-1"></div>
                            <div class="col-1">Refresh Token:</div>
                            <div class="col">
                                <pre>{{ auth.authorizationTokens.value.refresh_token }}</pre>
                            </div>
                        </div>
                        <div class="row" v-if="!isAuthenticated">
                            NOT CURRENTLY LOGGED IN.
                        </div>
                        <div class="row">
                            <div class="col-1"></div>
                            <div class="col-2 self-center" v-if="isAuthenticated">
                                <button v-on:click="logout">LogOut</button>
                            </div>
                            <div class="col-3">
                                <q-select v-model="loginWith" :options="availableProviderOptions"
                                    label="Authenticate With" />
                            </div>
                            <div class="self-center q-pl-md">
                                <button v-on:click="login" class="q-pl-sm">
                                    {{ isAuthenticated ? "Re-authenticate" : "Login" }}
                                </button>
                            </div>
                        </div>
                    </div>
                </q-card-section>
            </q-card>
        </div>
    </q-page>
</template>

<style lang="scss" scoped></style>
