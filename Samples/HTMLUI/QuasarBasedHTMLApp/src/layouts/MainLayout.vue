<script setup lang="ts">
import { ref, onMounted } from 'vue';

import { useRoute, useRouter } from 'vue-router';

import EssentialLink from 'components/EssentialLink.vue';
import ToolbarBreadcrumbs from 'components/ToolbarBreadcrumbs.vue';
import { useConfigurationStore } from '../stores/Configuration-Store';

const route = useRoute();
const router = useRouter();
const configurationStore = useConfigurationStore();

// Leave around stuff from sample layout, for a bit...
const linksList = [
  {
    title: 'Home',
    caption: 'Overview',
    icon: 'home',
    link: '/#/',
  },
  {
    title: 'About',
    caption: 'Component versions, and server stats',
    icon: 'api',
    link: '/#/about',
  },
];

// const leftDrawerOpen = ref(false);
const leftDrawerOpen = ref(configurationStore.getLeftDrawerOpen);

function toggleLeftDrawer() {
  leftDrawerOpen.value = !leftDrawerOpen.value;
  configurationStore.setLeftDrawerOpen(leftDrawerOpen.value);
}

const defaultContextMenu = [
  { name: 'No special operations on this page', enabled: false },
];
var breadcrumbs = ref([]);
var contextMenu = ref(defaultContextMenu);

function updateBreadcrumbs(newBreadCrumbs: any) {
  // explicit EVENTs can be used to update the breadcrumbs (like when they contain dynamic per page information)
  breadcrumbs.value = newBreadCrumbs;
}

function updateContextMenu(newContextMenu: any) {
  contextMenu.value = newContextMenu;
}

router.afterEach((to, from) => {
  // each time we nagivate, update the breadcrumbs
  breadcrumbs.value = to.meta.breadcrumbs;
  contextMenu.value = to.meta.contextMenu ?? defaultContextMenu;
});

onMounted(() => {
  // needed to capture the initial breadcrumbs value
  breadcrumbs.value = route.meta.breadcrumbs;
  contextMenu.value = route.meta.contextMenu ?? defaultContextMenu;
});

function phred() {
  console.log('called phred');
}
</script>

<template>
  <q-layout view="lHh Lpr lFf">
    <q-header elevated>
      <q-toolbar class="justify-between">
        <q-btn flat dense round icon="menu" aria-label="Menu" @click="toggleLeftDrawer" />
        <q-toolbar-title> Stroika Sample: HTMLUI - QuasarBasedHTML </q-toolbar-title>
        <ToolbarBreadcrumbs v-model:breadcrumbs="breadcrumbs" />
        <q-btn flat dense round icon="mdi-dots-vertical" style="margin-left: 1in" aria-label="Menu" color="white">
          <q-menu>
            <q-list style="min-width: 100px">
              <template v-for="(item, index) in contextMenu" :key="index">
                <q-item v-if="item.name" clickable v-close-popup :active="item.enabled" :onClick="item.onClick">
                  <q-item-section> {{ item.name }}</q-item-section>
                </q-item>
                <q-separator v-if="item?.dividerAfter" />
              </template>
            </q-list>
          </q-menu>
        </q-btn>
      </q-toolbar>
      <div class="row" id="CHILD_HEADER_SECTION" />
    </q-header>

    <q-drawer v-model="leftDrawerOpen" bordered>
      <q-list>
        <q-item-label header>UI Objects</q-item-label>
        <EssentialLink v-for="link in linksList" :key="link.title" v-bind="link" />
      </q-list>
    </q-drawer>

    <q-page-container>
      <router-view @update:breadcrumbs="updateBreadcrumbs" @update:contextMenu="updateContextMenu" />
    </q-page-container>
  </q-layout>
</template>
