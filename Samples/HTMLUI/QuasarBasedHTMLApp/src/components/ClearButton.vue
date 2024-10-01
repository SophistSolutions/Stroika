<script setup lang="ts">
const emit = defineEmits(['click']);

const onClick = () => {
  emit('click');
};
</script>

<template>
  <q-btn fab-mini dense dark size="sm" @click="onClick">
    <q-icon dense dark size="xs" name="close" />
  </q-btn>
</template>

<style scoped lang="scss">
q-btn {
  float: right;
}
</style>
