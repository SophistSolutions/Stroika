/*
 *  Return an array of image-url/label pairs, for the given array of device types.
 */
export function PluralizeNoun(t: string, n: number): string {
  let result = t;
  if (n == 0 || n > 1) {
    result += t.endsWith('s') ? 'es' : 's';
  }
  return result;
}
