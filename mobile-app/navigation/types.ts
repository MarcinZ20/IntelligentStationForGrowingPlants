export type StackParamList = {
  SelectDevice: { deviceIds: string[] }
  CurrentReadings: { deviceId: string }
  Login: undefined
  Register: undefined
  HistoricalData: { deviceId: string }
  AddDevice: undefined
}