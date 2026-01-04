<script setup lang="ts">
import { onMounted, ref, toRaw } from "vue";

import { getCurrentInstance } from "vue";
import { IAuthService } from "../plugins/auth";

let showPage = ref(false);

onMounted(async () => {
  const auth: IAuthService = getCurrentInstance()?.appContext.config.globalProperties
    .$auth;
  console.debug("In OAuth RedirectPage, onMounted Event: auth=", toRaw(auth));
  await auth.handleRedirectCallback();
  setTimeout(() => {
    showPage.value = true;
  }, 1000);
});
</script>

<template>
  <div v-if="showPage">
    shouldn't see this, the auth.handleRedirectCallback() should have redirected
  </div>
</template>
