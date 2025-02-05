<script setup lang="ts">
import { defineComponent, computed, ref, Ref, onMounted, getCurrentInstance } from "vue";

import { useRouter, useRoute } from "vue-router";
import { useConfigurationStore } from "../stores/Configuration-Store";
import { IAuthService, IUserInfo } from "../plugins/auth";

defineComponent({
    components: {},
});

const route = useRoute();
const router = useRouter();
const configurationStore = useConfigurationStore();
let auth: Ref<IAuthService> = getCurrentInstance()?.appContext.config.globalProperties.$auth;


const providers = [
    'Google'
];

const loginWith = ref("Google");

onMounted(() => {
    console.log("in /user page on-mounted auth=", auth);
});

const isAuthenticated = computed(() => auth.user.value != null)

const personName = computed(() => auth.user.value?.personName);
const personEMail = computed(() => auth.user.value?.email);
const personImageURL = computed(() => auth.user.value?.personImageURL);

function login() {
    auth.login();
}

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
                    <div class="row">Configuration:</div>
                    <pre v-if="auth.config.value">{{ JSON.stringify(auth.config.value, null, 2) }}</pre>
                    <div v-if="!auth.config.value">
                        <div class="row">
                            No OAuth configuration.
                        </div>
                        <div class="row">
                            See /api/v1/auth/oauth/configurations  (backend api)
                        </div>
                        <div class="row">
                            See C:/ProgramData/Stroika-Sample-HTMLUI/Server.json (or /var/opt/Stroika-Sample-HTMLUI on unix)
                        </div>
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
                "scopes" : [
                    "email",
                    "openid",
                    "profile"
                ],
                "clientSecret" : "ITS A SECRET"
            }
        ]
    }
                        </pre>
                        
                    </div>

                </q-card-section>
            </q-card>


            <q-card class="col-11 pageCard">
                <q-card-section>
                    <div class="row">Login/Logout:</div>
                    <div class="q-gutter-y-sm column">
                        <div class="row" v-if="isAuthenticated">
                            You are logged in as "{{ personName }}" (email: {{ personEMail }})
                            <img v-if="personImageURL" :src="personImageURL" />
                        </div>
                        <div class="row" v-if="personName == ''">
                            For your caregivers to associate test results with you, you must identify
                            yourself.
                        </div>
                        <div class="row" v-if="!isAuthenticated">
                            If you have already registered, and know your email, you may authenticate with
                            that email.
                        </div>
                        <div class="row">
                            <div class="col-1">
                            </div>
                            <div class="col-2 self-center" v-if="isAuthenticated">
                                <button v-on:click="logout">LogOut</button>
                            </div>
                            <div class="col-3">
                                <q-select v-model="loginWith" :options="providers" label="Authenticate With" />
                            </div>
                            <div class="self-center q-pl-md">
                                <button v-on:click="login" class="q-pl-sm">{{ isAuthenticated ? "Re-authenticate" :
                                    "Login" }}</button>
                            </div>
                        </div>
                    </div>
                </q-card-section>


            </q-card>
        </div>
    </q-page>
</template>

<style lang="scss" scoped></style>
