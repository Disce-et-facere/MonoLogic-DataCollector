<template>
  <div class="device-data">
    <div class="device-container-header">
      <div class="close-btn">
        <q-btn outline color="rgb(62, 230, 121)" label="CLOSE" @click="closeDeviceContainer"></q-btn>
      </div>
      <q-toggle
        v-if="multipleDevices"
        v-model="displayType"
        label="Display as:"
        :labels="['Humidity', 'Temperature']"
        class="toggle-display"
        @change="updateChartOptions"
      />
    </div>
    <div class="chart-container">
      <apexchart v-if="chartOptions" :options="chartOptions" :series="series" />
    </div>
  </div>
</template>

<script lang="ts">
import { defineComponent, onMounted, ref, watch, nextTick, PropType } from 'vue';
import ApexCharts from 'vue3-apexcharts';
import axios from 'axios';
import { ApexOptions } from 'apexcharts';

interface Device {
  id: number;
  mac: string;
  name: string;
  temperature: number;
  humidity: number;
  timestamp: string;
  latitude: number;
  longitude: number;
}

interface HistoryEntry {
  timestamp: string;
  temperature: number;
  humidity: number;
}

interface ApiResponse {
  value: HistoryEntry[];
}

interface SeriesData {
  name: string;
  type: 'line' | 'bar';
  data: Array<{ x: Date; y: number }>;
}

export default defineComponent({
  name: 'DeviceDataPanel',
  components: {
    apexchart: ApexCharts,
  },
  props: {
    selectedDevice: {
      type: Object as PropType<Device | null>,
      default: null,
    },
    selectedDevices: {
      type: Array as PropType<Device[]>,
      default: () => [],
    },
    multipleDevices: Boolean,
  },
  emits: ['close'],
  methods: {
    closeDeviceContainer() {
      this.$emit('close');
    }
  },
  setup(props) {
    const series = ref<SeriesData[]>([]);
    const chartOptions = ref<ApexOptions | null>(null);
    const displayType = ref('temperature');

    const fetchDeviceHistory = async () => {
      if (!props.selectedDevice || !props.selectedDevice.mac) return;

      try {
        const response = await axios.get<ApiResponse>(
          'http://localhost:8080/api/iot-device/${props.selectedDevice.mac}'
        );

        const temperatureData = response.data.value.map(entry => ({
          x: new Date(entry.timestamp),
          y: entry.temperature,
        }));

        const humidityData = response.data.value.map(entry => ({
          x: new Date(entry.timestamp),
          y: entry.humidity,
        }));

        series.value = [
          {
            name: 'Temperature',
            type: 'line',
            data: temperatureData,
          },
          {
            name: 'Humidity',
            type: 'bar',
            data: humidityData,
          },
        ];

        chartOptions.value = {
          chart: {
            zoom: { enabled: true },
            toolbar: { show: false },
            events: {
              mounted: (chart) => {
                chart.updateOptions({
                  chart: {
                    height: '100%',
                  },
                });
              },
            },
          },
          colors: ['#f73207', '#3EE679'],
          xaxis: {
            type: 'datetime',
            title: {
              text: 'Time',
              style: {
                color: '#3EE679',
              },
            },
            labels: {
              style: {
                colors: '#3EE679',
              },
            },
          },
          yaxis: [
            {
              title: {
                text: 'Temperature (°C)',
                style: {
                  color: '#f73207',
                },
              },
              labels: {
                style: {
                  colors: '#f73207',
                },
              },
            },
            {
              opposite: true,
              title: {
                text: 'Humidity (%)',
                style: {
                  color: '#3EE679',
                },
              },
              labels: {
                style: {
                  colors: '#3EE679',
                },
              },
            },
          ],
          title: {
            text: props.selectedDevice?.name || 'Device Temperature History',
            align: 'center',
            style: {
              color: '#3EE679',
              fontSize: '18px',
            },
          },
          grid: {
            borderColor: '#3EE679',
          },
          tooltip: {
            theme: 'dark',
          },
          stroke: {
            curve: 'smooth',
            width: 2,
          },
          legend: {
            labels: {
              colors: '#3EE679',  // Change the color of the legend text
            },
          },
          plotOptions: {
            bar: {
              columnWidth: '5%',
            }
          },
        };
      } catch (error) {
        console.error('Error fetching station history data:', error);
      }
    };

    const fetchMultipleDeviceHistory = async () => {
      if (!props.selectedDevices || props.selectedDevices.length === 0) return;

      try {
        const temperatureSeriesData: SeriesData[] = [];
        const humiditySeriesData: SeriesData[] = [];

        for (const device of props.selectedDevices) {
          const response = await axios.get<ApiResponse>(
            `http://localhost:8080/api/iot-device/${device.mac}`
          );

          const temperatureData = response.data.value.map(entry => ({
            x: new Date(entry.timestamp),
            y: entry.temperature,
          }));

          const humidityData = response.data.value.map(entry => ({
            x: new Date(entry.timestamp),
            y: entry.humidity,
          }));

          temperatureSeriesData.push({
            name: `${device.name} Temperature`,
            type: 'line',
            data: temperatureData,
          });

          humiditySeriesData.push({
            name: `${device.name} Humidity`,
            type: 'bar',
            data: humidityData,
          });
        }

        // Initially show both temperature and humidity for multiple devices
        series.value = [...temperatureSeriesData, ...humiditySeriesData];

        updateChartOptions(); // Initialize chart options based on the default display type
      } catch (error) {
        console.error('Error fetching multiple device history data:', error);
      }
    };

    const updateChartOptions = () => {
      const isHumidity = displayType.value === 'humidity';

      // Filter the series based on the display type (humidity or temperature)
      series.value = series.value.filter(s =>
        isHumidity ? s.name.includes('Humidity') : s.name.includes('Temperature')
      );

      chartOptions.value = {
        chart: {
          zoom: { enabled: true },
          toolbar: { show: false },
        },
        colors: isHumidity ? ['#3EE679'] : ['#f73207'],
        xaxis: {
          type: 'datetime',
          title: {
            text: 'Time',
          },
        },
        yaxis: {
          title: {
            text: isHumidity ? 'Humidity (%)' : 'Temperature (°C)',
          },
        },
        stroke: {
          curve: 'smooth',
        },
      };
    };
    onMounted(async () => {
      await nextTick();
      if (props.multipleDevices) {
        fetchMultipleDeviceHistory();
      } else {
        fetchDeviceHistory();
      }
    });

    watch(() => props.selectedDevice, () => {
      if(!props.multipleDevices){
        fetchDeviceHistory();
      }

    });

    watch(() => props.selectedDevices, () => {
      if (props.multipleDevices) {
        fetchMultipleDeviceHistory();
      }
    });

    return {
      series,
      chartOptions,
      updateChartOptions,
      displayType,
    };
  }
});
</script>

<style scoped>
.device-data {
  display:relative;
  display: flex;
  flex-direction: column;
  width: 90vw;
  max-width: 1000px;
  height: 80vh;
  border-radius: 16px;
  color: rgb(62, 230, 121);
  font-variant: small-caps;
  background: rgba(12, 25, 38, 0.56);
  box-shadow: 0 4px 30px rgba(0, 0, 0, 0.1);
  backdrop-filter: blur(3px);
  -webkit-backdrop-filter: blur(3px);
  border: 1px solid rgba(41, 86, 130, 0.23);
  padding: 20px;
  overflow: hidden;
}

.device-container-header {
  position: relative;
  display: flex;
  flex-direction: column;
  height: 80px;
  z-index: 10; /* Ensure the header stays above the chart */
}

.latest-record {
  position: absolute;
  display: flex;
  justify-content: center;
  align-items: center;
  top: 22px;
  left: 15px;
  font-size: medium;
}

.station-name {
  position: absolute;
  display: flex;
  justify-content: center;
  align-items: center;
  top: 15px;
  left: 50%;
  font-size: x-large;
  transform: translateX(-50%);
}

.close-btn {
  position: absolute;
  display: flex;
  justify-content: center;
  align-items: center;
  top: 15px;
  right: 15px;
}

.chart-container {
  width: 100%;
  height: calc(100%);
  position: relative;
  overflow: hidden;
}

.apexcharts-canvas {
  position: absolute;
  top: 0;
  left: 0;
  width: 100% !important;
  height: 100% !important;
}

/* Responsive layout for smaller screens */
@media (max-width: 600px) {
  .station-data {
    max-height: 100%; /* Adjust for smaller screens */
  }

  .station-container-header {
    height: 60px; /* Reduce header height */
  }

  .chart-container {
    height: calc(100% - 60px); /* Adjust chart height accordingly */
  }

  .latest-record {
    top:5px;
  }

  .close-btn {
    top:0px;
  }
}
</style>
