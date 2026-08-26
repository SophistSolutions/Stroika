<script setup lang="ts">
import { ref, Ref, ToRef, toRef, watch } from 'vue';

import { IBreadcrumb } from '@/models/INavigation';

const props = defineProps<{
  breadcrumbs: IBreadcrumb[];
}>();

// Based on code I had for Search.vue - not super logical - need to cleanup/better understand vue3 reactivity... LGP 2022-11-25
const breadcrumbsRO: ToRef<IBreadcrumb[] | undefined> = toRef(props, 'breadcrumbs'); // read-only reference to the prop, so we are notified if it changes
const breadcrumbs: Ref<IBreadcrumb[]> = ref(props.breadcrumbs || []); // writable prop, we can update with v-model

watch(breadcrumbsRO, () => {
  breadcrumbs.value = props.breadcrumbs || [];
});
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
