import { defineStore } from 'pinia';
import { computed, ComputedRef, ref, Ref } from 'vue';

import { IAbout } from 'src/models/IAbout';
import api from 'src/proxy/API'


export const useMainAppStateStore = defineStore('MainApp-State-Store', () => {

  // State
  const about: Ref<IAbout | undefined> = ref(undefined)
  const lastSuccessfulAPICall: Ref<Date | undefined> = ref(undefined)

  // Getters (not really sure these are useful anymore...)
  const getAboutInfo: ComputedRef<IAbout | undefined> = computed(() => {
    return about.value;
  });
  const getLastSuccessfulAPICall: ComputedRef<Date | undefined> = computed(() => {
    return lastSuccessfulAPICall.value;
  });

  // Actions (none for now)
  async function fetchAboutInfo() {
    about.value = await api.fetchAboutInfo();
    lastSuccessfulAPICall.value = new Date();
  }

  // add watchers (none)

  return {
    about,
    lastSuccessfulAPICall,
    getAboutInfo,
    getLastSuccessfulAPICall,
    fetchAboutInfo
  };

});