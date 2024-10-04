<script setup lang="ts">
import { onMounted, onUnmounted, computed } from "vue";
import { useQuasar } from "quasar";
import moment from "moment";
import prettyBytes from "pretty-bytes";

import { kCompileTimeConfiguration } from "../config/config";
import { IAbout, IAPIEndpoint, IComponent, IDatabase } from "../models/IAbout";
import { useMainAppStateStore } from "../stores/MainApp-State-store";
import { PluralizeNoun } from "src/utils/Linguistics";
import { useConfigurationStore } from "stores/Configuration-Store";
import { gRuntimeConfiguration } from 'boot/configuration';
let polling: undefined | NodeJS.Timeout;
const $q = useQuasar();

const kUIComponents: IComponent[] = [
  {
    name: "Vue ",
    version: kCompileTimeConfiguration.VUE_VERSION,
    URL: "https://vuejs.org/",
  },
  { name: "Quasar ", version: $q.version, URL: "https://quasar.dev/" },
];

const kRefreshFrequencyInSeconds_: number = 10;

const store = useMainAppStateStore();

let aboutData = computed(() => store.getAboutInfo);

const configurationStore = useConfigurationStore();

onMounted(() => {
  // first time check quickly, then more gradually
  store.fetchAboutInfo();
  if (polling) {
    clearInterval(polling);
  }
  polling = setInterval(() => {
    store.fetchAboutInfo();
  }, kRefreshFrequencyInSeconds_ * 1000);
});
onUnmounted(() => {
  clearInterval(polling);
});

function prettyPrintMSTime(time?: string) {
  if (time == undefined) {
    return "?";
  }
  var m = moment.duration(time);
  if (m.asMilliseconds() < 1.0) {
    return (m.milliseconds() * 1000).toFixed(1) + "μs";
  }
  return m.milliseconds().toFixed(1) + "ms";
}
function wsAPIMsg(info: IAPIEndpoint, showShort: boolean): string {
  let msg = "";
  if (!showShort) {
    msg += `${info.callsCompleted} calls; `;
  }
  if (!showShort || info.errors != 0) {
    msg += `${info.errors} ${PluralizeNoun("error", info.errors)}; `;
  }
  if (showShort) {
    msg += `${info.medianWebServerConnections ?? "?"} connections`;
    if (info.medianRunningAPITasks && info.medianRunningAPITasks > 0) {
      msg += `(${info.medianRunningAPITasks ?? "?"} active API calls); `;
    } else {
      msg += "; ";
    }
  } else {
    msg += `${info.medianWebServerConnections ?? "?"} Med connections (${info.medianProcessingWebServerConnections ?? "?"
      } active, and Med ${info.medianRunningAPITasks ?? "?"} active API calls); `;
  }
  if (showShort) {
    msg += `${prettyPrintMSTime(info.medianDuration)}, max ${prettyPrintMSTime(
      info.maxDuration
    )}`;
  } else {
    msg += `Med ${prettyPrintMSTime(
      info.medianDuration
    )} call time,  max ${prettyPrintMSTime(info.maxDuration)} call time`;
  }
  return msg;
}
function dbStatsMsg(info: IDatabase, showShort: boolean): string {
  let msg = "";
  if (!showShort || info.errors != 0) {
    msg += `${info.errors} ${PluralizeNoun("error", info.errors)}; `;
  }
  if (info.fileSize) {
    msg += `${prettyBytes(info.fileSize)}; `;
  }
  if (!showShort) {
    msg += `${info.reads} reads, ${info.writes} writes; `;
  }
  if (showShort) {
    msg += `${prettyPrintMSTime(info.medianReadDuration)} reads, ${prettyPrintMSTime(
      info.medianWriteDuration
    )} writes`;
  } else {
    msg += `Med ${prettyPrintMSTime(
      info.medianReadDuration
    )} read duration, Med ${prettyPrintMSTime(info.medianWriteDuration)} write duration`;
  }
  if (showShort) {
    if (info.maxDuration != undefined) {
      msg += `; max ${prettyPrintMSTime(info.maxDuration)}`;
    }
  } else {
    if (info.maxDuration != undefined) {
      msg += `; max ${prettyPrintMSTime(info.maxDuration)} I/O duration`;
    }
  }
  return msg;
}
</script>

<template>
  <q-page class="q-pa-md">
    <div class="row text-h5">
      <div class="col">About 'Sample HTMLUI App''</div>
    </div>

    <div class="row q-pa-md justify-center">
      <!--App Description Overview-->
      <q-card class="pageCard col-11">
        <q-card-section style="margin-left: 2em">
          About backend server for HTMLUI Sample App.
        </q-card-section>
      </q-card>

      <q-card class="pageCard col-11">
        <q-card-section style="margin-left: 2em">
          Connected to server: <b>{{gRuntimeConfiguration.API_ROOT}}</b>
        </q-card-section>
      </q-card>

      <!--App Stats-->
      <q-card class="pageCard col-11" v-if="aboutData">
        <q-card-section>
          <div class="row">
            <div class="col-6 text-h6">Sample HTMLUI</div>
          </div>
          <div class="row">
            <div class="col-1" />
            <div class="col-9">
              <div class="row" v-if="aboutData">
                <div class="col-3">Version</div>
                <div class="col">{{ aboutData.applicationVersion }}</div>
              </div>
              <div class="row" v-if="aboutData">
                <div class="col-3">Components</div>
                <div class="col">
                  <div class="row" v-for="c in aboutData.serverInfo.componentVersions.concat(
            kUIComponents
          )" :key="c.name">
                    <div class="col-9">
                      <a :href="c.URL" target="_new">{{ c.name }}</a>
                    </div>
                    <div class="col-2">{{ c.version }}</div>
                  </div>
                </div>
              </div>
              <div class="row" v-if="aboutData">
                <div class="col-3" title="Average CPU usage of the Backend (server app process) over the last 30 seconds;
Units 1=1 logical core">
                  CPU-Usage
                </div>
                <div class="col">
                  {{
            aboutData.serverInfo.currentProcess.averageCPUTimeUsed?.toFixed(2) ||
            "?"
          }}
                  CPUs
                </div>
              </div>
              <div class="row" v-if="aboutData" title="Combined I/O rate (network+disk)">
                <div class="col-3 truncateWithElipsis">IO Rate (read;write)</div>
                <div class="col-4" v-if="aboutData.serverInfo.currentProcess.combinedIOReadRate != undefined &&
            aboutData.serverInfo.currentProcess.combinedIOWriteRate != undefined
            ">
                  {{
            prettyBytes(aboutData.serverInfo.currentProcess.combinedIOReadRate)
          }}/sec ;
                  {{
              prettyBytes(aboutData.serverInfo.currentProcess.combinedIOWriteRate)
            }}/sec
                </div>
              </div>
              <div class="row" v-if="aboutData">
                <div class="col-3" title="How long has the service been running">
                  Uptime
                </div>
                <div class="col-4" v-if="aboutData.serverInfo.currentProcess.processUptime">
                  {{
            moment
              .duration(aboutData.serverInfo?.currentProcess?.processUptime)
              .humanize()
          }}
                </div>
              </div>
              <div class="row" v-if="aboutData">
                <div class="col-3" title="Working set size, or RSS resident set size (how much RAM is an active use)">
                  Memory
                </div>
                <div class="col-4" v-if="aboutData.serverInfo.currentProcess.workingOrResidentSetSize">
                  {{
            prettyBytes(
              aboutData.serverInfo.currentProcess.workingOrResidentSetSize
            )
          }}
                </div>
              </div>
              <div class="row" v-if="aboutData">
                <div class="col-3"
                  title="Information about app WebService endpoint (median #connections, timing, Q-lengths) over the last 5 minutes">
                  WSAPI
                </div>
                <div class="col" v-if="aboutData.serverInfo.apiEndpoint"
                  :title="wsAPIMsg(aboutData.serverInfo.apiEndpoint, false)">
                  {{ wsAPIMsg(aboutData.serverInfo.apiEndpoint, true) }}
                </div>
              </div>
              <div class="row" v-if="aboutData">
                <div class="col-3"
                  title="Information about database: size on disk, median read/write times over the last 5 minutes; hover for more details">
                  DB
                </div>
                <div class="col" v-if="aboutData.serverInfo.database"
                  :title="dbStatsMsg(aboutData.serverInfo.database, false)">
                  {{ dbStatsMsg(aboutData.serverInfo.database, true) }}
                </div>
              </div>
            </div>
          </div>
        </q-card-section>
      </q-card>

      <!--App Running on-->
      <q-card class="pageCard col-11" v-if="aboutData">
        <q-card-section>
          <div>
            <div class="row" v-if="aboutData">
              <div class="col-3 text-h6">Backend Running on</div>
              <div class="col-9">
                <div class="row">
                  <div class="col-3">OS</div>
                  <div class="col">
                    {{
            aboutData.serverInfo.currentMachine.operatingSystem
              .fullVersionedName
          }}
                  </div>
                </div>
                <div class="row" title="How long has the machine (hosting the service) been running">
                  <div class="col-3">Uptime</div>
                  <div class="col">
                    {{
              moment
                .duration(aboutData.serverInfo.currentMachine.machineUptime)
                .humanize()
            }}
                  </div>
                </div>
                <div class="row"
                  title="How many threads in each (logical) processors Run-Q on average. 0 means no use, 1 means ALL cores fully used with no Q, and 2 means all cores fully utilized and each core with a Q length of 1">
                  <div class="col-3">Run-Q</div>
                  <div class="col" v-if="aboutData.serverInfo.currentMachine.runQLength != null">
                    {{
            aboutData.serverInfo.currentMachine.runQLength?.toFixed(2) || "?"
          }}
                    threads
                  </div>
                </div>
                <div class="row" title="Average CPU usage for the last 30 seconds for the entire machine hosting the service.
Units 1=1 logical core">
                  <div class="col-3">CPU-Usage</div>
                  <div class="col" v-if="aboutData.serverInfo.currentMachine.totalCPUUsage != null">
                    {{
                    aboutData.serverInfo.currentMachine.totalCPUUsage?.toFixed(2) || "?"
                    }}
                    CPUs
                  </div>
                </div>
              </div>
            </div>
          </div>
        </q-card-section>
      </q-card>

      <!--Written by-->
      <q-card class="pageCard col-11">
        <q-card-section>
          <div class="row">
            <div class="col-3 text-h6">Written by</div>
            <div class="col-9">
              <div class="row">
                <div class="col-4">Lewis G. Pringle, Jr.</div>
                <div class="col">
                  <a href="https://www.linkedin.com/in/lewispringle/" target="_new">LinkedIn</a>
                  |
                  <a href="https://github.com/LewisPringle" target="_new">GitHub</a>
                </div>
              </div>
            </div>
          </div>
        </q-card-section>
      </q-card>

      <!--Report issues at-->
      <q-card class="pageCard q-mt-md col-11">
        <q-card-section>
          <div class="row">
            <div class="col-3 text-h6">Report issues at</div>
            <div class="col-9">
              <a href="https://github.com/SophistSolutions/WhyTheFuckIsMyNetworkSoSlow/issues" target="_new">github
                issues</a>
            </div>
          </div>
        </q-card-section>
      </q-card>
    </div>
  </q-page>
</template>

<style lang="scss" scoped>
.text-h6 {
  font-size: 11pt;
  line-height: 15pt;
}

.pageCard {
  margin-bottom: 1.2em;
  max-width: 750px;
}

.q-card__section {
  padding-top: 8px;
  padding-bottom: 8px;
}
</style>
