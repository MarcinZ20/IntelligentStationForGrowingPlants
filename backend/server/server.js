const express = require("express");
const mongoose = require("mongoose");
const dotenv = require("dotenv");
const bodyParser = require("body-parser");
const bcrypt = require("bcrypt");
const jwt = require("jsonwebtoken");
const cors = require("cors");
const mqtt = require("mqtt");
const { expressjwt } = require("express-jwt");
const { User, Reading } = require("./models");

dotenv.config();

mongoose.connect(
  `mongodb+srv://${process.env.MONGO_USER}:${process.env.MONGO_PASS}@cluster0.cugn5q3.mongodb.net/iot`
);

const app = express();

const activeTokens = {};
const currentReadings = {};

app.use(cors());
app.use(bodyParser.json());
app.use(express.json());

const isPathExcluded = (req) => {
  const excludedPaths = [
    { path: "/api/login", method: "POST" },
    { path: "/api/register", method: "POST" },
    { path: "/api/refresh-token", method: "POST" },
    { path: "/api/devices/history", method: "PUT" },
  ];

  const excluded = excludedPaths.find(
    (excludedPath) =>
      excludedPath.path === req.path &&
      (excludedPath.method === req.method || !excludedPath.method)
  );

  return !!excluded;
};

app.use(
  expressjwt({
    secret: process.env.JWT_SECRET_ACCESS,
    algorithms: ["HS256"],
  }).unless(isPathExcluded)
);

const validateObjectId = (id) => /^[\da-f]{24}$/.test(id);

const signAccessToken = (user) =>
  jwt.sign({ id: user.id }, process.env.JWT_SECRET_ACCESS, {
    expiresIn: process.env.JWT_ACCESS_TOKEN_EXPIRE,
  });

const signRefreshToken = (user) =>
  jwt.sign({ id: user.id }, process.env.JWT_SECRET_REFRESH, {
    expiresIn: process.env.JWT_REFRESH_TOKEN_EXPIRE,
  });

app.post("/api/register", async (req, res) => {
  const { username, email, password } = req.body;

  const existingUser = await User.findOne({ email });
  if (existingUser)
    return res.status(400).json({ error: "User already exists" });

  const hashedPassword = await bcrypt.hash(password, 10);

  const newUser = new User({
    username,
    email,
    hashedPassword,
    devicesIds: [],
  });

  await newUser.save();

  res.status(201).json({ message: "User registered successfully" });
});

app.post("/api/login", async (req, res) => {
  const { email, password } = req.body;

  const user = await User.findOne({ email });
  if (!user || !(await user.comparePassword(password)))
    return res.status(401).json({ error: "Invalid credentials" });

  const accessToken = signAccessToken(user);
  const refreshToken = signRefreshToken(user);

  const hashedAccess = bcrypt.hashSync(
    accessToken.slice(-20),
    parseInt(process.env.TOKEN_SALT_ITER, 10)
  );
  const hashedRefresh = bcrypt.hashSync(
    refreshToken.slice(-20),
    parseInt(process.env.TOKEN_SALT_ITER, 10)
  );
  // NOTE: Slice(-20) is neccessary, as bcrypt only takes first 72 bytes, and tokens are usually idenctical there
  // Without it, it returned true (match) for different tokens

  activeTokens[user.id] = [hashedAccess, hashedRefresh];
  res.json({ accessToken, refreshToken });
});

app.post("/api/refresh-token", async (req, res) => {
  const refreshToken = req.body.refreshToken;

  jwt.verify(
    refreshToken,
    process.env.JWT_SECRET_REFRESH,
    async (err, decoded) => {
      if (err) return res.status(401).json({ error: "Invalid refresh token" });

      const user = await User.findById(decoded.id);
      if (!user) return res.status(401).json({ error: "User does not exist" });

      if (!activeTokens[decoded.id])
        return res.status(401).json({ error: "Unauthorized" });

      if (
        !bcrypt.compareSync(
          refreshToken.slice(-20),
          activeTokens[decoded.id][1]
        )
      )
        return res.status(401).json({ error: "Unauthorized" });

      const accessToken = signAccessToken(user);
      const hashedAccess = bcrypt.hashSync(
        accessToken.slice(-20),
        parseInt(process.env.TOKEN_SALT_ITER, 10)
      );

      activeTokens[decoded.id][0] = hashedAccess;

      res.json({ accessToken });
    }
  );
});

app.put("/api/devices/history", async (req, res) => {
  try {
    const { deviceId, humidity, temperature, light_intensity, timestamp } =
      req.body;

    const existingUser = await User.findOne({
      devicesIds: { $in: [deviceId] },
    });

    if (!existingUser)
      return res
        .status(401)
        .json({ error: "Device not registered for any user" });

    if (!deviceId || !timestamp)
      return res
        .status(400)
        .json({ error: "deviceId and timestamp are required fields" });

    const newReading = await Reading.create({
      deviceId,
      humidity,
      temperature,
      light_intensity,
      timestamp,
    });

    currentReadings[deviceId] = newReading;

    return res.json({
      message: "Reading added successfully",
      reading: newReading,
    });
  } catch (error) {
    console.error(error);
    return res.status(500).json({ error: "Internal Server Error" });
  }
});

app.use((req, res, next) => {
  const token =
    req.headers.authorization && req.headers.authorization.split(" ")[1];
  if (token) {
    jwt.verify(token, process.env.JWT_SECRET_ACCESS, async (err, decoded) => {
      if (err) return res.status(401).json({ error: "Unauthorized" });

      if (!activeTokens[decoded.id])
        return res.status(401).json({ error: "Unauthorized" });

      if (!bcrypt.compareSync(token.slice(-20), activeTokens[decoded.id][0])) {
        return res
          .status(403)
          .json({ error: "Unauthorized - Token no longer active" });
      }

      req.user = decoded;
      next();
    });
  } else {
    return res.status(401).json({ error: "Unauthorized" });
  }
});

app.post("/api/logout", async (req, res) => {
  const userId = req.auth.id;

  delete activeTokens[userId];
  res.status(200).json({ message: "Logged out" });
});

app.get("/api/users", async (req, res) => {
  const user = await User.findById(req.auth.id);

  const users = await User.find({}, { hashedPassword: 0 });
  res.json(users);
});

app.get("/api/userId", async (req, res) => {
  const user = await User.findById(req.auth.id);
  if (user) {
    res.json({ id: req.auth.id, username: user.username });
  } else
    return res
      .status(403)
      .json({ error: "Access denied. You must be logged in" });
});

app.get("/api/devices/:id/history", async (req, res) => {
  const user = await User.findById(req.auth.id);
  const deviceId = req.params.id;

  const found = user.devicesIds.find((x) => x === deviceId);
  if (found) {
    const readings = await Reading.find({ deviceId: deviceId });
    return res.json(readings);
  } else {
    return res.status(403).json({ error: "Couldn't find device" });
  }
});

app.delete("/api/devices/:id/history", async (req, res) => {
  const user = await User.findById(req.auth.id);
  const deviceId = req.params.id;

  const found = user.devicesIds.find((x) => x === deviceId);
  if (found) {
    await Reading.deleteMany({ deviceId });
    return res.json({ message: "History deleted successfully" });
  } else {
    return res.status(403).json({ error: "Couldn't find device" });
  }
});

app.get("/api/devices/history", async (req, res) => {
  const user = await User.findById(req.auth.id);

  const readings = await Reading.find({ deviceId: { $in: user.devicesIds } });

  return res.json({ readings });
});

app.get("/api/devices/:id/current", async (req, res) => {
  const user = await User.findById(req.auth.id);
  const deviceId = req.params.id;

  const found = user.devicesIds.find((x) => x === deviceId);
  if (found)
    if (currentReadings[deviceId])
      return res.json({ reading: currentReadings[deviceId] });
    else
      return res.status(404).json({ error: "No current reading for device" });
  else return res.status(404).json({ error: "No such device found for user" });
});

app.get("/api/devices", async (req, res) => {
  const user = await User.findById(req.auth.id);
  return res.json({ devicesIds: user.devicesIds });
});

app.post("/api/devices", async (req, res) => {
  const newDevicesIds = req.body.devicesIds;

  const existingUser = await User.findOne({
    _id: { $ne: req.auth.id }, // Exclude the current user
    devicesIds: { $in: newDevicesIds },
  });

  if (existingUser)
    return res.status(401).json({ error: "Device is used by another user" });

  await User.findByIdAndUpdate(
    req.auth.id,
    { devicesIds: newDevicesIds },
    { new: true }
  );
  return res.json({
    message: "Successfuly updated devices",
    devices: newDevicesIds,
  });
});

app.put("/api/devices", async (req, res) => {
  const newDevicesIds = req.body.devicesIds;
  console.log(newDevicesIds);

  const uniqueDevicesIds = [...new Set(newDevicesIds)];
  for (let deviceId of uniqueDevicesIds) await resetDevice(deviceId);
  const existingUser = await User.findOne({
    _id: { $ne: req.auth.id }, // Exclude the current user
    devicesIds: { $in: newDevicesIds },
  });

  if (existingUser)
    return res.status(401).json({ error: "Device is used by another user" });

  const updatedUser = await User.findOneAndUpdate(
    { _id: req.auth.id },
    { $addToSet: { devicesIds: { $each: uniqueDevicesIds } } },
    { new: true, upsert: true }
  );
  return res.json({
    message: "Successfuly updated devices",
    user: updatedUser,
  });
});

app.delete("/api/devices", async (req, res) => {
  const updatedUser = await User.findByIdAndUpdate(
    req.auth.id,
    { devicesIds: [] },
    { new: true }
  );

  return res.json({
    message: "Removed all devices from user",
    user: updatedUser,
  });
});

app.delete("/api/devices/:id", async (req, res) => {
  const user = await User.findById(req.auth.id);
  const deviceId = req.params.id;

  const index = user.devicesIds.findIndex((x) => x === deviceId);
  if (index !== -1) {
    user.devicesIds.splice(index, 1);
    const updatedUser = await user.save();
    return res.json({
      message: "Succesfully removed device",
      user: updatedUser,
    });
  } else {
    return res.status(404).json({ error: "Device not found" });
  }
});

app.use((err, req, res, next) => {
  if (err.name === "UnauthorizedError")
    res.status(401).json({ error: "Unauthorized" });
  else {
    console.error(err);
    res.status(500).send("Internal server error");
  }
});

app.listen(8000, () =>
  console.log("Server started and listening on port 5000")
);

// ------------ MQTT ------------ //

const brokerUrl = "mqtt://test.mosquitto.org"; // "mqtt://local
const clientId = "server-mqtt";

const client = mqtt.connect(brokerUrl, { clientId, port: 1883 });

client.on("connect", () => {
  console.log("Connected to MQTT broker at " + brokerUrl);
  client.subscribe("agh/iot/+/data");
  client.subscribe("agh/iot/+/+/reset");
});

client.on("message", (topic, message) => {
  let split = topic.split("/");
  if (split.at(-1) === "data") sendDataToServer(message.toString(), split[2]);
  else if (split.at(-1) === "reset") resetDevice(split[3]);
});

async function sendDataToServer(data, deviceId) {
  let json = JSON.parse(data);
  let modified = {
    deviceId,
    temperature: json.data.temperature,
    humidity: json.data.humidity,
    light_intensity: json.data.light,
    timestamp: new Date(),
  };

  const existingUser = await User.findOne({
    // Check if device used by someone
    devicesIds: { $in: [deviceId] },
  });

  if (deviceId && modified.timestamp && existingUser) {
    const newReading = await Reading.create({ ...modified });
    currentReadings[deviceId] = newReading;
  }
}

async function resetDevice(deviceId) {
  await Reading.deleteMany({ deviceId }); // remove reading history for the device

  const user = await User.findOne({
    // remove device from user it's linked to
    devicesIds: { $in: [deviceId] },
  });
  if (user) {
    const index = user.devicesIds.findIndex((x) => x === deviceId);
    if (index !== -1) {
      user.devicesIds.splice(index, 1);
      await user.save();
    }
  }
}
