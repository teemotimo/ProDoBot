from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
from google import genai
import paho.mqtt.client as mqtt
import json

app = Flask(__name__)
CORS(app)

# ============= CONFIGURATION =============
GEMINI_API_KEY = "Gemini_API"
MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883

# ============= GEMINI SETUP =============
client = genai.Client(api_key=GEMINI_API_KEY)
MODEL_NAME = "gemini-2.0-flash-lite"

# ============= MQTT SETUP =============
mqtt_client = mqtt.Client()

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("✅ Connected to MQTT Broker!")
    else:
        print(f"❌ Failed to connect to MQTT, return code {rc}")

mqtt_client.on_connect = on_connect

try:
    mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
    mqtt_client.loop_start()
except Exception as e:
    print(f"⚠️ MQTT connection error: {e}")

# ============= SYSTEM PROMPT =============
SYSTEM_PROMPT = """You are a smart room controller. Convert natural language commands into JSON.

AVAILABLE DEVICES:

1. lighting (3 LED lights - red, green, blue)
   - color: "warm", "focus", "chill", "off"

2. motor (Cable roller)
   - action: "roll"

3. display (Mood display on LCD/matrix)
   - emotion: "moodChill", "moodWarm", "moodFocus"

4. timer (Countdown timer)
   - action: "start", "stop", "pause"
   - duration: seconds (integer)

IMPORTANT RULES:
- ALWAYS match lighting color with mood:
  - warm/moodWarm = red LED (happy, excited, party, energetic, warm)
  - focus/moodFocus = green LED (study, work, concentrate, focus)
  - chill/moodChill = blue LED (relax, calm, sleep, chill)
- When user mentions focus/study/work, ALWAYS include timer (default 25 min = 1500 sec)
- Return ONLY a valid JSON array, no other text

EXAMPLES:

User: "I want to focus" or "focus mode" or "I need to study"
[
  {"device": "lighting", "color": "focus"},
  {"device": "display", "emotion": "moodFocus"},
  {"device": "timer", "action": "start", "duration": 1500}
]

User: "warm" or "I feel warm" or "warm mode"
[
  {"device": "lighting", "color": "warm"},
  {"device": "display", "emotion": "moodWarm"}
]

User: "chill" or "relax" or "I want to relax"
[
  {"device": "lighting", "color": "chill"},
  {"device": "display", "emotion": "moodChill"}
]

User: "party mode" or "excited"
[
  {"device": "lighting", "color": "warm"},
  {"device": "display", "emotion": "moodWarm"}
]

User: "sleep mode" or "goodnight"
[
  {"device": "lighting", "color": "off"},
  {"device": "display", "emotion": "moodChill"}
]

User: "roll up the cable" or "roll cable"
[{"device": "motor", "action": "roll"}]

User: "set a 10 minute timer"
[{"device": "timer", "action": "start", "duration": 600}]

User: "turn off lights"
[{"device": "lighting", "color": "off"}]

User: "stop timer"
[{"device": "timer", "action": "stop"}]

Now convert this command: "{USER_COMMAND}"
Return ONLY the JSON array."""

# ============= PRESET COMMANDS =============
PRESETS = {
    "focus mode": [
        {"device": "lighting", "color": "focus"},
        {"device": "display", "emotion": "moodFocus"},
        {"device": "timer", "action": "start", "duration": 1500}
    ],
    "study mode": [
        {"device": "lighting", "color": "focus"},
        {"device": "display", "emotion": "moodFocus"},
        {"device": "timer", "action": "start", "duration": 1500}
    ],
    "warm mode": [
        {"device": "lighting", "color": "warm"},
        {"device": "display", "emotion": "moodWarm"}
    ],
    "chill mode": [
        {"device": "lighting", "color": "chill"},
        {"device": "display", "emotion": "moodChill"}
    ],
    "relax mode": [
        {"device": "lighting", "color": "chill"},
        {"device": "display", "emotion": "moodChill"}
    ],
    "party mode": [
        {"device": "lighting", "color": "warm"},
        {"device": "display", "emotion": "moodWarm"}
    ],
    "sleep mode": [
        {"device": "lighting", "color": "off"},
        {"device": "display", "emotion": "moodChill"}
    ],
    "turn everything off": [
        {"device": "lighting", "color": "off"},
        {"device": "display", "emotion": "moodChill"},
        {"device": "timer", "action": "stop"}
    ],
    "morning mode": [
        {"device": "lighting", "color": "warm"},
        {"device": "display", "emotion": "moodWarm"},
        {"device": "motor", "action": "roll"}
    ],
    "roll cable": [
        {"device": "motor", "action": "roll"}
    ]
}

# ============= SERVE FRONTEND =============
@app.route('/')
def home():
    return send_from_directory('static', 'index.html')

# ============= MAIN VOICE COMMAND ENDPOINT =============
@app.route('/voice-command', methods=['POST'])
def voice_command():
    data = request.json
    user_command = data.get('command', '').strip().lower()
    
    if not user_command:
        return jsonify({"error": "No command provided"}), 400
    
    print(f"\n📝 User command: {user_command}")
    
    if user_command in PRESETS:
        commands = PRESETS[user_command]
        print(f"✅ Using preset: {user_command}")
    else:
        try:
            prompt = SYSTEM_PROMPT.replace("{USER_COMMAND}", user_command)
            
            response = client.models.generate_content(
                model=MODEL_NAME,
                contents=prompt,
                config={
                    'temperature': 0.2,
                    'max_output_tokens': 500
                }
            )
            
            text = response.text.strip()
            
            if text.startswith('```json'):
                text = text.split('```json')[1].split('```')[0].strip()
            elif text.startswith('```'):
                text = text.split('```')[1].split('```')[0].strip()
            
            commands = json.loads(text)
            print(f"🤖 Gemini: {commands}")
            
        except json.JSONDecodeError as e:
            print(f"❌ JSON error: {e}")
            return jsonify({"error": "Failed to parse AI response"}), 500
        except Exception as e:
            print(f"❌ Gemini error: {e}")
            return jsonify({"error": str(e)}), 500
    
    try:
        sent_commands = []
        for cmd in commands:
            cmd_copy = cmd.copy()
            device = cmd_copy.pop('device')
            topic = f"room/{device}"
            payload = json.dumps(cmd_copy)
            
            result = mqtt_client.publish(topic, payload)
            
            if result.rc == 0:
                print(f"✅ {topic}: {payload}")
            else:
                print(f"❌ Failed: {topic}")
            
            sent_commands.append({"device": device, **cmd_copy})
        
        return jsonify({
            "success": True,
            "commands": sent_commands
        })
        
    except Exception as e:
        print(f"❌ MQTT error: {e}")
        return jsonify({"error": "Failed to send commands"}), 500

# ============= HEALTH CHECK =============
@app.route('/health', methods=['GET'])
def health():
    return jsonify({
        "status": "running",
        "mqtt_connected": mqtt_client.is_connected()
    })

# ============= START SERVER =============
if __name__ == '__main__':
    print("\n" + "="*50)
    print("🚀 Smart Room Assistant")
    print("="*50)
    print(f"📡 MQTT: {MQTT_BROKER}:{MQTT_PORT}")
    print(f"🌐 URL: http://localhost:5001")
    print("📦 Devices: lighting, motor, display, timer")
    print("="*50 + "\n")
    
    app.run(debug=False, host='0.0.0.0', port=5001)
