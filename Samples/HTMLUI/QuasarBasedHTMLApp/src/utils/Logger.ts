/*
 * https://eslint.org/docs/rules/no-console recommends not using console directly in finished
 * products. This wrapper allows conditionally expunging console log calls (for example in production code)
 * while leaving calls in place in the source.
 */
export class Logger {
  public static debug(message?: any, ...optionalParams: any[]): void {
    if (Logger.sEmitToLog) {
      // tslint:disable-next-line
      console.debug(message, ...optionalParams);
    }
  }
  public static info(message?: any, ...optionalParams: any[]): void {
    if (Logger.sEmitToLog) {
      // tslint:disable-next-line
      console.info(message, ...optionalParams);
    }
  }
  public static warn(message?: any, ...optionalParams: any[]): void {
    if (Logger.sEmitToLog) {
      // tslint:disable-next-line
      console.warn(message, ...optionalParams);
    }
  }
  public static error(message?: any, ...optionalParams: any[]): void {
    if (Logger.sEmitToLog) {
      // tslint:disable-next-line
      console.error(message, ...optionalParams);
    }
  }

  private static sEmitToLog: boolean = true; // @todo get from Proces.env.??? as .env config var?
}
