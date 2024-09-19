<template>
  <div class="loading-screen">
    <div class="spinner"></div>
    <div class="loading-text">Fetching Data...</div>
  </div>
</template>

<style scoped>
.loading-screen {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-color: #000011; /* Slightly transparent white background */
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  z-index: 10;
}

.spinner {
  border: 8px solid rgba(0, 0, 0, 0.1);
  border-left: 8px solid #3498db;
  border-radius: 50%;
  width: 50px;
  height: 50px;
  animation: spin 1s linear infinite;
}

.loading-text {
  margin-top: 10px;
  font-size: 18px;
  font-weight: bold;
  color:white;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}
</style>
