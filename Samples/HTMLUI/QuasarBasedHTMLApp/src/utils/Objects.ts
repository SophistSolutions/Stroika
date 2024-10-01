/*
 *  Surprised and disapointed that this is helpful with Pinia, to avoid needless
 *  changed events.
 */
export function Equals(l: object, r: object): boolean {
  return JSON.stringify(l) == JSON.stringify(r);
}
