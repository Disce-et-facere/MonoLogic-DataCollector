<template>
  <div class="station-data">
    <div class="station-container-header">
      <div v-if="!multipleStations" class="latest-record">
        Latest record: {{ stationData?.temperature }} °C
      </div>
      <div class="close-btn">
        <q-btn outline color="rgb(62, 230, 121)" label="CLOSE" @click="closeStationContainer"></q-btn>
      </div>
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

interface Station {
  key: string;
  name: string;
  latitude: number;
  longitude: number;
}

interface HistoryEntry {
  date: string;
  value: number;
}

interface ApiResponse {
  value: HistoryEntry[];
}

interface SeriesData {
  name: string;
  data: Array<{ x: Date; y: number }>;
}

export default defineComponent({
  name: 'StationDataPanel',
  components: {
    apexchart: ApexCharts,
  },
  props: {
    selectedStation: {
      type: Object as PropType<Station | null>,
      default: null,
    },
    stationData: {
      type: Object as PropType<{ temperature: string } | null>,
      default: () => ({ temperature: 'N/A' }),
    },
    selectedStations: {
      type: Array as PropType<Station[]>,
      default: () => [],
    },
    multipleStations: Boolean,
  },
  emits: ['close'],
  methods: {
    closeStationContainer() {
      this.$emit('close');
    }
  },
  setup(props) {
    const series = ref<SeriesData[]>([]);
    const chartOptions = ref<ApexOptions | null>(null);
    const fetchStationHistory = async () => {
      if (!props.selectedStation || !props.selectedStation.key) return;

      try {
        const response = await axios.get<ApiResponse>(
          `https://opendata-download-metobs.smhi.se/api/version/1.0/parameter/1/station/${props.selectedStation.key}/period/latest-day/data.json`
        );

        const historyData = response.data.value.map(entry => ({
          x: new Date(entry.date),
          y: entry.value
        }));

        series.value = [{
          name: 'Temperature',
          data: historyData
        }];

        chartOptions.value = {
          chart: {
            type: 'line',
            zoom: {
              enabled: true
            },
            toolbar: {
              show: false,
            },
            events: {
              mounted: (chart) => {
                chart.updateOptions({
                  chart: {
                    height: '100%'
                  }
                });
              }
            }
          },
          colors: ['#3EE679'],
          xaxis: {
            axisBorder:{
              color: '#3EE679',
            },
            type: 'datetime',
            title: {
              text: 'Time',
              style: {
                color: '#3EE679'
              }
            },
            labels: {
              style: {
                colors: '#3EE679'
              }
            }
          },
          yaxis: {
            title: {
              text: 'Temperature (°C)',
              style: {
                color: '#3EE679'
              }
            },
            labels: {
              style: {
                colors: '#3EE679'
              }
            }
          },
          title: {
            text: props.selectedStation?.name || 'Station Temperature History',
            align: 'center',
            style: {
              color: '#3EE679',
              fontSize: '18px'
            }
          },
          grid: {
            borderColor: '#3EE679'
          },
          tooltip: {
            theme: 'dark'
          },
          stroke: {
            curve: 'smooth',
            width: 2,
            colors: ['#FF5733']
          },
        };
      } catch (error) {
        console.error('Error fetching station history data:', error);
      }
    };

    const fetchMultipleStationHistory = async () => {
      if (!props.selectedStations || props.selectedStations.length === 0) return;

      try {
        const allSeriesData: SeriesData[] = [];

        for (const station of props.selectedStations) {
          const response = await axios.get<ApiResponse>(
            `https://opendata-download-metobs.smhi.se/api/version/1.0/parameter/1/station/${station.key}/period/latest-day/data.json`
          );

          const historyData = response.data.value.map(entry => ({
            x: new Date(entry.date),
            y: entry.value
          }));

          allSeriesData.push({
            name: station.name,
            data: historyData
          });
        }

        series.value = allSeriesData;

        chartOptions.value = {
          chart: {
            type: 'line',
            zoom: {
              enabled: true
            },
            toolbar: {
              show: false,
            },
            events: {
              mounted: (chart) => {
                chart.updateOptions({
                  chart: {
                    height: '100%'
                  }
                });
              }
            }
          },
          colors: ['#E63946', '#457B9D', '#FFB703', '#8A2BE2', '#2A9D8F'],
          xaxis: {
            axisBorder:{
              color: '#3EE679',
            },
            type: 'datetime',
            title: {
              text: 'Time',
              style: {
                color: '#3EE679'
              }
            },
            labels: {
              style: {
                colors: '#3EE679'
              }
            }
          },
          yaxis: {
            title: {
              text: 'Temperature (°C)',
              style: {
                color: '#3EE679'
              }
            },
            labels: {
              style: {
                colors: '#3EE679'
              }
            }
          },
          title: {
            text: 'Cross Reference',
            align: 'center',
            style: {
              color: '#3EE679',
              fontSize: '18px'
            }
          },
          grid: {
            borderColor: '#3EE679'
          },
          tooltip: {
            theme: 'dark'
          },
          stroke: {
            curve: 'smooth',
            width: 2,
            colors: ['#E63946', '#457B9D', '#FFB703', '#8A2BE2', '#2A9D8F']
          },
          legend: {
            labels: {
              colors: '#3EE679',  // Change the color of the legend text
            },
          },
        };
      } catch (error) {
        console.error('Error fetching station history data:', error);
      }
    };

    onMounted(async () => {
      await nextTick();
      if (props.multipleStations) {
        fetchMultipleStationHistory();
      } else {
        fetchStationHistory();
      }
    });

    watch(() => props.selectedStation, () => {
      if(!props.multipleStations){
        fetchStationHistory();
      }

    });

    watch(() => props.selectedStations, () => {
      if (props.multipleStations) {
        fetchMultipleStationHistory();
      }
    });

    return {
      series,
      chartOptions,
    };
  }
});
</script>

<style scoped>
.station-data {
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

.station-container-header {
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
