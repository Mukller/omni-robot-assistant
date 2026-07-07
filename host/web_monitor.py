#!/usr/bin/env python3
"""Web dashboard for Loki robot telemetry. Run on Raspberry Pi or any ROS2 host.
   Access: http://<robot-ip>:5000
"""
import math
import threading
import time
from flask import Flask, jsonify, render_template_string
import rclpy
from rclpy.node import Node
from nav_msgs.msg import Odometry
from sensor_msgs.msg import BatteryState, LaserScan, Imu
from geometry_msgs.msg import Twist

app = Flask(__name__)

state = {
    "x": 0.0, "y": 0.0, "theta_deg": 0.0,
    "vx": 0.0, "wz": 0.0,
    "battery_v": 0.0, "battery_pct": 0,
    "current_a": 0.0,
    "scan_min_m": 0.0, "scan_ranges": [],
    "imu_heading": 0.0,
    "cmd_vx": 0.0, "cmd_wz": 0.0,
    "connected": False,
    "last_odom_s": 0.0,
}

DASHBOARD = """<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Loki Monitor</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body { background: #0d1117; color: #e6edf3; font-family: system-ui, sans-serif; padding: 20px; }
    h1 { color: #58a6ff; margin-bottom: 16px; font-size: 20px; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(220px, 1fr)); gap: 12px; margin-bottom: 16px; }
    .card { background: #161b22; border: 1px solid #30363d; border-radius: 8px; padding: 14px; }
    .card h3 { color: #8b949e; font-size: 11px; text-transform: uppercase; letter-spacing: 0.05em; margin-bottom: 8px; }
    .val { font-size: 28px; font-weight: bold; color: #58a6ff; }
    .unit { font-size: 13px; color: #8b949e; margin-left: 4px; }
    .sub { font-size: 12px; color: #8b949e; margin-top: 4px; }
    .batt-bar { background: #21262d; border-radius: 4px; height: 8px; margin-top: 8px; }
    .batt-fill { height: 8px; border-radius: 4px; transition: width 0.5s; }
    .status { display: inline-block; width: 10px; height: 10px; border-radius: 50%; margin-right: 6px; }
    .ok { background: #3fb950; } .warn { background: #d29922; } .err { background: #f85149; }
    canvas { background: #161b22; border: 1px solid #30363d; border-radius: 8px; display: block; margin-top: 12px; }
  </style>
</head>
<body>
  <h1>🤖 Loki Robot Monitor</h1>
  <div class="grid">
    <div class="card">
      <h3>Status</h3>
      <div id="conn_status"><span class="status err"></span>Disconnected</div>
      <div class="sub" id="uptime"></div>
    </div>
    <div class="card">
      <h3>Position</h3>
      <div><span class="val" id="pos_x">—</span><span class="unit">m X</span></div>
      <div class="sub"><span id="pos_y">—</span> m Y &nbsp;|&nbsp; <span id="heading">—</span>°</div>
    </div>
    <div class="card">
      <h3>Velocity</h3>
      <div><span class="val" id="vx">—</span><span class="unit">m/s</span></div>
      <div class="sub">ω = <span id="wz">—</span> rad/s</div>
    </div>
    <div class="card">
      <h3>Battery</h3>
      <div><span class="val" id="batt_v">—</span><span class="unit">V</span>
           &nbsp;<span id="batt_pct" style="color:#3fb950;font-size:18px">—%</span></div>
      <div class="batt-bar"><div class="batt-fill" id="batt_bar" style="width:0%;background:#3fb950"></div></div>
      <div class="sub"><span id="curr">—</span> A draw</div>
    </div>
    <div class="card">
      <h3>LiDAR</h3>
      <div><span class="val" id="scan_min">—</span><span class="unit">m min</span></div>
      <div class="sub">Nearest obstacle</div>
    </div>
    <div class="card">
      <h3>Command</h3>
      <div><span class="val" id="cmd_vx">—</span><span class="unit">m/s</span></div>
      <div class="sub">ω = <span id="cmd_wz">—</span> rad/s</div>
    </div>
  </div>

  <canvas id="scan_canvas" width="360" height="360"></canvas>

  <script>
    function fmt(v, d=2) { return typeof v==='number' ? v.toFixed(d) : '—'; }

    function drawScan(canvas, ranges) {
      const ctx = canvas.getContext('2d');
      const cx = canvas.width/2, cy = canvas.height/2, r = cx - 20;
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      ctx.fillStyle = '#161b22';
      ctx.fillRect(0, 0, canvas.width, canvas.height);

      // Grid circles
      ctx.strokeStyle = '#21262d';
      ctx.lineWidth = 1;
      [0.25, 0.5, 0.75, 1.0].forEach(f => {
        ctx.beginPath(); ctx.arc(cx, cy, r*f, 0, Math.PI*2); ctx.stroke();
      });

      // Robot
      ctx.fillStyle = '#58a6ff';
      ctx.beginPath(); ctx.arc(cx, cy, 8, 0, Math.PI*2); ctx.fill();
      // Forward indicator
      ctx.strokeStyle = '#58a6ff'; ctx.lineWidth = 2;
      ctx.beginPath(); ctx.moveTo(cx, cy); ctx.lineTo(cx, cy - 18); ctx.stroke();

      if (!ranges || !ranges.length) return;
      const maxRange = 5.0;  // display max 5m
      ctx.fillStyle = 'rgba(88,166,255,0.4)';
      ctx.strokeStyle = '#58a6ff';
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.moveTo(cx, cy);
      ranges.forEach((d, i) => {
        const angle = (i / ranges.length) * Math.PI * 2 - Math.PI / 2;
        const dist = Math.min(isFinite(d) ? d : maxRange, maxRange);
        const px = cx + Math.cos(angle) * (dist / maxRange) * r;
        const py = cy + Math.sin(angle) * (dist / maxRange) * r;
        ctx.lineTo(px, py);
      });
      ctx.closePath();
      ctx.fill();
      ctx.stroke();
    }

    const canvas = document.getElementById('scan_canvas');
    let startTime = Date.now();

    function update() {
      fetch('/api/state').then(r => r.json()).then(d => {
        document.getElementById('conn_status').innerHTML =
          d.connected ? '<span class="status ok"></span>Connected'
                      : '<span class="status err"></span>Disconnected';
        document.getElementById('uptime').textContent =
          'Running ' + Math.floor((Date.now()-startTime)/1000) + 's';

        document.getElementById('pos_x').textContent = fmt(d.x);
        document.getElementById('pos_y').textContent = fmt(d.y);
        document.getElementById('heading').textContent = fmt(d.theta_deg, 1);
        document.getElementById('vx').textContent = fmt(d.vx, 3);
        document.getElementById('wz').textContent = fmt(d.wz, 3);
        document.getElementById('batt_v').textContent = fmt(d.battery_v, 1);
        document.getElementById('batt_pct').textContent = d.battery_pct + '%';
        document.getElementById('curr').textContent = fmt(d.current_a, 2);
        document.getElementById('scan_min').textContent = fmt(d.scan_min_m, 2);
        document.getElementById('cmd_vx').textContent = fmt(d.cmd_vx, 3);
        document.getElementById('cmd_wz').textContent = fmt(d.cmd_wz, 3);

        const pct = Math.max(0, Math.min(100, d.battery_pct));
        const bar = document.getElementById('batt_bar');
        bar.style.width = pct + '%';
        bar.style.background = pct > 50 ? '#3fb950' : pct > 20 ? '#d29922' : '#f85149';

        drawScan(canvas, d.scan_ranges);
      }).catch(() => {});
    }

    setInterval(update, 500);
    update();
  </script>
</body>
</html>"""


class TeleSub(Node):
    def __init__(self):
        super().__init__('web_monitor')
        self.create_subscription(Odometry,     '/odometry/filtered', self._odom_cb,  10)
        self.create_subscription(BatteryState, '/battery_state',     self._batt_cb,  10)
        self.create_subscription(LaserScan,    '/scan',              self._scan_cb,  10)
        self.create_subscription(Imu,          '/imu/data',          self._imu_cb,   10)
        self.create_subscription(Twist,        '/cmd_vel',           self._cmd_cb,   10)

    def _odom_cb(self, msg: Odometry):
        p = msg.pose.pose
        q = p.orientation
        # yaw from quaternion
        siny = 2.0 * (q.w * q.z + q.x * q.y)
        cosy = 1.0 - 2.0 * (q.y * q.y + q.z * q.z)
        yaw_deg = math.degrees(math.atan2(siny, cosy))
        state.update({
            "x": round(p.position.x, 3),
            "y": round(p.position.y, 3),
            "theta_deg": round(yaw_deg, 1),
            "vx": round(msg.twist.twist.linear.x, 3),
            "wz": round(msg.twist.twist.angular.z, 3),
            "connected": True,
            "last_odom_s": time.time(),
        })

    def _batt_cb(self, msg: BatteryState):
        state.update({
            "battery_v":   round(msg.voltage, 2),
            "battery_pct": int(msg.percentage * 100),
            "current_a":   round(abs(msg.current), 2),
        })

    def _scan_cb(self, msg: LaserScan):
        ranges = [r for r in msg.ranges]
        valid  = [r for r in ranges if math.isfinite(r) and r > 0.01]
        state.update({
            "scan_min_m": round(min(valid), 3) if valid else 0.0,
            "scan_ranges": [round(r, 2) if math.isfinite(r) else 99.0
                            for r in ranges[::4]],  # downsample 4x
        })

    def _imu_cb(self, msg: Imu):
        q = msg.orientation
        siny = 2.0 * (q.w * q.z + q.x * q.y)
        cosy = 1.0 - 2.0 * (q.y * q.y + q.z * q.z)
        state["imu_heading"] = round(math.degrees(math.atan2(siny, cosy)), 1)

    def _cmd_cb(self, msg: Twist):
        state.update({
            "cmd_vx": round(msg.linear.x, 3),
            "cmd_wz": round(msg.angular.z, 3),
        })


@app.route('/')
def index():
    return render_template_string(DASHBOARD)

@app.route('/api/state')
def api_state():
    # Mark disconnected if no odom for >2s
    if time.time() - state["last_odom_s"] > 2.0:
        state["connected"] = False
    return jsonify(state)


def ros_thread():
    rclpy.init()
    node = TeleSub()
    rclpy.spin(node)


if __name__ == '__main__':
    threading.Thread(target=ros_thread, daemon=True).start()
    app.run(host='0.0.0.0', port=5000, debug=False)
