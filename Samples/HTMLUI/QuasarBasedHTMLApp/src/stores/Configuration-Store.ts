import { defineStore } from 'pinia';

import { gRuntimeConfiguration } from 'boot/configuration';

export const useConfigurationStore = defineStore('Configuration-Store', {
  state: () => ({
    apiServer: `${gRuntimeConfiguration.API_ROOT}` as string,
    leftDrawerOpen: false as boolean
  }),
  getters: {
    // getAPIServer: (state) => {
    //   return state.apiServer;
    // },
    getLeftDrawerOpen: (state) => {
      return state.leftDrawerOpen;
    },
  },
  actions: {
    // async setAPIServer(i: string) {
    //   this.apiServer = i;
    // },
    async setLeftDrawerOpen(i: boolean) {
      this.leftDrawerOpen = i;
    },
  },
  persist: true
});