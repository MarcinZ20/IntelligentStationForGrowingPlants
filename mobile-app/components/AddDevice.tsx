import { NativeStackScreenProps } from '@react-navigation/native-stack'
import React, { useEffect, useState } from 'react'
import { Alert, ScrollView, StyleSheet, Text, TouchableOpacity, View } from 'react-native'
import { Device as BluetoothDevice } from 'react-native-ble-plx'
import Dialog from 'react-native-dialog'
import Config from '../Config'
import { StackParamList } from '../navigation/types'
import API from '../utils/API'
import useBLE from '../utils/ble'
import { Device } from '../types/Device'

type AddDeviceProps = NativeStackScreenProps<StackParamList, 'AddDevice'>

const AddDevice = ({ route, navigation }: AddDeviceProps) => {
  const [isModalOpen, setIsModalOpen] = useState<boolean>(false)
  const [SSID, setSSID] = useState<string>('')
  const [password, setPassword] = useState<string>('')

  const {
    scanForPeripherals,
    connectToDevice,
    sendData,
    disconnectFromDevice,
    allDevices,
    connectedDevice,
  } = useBLE()

  useEffect(() => {
    scanForPeripherals()
  }, [])

  const connect = async (device: BluetoothDevice) => {
    try {
      await connectToDevice(device)
      setIsModalOpen(true)
    } catch (error) {
      console.error(error)
      Alert.alert('Error connecting!')
    }
  }

  const send = async () => {
    if (!connectedDevice)
      return Alert.alert('Not connected to the device!')

    try {
      await sendData(connectedDevice, API.getAccessToken(), SSID, password)
      disconnectFromDevice()
      setIsModalOpen(false)

      const devicesResponse = await API.devices()
      if (devicesResponse.error)
        return Alert.alert(devicesResponse.error)

      navigation.navigate('SelectDevice', { deviceIds: devicesResponse.devicesIds })
    } catch (error) {
      console.error(error)
      Alert.alert('Error sending data!')
    }
  }

  return (
    <>
      <Dialog.Container visible={isModalOpen}>
        <Dialog.Title>Add a device</Dialog.Title>
        <Dialog.Input label='SSID' onChangeText={setSSID} />
        <Dialog.Input label='Password' secureTextEntry={true} onChangeText={setPassword} />
        <Dialog.Button label='Cancel' onPress={() => setIsModalOpen(false)} />
        <Dialog.Button label='Send' onPress={send} />
      </Dialog.Container>
      <ScrollView style={styles.container} contentContainerStyle={styles.contentContainer}>
        {
          allDevices.map((device, i) => {
            return <TouchableOpacity key={i} style={styles.selectDevice} onPress={() => connect(device)}>
              <View style={styles.textContainer}>
                <Text style={styles.deviceName}>{device.name}</Text>
                <Text style={styles.deviceId}>ID: {device.id}</Text>
              </View>
            </TouchableOpacity>
          })
        }
      </ScrollView>
    </>
  )
}

export default AddDevice

const styles = StyleSheet.create({
  container: {
    backgroundColor: Config.COLOR_BACKGROUND,
  },
  contentContainer: {
    alignItems: 'center'
  },
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
    justifyContent: 'center',
    alignItems: 'center',
  },
  deviceName: {
    fontSize: 30,
    color: 'white'
  },
  deviceId: {
    color: Config.COLOR_TEXT_DARKER
  }
})