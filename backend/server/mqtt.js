const mqtt = require('mqtt')
const http = require('http')

const brokerUrl = 'mqtt://test.mosquitto.org'
const clientId = 'server-mqtt'

const client = mqtt.connect(brokerUrl, {clientId, port: 1883 })

client.on('connect', () => {
  console.log('Connected to MQTT broker')
  client.subscribe('agh/iot/test')
})

client.on('message', (topic, message) => {
  console.log(`Received message on topic ${topic}: ${message.toString()}`)
  sendDataToServer()
})

function sendDataToServer(data) {
  const options = {
    hostname: 'localhost',
    port: 5000,
    path: '/api/devices/history',
    method: 'PUT',
    headers: {
      'Content-Type': 'application/json',
    },
  };

  const request = http.request(options, response => {
    let responseData = '';

    response.on('data', chunk => {
      responseData += chunk;
    });

    response.on('end', () => {
      console.log('PUT request successful:', responseData);
    });
  });

  request.on('error', error => {
    console.error('PUT request failed:', error.message);
  });

  request.write(JSON.stringify(data));
  request.end();
}
