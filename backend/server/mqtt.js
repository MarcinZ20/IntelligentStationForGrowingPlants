const mqtt = require('mqtt')
const http = require('http')

const brokerUrl = 'mqtt://test.mosquitto.org'
const clientId = 'server-mqtt'

const client = mqtt.connect(brokerUrl, {clientId, port: 1883 })

client.on('connect', () => {
  console.log('Connected to MQTT broker')
  client.subscribe('agh/iot/+/+/temperature')
  client.subscribe('agh/iot/+/+/reset')
})

client.on('message', (topic, message) => {
  console.log(`Received message on topic ${topic}: ${message.toString()}`)

  let split = topic.split("/")
  if ( split.at(-1) === "temperature" )
    sendDataToServer(message.toString(), split[3])
  else if ( split.at(-1) === "reset" )
    console.log("reset")
})

function sendDataToServer(data, deviceId) {
  let json = JSON.parse(data)
  let modified = 
  { deviceId , 
    temperature: json.data.temperature, 
    humidity: json.data.humidity, 
    light_intensity: json.data.light, 
    timestamp: new Date() 
  }

  const options = {
    hostname: 'localhost',
    port: 5000,
    path: '/api/devices/history',
    method: 'PUT',
    headers: {
      'Content-Type': 'application/json',
    },
  }

  const request = http.request(options, response => {
    let responseData = ''

    response.on('data', chunk => {
      responseData += chunk
    })

    response.on('end', () => {
      console.log('PUT request successful:', responseData)
    })
  })

  request.on('error', error => {
    console.error('PUT request failed:', error.message)
  })

  request.write(JSON.stringify(modified))
  request.end()
}
