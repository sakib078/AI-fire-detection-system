from flask import Flask, request, jsonify
import os
import json
from imageai.Detection import ObjectDetection
import time


app = Flask(__name__)

# Configure folders
UPLOAD_FOLDER = 'static/images'
SENSOR_FOLDER = 'static/sensor_data'
if not os.path.exists(UPLOAD_FOLDER):
    os.makedirs(UPLOAD_FOLDER)
if not os.path.exists(SENSOR_FOLDER):
    os.makedirs(SENSOR_FOLDER)
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
app.config['SENSOR_FOLDER'] = SENSOR_FOLDER


detector = ObjectDetection()
detector.setModelTypeAsYOLOv3()
detector.setModelPath("models\detection_model-ex-33--loss-4.97.h5")
detector.loadModel()

@app.route('/receive_sensor_data', methods=['POST'])
def receive_sensor_data():
    try:
        sensor_data = request.get_json()
        if not sensor_data or 'timestamp' not in sensor_data:
            return jsonify({'error': 'No JSON data or missing timestamp'}), 400

        timestamp = sensor_data['timestamp']
        sensor_filename = f"data_{timestamp}.json"  # Single file per timestamp
        sensor_path = os.path.join(app.config['SENSOR_FOLDER'], sensor_filename)

        # Save all sensor data (gas and dht22) in one file
        with open(sensor_path, 'w') as f:
            json.dump(sensor_data, f, indent=2)

        print(f"Sensor data saved: {sensor_path}")

        # Analyze gas data (example logic)
        gas_ratio = sensor_data.get('gas', {}).get('ratio', 100.0)
        alert_level = "Normal"
        if gas_ratio < 10.0:
            alert_level = "Warning: Elevated gas levels detected!"
        if gas_ratio < 3.0:
            alert_level = "DANGER: High gas concentration detected!"

        return jsonify({
            'message': 'Sensor data received and saved successfully',
            'filename': sensor_filename,
            'timestamp': timestamp,
            'alert_level': alert_level
        }), 200

    except Exception as e:
        print(f"Error processing sensor data: {str(e)}")
        return jsonify({'error': str(e)}), 500


@app.route('/receive_image', methods=['POST'])
def receive_image():
    if 'imageFile' not in request.files or 'timestamp' not in request.form:
        return jsonify({'error': 'Missing image file or timestamp'}), 400

    imageFile = request.files['imageFile']
    timestamp = request.form['timestamp']

    if imageFile.filename == '':
        return jsonify({'error': 'No selected file'}), 400

    # Save the image
    filename = f"image_{timestamp}.jpg"
    file_path = os.path.join(app.config['UPLOAD_FOLDER'], filename)
    imageFile.save(file_path)

    # Perform fire detection
    detections = detector.detectObjectsFromImage(input_image=file_path, minimum_percentage_probability=50)
    fire_detections = [d for d in detections if d['name'].lower() == 'fire']

    # Load corresponding sensor data
    sensor_path = os.path.join('static/sensor_data', f"data_{timestamp}.json")
    sensor_data = None
    if os.path.exists(sensor_path):
        with open(sensor_path, 'r') as f:
            sensor_data = json.load(f)
        gas_ratio = sensor_data.get('gas', {}).get('ratio', 100.0)
    else:
        gas_ratio = 100.0  # Default if no sensor data

    # Combine results for decision-making
    if fire_detections and gas_ratio < 10.0:
        final_alert = "Fire detected with high gas levels!"
    elif fire_detections:
        final_alert = "Fire detected"
    elif gas_ratio < 10.0:
        final_alert = "High gas levels detected"
    else:
        final_alert = "Normal"

    return jsonify({
        'message': final_alert,
        'fire_detections': fire_detections,
        'sensor_data': sensor_data,
        'timestamp': timestamp
    }), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)