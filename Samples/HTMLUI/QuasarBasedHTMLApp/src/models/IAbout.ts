import { OperatingSystem } from '../models/OperatingSystem';

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
export interface IAPIEndpoint {
  callsCompleted: number;
  errors: number;
  maxDuration?: string;
  meanDuration?: string;
  medianDuration?: string;
  medianProcessingWebServerConnections?: number;
  medianRunningAPITasks?: number;
  medianWebServerConnections?: number;
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
  componentVersions: IComponent[];
  currentMachine: ICurrentMachine;
  currentProcess: ICurrentProcess;
  database: IDatabase;
}
export interface IAbout {
  applicationVersion: string;
  serverInfo: IServerInfo;
}
