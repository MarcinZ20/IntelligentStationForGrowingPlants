import Config from '../Config'

let accessToken = ''

export default {
  setAccessToken: (token: string) => {
    accessToken = token
  },
  getAccessToken: () => accessToken,
  register: async (username: string, email: string, password: string) => {
    const response = await fetch(Config.BASE_URL + '/register', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ username, email, password })
    })

    return await response.json()
  },
  login: async (email: string, password: string) => {
    const response = await fetch(Config.BASE_URL + '/login', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ email, password })
    })

    return await response.json()
  },
  devices: async () => {
    const response = await fetch(Config.BASE_URL + '/devices', {
      method: 'GET',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${accessToken}`
      }
    })

    return await response.json()
  },
  addDevice: async (deviceId: string) => {
    const response = await fetch(Config.BASE_URL + '/devices', {
      method: 'PUT',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${accessToken}`
      },
      body: JSON.stringify({ devicesIds: [deviceId] })
    })

    return await response.json()
  },
  deleteDevice: async (deviceId: string) => {
    const response = await fetch(Config.BASE_URL + '/devices/' + deviceId, {
      method: 'DELETE',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${accessToken}`
      }
    })

    return await response.json()
  },
  getCurrentReadings: async (deviceId: string) => {
    const response = await fetch(Config.BASE_URL + '/devices/' + deviceId + '/current', {
      method: 'GET',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${accessToken}`
      }
    })

    return await response.json()
  },
  getHistoryReadings: async (deviceId: string) => {
    const response = await fetch(Config.BASE_URL + '/devices/' + deviceId + '/history', {
      method: 'GET',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${accessToken}`
      }
    })

    return await response.json()
  }
}