import { defineStore } from 'pinia';

export const useConfigurationStore = defineStore('Configuration-Store', {
  state: () => ({
    leftDrawerOpen: false as boolean
  }),
  getters: {
    getLeftDrawerOpen: (state) => {
      return state.leftDrawerOpen;
    },
  },
  actions: {
    async setLeftDrawerOpen(i: boolean) {
      this.leftDrawerOpen = i;
    },
  },
  // see docs from https://github.com/prazdevs/pinia-plugin-persistedstate?tab=readme-ov-file
  persist: {
    // somenow hot working - and really persisting everything?
    pick: ['leftDrawerOpen'],
  }
});