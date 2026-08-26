/**
 *  Types for the layout chrome - the breadcrumb trail and the toolbar context menu -
 *  and for the route 'meta' entries that supply them (see router/routes.ts).
 */

export interface IBreadcrumb {
  text: string;
  href?: string;
  exact?: boolean;
  disabled?: boolean;
}

export interface IContextMenuItem {
  name?: string;
  enabled?: boolean;
  onClick?: () => void;
  dividerAfter?: boolean;
}
