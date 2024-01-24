import { View, Text, StyleSheet, TextInput, Button, Alert } from 'react-native'
import React, { useState } from 'react'
import Config from '../utils/Config'
import { NativeStackScreenProps } from '@react-navigation/native-stack'
import { StackParamList } from '../navigation/types'
import API from '../utils/API'
import { Device } from '../types/Device'

type LoginProps = NativeStackScreenProps<StackParamList, 'Login'>

const Login = ({ route, navigation }: LoginProps) => {
  const [email, setEmail] = useState<string>('')
  const [password, setPassword] = useState<string>('')

  const login = async () => {
    try {
      const response = await API.login(email, password)
      if (response.error)
        return Alert.alert(response.error)

      API.setAccessToken(response.accessToken)
      const devicesResponse = await API.devices()
      if (devicesResponse.error)
        return Alert.alert(devicesResponse.error)

      const devices: Device[] = devicesResponse.devicesIds.map((deviceId: string) => new Device(deviceId))

      navigation.navigate('SelectDevice', { deviceIds: devices.map(d => d.id) })
    } catch (error) {
      console.error(error)
    }
  }

  const goToRegister = () => {
    navigation.navigate('Register')
  }

  return (
    <View style={styles.container}>
      <View style={styles.form}>
        <TextInput style={styles.input} placeholder='E-Mail' onChangeText={setEmail} />
        <TextInput style={styles.input} placeholder='Password' onChangeText={setPassword} secureTextEntry={true} />
        <Button title='Login' color='#898989' onPress={login}></Button>
        <View>
          <Text style={{ color: 'white' }}>Don't have an account?</Text>
          <Button title='Register' color='#898989' onPress={goToRegister}></Button>
        </View>
      </View>
    </View>
  )
}

export default Login

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: Config.COLOR_BACKGROUND,
    justifyContent: 'center',
    alignItems: 'center'
  },
  form: {
    backgroundColor: Config.COLOR_CONTENT,
    padding: 50,
    borderRadius: 20,
    gap: 20
  },
  input: {
    width: 250,
    borderRadius: 2,
    padding: 10,
    backgroundColor: 'white'
  }
})