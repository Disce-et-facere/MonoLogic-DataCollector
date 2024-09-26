<template>
  <div class="device-data">
    <div class="device-container-header">
      <div class="close-btn">
        <q-btn
          outline
          color="rgb(62, 230, 121)"
          label="CLOSE"
          @click="closeDeviceContainer"
        ></q-btn>
      </div>
    </div>
    <div class="chart-container">
      <apexchart v-if="chartOptions" :options="chartOptions" :series="series" />
    </div>
  </div>
</template>

<script lang="ts">
import {
  defineComponent,
  onMounted,
  ref,
  watch,
  nextTick,
  PropType,
} from 'vue';
import ApexCharts from 'vue3-apexcharts';
import { ApexOptions } from 'apexcharts';

interface Device {
  mac: string;
  name: string;
  temperature: number[];
  humidity: number[];
  timestamp: string[];
  latitude: number;
  longitude: number;
}

interface SeriesData {
  name: string;
  type: 'line' | 'bar' | 'area' | 'column';
  data: Array<{ x: Date; y: number }>;
}

export default defineComponent({
  name: 'DeviceDataPanel',
  components: {
    apexchart: ApexCharts,
  },
  props: {
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
    },
  },
  setup(props) {
    const series = ref<SeriesData[]>([]);
    const chartOptions = ref<ApexOptions | null>(null);
    const displayType = ref('temperature');

    interface DataPoint {
        x: Date;
        y: number;
    }

    const temperatureData: DataPoint[] = [];
    const humidityData: DataPoint[] = [];

    const fetchDeviceHistory = () => {
        if (!props.selectedDevices || props.selectedDevices.length === 0) return;

        temperatureData.length = 0;
        humidityData.length = 0;

        props.selectedDevices.forEach((device: Device) => {
            const length = device.timestamp.length;
            for (let index = 0; index < length; index++) {
                temperatureData.push({
                    x: new Date(device.timestamp[index]),
                    y: device.temperature[index],
                });

                humidityData.push({
                    x: new Date(device.timestamp[index]),
                    y: device.humidity[index],
                });
            }
        });

        series.value = [
            {
                name: 'Temperature',
                type: 'bar',
                data: temperatureData,
            },
            {
                name: 'Humidity',
                type: 'area',
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
          text: props.selectedDevices[0].name || 'Device Temperature History',
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
            colors: '#3EE679',
          },
        },
        plotOptions: {
          bar: {
            columnWidth: '15%',
          },
        },
      };
    };

    const fetchMultipleDeviceHistory = () => {
      if (!props.selectedDevices || props.selectedDevices.length === 0) return;

      const temperatureSeriesData: SeriesData[] = [];
      const humiditySeriesData: SeriesData[] = [];

      props.selectedDevices.forEach((device) => {
          const temperatureData: DataPoint[] = device.temperature.map((temp, index) => ({
              x: new Date(device.timestamp[index]),
              y: temp,
          }));

          const humidityData: DataPoint[] = device.humidity.map((hum, index) => ({
              x: new Date(device.timestamp[index]),
              y: hum,
          }));

          temperatureSeriesData.push({
              name: `${device.name} Temperature`,
              type: 'bar',
              data: temperatureData,
          });

          humiditySeriesData.push({
              name: `${device.name} Humidity`,
              type: 'area',
              data: humidityData,
          });
      });

      series.value = [...temperatureSeriesData, ...humiditySeriesData];
      updateChartOptions();
  };

    const updateChartOptions = () => {
      const isDisplayingHumidity = displayType.value === 'humidity';

      const filteredSeries = isDisplayingHumidity
          ? series.value.filter((s) => s.name.includes('Humidity'))
          : series.value.filter((s) => s.name.includes('Temperature'));

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
          colors: ['#f70f0f', '#e80c94', '#3EE679', '#58cc41'],
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
              text: 'Temp / Humid',
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
              width: 1,
              colors: ['#f70f0f', '#e80c94', '#3EE679', '#58cc41'],
          },
            fill: {
            opacity: [0.85, 0.90, 1],
            gradient: {
              inverseColors: true,
              shade: 'light',
              opacityFrom: 0.85,
              opacityTo: 0.55,
              stops: [0, 100, 100, 100]
            }
          },
          legend: {
              labels: {
                  colors: '#3EE679',
              },
          },
          plotOptions: {
              bar: {
                  columnWidth: '20%',
              },
          },
          series: filteredSeries,
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

    watch(
      () => props.selectedDevices,
      () => {
        if (!props.multipleDevices) {
          fetchDeviceHistory();
        }
      }
    );

    watch(
      () => props.selectedDevices,
      () => {
        if (props.multipleDevices) {
          fetchMultipleDeviceHistory();
        }
      }
    );

    return {
      series,
      chartOptions,
      updateChartOptions,
      displayType,
    };
  },
});
</script>

<style scoped>
.device-data {
  display: relative;
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

.device-name {
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
  .device-data {
    max-height: 100%; /* Adjust for smaller screens */
  }

  .device-container-header {
    height: 60px; /* Reduce header height */
  }

  .chart-container {
    height: calc(100% - 60px); /* Adjust chart height accordingly */
  }

  .latest-record {
    top: 5px;
  }

  .close-btn {
    top: 0px;
  }
}
</style>
