#!/usr/bin/env python3
"""Simple Flask dashboard for robot telemetry via microROS."""
import json, threading
from flask import Flask, render_template_string, jsonify
import rclpy
from rclpy.node import Node
from nav_msgs.msg import Odometry
from sensor_msgs.msg import BatteryState

app = Flask(__name__)
state = {"x": 0, "y": 0, "theta": 0, "battery": 0}

class TeleSub(Node):
    def __init__(self):
        super().__init__("web_monitor")
        self.create_subscription(Odometry, "/odom", self._odom_cb, 10)
        self.create_subscription(BatteryState, "/battery", self._batt_cb, 10)
    def _odom_cb(self, msg):
        state["x"] = round(msg.pose.pose.position.x, 3)
        state["y"] = round(msg.pose.pose.position.y, 3)
    def _batt_cb(self, msg):
        state["battery"] = round(msg.percentage * 100, 1)

TMPL = """<!DOCTYPE html>
<html><head><title>Loki Monitor</title>
<style>body{background:#0d1117;color:#e6edf3;font-family:system-ui}</style>
</head><body>
<h2>Loki Robot Monitor</h2>
<pre id="data">Loading...</pre>
<script>
setInterval(()=>fetch('/api/state').then(r=>r.json()).then(d=>{
  document.getElementById('data').textContent=JSON.stringify(d,null,2);
}),500);
</script></body></html>"""

@app.route("/")
def index(): return render_template_string(TMPL)

@app.route("/api/state")
def api_state(): return jsonify(state)

def ros_thread():
    rclpy.init()
    node = TeleSub()
    rclpy.spin(node)

if __name__ == "__main__":
    threading.Thread(target=ros_thread, daemon=True).start()
    app.run(host="0.0.0.0", port=5000)
