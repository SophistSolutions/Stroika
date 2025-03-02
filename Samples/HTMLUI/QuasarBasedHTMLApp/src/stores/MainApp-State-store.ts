import { defineStore } from 'pinia';
import { computed, ComputedRef, ref, Ref } from 'vue';

import { IAbout } from 'src/models/IAbout';
import api from 'src/proxy/API'


export const useMainAppStateStore = defineStore('MainApp-State-Store', () => {

  // State
 const about: Ref< IAbout | undefined> = ref (undefined)

  // Getters (not really sure these are useful anymore...)
  const getAboutInfo: ComputedRef<IAbout|undefined> = computed(() => {
    return about.value;
  });

  // Actions (none for now)
  async function fetchAboutInfo() {
    about.value = await api.fetchAboutInfo();
  }

  // add watchers (none)

  return {
    about, 
    getAboutInfo, 
    fetchAboutInfo
  };

});