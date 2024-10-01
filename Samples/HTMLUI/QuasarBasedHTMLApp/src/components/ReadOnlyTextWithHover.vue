<script setup lang="ts">
import { onMounted, watch, ref, Ref } from 'vue';

const props = defineProps({
  message: { type: String, required: true },
  link: { type: String, required: false, default: null },
  popupTitle: { type: String, required: false, default: null },
});

function onChange_() {
  useTitle.value = props.popupTitle == null ? props.message : props.popupTitle;
  if (useTitle.value === null) {
    useTitle.value = '';
  }
}

onMounted(() => {
  onChange_();
});

watch([() => props.message, () => props.popupTitle], onChange_);

var useTitle: Ref<string> = ref('');
</script>

<template>
  <span>
    <span v-if="!link" :title="useTitle">{{ message }}</span>
    <a v-if="link" :href="link" :title="useTitle">{{ message }}</a>
  </span>
</template>
