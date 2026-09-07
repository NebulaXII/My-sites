import "dotenv/config";
import express from "express";
import { authRouter } from "./routes/auth.js";
import { manufacturingRouter } from "./routes/manufacturing.js";
import { devicesRouter } from "./routes/devices.js";
import { deviceApiRouter } from "./routes/deviceApi.js";
import { devRouter } from "./routes/dev.js";
import "./db.js"; // runs schema init as a side effect

const app = express();
app.use(express.json());

// Permissive CORS so a browser-hosted client (the web app, self-hosted or
// opened as a local file — an Artifact's sandbox blocks this entirely
// regardless of CORS, see ../README.md) can call this API cross-origin.
// Safe here because auth is a Bearer/Basic header, not a cookie — there's no
// CSRF exposure from allowing any origin. Restrict this to known origins
// before any real deployment, though; wildcard is a dev/test convenience.
app.use((req, res, next) => {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Manufacturing-Key");
  res.header("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  if (req.method === "OPTIONS") return res.sendStatus(204);
  next();
});

app.get("/api/health", (req, res) => res.json({ status: "ok" }));

app.use("/api/auth", authRouter);
app.use("/api/manufacturing", manufacturingRouter);
app.use("/api/devices", devicesRouter);
app.use("/api/device", deviceApiRouter); // device-facing, singular — distinct from /api/devices (user-facing)
app.use("/api/dev", devRouter);

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
