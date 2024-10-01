<script setup lang="ts">
import { ref, reactive, watchEffect, toRef, watch } from 'vue';

const props = defineProps({
  // This is the value - if initialValue/newSetValue===null, that will be used; this is just used for display so no need to set to null sometimes
  defaultValue: { type: String, required: true },
  // This is the value of the field being edited. If null, means using default value, and if not-null, must be valid string
  initialValue: { type: String, required: false },
  // Called before allowing 'userSetValue' event
  validator: { type: Function, required: false },
  validateFailedMsg: { type: String, required: false },
  thingBeingEdited: { type: String, required: true },
});

const emit = defineEmits(['update:userSetValue']);

// Call this not notify parent component of an edit/change
const updateEditValue = (newValue: string | null) => {
  emit('update:userSetValue', newValue);
};

let reactiveData: {
  defaultValue: string;
  initialValue: string | undefined;
  // if newSetUserValue undefined, it hasn't been set by user yet. If ===  null, means CLEARED TO DEFAULT
  newSetUserValue: string | undefined | null;
  // Like newSetUserValue, but live updated and not pushed to actual value until right time
  newUserSetValueUI: string | undefined | null;
} = reactive({
  defaultValue: props.defaultValue,
  initialValue: props.initialValue,
  newSetUserValue: undefined,
  newUserSetValueUI: undefined,
});

// Pointer to DOM field, to use internally in select-all UI flourish
let thePoupEdit = ref(null);
let thisInputFieldName = ref(null);

function startEdit() {
  thePoupEdit.value.show();
}

defineExpose({ startEdit });

// Forward props changes to reactiveData we use in component
watch(
  toRef(props, 'defaultValue'),
  () => (reactiveData.defaultValue = props.defaultValue || '')
);
watch(
  toRef(props, 'initialValue'),
  () => (reactiveData.initialValue = props.initialValue || '')
);

function doValidate_(v: any) {
  // console.log('enter doValidate_ v=', v, v && v.length >= 1)
  if (props.validator) {
    return props.validator(v);
  }
  return true;
}

watchEffect(() => {
  if (reactiveData.newSetUserValue === undefined) {
    // never set
    reactiveData.newUserSetValueUI = reactiveData.initialValue;
    // console.log('CHANGE: since newSetUserValue=undefined, SETTING newUserSetValueUI=', reactiveData.newUserSetValueUI)
  } else {
    reactiveData.newUserSetValueUI = reactiveData.newSetUserValue; // last value ACTUALLY set (not canceled)
    // console.log('CHANGE: since newSetUserValue DEFINED, SETTING newUserSetValueUI=', reactiveData.newUserSetValueUI)
  }
});

function updateValue_(event: any, scope: any, newValue: string | null) {
  // console.log('CHANGE: updateValue newSetUserValue BEING SET TO=', newValue)
  reactiveData.newSetUserValue = newValue;
  scope.value = newValue;
  scope.set();
}
</script>

<template>
  <q-popup-edit
    ref="thePoupEdit"
    v-model="reactiveData.newUserSetValueUI"
    v-slot="scope"
    @hide="doValidate_"
    :validate="doValidate_"
    @update:modelValue="updateEditValue"
  >
    <q-input
      autofocus
      ref="thisInputFieldName"
      dense
      v-model="scope.value"
      :hint="`Use ${props.thingBeingEdited} ('${reactiveData.defaultValue}\' is the default)`"
      :placeholder="reactiveData.defaultValue"
      :rules="[
        (val) =>
          scope.validate(val) || props.validateFailedMsg || 'Failed validation',
      ]"
      @focus="this.$refs?.thisInputFieldName.select()"
    >
      <template v-slot:after>
        <q-btn
          flat
          dense
          color="black"
          icon="cancel"
          @click.stop.prevent="scope.cancel"
          title="Make no changes"
        />
        <q-btn
          flat
          dense
          color="positive"
          icon="check_circle"
          @click.stop.prevent="updateValue_($event, scope, scope.value)"
          :disable="
            scope.validate(scope.value) === false || scope.value == null
          "
          :title="`Use this as (override) ${props.thingBeingEdited}`"
        />
        <q-btn
          flat
          dense
          color="negative"
          icon="delete"
          title="Clear override: use default"
          @click.stop.prevent="updateValue_($event, scope, null)"
        />
      </template>
    </q-input>
  </q-popup-edit>
</template>
