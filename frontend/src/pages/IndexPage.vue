<template>
  <div class="page-container">
    <LoadingScreen v-if="loading" />
    <div ref="globeContainer" class="globe-container" v-show="!loading"></div>

    <div class="controls-container">
      <!--DEVICE-->
      <div class="select-devices-btn">
        <q-btn-group>
          <q-btn
            color="primary"
            label="select iot devices to"
            @click="closeDeviceContainer"
          >
            <q-menu anchor="bottom left" class="bg-transparent">
              <q-input
                v-model="deviceSearchText"
                placeholder="Search Iot Devices"
                class="q-mt-sm searchbar-devices"
                input-class="station-input-text-devices"
                color="green"
                persist
                sticky
              />
              <q-list color="primary" separator class="select-devices-list">
                <q-item
                  v-for="device in filteredDevices"
                  :key="device.mac"
                  clickable
                  @click="selectDevice(device)"
                >
                  <q-item-label>{{ device.name }}</q-item-label>
                </q-item>
              </q-list>
            </q-menu>
          </q-btn>
          <q-btn
            :label="selectedDevices.length > 1 ? 'X-REF' : 'Show'"
            color="secondary"
            @click="handleSelectedDevices"
          ></q-btn>
        </q-btn-group>
      </div>

      <!--ADD/DELETE DEVICE-->
      <div class="add-delete-btn">
        <q-btn color="secondary" label="Add device" @click="openAddDeviceMenu">
          <q-menu dark anchor="bottom left" class="bg-transparent no-shadow">
            <div class="add-btn-form-container">
              <q-form class="q-gutter-md">
                <q-input
                  v-model="nameValue"
                  outlined
                  class="custom-input"
                  color="secondary"
                  label-color="positive"
                  border-color="secondary"
                  label="Device Name"
                  lazy-rules
                />

                <q-input
                  v-model="macValue"
                  outlined
                  class="custom-input"
                  color="secondary"
                  label-color="positive"
                  label="Device Mac"
                  lazy-rules
                />

                <div class="submit-btn">
                  <q-btn
                    label="Add"
                    type="submit"
                    color="secondary"
                    @click="addDevice"
                  ></q-btn>
                </div>
              </q-form>
            </div>
          </q-menu>
        </q-btn>

        <q-btn
          color="accent"
          label="Delete device"
          @click="openDeleteDeviceMenu"
        >
          <q-menu dark anchor="bottom left" class="bg-transparent no-shadow">
            <div class="add-btn-form-container">
              <q-form class="q-gutter-md">
                <q-input
                  v-model="macValue"
                  outlined
                  class="custom-input"
                  color="accent"
                  label-color="negative"
                  label="Device Mac"
                  lazy-rules
                />

                <div class="submit-btn">
                  <q-btn
                    label="DELETE"
                    type="submit"
                    color="accent"
                    @click="deleteDevice"
                  ></q-btn>
                </div>
              </q-form>
            </div>
          </q-menu>
        </q-btn>
      </div>

      <!--STATION-->
      <div class="select-stations-btn" ref="stationSearchDropdown">
        <q-btn-group>
          <q-btn
            color="primary"
            label="select stations to"
            @click="closeStationContainer"
          >
            <q-menu anchor="bottom left" class="bg-transparent">
              <q-input
                v-model="stationSearchText"
                placeholder="Search Stations"
                class="q-mt-sm station-searchbar"
                input-class="station-input-text"
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
          <q-btn
            :label="selectedStations.length > 1 ? 'X-REF' : 'Show'"
            color="secondary"
            @click="handleSelectedStations"
          ></q-btn>
        </q-btn-group>
      </div>
    </div>

    <!--DEVICE-->
    <div
      v-if="selectedDevices.length > 0 && !multipleDevices"
      class="q-mt-sm devices-tags"
    >
      <q-chip
        v-for="device in selectedDevices"
        :key="device.mac"
        removable
        @remove="removeDevice(device.mac)"
        class="device-chip"
      >
        {{ device.name }}
      </q-chip>
    </div>

    <!--STATION-->
    <div
      v-if="selectedStations.length > 0 && !multipleStations"
      class="q-mt-sm station-tags"
    >
      <q-chip
        v-for="station in selectedStations"
        :key="station.key"
        removable
        @remove="removeStation(station.key)"
        class="station-chip"
      >
        {{ station.name }}
      </q-chip>
    </div>

    <!--DEVICE  v-if="selectedDevices.length > 0 && isDeviceSelected" -->
    <div
      v-if="selectedDevices.length > 0 && isDeviceSelected"
      class="device-container"
    >
      <DeviceDataPanel
        :selected-devices="selectedDevices"
        :multipleDevices
        @close="closeDeviceContainer"
      />
    </div>

    <!--STATION-->
    <div
      v-if="selectedStations.length > 0 && isStationSelected"
      class="station-container"
    >
      <StationDataPanel
        :selected-stations="selectedStations"
        :multipleStations
        @close="closeStationContainer"
      />
    </div>
  </div>
</template>

<script lang="ts">
import {
  defineComponent,
  ref,
  computed,
  onMounted,
  onBeforeUnmount,
} from 'vue';
import Globe from 'globe.gl';
import LoadingScreen from '../components/LoadingScreen.vue';
import StationDataPanel from '../components/StationDataPanel.vue';
import DeviceDataPanel from '../components/DeviceDataPanel.vue';
import earthTexture from 'src/assets/earth-high-res.jpg';
import axios from 'axios';

interface Station {
  key: string;
  name: string;
  latitude: number;
  longitude: number;
}

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

export default defineComponent({
  components: {
    LoadingScreen,
    StationDataPanel,
    DeviceDataPanel,
  },
  setup() {
    const globeContainer = ref<HTMLElement | null>(null);
    const globeInstance = ref<ReturnType<typeof Globe> | null>(null);
    const loading = ref(true);
    // station
    const stationSearchText = ref('');
    const stations = ref<Station[]>([]);
    const selectedStations = ref<Station[]>([]);
    const multipleStations = ref(false);
    const isStationSelected = ref(false);
    // devices
    const deviceSearchText = ref('');
    const devices = ref<Device[]>([]);
    const selectedDevices = ref<Device[]>([]);
    const multipleDevices = ref(false);
    const isDeviceSelected = ref(false);
    const isAddDeviceClicked = ref(false);
    const isDeleteDeviceClicked = ref(false);
    // add device
    const nameValue = ref('');
    const macValue = ref('');
    const addDeviceMenu = ref(false);
    const deleteDeviceMenu = ref(false);

    // Station sections -->
    const fetchStations = async () => {
      console.log('API - Fetching stations...');
      try {
        const response = await axios.get(
          'https://opendata-download-metobs.smhi.se/api/version/1.0/parameter/1/station-set/all/period/latest-hour/data.json'
        );
        stations.value = response.data.station;
        updateGlobePoints();
      } catch (error) {
        console.error('Error fetching stations:', error);
      }
    };

    const filteredStations = computed(() => {
      return stations.value.filter((station) =>
        station.name
          .toLowerCase()
          .includes(stationSearchText.value.toLowerCase())
      );
    });

    const selectStation = (station: Station) => {
      if (!selectedStations.value.find((s) => s.key === station.key)) {
        selectedStations.value.push(station);
        updateGlobePoints();
      }
    };

    const removeStation = (stationKey: string) => {
      selectedStations.value = selectedStations.value.filter(
        (s) => s.key !== stationKey
      );
    };

    const handleSelectedStations = () => {
      if (selectedStations.value && selectedStations.value.length > 1) {
        displayMultipleStations();
      } else {
        displaySingleStation(selectedStations.value[0]);
      }
    };

    const displaySingleStation = (station: Station) => {
      isStationSelected.value = true;
      if (globeInstance.value) {
        globeInstance.value.pointOfView(
          { lat: station.latitude, lng: station.longitude, altitude: 0.1 },
          2000
        );
        updateGlobePoints();
      } else {
        console.error('Globe instance is not initialized.');
      }
    };

    const displayMultipleStations = () => {
      multipleStations.value = true;
      isStationSelected.value = true;
      stationSearchText.value = '';
      if (globeInstance.value) {
        globeInstance.value.pointOfView(
          { lat: 62.1282, lng: 13.6435, altitude: 0.3 },
          2000
        );
      }
    };

    const closeStationContainer = () => {
      selectedStations.value = [];
      multipleStations.value = false;
      updateGlobePoints();
      if (isStationSelected.value) {
        if (globeInstance.value) {
          globeInstance.value.pointOfView(
            { lat: 62.1282, lng: 13.6435, altitude: 0.4 },
            2000
          );
        }
      }
      isStationSelected.value = false;
    };
    // <-- station sections

    // device section -->
    const fetchDevices = async () => {
      try {
        const response = await axios.get('/api/sensor-data');
        devices.value = response.data.device;
        updateGlobePoints();
      } catch (error) {
        console.error('Error fetching devices:', error);
      }
    };

    const filteredDevices = computed(() => {
      return devices.value.filter((devices) =>
        devices.name
          .toLowerCase()
          .includes(deviceSearchText.value.toLowerCase())
      );
    });

    const selectDevice = (device: Device) => {
      if (!selectedDevices.value.find((s) => s.mac === device.mac)) {
        selectedDevices.value.push(device);
        updateGlobePoints();
      }
    };

    const removeDevice = (stationKey: string) => {
      selectedStations.value = selectedStations.value.filter(
        (s) => s.key !== stationKey
      );
    };

    const handleSelectedDevices = () => {
      if (selectedStations.value && selectedStations.value.length > 1) {
        displayMultipleDevices();
      } else {
        displaySingleDevice(selectedStations.value[0]);
      }
    };

    const displaySingleDevice = (station: Station) => {
      isStationSelected.value = true;
      if (globeInstance.value) {
        globeInstance.value.pointOfView(
          { lat: station.latitude, lng: station.longitude, altitude: 0.1 },
          2000
        );
        updateGlobePoints();
      } else {
        console.error('Globe instance is not initialized.');
      }
    };

    const displayMultipleDevices = () => {
      multipleStations.value = true;
      isStationSelected.value = true;
      stationSearchText.value = '';
      if (globeInstance.value) {
        globeInstance.value.pointOfView(
          { lat: 62.1282, lng: 13.6435, altitude: 0.3 },
          2000
        );
      }
    };

    const closeDeviceContainer = () => {
      selectedDevices.value = [];
      multipleDevices.value = false;
      updateGlobePoints();
      if (isDeviceSelected.value) {
        if (globeInstance.value) {
          globeInstance.value.pointOfView(
            { lat: 62.1282, lng: 13.6435, altitude: 0.4 },
            2000
          );
        }
      }
      isDeviceSelected.value = false;
    };
    // <-- device section

    // add device -->
    function openAddDeviceMenu() {
      addDeviceMenu.value = true;
      nameValue.value = '';
      macValue.value = '';
    }

    function openDeleteDeviceMenu() {
      deleteDeviceMenu.value = true;
      macValue.value = '';
    }

    function addDevice() {
      const deviceReg = {
        mac: macValue.value,
        name: nameValue.value,
      };
      axios
        .post('/api/iot-device', deviceReg)
        .then((response) => {
          console.log('Device added successfully:', response.data);
          nameValue.value = '';
          macValue.value = '';
          addDeviceMenu.value = false;
        })
        .catch((error) => {
          console.error('There was an error adding the device:', error);
        });
    }

    function deleteDevice() {
      const macAddress = macValue.value;

      axios
        .delete(`/api/iot-device/${macAddress}`)
        .then((response) => {
          console.log('Device deleted successfully:', response.data);
          macValue.value = '';
          deleteDeviceMenu.value = false;
        })
        .catch((error) => {
          console.error('There was an error deleting the device:', error);
        });
    }
    // <-- add device

    const handleResize = () => {
      if (globeInstance.value) {
        globeInstance.value.width(window.innerWidth);
        globeInstance.value.height(window.innerHeight);
      }
    };

    const updateGlobePoints = () => {
      if (globeInstance.value) {
        const pointsData: Array<{
          lat: number;
          lng: number;
          size: number;
          radius: number;
          color: string;
          label: string;
          name: string;
        }> = [];

        // Adds stations
        stations.value.forEach((station) => {
          const isInSelectedStations = selectedStations.value?.some(
            (selected) => selected.key === station.key
          );
          pointsData.push({
            lat: station.latitude,
            lng: station.longitude,
            size: 0,
            radius: 0.05,
            color: isInSelectedStations ? 'green' : 'red',
            label: station.name,
            name: station.name,
          });
        });

        // Adds devices
        devices.value.forEach((device) => {
          const isInSelectedDevices = selectedDevices.value?.some(
            (selected) => selected.mac === device.mac
          );
          pointsData.push({
            lat: device.latitude,
            lng: device.longitude,
            size: 0,
            radius: 0.05,
            color: isInSelectedDevices ? 'green' : 'blue',
            label: device.name,
            name: device.name,
          });
        });

        globeInstance.value.pointsData(pointsData);
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

          const globe = Globe({ waitForGlobeReady: true, animateIn: true })(
            globeContainer.value
          )
            .globeImageUrl(earthTexture)
            .pointAltitude('size')
            .pointColor('color')
            .pointLabel('label')
            .pointRadius('radius');

          globe.pointOfView({ lat: 60.1282, lng: 18.6435, altitude: 1.6 }); // Sweden
          globe.onGlobeReady(() => {
            loading.value = false;
            fetchStations();
            fetchDevices();
          });

          globe.onPointClick((point: { name?: string }) => {
            if (point?.name) {
              const clickedStation = stations.value.find(
                (station) => station.name === point.name
              );
              if (clickedStation) {
                selectStation(clickedStation);
                displaySingleStation(selectedStations.value[0]);
              }
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
      // station
      stationSearchText,
      stations,
      filteredStations,
      selectedStations,
      multipleStations,
      isStationSelected,
      selectStation,
      closeStationContainer,
      handleSelectedStations,
      removeStation,
      // devices
      deviceSearchText,
      devices,
      filteredDevices,
      selectedDevices,
      multipleDevices,
      isDeviceSelected,
      isAddDeviceClicked,
      isDeleteDeviceClicked,
      selectDevice,
      closeDeviceContainer,
      handleSelectedDevices,
      removeDevice,
      // add device
      nameValue,
      macValue,
      openAddDeviceMenu,
      openDeleteDeviceMenu,
      addDevice,
      deleteDevice,
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
  align-items: center;
  padding: 10px;
  background-color: rgba(12, 25, 38, 0.6);
  z-index: 3;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
}

/* STATION */
.select-stations-btn {
  position: absolute;
  color: rgb(62, 230, 121);
  right: 10px;
  z-index: 3;
}

.staion-searchbar {
  background-color: rgba(12, 25, 38, 1);
  z-index: 3;
}

.station.searchbar ::placeholder {
  padding-left: 10px;
  color: rgb(31, 110, 59);
}

::v-deep .q-field__native {
  color: rgb(62, 230, 121) !important;
}

.station-input-text {
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
  margin-top: 0px;
  display: flex;
  flex-wrap: wrap;
  left: 10px;
  gap: 5px;
  border-radius: 5px;
  z-index: 1;
  position: relative;
}

.station-chip {
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

/* DEVICE */
.select-devices-btn {
  position: absolute;
  color: rgb(62, 230, 121);
  left: 10px;
  z-index: 3;
}

.device-searchbar {
  background-color: rgba(12, 25, 38, 1);
  z-index: 3;
}

.device.searchbar ::placeholder {
  padding-left: 10px;
  color: rgb(31, 110, 59);
}

.device-input-text {
  color: rgb(62, 230, 121);
  z-index: 3;
}

.select-devices-list {
  width: 250px;
  background-color: rgba(12, 25, 38, 0.6);
  color: rgb(62, 230, 121);
  z-index: 3;
}

.device-tags {
  margin-top: 0px;
  display: flex;
  flex-wrap: wrap;
  left: 10px;
  gap: 5px;
  border-radius: 5px;
  z-index: 1;
  position: relative;
}

.device-chip {
  background-color: rgba(62, 230, 121, 0.7);
  color: white;
}

.device-container {
  display: flex;
  justify-content: center;
  align-items: center;
  width: 100vw;
  height: 100vh;
  position: absolute;
  top: 0;
  left: 0;
}

/* ADD/DELETE DEVICE */
.add-delete-btn {
  display: flex;
  flex-grow: 1;
  color: rgb(62, 230, 121);
  justify-content: center;
  gap: 10px;
}

.add-device {
  display: flex;
  flex-direction: column;
  width: 90vw;
  max-width: 1000px;
}

.add-delete-btns {
  display: flex;
  justify-content: space-between;
  margin-top: 20px;
}

.add-btn-content {
  background-color: rgba(12, 25, 38, 0.9);
}

.submit-btn {
  display: flex;
  justify-content: center;
  margin-bottom: 10px;
}

.add-btn-form-container {
  padding: 20px;
  background-color: rgba(12, 25, 38, 0.9);
}

.custom-input {
  background-color: rgba(8, 34, 61, 0.9);
  border-radius: 5px;
  -webkit-box-shadow: 0px 3px 5px 1px rgba(0, 0, 0, 1);
  -moz-box-shadow: 0px 3px 5px 1px rgba(0, 0, 0, 1);
  box-shadow: 0px 3px 5px 1px rgba(0, 0, 0, 1);
}
</style>
