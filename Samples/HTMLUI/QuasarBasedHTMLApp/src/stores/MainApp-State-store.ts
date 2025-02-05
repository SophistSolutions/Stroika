import { useConfigurationStore } from './Configuration-Store';
import { defineStore } from 'pinia';

import { IAbout } from '../models/IAbout';
import {
  fetchAboutInfo,
} from '../proxy/API';

import { gRuntimeConfiguration } from 'src/boot/configuration';


// @todo perhaps add in 'lasttimerequested' and 'lastTimeSuccessfulResponse' and throttle/dont request
// (not sure where in model) if outtsanding requests etc) and maybe show in UI if data stale
interface ILoading {
  numberOfTimesLoaded: number;
  numberOfOutstandingLoadRequests: number;
}


/// DRAFT new WTF app data store  - maybe should be called cached-network-state-store?


export const useMainAppStateStore = defineStore('MainApp-State-Store', {
  state: () => ({
    about: undefined as IAbout | undefined,
  }),
  getters: {
    getAboutInfo: (state) => {
      return state.about;
    },
  },
  actions: {
    async fetchAboutInfo() {
      const configurationStore = useConfigurationStore();
      this.about = await fetchAboutInfo(gRuntimeConfiguration.API_ROOT);
    },
  },
});