# **Urban Fire Detection System**

This independent project develops an early fire detection system for urban and industrial environments. By integrating AI-powered image analysis with environmental sensing, it addresses limitations in traditional fire detection systems, such as delayed responses and false alarms. The system enhances safety in high-risk areas like commercial buildings and industrial facilities.

---

## **📋 Project Overview**

The Urban Fire Detection System employs a multi-modal approach to detect fires at their earliest stages. It uses the ESP32-S3-WROOM camera module for visual monitoring, complemented by DHT22 temperature/humidity and MQ-9 smoke sensors for environmental data. A fire-specific YOLOv8 model, trained on 6,391 images with over 92% mAP@50, processes images to detect fire and smoke, while sensor data reduces false positives. The system runs on the FRDM-K66F board for local processing and operates offline, making it ideal for areas with limited connectivity.

### **Key Features**
- **🔥 AI-Powered Detection**: Utilizes a lightweight YOLOv8 model (6-12 MB) for reliable fire and smoke detection.
- **✅ Multi-Modal Validation**: Combines visual and sensor data to minimize false alarms.
- **🌐 Offline Capability**: Functions without constant internet, suitable for underserved areas.
- **🚨 Rapid Alerts**: Displays alerts locally on an LCD and supports integration with existing alarm systems.

---

## **⚙️ How It Works**

1. **📸 Image Capture**: The ESP32-S3-WROOM camera captures images at regular intervals and sends them to the Flask server.
2. **🌡️ Sensor Data Collection**: DHT22 and MQ-9 sensors collect temperature, humidity, and gas data, sent to the server as JSON.
3. **🧠 AI Inference**: The server uses a YOLOv8 model hosted on Roboflow to analyze images for fire or smoke with high accuracy.
4. **🤝 Decision Logic**: Visual predictions are cross-validated with sensor data (e.g., gas ratio < 10.0 or temperature > 25°C) to trigger fire or smoke alerts.
5. **🔔 Alerts**: Alerts are displayed locally and can be integrated with external systems for broader notification.

---

## **🛠️ Challenges Faced**

- **🔌 Hardware Integration**: Configured the ESP32-S3-WROOM camera with precise pin and memory setup.
- **🌐 Connectivity**: Resolved CP210x USB-to-UART bridge driver issues for stable communication.
- **🖥️ AI Model**: Fixed NumPy and PyTorch dependency conflicts by upgrading to Python 3.12.4 and PyTorch 2.2.2.
- **📡 Network Issues**: Adjusted protocols to ensure reliable image and sensor data transmission.

---

## **🚀 Future Improvements**

- **📈 Model Enhancement**: Train the YOLOv8 model with more data for improved accuracy.
- **🌍 IoT Integration**: Connect with IoT platforms for real-time emergency service alerts.
- **🔋 Power Optimization**: Reduce power consumption for remote deployments.
- **📷 Multi-Camera Support**: Expand coverage with additional cameras.

---

## **🌟 Why This Matters**

This system tackles critical gaps in urban fire safety by offering faster, more reliable detection than traditional systems. Its offline capability and reduced false positives make it practical for both urban and underserved areas, potentially saving lives and minimizing property damage.
