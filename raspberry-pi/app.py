from flask import Flask, render_template, redirect, request, jsonify
import paho.mqtt.client as mqtt

app = Flask(__name__)

MQTT_BROKER = "localhost"
MQTT_PORT = 1883
MQTT_TOPIC_ENTRY = "gate/entry/command"
MQTT_TOPIC_EXIT  = "gate/exit/command"
MQTT_TOPIC_STATUS = "gate/status"   # ESP32 posts the count here

gate_status = {
    "entry_gate": "Closed",
    "exit_gate": "Closed",
    "vehicle_detected": False,
    "spots_available": 7,
    "spots_total": 7
}

# --- This runs whenever the ESP32 posts a new count ---
def on_message(client, userdata, msg):
    import json
    try:
        data = json.loads(msg.payload.decode())
        if "available" in data:
            gate_status["spots_available"] = int(data["available"])
            print(f"Updated spots: {gate_status['spots_available']}")
    except Exception as e:
        print(f"Bad message: {e}")

mqtt_client = mqtt.Client()
mqtt_client.on_message = on_message
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
mqtt_client.subscribe(MQTT_TOPIC_STATUS)
mqtt_client.loop_start()

@app.route("/")
def index():
    return render_template("index.html", status=gate_status)

@app.route("/status")
def status():
    return jsonify(gate_status)

@app.route("/open_entry")
def open_entry():
    mqtt_client.publish(MQTT_TOPIC_ENTRY, "OPEN")
    gate_status["entry_gate"] = "Open"
    return redirect("/")

@app.route("/open_exit")
def open_exit():
    mqtt_client.publish(MQTT_TOPIC_EXIT, "OPEN")
    gate_status["exit_gate"] = "Open"
    return redirect("/")

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)