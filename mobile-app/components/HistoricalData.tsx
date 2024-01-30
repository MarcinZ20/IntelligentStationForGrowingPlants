import { NativeStackScreenProps } from '@react-navigation/native-stack'
import React, { useEffect, useState } from 'react'
import { StyleSheet, Text, TouchableOpacity, View } from 'react-native'
import Config from '../Config'
import { StackParamList } from '../navigation/types'
import { Device, Reading } from '../types/Device'

type HistoricalDataProps = NativeStackScreenProps<StackParamList, 'HistoricalData'>

const HistoricalData = ({ route, navigation }: HistoricalDataProps) => {
  const [readings, setReadings] = useState<Reading[]>([])
  const [error, setError] = useState<string | null>(null)
  const [deviceName, setDeviceName] = useState<string>('')

  const device = new Device(route.params.deviceId)

  useEffect(() => {
    const getDeviceName = async () => {
      const name = await device.getName()
      setDeviceName(name)
      navigation.setOptions({ title: device.name + ' - Historical Data' })
    }

    const getReadings = async () => {
      try {
        const data = await device.historyReadings()
        setReadings(data)
      } catch (error) {
        console.error(error)
        setError('Failed to fetch historical readings')
      }
    }

    getDeviceName()
    getReadings()
  }, [])

  if (error)
    return (
      <View style={styles.container}>
        <Text style={styles.text}>Error: {error}</Text>
      </View>
    )

  if (readings.length === 0)
    return (
      <View style={styles.container}>
        <Text style={styles.textBig}>Historical data empty</Text>
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
          <Text style={styles.textSmall}>Average Temperature</Text>
          <Text style={styles.textBigNumber}>{readings.reduce((sum, reading) => sum + reading.temperature, 0) / readings.length}</Text>
        </TouchableOpacity>
        <TouchableOpacity style={styles.panel}>
          <Text style={styles.textSmall}>Average Humidity</Text>
          <Text style={styles.textBigNumber}>{readings.reduce((sum, reading) => sum + reading.humidity, 0) / readings.length}</Text>
        </TouchableOpacity>
        <TouchableOpacity style={styles.panel}>
          <Text style={styles.textSmall}>Avergae Light Intensity</Text>
          <Text style={styles.textBigNumber}>{readings.reduce((sum, reading) => sum + reading.lightIntensity, 0) / readings.length}</Text>
        </TouchableOpacity>
      </View>
    </View>
  )
}

export default HistoricalData

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
    color: Config.COLOR_TEXT
  },
  textSmall: {
    color: Config.COLOR_TEXT,
    position: 'absolute',
    top: 25,
    left: 25
  },
  textBig: {
    fontSize: 25,
    color: Config.COLOR_TEXT
  },
  textBigNumber: {
    fontSize: 60,
    color: Config.COLOR_TEXT
  },
})