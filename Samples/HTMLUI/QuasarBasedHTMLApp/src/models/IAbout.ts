import { OperatingSystem } from 'src/models/OperatingSystem';

export interface IComponent {
  URL?: string;
  name: string;
  version: string;
}
export interface ICurrentMachine {
  operatingSystem: OperatingSystem;
  machineUptime?: string;
  runQLength?: number;
  totalCPUUsage?: number;
}
 interface ICommonStatistics {
  min?: string;
  max?: string;
  mean?: string;
  median?: string;
  stddev?: string;
}
// stats over the reporting period (I think currently 15 seconds)
export interface IAPIEndpoint {
  callsCompleted: number;
  errors: number;
  // duration of each of the different calls
  callTimes?: ICommonStatistics;
  medianRunningAPITasks?: number;
}
export interface IWebServerStats {
  threadPool: {threads: number, tasksStillQueued: number, averageTaskRunTime?: string;}
  connections: {open: number; active: number; openConnectionsLifetime: ICommonStatistics, openConnectionsRequests: ICommonStatistics, activeConnectionsRequests: ICommonStatistics, piningForTheFjords: number}
}
export interface IDatabase {
  errors?: number;
  fileSize?: number;
  maxDuration?: string;
  meanReadDuration?: string;
  meanWriteDuration?: string;
  medianReadDuration?: string;
  medianWriteDuration?: string;
  reads?: number;
  writes?: number;
}
export interface ICurrentProcess {
  averageCPUTimeUsed?: number;
  combinedIOReadRate?: number;
  combinedIOWriteRate?: number;
  processUptime?: string;
  workingOrResidentSetSize?: number;
}
export interface IServerInfo {
  apiEndpoint?: IAPIEndpoint;
  webServer?:IWebServerStats;
  componentVersions: IComponent[];
  currentMachine: ICurrentMachine;
  currentProcess: ICurrentProcess;
  database: IDatabase;
}
export interface IAbout {
  applicationVersion: string;
  serverInfo: IServerInfo;
}
