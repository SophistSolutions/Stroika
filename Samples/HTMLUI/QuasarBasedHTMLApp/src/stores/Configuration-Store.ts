import { computed, ComputedRef, ref, Ref, watch } from 'vue';
import { defineStore } from 'pinia';

import { gRuntimeConfiguration } from 'boot/configuration';


export const useConfigurationStore = defineStore('Configuration-Store', () => {

  // Note we PERSIST some peices of state in localStorage by loading at start, and saving with watcher

  // State
  const apiServer: string = `${gRuntimeConfiguration.API_ROOT}`;
  const leftDrawerOpen: Ref<boolean> = ref(localStorage.getItem('Configuration-Store.leftDrawerOpen') === 'true');

  // Getters (not really sure these are useful anymore...)
  const getLeftDrawerOpen: ComputedRef<boolean> = computed(() => {
    return leftDrawerOpen.value;
  });

  // Actions (none for now)
  // async function setThisOrThat(u: arg) {
  //   leftDrawerOpen.value = u;
  // }

  // add watchers
  watch(leftDrawerOpen,
    (newV, oldV) => localStorage.setItem('Configuration-Store.leftDrawerOpen', JSON.stringify(newV)));

  return {
    apiServer, leftDrawerOpen, 
    getLeftDrawerOpen, 
  };

});

