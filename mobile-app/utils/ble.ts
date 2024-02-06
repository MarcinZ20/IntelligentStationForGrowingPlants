/* eslint-disable no-bitwise */
import { useMemo, useState } from 'react'
import { PermissionsAndroid, Platform } from 'react-native'
import { BleManager, Device as BluetoothDevice } from 'react-native-ble-plx'

import * as ExpoDevice from 'expo-device'

import base64 from 'react-native-base64'
import Config from '../Config'

const HEART_RATE_UUID = '0000180d-0000-1000-8000-00805f9b34fb'
const HEART_RATE_CHARACTERISTIC = '00002a37-0000-1000-8000-00805f9b34fb'

interface BluetoothLowEnergyApi {
  requestPermissions(): Promise<boolean>
  enableBluetooth(): Promise<void>
  scanForPeripherals(): void
  connectToDevice: (device: BluetoothDevice) => Promise<void>
  sendData: (device: BluetoothDevice, accessToken: string, ssid: string, password: string) => Promise<void>
  disconnectFromDevice: () => void
  connectedDevice: BluetoothDevice | null
  allDevices: BluetoothDevice[]
}

const useBLE = (): BluetoothLowEnergyApi => {
  const bleManager = useMemo(() => new BleManager(), [])
  const [allDevices, setAllDevices] = useState<BluetoothDevice[]>([])
  const [connectedDevice, setConnectedDevice] = useState<BluetoothDevice | null>(null)

  const requestAndroid31Permissions = async () => {
    const bluetoothScanPermission = await PermissionsAndroid.request(
      PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
      {
        title: 'Location Permission',
        message: 'Bluetooth Low Energy requires Location',
        buttonPositive: 'OK',
      }
    )
    const bluetoothConnectPermission = await PermissionsAndroid.request(
      PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
      {
        title: 'Location Permission',
        message: 'Bluetooth Low Energy requires Location',
        buttonPositive: 'OK',
      }
    )
    const fineLocationPermission = await PermissionsAndroid.request(
      PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
      {
        title: 'Location Permission',
        message: 'Bluetooth Low Energy requires Location',
        buttonPositive: 'OK',
      }
    )

    return (
      bluetoothScanPermission === 'granted' &&
      bluetoothConnectPermission === 'granted' &&
      fineLocationPermission === 'granted'
    )
  }

  const requestPermissions = async () => {
    if (Platform.OS === 'android') {
      if ((ExpoDevice.platformApiLevel ?? -1) < 31) {
        const granted = await PermissionsAndroid.request(
          PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
          {
            title: 'Location Permission',
            message: 'Bluetooth Low Energy requires Location',
            buttonPositive: 'OK',
          }
        )
        return granted === PermissionsAndroid.RESULTS.GRANTED
      } else {
        const isAndroid31PermissionsGranted =
          await requestAndroid31Permissions()

        return isAndroid31PermissionsGranted
      }
    } else {
      return true
    }
  }

  const enableBluetooth = async () => {
    if (await bleManager.state() === 'PoweredOn')
      return

    try {
      await bleManager.enable()
    } catch (error) {
      console.error(error)
    }
  }

  const isDuplicteDevice = (devices: BluetoothDevice[], nextDevice: BluetoothDevice) =>
    devices.findIndex((device) => nextDevice.id === device.id) > -1

  const scanForPeripherals = () =>
    bleManager.startDeviceScan(null, null, (error, device) => {
      if (error)
        return console.log(error)

      if (device && device.name?.includes(Config.DEVICE_NAME)) {
        setAllDevices((prevState: BluetoothDevice[]) => {
          if (!isDuplicteDevice(prevState, device)) {
            return [...prevState, device]
          }
          return prevState
        })
      }
    })

  const connectToDevice = async (device: BluetoothDevice) => {
    const deviceConnection = await bleManager.connectToDevice(device.id, { requestMTU: 500 })
    setConnectedDevice(deviceConnection)
    await deviceConnection.discoverAllServicesAndCharacteristics()
    bleManager.stopDeviceScan()
  }

  const disconnectFromDevice = () => {
    if (connectedDevice) {
      bleManager.cancelDeviceConnection(connectedDevice.id)
      setConnectedDevice(null)
    }
  }

  const sendData = async (device: BluetoothDevice, accessToken: string, ssid: string, password: string) => {
    if (!device)
      return console.log('No device connected')

    const data = base64.encode(`${ssid};${password};${accessToken}`)

    await device.writeCharacteristicWithoutResponseForService(
      Config.SERVICE_UUID,
      Config.CHARACTERISTIC_UUID,
      data
    )
  }

  return {
    scanForPeripherals,
    enableBluetooth,
    requestPermissions,
    connectToDevice,
    sendData,
    disconnectFromDevice,
    allDevices,
    connectedDevice
  }
}

export default useBLE