import { RouteRecordRaw } from 'vue-router';

const routes: RouteRecordRaw[] = [
  {
    path: '/',
    name: 'Home',
    component: () => import('layouts/MainLayout.vue'),
    children: [{ path: '/', component: () => import('pages/HomePage.vue') }],
    meta: {
      breadcrumbs: [{ text: 'Home', exact: true, disabled: true }],
      dividerAfter: true,
      showInDotDotDotMenu: true,
    },
  },
  {
    path: '/about',
    name: 'About',
    component: () => import('layouts/MainLayout.vue'),
    children: [
      { path: '/about', component: () => import('pages/AboutPage.vue') },
    ],
    meta: {
      breadcrumbs: [
        { text: 'Home', href: '/' },
        { text: 'About', disabled: true },
      ],
      showInDotDotDotMenu: true,
    },
  },
  {
    path: '/authtest',
    name: 'Auth-Test',
    component: () => import('layouts/MainLayout.vue'),
    children: [
      { path: '/authtest', component: () => import('pages/AuthTestPage.vue') },
    ],
    meta: {
      breadcrumbs: [
        { text: 'Home', href: '/#/' },
        { text: 'Auth-Test-Page', disabled: true },
      ],
      showInDotDotDotMenu: true,
    },
  },
  { path: '/oauth/callback', component: () => import('pages/OAuthCallback.vue') },
  // Always leave this as last one,
  // but you can also remove it
  {
    path: '/:catchAll(.*)*',
    component: () => import('pages/ErrorNotFound.vue'),
  },
];

export default routes;
