import { StyleSheet } from 'react-native'
import { NavigationContainer } from '@react-navigation/native'
import { createNativeStackNavigator } from '@react-navigation/native-stack'
import { StackParamList } from './navigation/types'
import SelectDevice from './components/SelectDevice'
import CurrentReadings from './components/CurrentReadings'
import Login from './components/Login'
import Register from './components/Register'
import HistoricalData from './components/HistoricalData'

const Stack = createNativeStackNavigator<StackParamList>()

const App = () => {
  return (
    <NavigationContainer>
      <Stack.Navigator screenOptions={{ headerStyle: styles.header, headerTitleStyle: styles.headerText, headerTintColor: 'white' }}>
        <Stack.Screen name='Login' component={Login} options={{ title: 'Login' }} />
        <Stack.Screen name='Register' component={Register} options={{ title: 'Register' }} />
        <Stack.Screen name='SelectDevice' component={SelectDevice} options={{ title: 'Device Selection' }} />
        <Stack.Screen name='CurrentReadings' component={CurrentReadings} options={({ route }) => ({ title: route.params.deviceId + ' - Current Readings' })} />
        <Stack.Screen name='HistoricalData' component={HistoricalData} options={({ route }) => ({ title: route.params.deviceId + ' - Historical Data' })} />
      </Stack.Navigator>
    </NavigationContainer>
  )
}

export default App

const styles = StyleSheet.create({
  header: {
    backgroundColor: '#202020',
  },
  headerText: {
    color: 'white',
  }
})
