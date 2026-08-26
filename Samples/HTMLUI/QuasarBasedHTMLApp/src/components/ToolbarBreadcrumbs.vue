<script setup lang="ts">
import { IBreadcrumb } from '@/models/INavigation';

// Props are already reactive, so the trail can be rendered straight from the prop.
// (This used to copy the prop into a local ref and keep it in sync with a watcher,
// which achieved nothing - and, being bound with v-model: while never emitting
// update:breadcrumbs, implied a two-way binding that did not exist.)
defineProps<{
  breadcrumbs: IBreadcrumb[];
}>();
</script>

<template>
  <q-breadcrumbs separator=">" active-color="secondary">
    <template v-for="(item, index) in breadcrumbs" :key="index">
      <q-breadcrumbs-el
        :to="item.href"
        :disabled="item.disabled"
        :label="item.text"
        class="breadcrumbElt"
      />
    </template>
  </q-breadcrumbs>
</template>

<style scoped lang="scss">
.breadcrumbElt {
  text-transform: uppercase;
}
</style>
