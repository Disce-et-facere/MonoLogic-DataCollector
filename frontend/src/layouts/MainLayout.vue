<template>
  <q-layout view="lHh Lpr lFf">
    <HeaderContent />
    <q-page-container>
      <router-view />
    </q-page-container>
  </q-layout>
</template>

<script setup lang="ts">
import HeaderContent from 'src/components/HeaderContent.vue';

defineOptions({
  name: 'MainLayout'
});
</script>
