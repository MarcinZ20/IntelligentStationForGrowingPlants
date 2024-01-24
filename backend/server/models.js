const mongoose = require('mongoose')
const bcrypt = require('bcrypt')

const userSchema = new mongoose.Schema(
  {
    username: {
      type: String,
      required: true
    },
    email: {
      type: String,
      required: true
    },
    hashedPassword: {
      type: String,
      required: true
    },
    devicesIds: {
      type: [String],
      required: true
    }
  },
  {
    methods: {
      comparePassword: async function (password) {
        return bcrypt.compare(password, this.hashedPassword)
      },
    },
  }
)

const User = mongoose.model('User', userSchema, 'users')

const Reading = mongoose.model('Reading', {
  deviceId: {
    type: String,
    required: true,
  },
  humidity: {
    type: Number
  },
  temperature: {
    type: Number
  },
  light_intensity: {
    type: Number
  },
  timestamp: {
    type: Date,
    required: true
  }
}, 'readings')

module.exports = {
  User,
  Reading
}