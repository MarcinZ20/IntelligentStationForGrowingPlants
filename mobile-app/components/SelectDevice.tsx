import { NativeStackScreenProps } from '@react-navigation/native-stack'
import React, { useEffect, useState } from 'react'
import { Alert, Button, PermissionsAndroid, Platform, ScrollView, StyleSheet, Text, View } from 'react-native'
import Dialog from 'react-native-dialog'
import Config from '../Config'
import { StackParamList } from '../navigation/types'
import { Device } from '../types/Device'
import API from '../utils/API'
import useBLE from '../utils/ble'
import DeviceListItem from './DeviceListItem'

type SelectDeviceProps = NativeStackScreenProps<StackParamList, 'SelectDevice'>

const SelectDevice = ({ route, navigation }: SelectDeviceProps) => {
  const [isModalOpen, setIsModalOpen] = useState<boolean>(false)
  const [deviceId, setDeviceId] = useState<string>('')
  const [devices, setDevices] = useState<Device[]>([])

  const {
    requestPermissions,
    scanForPeripherals,
    connectToDevice,
    sendData,
    disconnectFromDevice,
    enableBluetooth,
    allDevices,
    connectedDevice,
  } = useBLE()

  useEffect(() => {
    setDevices(route.params.deviceIds.map(id => new Device(id)))
  }, [])

  const chooseDevice = (device: Device) => {
    navigation.navigate('CurrentReadings', { deviceId: device.id })
  }

  const deleteDevice = async (device: Device) => {
    try {
      const response = await API.deleteDevice(device.id)
      if (response.error)
        return Alert.alert(response.error)

      setDevices(response.user.devicesIds.map((deviceId: string) => new Device(deviceId)))
    } catch (error) {
      console.error(error)
    }
  }

  const scanDevices = async () => {
    const permissions = await requestPermissions()
    if (!permissions)
      return

    await enableBluetooth()
    navigation.navigate('AddDevice')
  }

  const requestBluetoothPermission = async () => {
    if (Platform.OS === 'ios') {
      return true
    }
    if (Platform.OS === 'android' && PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION) {
      const apiLevel = parseInt(Platform.Version.toString(), 10)

      if (apiLevel < 31) {
        const granted = await PermissionsAndroid.request(PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION)
        return granted === PermissionsAndroid.RESULTS.GRANTED
      }
      if (PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN && PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT) {
        const result = await PermissionsAndroid.requestMultiple([
          PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
          PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
          PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION
        ])
  
        return (
          result['android.permission.BLUETOOTH_CONNECT'] === PermissionsAndroid.RESULTS.GRANTED &&
          result['android.permission.BLUETOOTH_SCAN'] === PermissionsAndroid.RESULTS.GRANTED &&
          result['android.permission.ACCESS_FINE_LOCATION'] === PermissionsAndroid.RESULTS.GRANTED
        )
      }
    }
  
    return false
  }

  const addDevice = async () => {
    try {
      const result = await requestBluetoothPermission()
      console.log(result)

      const response = await API.addDevice(deviceId)
      if (response.error)
        return Alert.alert(response.error)
      
      setIsModalOpen(false)
      setDevices(response.user.devicesIds.map((deviceId: string) => new Device(deviceId)))
    } catch (error) {
      console.error(error)
    }
  }

  return (
    <>
      <Dialog.Container visible={isModalOpen}>
      <Dialog.Title>Add a device</Dialog.Title>
        <Dialog.Input label='Device ID' onChangeText={setDeviceId} />
        <Dialog.Button label='Cancel' onPress={() => setIsModalOpen(false)}/>
        <Dialog.Button label='Add' onPress={addDevice}/>
      </Dialog.Container>
      <ScrollView style={styles.container} contentContainerStyle={styles.contentContainer}>
        {
          devices.map((device, i) => <DeviceListItem key={i} device={device} onPress={chooseDevice} onDelete={deleteDevice} />)
        }
        {/* <View style={styles.buttonContainer}>
          <Button title='Add a device' color={Config.COLOR_CONTENT} onPress={() => setIsModalOpen(true)}></Button>
        </View> */}
        <View style={styles.buttonContainer}>
          <Button title='Add a device' color={Config.COLOR_CONTENT} onPress={scanDevices}></Button>
        </View>
        {/* {
          allDevices.map((device, i) => <Text key={i}>{device.name} - {device.id}</Text>)
        } */}
      </ScrollView>
    </>
  )
}

export default SelectDevice

const styles = StyleSheet.create({
  container: {
    backgroundColor: Config.COLOR_BACKGROUND,
  },
  contentContainer: {
    alignItems: 'center'
  },
  buttonContainer: {
    marginVertical: 20,
    justifyContent: 'center',
    alignItems: 'center',
  }
})