import AsyncStorage from '@react-native-async-storage/async-storage'
import React, { useEffect, useState } from 'react'
import { Alert, Button, StyleSheet, Text, TouchableOpacity, View } from 'react-native'
import Dialog from 'react-native-dialog'
import Config from '../Config'
import { Device } from '../types/Device'

type DeviceProps = {
  device: Device
  onPress: (device: Device) => void
  onDelete: (device: Device) => void
}

const DeviceListItem = (props: DeviceProps) => {
  const [isModalOpen, setIsModalOpen] = useState<boolean>(false)
  const [updatedName, setUpdatedName] = useState<string>('')
  const [deviceName, setDeviceName] = useState<string>('')

  useEffect(() => {
    const getDeviceName = async () => {
      setDeviceName(await props.device.getName())
    }

    getDeviceName()
  }, [])

  const updateDeviceName = () => {
    setIsModalOpen(false)
    if (updatedName.length < 3)
      return Alert.alert('Name too short!')

    if (updatedName.length > 15)
      return Alert.alert('Name too long!')

    setDeviceName(updatedName)
    AsyncStorage.setItem(props.device.id, updatedName)
  }

  return (
    <>
      <Dialog.Container visible={isModalOpen}>
        <Dialog.Title>Edit device name</Dialog.Title>
        <Dialog.Input label='New name' defaultValue={props.device.name} onChangeText={setUpdatedName} />
        <Dialog.Button label='Cancel' onPress={() => setIsModalOpen(false)} />
        <Dialog.Button label='Update' onPress={updateDeviceName} />
      </Dialog.Container>
      <TouchableOpacity style={styles.selectDevice} onPress={() => props.onPress(props.device)}>
        <View style={styles.textContainer}>
          <Text style={styles.deviceName}>{deviceName}</Text>
          <Text style={styles.deviceId}>ID: {props.device.id}</Text>
        </View>
        <View style={styles.buttonContainer}>
          <Button title='Edit Name' color='#898989' onPress={() => setIsModalOpen(true)}></Button>
          <Button title='Delete' color='#898989' onPress={() => props.onDelete(props.device)}></Button>
        </View>
      </TouchableOpacity>
    </>
  )
}

export default DeviceListItem

const styles = StyleSheet.create({
  selectDevice: {
    justifyContent: 'space-between',
    alignItems: 'center',
    marginTop: 20,
    marginHorizontal: 20,
    borderRadius: 20,
    flexDirection: 'row',
    gap: 20,
    padding: 20,
    backgroundColor: Config.COLOR_CONTENT
  },
  textContainer: {
    flex: 3,
    justifyContent: 'center',
    alignItems: 'center',
  },
  buttonContainer: {
    justifyContent: 'center',
    alignItems: 'center',
    flexDirection: 'row',
    gap: 10,
  },
  deviceName: {
    fontSize: 30,
    color: 'white'
  },
  deviceId: {
    color: Config.COLOR_TEXT_DARKER
  },
  button: {
    backgroundColor: Config.COLOR_TEXT_DARKER
  }
})