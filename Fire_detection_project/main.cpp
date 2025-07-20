#include "mbed.h"
#include "DHT.h"
#include <cstring>

// Custom strlen function for UART write
size_t custom_strlen(const char *str) {
    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

// UART connection to ESP32-S3
static BufferedSerial esp32_uart(PTC4, PTC3, 115200);

// MQ-9 gas sensor
AnalogIn mq9(PTB4);

// DHT22 temperature and humidity sensor
DHT sensor(PTC16, DHT22);

int main() {
    // Configure UART
    esp32_uart.set_format(8, BufferedSerial::None, 1);
    printf("MQ-9 Gas Sensor and DHT22 with UART Communication\r\n");

    // Constants for MQ-9 gas sensor calculations
    const float R0 = 0.91;  // Calibration value
    const float RL = 10.0;  // Load resistance in kΩ

    while (true) {
        // Read MQ-9 sensor
        uint16_t gasValue = mq9.read_u16();
        float sensor_volt = (gasValue * 3.3f) / 65535.0f;
        float RS_gas = ((5.0f - sensor_volt) / sensor_volt) * RL;
        float ratio = RS_gas / R0;

        // Read DHT22 sensor
        int error = sensor.readData();
        float c = 0.0f, h = 0.0f;
        if (error == 0) {
            c = sensor.ReadTemperature(CELCIUS);
            h = sensor.ReadHumidity();
        } else {
            printf("DHT22 Error: %d\n", error);
            c = -1.0f;  // Indicate error
            h = -1.0f;
        }

        // Format combined data as JSON
        char data_buffer[256];  // Increased buffer size for additional data
        sprintf(data_buffer, "{\"gas\":{\"voltage\":%.2f,\"rs_gas\":%.2f,\"ratio\":%.2f},\"dht22\":{\"temp\":%.2f,\"humidity\":%.2f}}\n",
                sensor_volt, RS_gas, ratio, c, h);

        // Send to ESP32-S3 via UART
        esp32_uart.write(data_buffer, custom_strlen(data_buffer));

        // Debug output
        printf("Sent combined data: %s", data_buffer);

        // Wait 10 seconds
        ThisThread::sleep_for(10000ms);
    }
}