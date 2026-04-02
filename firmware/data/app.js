const relayGrid = document.getElementById("relayGrid");
const temperatureValue = document.getElementById("temperatureValue");
const motionValue = document.getElementById("motionValue");
const sampleTimeValue = document.getElementById("sampleTimeValue");

const relayNames = ["Light 1", "Light 2", "Appliance 1", "Appliance 2"];
let relayState = [false, false, false, false];

function formatMs(ms) {
  if (!ms && ms !== 0) return "--";
  const now = Date.now();
  const offset = now - performance.now();
  const stamp = new Date(offset + ms);
  return stamp.toLocaleTimeString();
}

function renderRelays() {
  relayGrid.innerHTML = "";

  relayState.forEach((on, index) => {
    const card = document.createElement("article");
    card.className = "relay-card";

    const title = document.createElement("p");
    title.className = "relay-title";
    title.textContent = relayNames[index] || `Relay ${index + 1}`;

    const stateLabel = document.createElement("span");
    stateLabel.className = "relay-state";
    stateLabel.textContent = on ? "State: ON" : "State: OFF";

    const btn = document.createElement("button");
    btn.className = `relay-btn ${on ? "" : "is-off"}`;
    btn.textContent = on ? "Turn OFF" : "Turn ON";
    btn.onclick = async () => {
      btn.disabled = true;
      try {
        const res = await fetch(`/api/relays/${index}/toggle`, { method: "POST" });
        if (!res.ok) throw new Error("toggle failed");
        const json = await res.json();
        updateRelays(json.channels || []);
      } catch (err) {
        console.error(err);
      } finally {
        btn.disabled = false;
      }
    };

    card.append(title, stateLabel, btn);
    relayGrid.append(card);
  });
}

function updateRelays(channels) {
  relayState = [0, 1, 2, 3].map((i) => Boolean(channels[i]));
  renderRelays();
}

function updateSensors(sensors) {
  const temp = sensors.temperatureC;
  temperatureValue.textContent = temp === null || Number.isNaN(temp) ? "N/A" : `${temp.toFixed(1)} C`;
  motionValue.textContent = sensors.motionDetected ? "Detected" : "No motion";
  sampleTimeValue.textContent = formatMs(sensors.sampledAtMs);
}

async function loadInitial() {
  const res = await fetch("/api/state");
  const state = await res.json();
  updateRelays(state.relays.channels || []);
  updateSensors(state.sensors || {});
}

function setupEvents() {
  const source = new EventSource("/events");

  source.addEventListener("system", (event) => {
    const payload = JSON.parse(event.data);
    updateRelays(payload.relays.channels || []);
    updateSensors(payload.sensors || {});
  });

  source.onerror = () => {
    source.close();
    setTimeout(setupEvents, 2000);
  };
}

loadInitial().catch((err) => console.error("Initial load failed", err));
setupEvents();
