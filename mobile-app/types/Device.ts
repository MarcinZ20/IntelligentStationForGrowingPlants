import AsyncStorage from "@react-native-async-storage/async-storage"
import API from "../utils/API"

export class Device {
  public id: string
  public name: string

  constructor(id: string) {
    this.id = id
    this.name = id
   }

  getName = async () => {
    const name = await AsyncStorage.getItem(this.id)
    if (name) {
      this.name = name
      return this.name
    }

    return this.id
  }

  currentReadings = async (): Promise<Reading | null> => {
    const response = await API.getCurrentReadings(this.id)
    if (response.error)
      return null

    return response.reading
  }

  historyReadings = async (): Promise<Reading[]> => {
    const response = await API.getHistoryReadings(this.id)
    if (response.error)
      return []
    
    return response
  }
}

export type Reading = {
  temperature: number,
  humidity: number,
  light_intensity: number
}