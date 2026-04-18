#include "esp_event.h" // Thư viện quản lý sự kiện (như khi kết nối/mất WiFi)
#include "esp_log.h"   // Thư viện in log màu sắc ra Terminal (thay cho printf)
#include "esp_netif.h" // Lớp giao diện mạng (chuyển đổi dữ liệu thô sang gói tin mạng)
#include "esp_wifi.h"          // Thư viện điều khiển driver WiFi của ESP32
#include "freertos/FreeRTOS.h" // Hệ điều hành thời gian thực (RTOS)
#include "freertos/task.h"     // Quản lý đa nhiệm (Task)
#include "mdns.h"              // Giao thức tự tìm IP thiết bị trong mạng nội bộ
#include "mqtt_client.h"       // Giao thức truyền tin MQTT (Publish/Subscribe)
#include "nvs_flash.h" // Bộ nhớ không tự xóa (dùng để lưu cấu hình WiFi)
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Header linh kiện
#include "driver/i2c.h" // Driver điều khiển ngoại vi I2C
#include "oledi2c.h"    // Thư viện màn hình OLED
#include "sht30.h"      // Thư viện cảm biến nhiệt độ SHT30

static const char *TAG = "TEMP_LOG"; // Tên thẻ để lọc log trong Terminal

// 1. CẤU HÌNH THÔNG SỐ
#define WIFI_SSID "UIT Public"
#define WIFI_PASS ""
#define SERVER_HOST "lenovo-slim-3" // Tên máy tính (Hostname) để mDNS đi tìm

#define I2C_MASTER_SCL_IO 22      // Chân SCL nối vào GPIO 22
#define I2C_MASTER_SDA_IO 21      // Chân SDA nối vào GPIO 21
#define I2C_MASTER_NUM I2C_NUM_0  // Sử dụng bộ I2C số 0 của ESP32
#define I2C_MASTER_FREQ_HZ 100000 // Tốc độ I2C (100kHz)

// 2. BITMAP
// 2.1 Icon ống nhiệt kế 32x32 pixel = 128 byte
const uint8_t icon_temp_32x32[128] = {
    0x00, 0x00, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0xfc, 0x06, 0x03, 0x01,
    0x03, 0xfe, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60,
    0x69, 0x69, 0x09, 0x00, 0xff, 0x00, 0x00, 0xe0, 0x00, 0xff, 0xff, 0x00,
    0x00, 0xf8, 0xfc, 0x06, 0x06, 0x06, 0x06, 0x0c, 0x1c, 0x00, 0x00, 0x1c,
    0x16, 0x1e, 0x0c, 0x00, 0x00, 0x00, 0x60, 0x60, 0x69, 0x69, 0x09, 0x00,
    0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x07, 0x1f, 0x18,
    0x10, 0x10, 0x10, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x63, 0xcc, 0xde, 0x97,
    0xdc, 0xc1, 0x77, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// 2.2 Icon giọt nước cho độ ẩm 32x32 pixel = 128 byte
const uint8_t icon_hum_32x32[128] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x70, 0x3c, 0x1e, 0x0e,
    0x0e, 0x1c, 0x38, 0xf0, 0xe0, 0xc0, 0x80, 0xc0, 0xe0, 0x70, 0x3c, 0x1c,
    0x1c, 0x38, 0xf0, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x80, 0xe0, 0xf8, 0x7c,
    0x1f, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x03, 0x0f, 0x1f, 0x7d, 0xf0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x03, 0xdf, 0xfe, 0xf8, 0x3f, 0xff, 0xf3, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xf0, 0x20,
    0x80, 0xff, 0xff, 0x3f, 0x38, 0x38, 0x18, 0x1c, 0x0e, 0x07, 0x03, 0x00,
    0x00, 0x01, 0x03, 0x07, 0x0e, 0x1c, 0x38, 0x38, 0x70, 0x70, 0x70, 0x60,
    0x60, 0x74, 0x76, 0x37, 0x3b, 0x3d, 0x1c, 0x0f, 0x07, 0x03, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Handler để điều khiển MQTT
esp_mqtt_client_handle_t mqtt_client = NULL;

// 3. MODULE MẠNG (WIFI -> MDNS -> MQTT)
/**
 * @brief mDNS:ESP32 hét lên "Ai là TEN_HOST_LAPTOP ?"
 */
static void resolve_server_and_start_mqtt() {
  struct esp_ip4_addr addr;
  addr.addr = 0; // Khởi tạo biến chứa địa chỉ IP tìm được

  ESP_LOGI(TAG, "Đang tìm %s.local...", SERVER_HOST);

  // Gửi gói tin mDNS đi tìm IP của Hostname trong n ms ví dụ ở đây là 3000ms
  // tức 3 giây
  esp_err_t err = mdns_query_a(SERVER_HOST, 3000, &addr);

  if (err == ESP_OK) { // Nếu tìm thấy Laptop
    char ip_str[16];
    sprintf(ip_str, IPSTR,
            IP2STR(&addr)); // Chuyển số IP sang chuỗi "192.168.x.x"
    ESP_LOGI(TAG, "Đã thấy Server tại: %s", ip_str);

    // Tạo chuỗi URI kết nối: ví dụ "mqtt://192.168.1.15:1883"
    char uri[64];
    sprintf(uri, "mqtt://%s:1883", ip_str);

    // Cấu hình MQTT với địa chỉ vừa tìm được
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
    };

    // Khởi tạo và bắt đầu chạy Client MQTT (nó sẽ chạy ngầm dưới dạng 1 Task)
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(mqtt_client);
  } else {
    ESP_LOGE(
        TAG,
        "Không tìm thấy laptop! Kiểm tra xem máy tính có đang bật mDNS không.");
  }
}

/**
 * @brief Handler: Hàm này tự động được gọi mỗi khi có "event" về WiFi/IP
 */
static void event_handler(void *arg, esp_event_base_t base, int32_t id,
                          void *data) {
  // Nếu WiFi vừa khởi động xong -> Ra lệnh cho nó đi tìm Router để nối
  if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  }
  // Nếu đang nối mà bị rớt mạng -> Tiếp tục thử nối lại
  else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
    ESP_LOGW(TAG, "WiFi bị ngắt, đang kết nối lại...");
    esp_wifi_connect();
  }
  // Nếu Router đã cấp IP thành công cho ESP32
  else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)data;
    ESP_LOGI(TAG, "WiFi Connected! IP: " IPSTR, IP2STR(&event->ip_info.ip));

    // Khi có mạng rồi, mới bắt đầu đi tìm Laptop qua mDNS để nối MQTT
    resolve_server_and_start_mqtt();
  }
}

/**
 * @brief Thiết lập phần cứng WiFi (Antenna, bộ nhớ đệm, ngăn xếp giao thức)
 */
void wifi_init_sta(void) {
  // 1. Khởi tạo TCP/IP stack (cần thiết để gói tin có thể đi ra internet)
  ESP_ERROR_CHECK(esp_netif_init());

  // 2. Tạo vòng lặp sự kiện mặc định (nơi event_handler đăng ký lắng nghe)
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // 3. Cấu hình ESP32 thành chế độ Station (thiết bị khách nhận WiFi)
  esp_netif_create_default_wifi_sta();

  // 4. Khởi tạo tài nguyên phần cứng WiFi (RAM, Buffer)
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // 5. Đăng ký hàm event_handler để theo dõi sự kiện WiFi và IP
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

  // 6. Cài đặt SSID và Password cho chip
  wifi_config_t wifi_config = {
      .sta = {.ssid = WIFI_SSID, .password = WIFI_PASS},
  };
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

  // 7. Bật sóng WiFi lên
  ESP_ERROR_CHECK(esp_wifi_start());
}

// 4. MODULE CẢM BIẾN & HIỂN THỊ (LOGIC CHÍNH)
/**
 * @brief Thiết lập các thanh ghi cho bộ I2C Master
 */
static esp_err_t i2c_master_init(void) {
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .sda_pullup_en = GPIO_PULLUP_ENABLE, // Kích hoạt điện trở kéo lên cho SDA
      .scl_pullup_en = GPIO_PULLUP_ENABLE, // Kích hoạt điện trở kéo lên cho SCL
      .master.clk_speed = I2C_MASTER_FREQ_HZ,
  };
  i2c_param_config(I2C_MASTER_NUM, &conf);
  // Cài đặt driver vào hệ thống để bắt đầu truyền nhận
  return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

/**
 * @brief Task chạy song song: Vừa cập nhật OLED vừa bắn dữ liệu lên MQTT
 */
void sensor_display_task(void *pvParameters) {
  float temp = 0.0, hum = 0.0;
  char buffer[32];

  // Vẽ giao diện cố định (Static UI): Các chữ tiêu đề và Icon không thay đổi
  oled_draw_string(I2C_MASTER_NUM, 0, 0, "Temperature:");
  oled_draw_bitmap(I2C_MASTER_NUM, 96, 0, 32, 32, icon_temp_32x32);
  oled_draw_string(I2C_MASTER_NUM, 0, 4, "Humidity:");
  oled_draw_bitmap(I2C_MASTER_NUM, 96, 4, 32, 32, icon_hum_32x32);

  while (1) {
    // Đọc giá trị từ SHT30 qua I2C
    if (sht30_read(I2C_MASTER_NUM, &temp, &hum) == ESP_OK) {

      // --- PHẦN 1: CẬP NHẬT OLED ---
      sprintf(buffer, "%5.2f", temp);
      oled_draw_medium_numbers(I2C_MASTER_NUM, 0, 1, buffer);
      oled_draw_string(I2C_MASTER_NUM, 75, 1, "o");
      oled_draw_string(I2C_MASTER_NUM, 82, 2, "C");

      sprintf(buffer, "%5.2f", hum);
      oled_draw_medium_numbers(I2C_MASTER_NUM, 0, 5, buffer);
      oled_draw_string(I2C_MASTER_NUM, 80, 6, "%");

      // --- PHẦN 2: GỬI MQTT (Dữ liệu lên Web) ---
      if (mqtt_client != NULL) { // Chỉ gửi khi MQTT đã kết nối thành công
        char json_payload[128];
        // Tạo chuỗi JSON: {"temp": 32.50, "hum": 65.00}
        sprintf(json_payload, "{\"temp\": %.2f, \"hum\": %.2f}", temp, hum);

        // Publish lên Topic "room/sensor/data", QoS level 1 (đảm bảo tới nơi ít
        // nhất 1 lần)
        esp_mqtt_client_publish(mqtt_client, "room/sensor/data", json_payload,
                                0, 1, 0);
      }

      // In log ra Terminal máy tính để debug
      ESP_LOGI(TAG, "Dữ liệu: %.2f°C | %.2f%% | MQTT: %s", temp, hum,
               (mqtt_client ? "Connected" : "Waiting..."));

    } else {
      // Nếu hỏng dây I2C hoặc cảm biến chết thì báo lỗi lên màn hình
      ESP_LOGE(TAG, "Lỗi đọc cảm biến SHT30!");
      oled_draw_string(I2C_MASTER_NUM, 0, 1, "Sensor Err!  ");
    }

    // Nghỉ 2 giây trước khi lặp lại (giảm tải cho CPU và mạng)
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

// 5. ĐIỂM BẮT ĐẦU CHƯƠNG TRÌNH (ENTRY POINT)
void app_main(void) {
  // 1. NVS Flash: WiFi cần bộ nhớ này để lưu các thông số hiệu chuẩn sóng vô
  // tuyến (RF)
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(
        nvs_flash_erase()); // Nếu lỗi bộ nhớ thì xóa đi khởi tạo lại
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // 2. Khởi tạo I2C và màn hình OLED đầu tiên
  ESP_ERROR_CHECK(i2c_master_init());
  oled_init(I2C_MASTER_NUM);
  oled_clear(I2C_MASTER_NUM);

  // 3. Khởi tạo mDNS và bắt đầu quy trình nối WiFi
  mdns_init();     // Bật dịch vụ nhận diện tên miền cục bộ
  wifi_init_sta(); // Bật sóng WiFi và đăng ký sự kiện kết nối

  // 4. Tạo một Task (Tiến trình) chạy độc lập để xử lý cảm biến và hiển thị
  // "sensor_task": Tên task
  // 4096: Kích thước vùng nhớ (Stack size) cấp cho task này
  // 5: Độ ưu tiên (Priority) - Càng cao càng quan trọng
  xTaskCreate(sensor_display_task, "sensor_task", 4096, NULL, 5, NULL);
}
