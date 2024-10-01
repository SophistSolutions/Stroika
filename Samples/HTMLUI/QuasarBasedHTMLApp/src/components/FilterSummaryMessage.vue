<script setup lang="ts">
import { onMounted, watch, ref } from 'vue';

const props = defineProps({
  nTotalItems: { type: Number, required: true },
  filtered: { type: Boolean, default: false },
  nItemsSelected: { type: Number, required: true },
  itemsName: { type: String, default: 'items' },
});

function onChange() {
  let filtered = props.filtered;
  if (filtered === null) {
    filtered = props.nItemsSelected === props.nTotalItems;
  }
  if (filtered) {
    msg.value = `Filtered: showing ${props.nItemsSelected} of ${props.nTotalItems} ${props.itemsName}`;
  } else {
    msg.value = `Unfiltered: all ${props.nTotalItems} ${props.itemsName} showing`;
  }
}

onMounted(() => {
  onChange();
});

watch(
  [
    () => props.filtered,
    () => props.nTotalItems,
    () => props.nItemsSelected,
    () => props.itemsName,
  ],
  onChange
);

/*
 *  This is for use in the filter section of the app-bar, to say how much is filtered out.
 */
var msg = ref('');
</script>

<template>
  <span>{{ msg }}</span>
</template>

<style scoped lang="scss">
div {
  text-align: right;
}
</style>
