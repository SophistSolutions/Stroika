<script setup lang="ts">
import { ref, Ref, ToRef, toRef, watch } from 'vue';

const props = defineProps({
  searchFor: { type: String, required: false },
});

const emit = defineEmits(['update:searchFor']);

var searchRO: ToRef<string | undefined> = toRef(props, 'searchFor'); // read-only reference to the prop, so we are notified if it changes
let search: Ref<string> = ref(props.searchFor || ''); // writable prop, we can update with v-model

watch(searchRO, () => (search.value = props.searchFor || ''));

const updateSearchFor = (newValue: string | number | null) => {
  emit('update:searchFor', newValue);
};
</script>

<template>
  <div class="row no-wrap items-baseline">
    <q-icon name="mdi-magnify" :left="true" />
    <q-input
      dense
      dark
      v-model="search"
      label="Search"
      @update:modelValue="updateSearchFor"
    />
  </div>
</template>
