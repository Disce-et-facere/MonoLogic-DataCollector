<template>
  <div class="page-container">
    <div class="controls-container">
      <!-- Data Stations Button -->
      <div class="data-stations-btn">
        <q-btn color="primary" label="Data STATIONS" style="width: 240px;">
          <q-menu fit auto-close class="bg-transparent">
            <q-list color="primary" class="station-list">
              <q-item color="primary" v-for="station in filteredStations" :key="station.key" clickable @click="focusOnStation(station)">
                <q-item-label>{{ station.name }}</q-item-label>
            </q-item>
            </q-list>
          </q-menu>
        </q-btn>
      </div>
      <div class="select-stations-btn" ref="stationSearchDropdown">
        <q-btn-group>
          <q-btn color="primary" label="select stations to" @click="closeContainer">
            <q-menu anchor="bottom left" class="bg-transparent">
              <q-input
                v-model="searchText"
                placeholder="Search Stations"
                class="q-mt-sm searchbar"
                input-class="input-text"
                color="green"
                persist
                sticky
              />
              <q-list color="primary" separator class="select-stations-list">
                <q-item
                  v-for="station in filteredStations"
                  :key="station.key"
                  clickable
                  @click="selectStation(station)"
                >

            <q-item-label>{{ station.name }}</q-item-label>
          </q-item>
              </q-list>
            </q-menu>
          </q-btn>
          <q-btn label="X - REF" color="secondary" @click="crossReferenceStations"></q-btn>
        </q-btn-group>
      </div>
    </div>

    <div v-if="selectedStations.length > 0 && !multipleStations" class="q-mt-sm station-tags">
      <q-chip
        v-for="station in selectedStations"
        :key="station.key"
        removable
        @remove="removeStation(station.key)"
      >
        {{ station.name }}
      </q-chip>
    </div>

    <LoadingScreen v-if="loading" />
    <div ref="globeContainer" class="globe-container" v-show="!loading"></div>

    <div v-if="selectedStation && stationData || selectedStations && multipleStations" class="station-container">
      <StationDataPanel
        :selected-station="selectedStation"
        :station-data="stationData"
        :selected-stations="selectedStations"
        :multipleStations
        @close="closeContainer"
      />
    </div>
  </div>
</template>

<script lang="ts">
import { defineComponent, ref, computed, onMounted, onBeforeUnmount } from 'vue';
import Globe from 'globe.gl';
import LoadingScreen from '../components/LoadingScreen.vue';
import StationDataPanel from '../components/StationDataPanel.vue';
import earthTexture from 'src/assets/earth-high-res.jpg';
import axios from 'axios';

interface Station {
  key: string;
  name: string;
  latitude: number;
  longitude: number;
}

export default defineComponent({
  components: {
    LoadingScreen,
    StationDataPanel,
  },
  setup() {
    const globeContainer = ref<HTMLElement | null>(null);
    const globeInstance = ref<ReturnType<typeof Globe> | null>(null);
    const loading = ref(true);
    const searchText = ref('');
    const stations = ref<Station[]>([]);
    const selectedStations = ref<Station[]>([]);
    const selectedStation = ref<Station | null>(null);
    const stationData = ref<{ temperature: string } | null>(null);
    const multipleStations = ref(false);

    const filteredStations = computed(() => {
      return stations.value.filter(station =>
        station.name.toLowerCase().includes(searchText.value.toLowerCase())
      );
    });

    const fetchStations = async () => {
      try {
        const response = await axios.get('https://opendata-download-metobs.smhi.se/api/version/1.0/parameter/1/station-set/all/period/latest-hour/data.json');
        stations.value = response.data.station;
        updateGlobePoints();
      } catch (error) {
        console.error('Error fetching stations:', error);
      }
    };

    const fetchTemperatureData = async (stationKey: string) => {
      try {
        const response = await axios.get('https://opendata-download-metobs.smhi.se/api/version/1.0/parameter/1/station-set/all/period/latest-hour/data.json');
        const station = response.data.station.find((s: Station) => s.key === stationKey);
        if (station) {
          const latestValue = station.value?.length ? station.value[station.value.length - 1] : null;
          stationData.value = latestValue ? { temperature: latestValue.value } : { temperature: 'N/A' };
        } else {
          stationData.value = { temperature: 'N/A' };
        }
      } catch (error) {
        console.error('Error fetching data for station', stationKey, error);
        stationData.value = { temperature: 'N/A' };
      }
    };

    const focusOnStation = (station: Station) => {
      if (globeInstance.value) {
        globeInstance.value.pointOfView({ lat: station.latitude, lng: station.longitude, altitude: 0.1 }, 2000);
        selectedStation.value = station;
        updateGlobePoints();
        fetchTemperatureData(station.key);
      }
    };

    const removeStation = (stationKey: string) => {
      selectedStations.value = selectedStations.value.filter(s => s.key !== stationKey);
    };


    const crossReferenceStations = () => {
      if (selectedStations.value && selectedStations.value.length > 1) {
        multipleStations.value = true;
        searchText.value = '';
        if(globeInstance.value){
          globeInstance.value.pointOfView({ lat: 62.1282, lng: 13.6435, altitude: 0.3 }, 2000);
        }
      } else {
        multipleStations.value = false;
      }
    };

    const selectStation = (station: Station) => {
      if (!selectedStations.value.find(s => s.key === station.key)) {
        selectedStations.value.push(station);
        updateGlobePoints();
      }
    };

    const handleResize = () => {
      if (globeInstance.value) {
        globeInstance.value.width(window.innerWidth);
        globeInstance.value.height(window.innerHeight);
      }
    };

    const updateGlobePoints = () => {
      if (globeInstance.value) {
        globeInstance.value.pointsData(stations.value.map(station => {
          // Check if the station is in the selectedStations array
          const isInSelectedStations = selectedStations.value?.some(selected => selected.key === station.key);
          // Check if it's the single selectedStation
          const isSelectedStation = selectedStation.value?.key === station.key;

          return {
            lat: station.latitude,
            lng: station.longitude,
            size: 0,
            radius: 0.05,
            // Color based on whether it's in selectedStations or the selectedStation
            color: isInSelectedStations ? 'green' : (isSelectedStation ? 'green' : 'red'),
            label: station.name,
            name: station.name
          };
        }));
      }
    };

    const closeContainer = () => {
      selectedStation.value = null;
      stationData.value = null;
      selectedStations.value = [];
      multipleStations.value = false;
      updateGlobePoints();
      if(globeInstance.value){
        globeInstance.value.pointOfView({ lat: 62.1282, lng: 13.6435, altitude: 0.3 }, 2000);
      }
    };

    onMounted(async () => {
      if (globeContainer.value) {
        try {
          await new Promise<void>((resolve, reject) => {
            const img = new Image();
            img.src = earthTexture;
            img.onload = () => resolve();
            img.onerror = () => reject(new Error('Failed to load image'));
          });

          const globe = Globe({ waitForGlobeReady: true, animateIn: true })(globeContainer.value)
            .globeImageUrl(earthTexture)
            .pointAltitude('size')
            .pointColor('color')
            .pointLabel('label')
            .pointRadius('radius');

          globe.pointOfView({ lat: 60.1282, lng: 18.6435, altitude: 1.6 }); // Sweden
          globe.onGlobeReady(() => {
            loading.value = false;
            fetchStations();
          });

          globe.onPointClick((point: { name?: string }) => {
            if (point?.name) {
              const clickedStation = stations.value.find(station => station.name === point.name);
              if (clickedStation) focusOnStation(clickedStation);
            }
          });

          globeInstance.value = globe;
          window.addEventListener('resize', handleResize);
        } catch (error) {
          console.error(error);
        }
      }
    });

    onBeforeUnmount(() => {
      window.removeEventListener('resize', handleResize);
    });

    return {
      globeContainer,
      globeInstance,
      loading,
      searchText,
      stations,
      filteredStations,
      selectedStations,
      selectedStation,
      stationData,
      multipleStations,
      selectStation,
      focusOnStation,
      closeContainer,
      crossReferenceStations,
      removeStation,

    };
  },
});
</script>



<style scoped>
.page-container {
  position: fixed;
  width: 100vw;
  height: 100vh;
  overflow: hidden;
}

.globe-container {
  width: 100vw;
  height: 100vh;
  position: absolute;
  top: 0;
  left: 0;
  overflow: hidden;
}

.controls-container {
  position: relative;
  display: flex;
  height: 55px;
  justify-content: space-between;
  padding: 10px;
  background-color: rgba(12, 25, 38, 0.6);
  z-index: 3;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
}

.data-stations-btn {
  color: rgb(62, 230, 121);
  left: 10px;
  z-index: 3;

}

.station-list-menu {
  background-color: green;
}

.station-list {
  background-color: rgba(12, 25, 38, 0.6);
  color: rgb(62, 230, 121);

}

.station-list-button-container {
  right: 0;
}

.select-stations-btn {
  position: absolute;
  color: rgb(62, 230, 121);
  right: 10px;
  z-index: 3;
}

.searchbar {
  background-color: rgba(12, 25, 38, 1);
  z-index: 3;
}

.searchbar ::placeholder {
  padding-left: 10px;
  color: rgb(31, 110, 59);
}

::v-deep .q-field__native { /* deprecated, but no other solution so far */
  color: rgb(62, 230, 121) !important; /* Adjust input text color */
}

.input-text {
  color: rgb(62, 230, 121);
  z-index: 3;
}

.select-stations-list {
  width: 250px;
  background-color: rgba(12, 25, 38, 0.6);
  color: rgb(62, 230, 121);
  z-index: 3;
}

.station-tags {
  margin-top: 0px; /* Ensure there is a gap between controls and tags */
  display: flex;
  flex-wrap: wrap;
  left: 10px;
  gap: 5px; /* Adds space between the tags */
  border-radius: 5px;
  z-index: 1; /* Ensure tags appear above the globe */
  position: relative; /* Maintain relative positioning */
}

.q-chip {
  background-color: rgba(62, 230, 121, 0.7);
  color: white;
}

.station-container {
  display: flex;
  justify-content: center;
  align-items: center;
  width: 100vw;
  height: 100vh;
  position: absolute;
  top: 0;
  left: 0;
}
</style>
