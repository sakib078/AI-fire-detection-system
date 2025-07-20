#Project Overview

The Urban Fire Detection System uses a multi-modal approach to detect fires at their earliest stages. It leverages the ESP32-S3-WROOM camera module for visual monitoring, alongside DHT22 temperature/humidity and MQ-9 smoke sensors for environmental data. A fire-specific YOLOv8 model, trained on 6,391 images with over 92% mAP@50, processes images to detect fire and smoke, while sensor data validates these findings to reduce false positives. The system runs on the FRDM-K66F board for local processing and can operate offline, making it suitable for areas with limited connectivity.

#Key features:
AI-Powered Detection: Uses a lightweight YOLOv8 model (6-12 MB) for reliable fire and smoke detection.
Multi-Modal Validation: Combines visual and sensor data to minimize false alarms.
Offline Capability: Functions without constant internet access, ideal for underserved areas.
Rapid Alerts: Displays alerts locally on an LCD and can integrate with existing alarm systems.


#Documentation
Refer to the project report (report_fire_detection_system.docx) for detailed insights into the system's design, challenges, and implementation.

#How It Works

Image Capture: The ESP32-S3-WROOM camera captures images at regular intervals and sends them to the Flask server.
Sensor Data Collection: DHT22 and MQ-9 sensors collect temperature, humidity, and gas data, which are sent to the server as JSON.
AI Inference: The server uses a YOLOv8 model hosted on Roboflow to analyze images for fire or smoke, achieving high accuracy with a confidence threshold.
Decision Logic: The system cross-validates visual predictions with sensor data (e.g., gas ratio < 10.0 or temperature > 25°C) to determine if a fire or smoke alert should be triggered.
Alerts: Alerts are displayed locally and can be integrated with external systems for broader notification.

#Challenges Faced
Hardware Integration: Configuring the ESP32-S3-WROOM camera required precise pin and memory setup.
Connectivity: Resolved issues with the CP210x USB-to-UART bridge driver for stable communication.
AI Model: Fixed dependency conflicts between NumPy and PyTorch by upgrading to Python 3.12.4 and PyTorch 2.2.2.
Network Issues: Adjusted protocols to ensure reliable image and sensor data transmission.

#Future Improvements
Enhance the YOLOv8 model with additional training data for even higher accuracy.
Integrate with IoT platforms for real-time alerts to emergency services.
Optimize power consumption for deployment in remote areas.
Add support for multi-camera setups to cover larger areas.

#Why This Matters

This system addresses critical gaps in urban fire safety by providing faster, more reliable detection than traditional systems. Its ability to operate offline and reduce false positives makes it a practical solution for both urban and underserved areas, potentially saving lives and reducing property damage.
