import { NativeStackScreenProps } from '@react-navigation/native-stack'
import React, { useEffect, useState } from 'react'
import { Dimensions, ScrollView, StyleSheet, Text, TouchableOpacity, View } from 'react-native'
import Config from '../Config'
import { StackParamList } from '../navigation/types'
import { Device, Reading } from '../types/Device'
import { LineChart } from 'react-native-chart-kit'

type HistoricalDataProps = NativeStackScreenProps<StackParamList, 'HistoricalData'>

const formatDate = (date: Date): string => {
  return `${date.getDate()}.${(date.getMonth() + 1).toString().padStart(2, '0')} ${date.getHours()}:${date.getMinutes().toString().padStart(2, '0')}`
}

const screenWidth = Dimensions.get('screen').width

const HistoricalData = ({ route, navigation }: HistoricalDataProps) => {
  const [readings, setReadings] = useState<Reading[]>([])
  const [error, setError] = useState<string | null>(null)
  const [deviceName, setDeviceName] = useState<string>('')
  const [chartData, setChartData] = useState({
    datasets: [
      {
        data: [1],
        color: (opacity = 1) => `rgba(255, 0, 0, ${opacity})`
      },
      {
        data: [1],
        color: (opacity = 1) => `rgba(0, 255, 0, ${opacity})`
      },
      {
        data: [1],
        color: (opacity = 1) => `rgba(0, 0, 255, ${opacity})`
      },
    ],
    legend: ["A", "B", "C"],
    labels: ["test"]
  })

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
        console.log(data)
        setReadings(data)

        setChartData({
          datasets: [
            {
              data: data.map(reading => reading.temperature),
              color: (opacity = 1) => `rgba(235, 64, 52, ${opacity})`
            },
            {
              data: data.map(reading => reading.humidity),
              color: (opacity = 1) => `rgba(50, 168, 82, ${opacity})`
            },
            {
              data: data.map(reading => reading.light_intensity),
              color: (opacity = 1) => `rgba(66, 135, 245, ${opacity})`
            },
          ],
          legend: ['Temp [°C]', 'Humidity [%]', 'Light [lx]'],
          labels: data.map(reading => formatDate(new Date(reading.timestamp))).filter((label, index) => index % 10 === 0)
        })
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
    <ScrollView style={styles.background} contentContainerStyle={styles.scrollViewChildren}>
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
          <Text style={styles.textBigNumber}>{(readings.reduce((sum, reading) => sum + reading.temperature, 0) / readings.length).toFixed(1)} °C</Text>
        </TouchableOpacity>
        <TouchableOpacity style={styles.panel}>
          <Text style={styles.textSmall}>Average Humidity</Text>
          <Text style={styles.textBigNumber}>{(readings.reduce((sum, reading) => sum + reading.humidity, 0) / readings.length).toFixed(1)} %</Text>
        </TouchableOpacity>
        <TouchableOpacity style={styles.panel}>
          <Text style={styles.textSmall}>Average Light Intensity</Text>
          <Text style={styles.textBigNumber}>{(readings.reduce((sum, reading) => sum + reading.light_intensity, 0) / readings.length).toFixed(1)} lx</Text>
        </TouchableOpacity>
      </View>
      <View>
        <LineChart
          data={chartData}
          width={screenWidth - 40}
          height={280}
          chartConfig={{
            backgroundGradientFrom: Config.COLOR_CONTENT,
            backgroundGradientTo: Config.COLOR_CONTENT,
            decimalPlaces: 1,
            color: (opacity = 1) => `rgba(255, 255, 255, ${opacity})`,
            labelColor: (opacity = 1) => `rgba(255, 200, 255, ${opacity})`,
            style: {
              borderRadius: 16,
            },
            propsForDots: {
              r: '3',
              strokeWidth: '1',
              stroke: Config.COLOR_CONTENT,
            },
          }}
          bezier
          style={{
            marginVertical: 10,
            borderRadius: 16,
          }}
        />
      </View>
      <View>
        <LineChart
          data={{datasets: [chartData.datasets[0]], labels: chartData.labels, legend: [chartData.legend[0]]}}
          width={screenWidth - 40}
          height={280}
          chartConfig={{
            backgroundGradientFrom: Config.COLOR_CONTENT,
            backgroundGradientTo: Config.COLOR_CONTENT,
            decimalPlaces: 1,
            color: (opacity = 1) => `rgba(255, 255, 255, ${opacity})`,
            labelColor: (opacity = 1) => `rgba(255, 200, 255, ${opacity})`,
            style: {
              borderRadius: 16,
            },
            propsForDots: {
              r: '3',
              strokeWidth: '1',
              stroke: Config.COLOR_CONTENT,
            },
          }}
          bezier
          style={{
            marginVertical: 10,
            borderRadius: 16,
          }}
        />
      </View>
      <View>
        <LineChart
          data={{datasets: [chartData.datasets[1]], labels: chartData.labels, legend: [chartData.legend[1]]}}
          width={screenWidth - 40}
          height={280}
          chartConfig={{
            backgroundGradientFrom: Config.COLOR_CONTENT,
            backgroundGradientTo: Config.COLOR_CONTENT,
            decimalPlaces: 1,
            color: (opacity = 1) => `rgba(255, 255, 255, ${opacity})`,
            labelColor: (opacity = 1) => `rgba(255, 200, 255, ${opacity})`,
            style: {
              borderRadius: 16,
            },
            propsForDots: {
              r: '3',
              strokeWidth: '1',
              stroke: Config.COLOR_CONTENT,
            },
          }}
          bezier
          style={{
            marginVertical: 10,
            borderRadius: 16,
          }}
        />
      </View>
      <View>
        <LineChart
          data={{datasets: [chartData.datasets[2]], labels: chartData.labels, legend: [chartData.legend[2]]}}
          width={screenWidth - 40}
          height={280}
          chartConfig={{
            backgroundGradientFrom: Config.COLOR_CONTENT,
            backgroundGradientTo: Config.COLOR_CONTENT,
            decimalPlaces: 1,
            color: (opacity = 1) => `rgba(255, 255, 255, ${opacity})`,
            labelColor: (opacity = 1) => `rgba(255, 200, 255, ${opacity})`,
            style: {
              borderRadius: 16,
            },
            propsForDots: {
              r: '3',
              strokeWidth: '1',
              stroke: Config.COLOR_CONTENT,
            },
          }}
          bezier
          style={{
            marginVertical: 10,
            borderRadius: 16,
          }}
        />
      </View>
    </ScrollView>
  )
}

export default HistoricalData

const styles = StyleSheet.create({
  background: {
    flex: 1,
    backgroundColor: Config.COLOR_BACKGROUND,
  },
  scrollViewChildren: {
    alignItems: 'center'
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
    width: (screenWidth / 2) - 38,
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
    fontSize: 35,
    color: Config.COLOR_TEXT
  },
})