import { Alert, Button, FlatList, ScrollView, StyleSheet, View } from 'react-native'
import React, { useEffect, useState } from 'react'
import { NativeStackScreenProps } from '@react-navigation/native-stack'
import DeviceListItem from './DeviceListItem'
import { StackParamList } from '../navigation/types'
import { Device } from '../types/Device'
import Config from '../utils/Config'
import API from '../utils/API'
import Dialog from 'react-native-dialog'

type SelectDeviceProps = NativeStackScreenProps<StackParamList, 'SelectDevice'>

const SelectDevice = ({ route, navigation }: SelectDeviceProps) => {
  const [isModalOpen, setIsModalOpen] = useState<boolean>(false)
  const [deviceId, setDeviceId] = useState<string>('')
  const [devices, setDevices] = useState<Device[]>([])

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

  const addDevice = async () => {
    try {
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
        <View style={styles.buttonContainer}>
          <Button title='Add a device' color={Config.COLOR_CONTENT} onPress={() => setIsModalOpen(true)}></Button>
        </View>
      </ScrollView>
    </>
  )
}

export default SelectDevice

const styles = StyleSheet.create({
  container: {
    backgroundColor: '#121212',
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