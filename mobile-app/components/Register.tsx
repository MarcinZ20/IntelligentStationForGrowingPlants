import { View, Text, StyleSheet, TextInput, Button, Alert } from 'react-native'
import React, { useState } from 'react'
import Config from '../utils/Config'
import { NativeStackScreenProps } from '@react-navigation/native-stack'
import { StackParamList } from '../navigation/types'
import API from '../utils/API'

type RegisterProps = NativeStackScreenProps<StackParamList, 'Register'>

const Register = ({ route, navigation }: RegisterProps) => {
  const [email, setEmail] = useState<string>('')
  const [username, setUsername] = useState<string>('')
  const [password, setPassword] = useState<string>('')

  const register = async () => {
    try {
      const response = await API.register(username, email, password)
      if (response.error)
        return Alert.alert(response.error)

      Alert.alert(response.message)
      navigation.navigate('Login')
    } catch (error) {
      console.error(error)
    }
  }

  return (
    <View style={styles.container}>
      <View style={styles.form}>
        <TextInput style={styles.input} placeholder='E-Mail' onChangeText={setEmail} />
        <TextInput style={styles.input} placeholder='Username' onChangeText={setUsername} />
        <TextInput style={styles.input} placeholder='Password' onChangeText={setPassword} secureTextEntry={true} />
        <Button title='Register' color='#898989' onPress={register}></Button>
      </View>
    </View>
  )
}

export default Register

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