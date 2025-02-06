<script setup lang="ts">
import { onMounted, ref } from "vue";

import { getCurrentInstance } from "vue";
import { IAuthService } from "../plugins/auth";

let showPage = ref(false);

onMounted(async () => {
  const auth: IAuthService = getCurrentInstance()?.appContext.config.globalProperties.$auth;
  console.log("In RedirectPage onMounted Event: auth=", auth);
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
