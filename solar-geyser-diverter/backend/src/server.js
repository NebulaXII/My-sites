import "dotenv/config";
import express from "express";
import { authRouter } from "./routes/auth.js";
import { manufacturingRouter } from "./routes/manufacturing.js";
import { devicesRouter } from "./routes/devices.js";
import { deviceApiRouter } from "./routes/deviceApi.js";
import "./db.js"; // runs schema init as a side effect

const app = express();
app.use(express.json());

app.get("/api/health", (req, res) => res.json({ status: "ok" }));

app.use("/api/auth", authRouter);
app.use("/api/manufacturing", manufacturingRouter);
app.use("/api/devices", devicesRouter);
app.use("/api/device", deviceApiRouter); // device-facing, singular — distinct from /api/devices (user-facing)

app.use((req, res) => res.status(404).json({ error: "not found" }));

// Express's default error handler leaks stack traces to the client — this
// swaps in a version that logs the real error server-side but only ever
// tells the caller "internal error", not what actually happened.
app.use((err, req, res, next) => {
  console.error(err);
  res.status(500).json({ error: "internal error" });
});

const port = process.env.PORT || 8787;
app.listen(port, () => {
  console.log(`[server] Geyser Diverter backend listening on :${port}`);
});
