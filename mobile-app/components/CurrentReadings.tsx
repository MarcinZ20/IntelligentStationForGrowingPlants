import { View, Text, StyleSheet, TouchableOpacity } from 'react-native'
import React, { useEffect, useState } from 'react'
import { NativeStackScreenProps } from '@react-navigation/native-stack'
import { StackParamList } from '../navigation/types'
import { Device, Reading } from '../types/Device'
import Config from '../utils/Config'

type CurrentReadingsProps = NativeStackScreenProps<StackParamList, 'CurrentReadings'>

const CurrentReadings = ({ route, navigation }: CurrentReadingsProps) => {
  const [readings, setReadings] = useState<Reading>({ temperature: 0, humidity: 0, lightIntensity: 0 })
  const [error, setError] = useState<string | null>(null)
  const [deviceName, setDeviceName] = useState<string>('')

  const device = new Device(route.params.deviceId)

  useEffect(() => {
    const getDeviceName = async () => {
      const name = await device.getName()
      setDeviceName(name)
      navigation.setOptions({ title: device.name + ' - Current Readings' })
    }
    const updateReadings = async () => {
      try {
        const data = await device.currentReadings()
        if (data)
          setReadings(data)
      } catch (error) {
        console.error(error)
        setError('Failed to fetch current readings')
      }
    }
    
    getDeviceName()
    updateReadings()
  
    const interval = setInterval(() => {
      updateReadings()
    }, 5000)
    return () => clearInterval(interval)
  }, [])

  const goToHistoricalData = () => {
    navigation.navigate('HistoricalData', { deviceId: device.id })
  }

  if (error)
    return (
      <View style={styles.container}>
        <Text style={styles.text}>Error: {error}</Text>
      </View>
    )

  return (
    <View style={styles.background}>
      <View style={styles.container}>
        <TouchableOpacity style={styles.panel}>
          <Text style={styles.textSmall}>Device ID</Text>
          <Text style={styles.textBig}>{device.id}</Text>
        </TouchableOpacity>
        <TouchableOpacity style={styles.panel}>
          <Text style={styles.textSmall}>Name</Text>
          <Text style={styles.textBig}>{deviceName}</Text>
        </TouchableOpacity>
        <TouchableOpacity style={styles.panel}>
          <Text style={styles.textSmall}>Temperature</Text>
          <Text style={styles.textBigNumber}>{readings.temperature}</Text>
        </TouchableOpacity>
        <TouchableOpacity style={styles.panel}>
          <Text style={styles.textSmall}>Humidity</Text>
          <Text style={styles.textBigNumber}>{readings.humidity}</Text>
        </TouchableOpacity>
        <TouchableOpacity style={styles.panel}>
          <Text style={styles.textSmall}>Light Intensity</Text>
          <Text style={styles.textBigNumber}>{readings.lightIntensity}</Text>
        </TouchableOpacity>
      </View>
      <TouchableOpacity style={styles.history} onPress={goToHistoricalData}>
        <Text style={styles.textBig}>Historical Data</Text>
      </TouchableOpacity>
    </View>
  )
}

export default CurrentReadings

const styles = StyleSheet.create({
  background: {
    flex: 1,
    backgroundColor: Config.COLOR_BACKGROUND,
  },
  container: {
    flex: 1,
    backgroundColor: Config.COLOR_BACKGROUND,
    justifyContent: 'center',
    alignItems: 'center',
    flexDirection: 'row',
    flexWrap: 'wrap',
    gap: 25,
    padding: 25
  },
  panel: {
    backgroundColor: Config.COLOR_CONTENT,
    width: '46%',
    aspectRatio: 1,
    padding: 25,
    borderRadius: 25,
    justifyContent: 'center',
    alignItems: 'center'
  },
  text: {
    color: 'white'
  },
  textSmall: {
    color: 'white',
    position: 'absolute',
    top: 25,
    left: 25
  },
  textBig: {
    fontSize: 25,
    color: 'white'
  },
  textBigNumber: {
    fontSize: 60,
    color: 'white'
  },
  history: {
    backgroundColor: Config.COLOR_CONTENT,
    borderRadius: 20,
    padding: 25,
    justifyContent: 'center',
    alignItems: 'center'
  },
})