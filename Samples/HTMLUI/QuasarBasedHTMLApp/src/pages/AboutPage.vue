<script setup lang="ts">
import { onMounted, onUnmounted, computed, Ref, ref } from "vue";
import { storeToRefs } from "pinia";
import { useQuasar } from "quasar";
import prettyBytes from "pretty-bytes";
import { Duration, DateTime } from 'luxon';

import { kCompileTimeConfiguration } from "src/config/config";
import { IAPIEndpoint, IWebServerStats, IComponent, IDatabase } from "src/models/IAbout";
import { useMainAppStateStore } from "src/stores/MainApp-State-store";
import { PluralizeNoun } from "src/utils/Linguistics";
import { useConfigurationStore } from "stores/Configuration-Store";
import { gRuntimeConfiguration } from "boot/configuration";
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

const { about } = storeToRefs(store);
const { lastSuccessfulAPICall } = storeToRefs(store);
const aboutData = about;

// Data / functions to show 'last successful communications'
const pageLoadedAt = new Date();
const now = ref(new Date());  // reactive now
function timeDiffInSeconds_(start: Date, end: Date) {
  return DateTime.fromJSDate(start)
    .diff(DateTime.fromJSDate(end), 'seconds')
    .seconds;
}
const lastSuccessfulAPICallMessageStyle = computed(() => {
  // show in red if we've gotten some data, but not recently
  if (lastSuccessfulAPICall.value) {
    return timeDiffInSeconds_(now.value, lastSuccessfulAPICall.value) < 30 ? "" : "color: red";
  }
  else {
    // show in red if we've never gotten data, and the page loaded a while ago
    return timeDiffInSeconds_(now.value, pageLoadedAt) < 5 ? "" : "color: red";
  }
})
function mySince_(agoDate: Date, nowDate: Date) {
  if (Math.abs(timeDiffInSeconds_(nowDate, agoDate)) < kRefreshFrequencyInSeconds_) {
    return "now";
  }
  return DateTime.fromJSDate(agoDate).toRelative({ base: DateTime.fromJSDate(nowDate), round: true });
}
const lastSuccessfulAPICallMessage = computed(() => {
  return lastSuccessfulAPICall.value ?
    mySince_(lastSuccessfulAPICall.value, now.value) : "no data received yet";
})


const configurationStore = useConfigurationStore();


onMounted(() => {
  // first time check quickly, then more gradually
  store.fetchAboutInfo();
  if (polling) {
    clearInterval(polling);
  }
  polling = setInterval(() => {
    now.value = new Date(); // keep updating reactive now date, so lastSuccessfulAPICallMessage changes
    lastSuccessfulAPICall.value = lastSuccessfulAPICall.value;  // hack to force refresh (not working)
    store.fetchAboutInfo();
  }, kRefreshFrequencyInSeconds_ * 1000);
});
onUnmounted(() => {
  clearInterval(polling);
});

function prettyPrintMSDuration(time?: string) {
  if (time == undefined) {
    return "?";
  }
  return Duration.fromISO(time).toHuman({ unitDisplay: 'narrow', showZeros: false });
}
function wsAPIMsg(info: IAPIEndpoint, showShort: boolean): string {
  let msg = "";
  msg += `${info.callsCompleted} calls completed; `;
  msg += `${info.medianRunningAPITasks} running tasks; `;
  msg += `${info.errors} ${PluralizeNoun("error", info.errors)}; `;
  msg += `times: ${prettyPrintMSDuration(info.callTimes.median)}, max ${prettyPrintMSDuration(
    info.callTimes.max
  )}`;
  return msg;
}
function webServerMsg_(info: IWebServerStats): string {
  let msg = "";
  msg += `threadPool: {size: ${info.threadPool.threads}, queued: ${info.threadPool.tasksStillQueued
    }, aveRunTime: ${prettyPrintMSDuration(info.threadPool.averageTaskRunTime)}}\n`;
  msg += `connections: {open: ${info.connections.open}, active: ${info.connections.active
    }, openLifetime: ${prettyPrintMSDuration(
      info.connections.openConnectionsLifetime.median
    )}, openRequestsLifetime: ${prettyPrintMSDuration(
      info.connections.openConnectionsRequests.median
    )}, activeRequestsLifetime: ${prettyPrintMSDuration(
      info.connections.activeConnectionsRequests.median
    )}}`;
  if (info.connections.piningForTheFjords != 0) {
    msg += `piningForTheFjords: ${info.connections.piningForTheFjords},`;
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
    msg += `${prettyPrintMSDuration(info.medianReadDuration)} reads, ${prettyPrintMSDuration(
      info.medianWriteDuration
    )} writes`;
  } else {
    msg += `Med ${prettyPrintMSDuration(
      info.medianReadDuration
    )} read duration, Med ${prettyPrintMSDuration(info.medianWriteDuration)} write duration`;
  }
  if (showShort) {
    if (info.maxDuration != undefined) {
      msg += `; max ${prettyPrintMSDuration(info.maxDuration)}`;
    }
  } else {
    if (info.maxDuration != undefined) {
      msg += `; max ${prettyPrintMSDuration(info.maxDuration)} I/O duration`;
    }
  }
  return msg;
}
</script>

<template>
  <q-page class="q-pa-md">
    <div class="row text-h5">
      <div class="col">About 'Sample HTMLUI App'</div>
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
          <div class="row">
            Web Services URL: <b>{{ gRuntimeConfiguration.API_ROOT }}</b>
          </div>
          <div class="row" style="margin-left: 2em;">
            Last succesful communication:&nbsp; <span :style="lastSuccessfulAPICallMessageStyle">{{
              lastSuccessfulAPICallMessage }}</span>
          </div>
          <div class="row" style="margin-left: 2em">
            API (Web Service) Server Docs:
            <a :href="gRuntimeConfiguration.API_ROOT + '/api'" target="_new">{{ gRuntimeConfiguration.API_ROOT
            }}/api</a>
          </div>
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
                <div class="col-4" v-if="
                  aboutData.serverInfo.currentProcess.combinedIOReadRate != undefined &&
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
                    Duration.fromISO(aboutData.serverInfo?.currentProcess?.processUptime)
                      .toHuman()
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
                  title="Information about app WebServer Stats (median #connections, timing, Q-lengths) over the last 5 minutes">
                  WebServer
                </div>
                <div class="col" v-if="aboutData.serverInfo.webServer"
                  :title="webServerMsg_(aboutData.serverInfo.webServer)">
                  {{ webServerMsg_(aboutData.serverInfo.webServer) }}
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
                      Duration.fromISO(aboutData.serverInfo.currentMachine.machineUptime)
                        .toHuman()
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
