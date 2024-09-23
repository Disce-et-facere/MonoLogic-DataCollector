<template>
  <q-header elevated>
    <q-toolbar class="bg-primary glossy">
      <q-avatar>
        <img src="../assets/logo.png" alt="Logo">
      </q-avatar>

      <q-toolbar-title class="title-text">
        SQUIDY
      </q-toolbar-title>

      <div class="slogan">Data collection → Visualized</div>
    </q-toolbar>
  </q-header>
</template>

<script setup lang="ts">
defineOptions({
  name: 'HeaderContent'
});
</script>

<style scoped>
.title-text {
  color: rgb(62, 230, 121);
}

.q-toolbar {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.q-toolbar-title {
  flex-grow: 1;
  text-align: center;
}

.slogan {
  position: absolute;
  left: 50%;
  font-variant: small-caps;
  transform: translateX(-50%);
  font-size: 14px;
  color: rgb(62, 230, 121);
}

@media (max-width: 600px) {
  .slogan{
    left: auto;  /* Reset left positioning */
    right: 10px;  /* Move it to the right side */
    transform: none;
  }
}
</style>
