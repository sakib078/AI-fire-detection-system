#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define CAMERA_MODEL_ESP32S3_EYE
#include "camera_pins.h"

// UART Configuration
#define RXD1 21  // ESP32 RX (GPIO21) <-> K66 TX (PTC4)
#define TXD1 47  // ESP32 TX (GPIO47) <-> K66 RX (PTC3)
#define UART_BAUD_RATE 115200
HardwareSerial K66Serial(1);  // UART1

// WiFi and Server Configuration
const char* ssid = "your_wifi_name";
const char* password = "your_wifi_password";
String serverName = "http://192.168.2.18:5000";
String sensorPath = "/receive_sensor_data";
String imagePath = "/receive_image";

// Data Handling
String sensorDataBuffer = "";
bool newSensorData = false;
unsigned long lastSendTime = 0;
const int minSendInterval = 5000;

void setup() {
  Serial.begin(115200);
  K66Serial.begin(UART_BAUD_RATE, SERIAL_8N1, RXD1, TXD1);

  // Camera Configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_SVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 2;

  Serial.println("Starting camera init...");
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  Serial.println("Camera initialized successfully");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  handleSerialData();
  processCombinedData();
}

void handleSerialData() {
  while (K66Serial.available()) {
    char c = K66Serial.read();
    sensorDataBuffer += c;
    if (c == '\n') {
      Serial.print("Received from K66: ");
      Serial.println(sensorDataBuffer);
      newSensorData = true;
    }
  }
}

void processCombinedData() {
  if (!newSensorData || millis() - lastSendTime < minSendInterval) return;

  // Generate timestamp
  String timestamp = String(millis());

  // Capture image
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    sensorDataBuffer = "";
    newSensorData = false;
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    // Parse received sensor data and add timestamp
    StaticJsonDocument<300> doc;
    DeserializationError error = deserializeJson(doc, sensorDataBuffer);
    if (error) {
      Serial.print("JSON parse error: ");
      Serial.println(error.c_str());
    } else {
      doc["timestamp"] = timestamp;

      // Serialize to string
      String sensorDataWithTimestamp;
      serializeJson(doc, sensorDataWithTimestamp);

      // Send sensor data
      HTTPClient httpSensor;
      httpSensor.begin(serverName + sensorPath);
      httpSensor.addHeader("Content-Type", "application/json");
      int httpCodeSensor = httpSensor.POST(sensorDataWithTimestamp);
      if (httpCodeSensor > 0) {
        Serial.printf("Sensor data sent: HTTP %d\n", httpCodeSensor);
      } else {
        Serial.printf("Sensor data error: %s\n", httpSensor.errorToString(httpCodeSensor).c_str());
      }
      httpSensor.end();
    }

    // Send image with timestamp
    HTTPClient httpImage;
    httpImage.begin(serverName + imagePath);
    String boundary = "FireDetectionBoundary";
    String head = "--" + boundary + "\r\nContent-Disposition: form-data; name=\"timestamp\"\r\n\r\n" + 
                  timestamp + "\r\n" +
                  "--" + boundary + "\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"capture.jpg\"\r\n" +
                  "Content-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--" + boundary + "--\r\n";

    uint32_t imageLen = fb->len;
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;

    httpImage.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);

    uint8_t *buffer = (uint8_t *)malloc(totalLen);
    if (!buffer) {
      Serial.println("Failed to allocate buffer for image");
    } else {
      memcpy(buffer, head.c_str(), head.length());
      memcpy(buffer + head.length(), fb->buf, fb->len);
      memcpy(buffer + head.length() + fb->len, tail.c_str(), tail.length());

      int httpCodeImage = httpImage.POST(buffer, totalLen);
      if (httpCodeImage > 0) {
        Serial.printf("Image sent: HTTP %d\n", httpCodeImage);
      } else {
        Serial.printf("Image error: %s\n", httpImage.errorToString(httpCodeImage).c_str());
      }
      free(buffer);
    }
    httpImage.end();
  }

  esp_camera_fb_return(fb);
  sensorDataBuffer = "";
  newSensorData = false;
  lastSendTime = millis();
}

