#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "nvs_flash.h"

#include <cstring>

#include "/home/intkonst/esp/esp-idf/components/json/cJSON/cJSON.h"

// #include "/home/intkonst/esp/DHT22-lib-for-esp-idf/include/DHT.h"
// #include "/home/intkonst/esp/DHT22-lib-for-esp-idf/include/DHT.hpp"

#include <string.h>
#include <string>

// Настройки Wi-Fi
#define WIFI_SSID "limon"
#define WIFI_PASS "iqag0550"

// Настройки UDP
#define HOST_IP_ADDR "192.168.235.74"  // IP сервера
#define PORT 5552                      // Порт сервера
#define UDP_BUF_SIZE 1024              // Размер буфера

namespace message_keys {
    auto NewConnection = "S0VZX05FV19DT05ORUNUSU9O";
    auto Ping = "S0VZX1BJTkc=";
    auto GetDeviceTelemetry = "S0VZX0dFVF9ERVZJQ0VfVEVMRU1FVFJZ";
    auto GetDeviceStatus = "S0VZX0dFVF9ERVZJQ0VfU1RBVFVT";
    auto StatusFailedConnection = "S0VZX1NUQVRVU19GQUlMRURfQ09OTkVDVElPTg==";
    auto StatusSuccessfulConnection = "S0VZX1NUQVRVU19TVUNDRVNTRlVMX0NPTk5FQ1RJT04=";
    auto Exit = "S0VZX0VYSVQ=";
}  // namespace message_keys

static const char* TAG = "UDP_EXAMPLE";
char client_ip[16];

// Объявляем функции перед использованием
static void wifi_event_handler(
    void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data
);
static void udp_client_task(void* pvParameters);

// Функция для подключения к Wi-Fi
static void wifi_connect(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id
    ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip
    ));

    wifi_config_t wifi_config = {
      .sta =
          {
              .ssid = WIFI_SSID,
              .password = WIFI_PASS,
              .scan_method = WIFI_ALL_CHANNEL_SCAN,
              .bssid_set = false,
              .bssid = {0},
              .channel = 0,
              .listen_interval = 0,
              .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
              .threshold =
                  {
                      .rssi = -127,
                      .authmode = WIFI_AUTH_WPA2_PSK,
                  },
              .pmf_cfg = {.capable = true, .required = false},
              .rm_enabled = true,
              .btm_enabled = true,
              .mbo_enabled = true,
              .ft_enabled = false,
              .owe_enabled = false,
          },
  };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi начал подключение к SSID:%s", WIFI_SSID);
}

// Обработчик событий Wi-Fi
static void wifi_event_handler(
    void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data
) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Повторное подключение к Wi-Fi...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Получен IP:" IPSTR, IP2STR(&event->ip_info.ip));
        esp_ip4_addr_t ip = event->ip_info.ip;

        // Форматируем IP-адрес в строку
        snprintf(client_ip, sizeof(client_ip), IPSTR, IP2STR(&ip));
    }
}
// UDP клиент
extern "C" void app_main() {
    // Инициализация NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Подключение к Wi-Fi
    wifi_connect();

    // Ждём подключения Wi-Fi (реализуйте это через событие IP_EVENT_STA_GOT_IP)
    // Временное решение - задержка (не рекомендуется для production)
    vTaskDelay(15000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Run UDP client");
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[1024];

    // 1. Создаём UDP-сокет
    ESP_LOGI(TAG, "Create socket");
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        ESP_LOGE(TAG, "Socket creation failed");
        return;
    }

    // Устанавливаем таймаут на получение данных (5 секунд)
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // 2. Настраиваем адрес сервера
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);

    ESP_LOGI(TAG, "Server IP: %s, Port: %d", HOST_IP_ADDR, PORT);

    // 3. Отправляем сообщение
    ESP_LOGI(TAG, "Try send message: %s", message_keys::NewConnection);
    if (sendto(
            sockfd, message_keys::NewConnection, strlen(message_keys::NewConnection), 0,
            (const struct sockaddr*) &servaddr, sizeof(servaddr)
        ) < 0) {
        ESP_LOGE(TAG, "Send failed, errno=%d", errno);
        close(sockfd);
        return;
    }

    // 4. Получаем ответ
    socklen_t len = sizeof(servaddr);
    ESP_LOGI(TAG, "Waiting for response...");
    int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*) &servaddr, &len);
    if (n < 0) {
        ESP_LOGE(TAG, "Recv failed, errno=%d", errno);
    } else {
        buffer[n] = '\0';
        ESP_LOGI(TAG, "Recv message: %s", buffer);
    }

    if (!std::strcmp(buffer, message_keys::StatusSuccessfulConnection)) {
        while (true) {
            socklen_t len = sizeof(servaddr);
            ESP_LOGI(TAG, "Waiting for response...");
            int n =
                recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*) &servaddr, &len);
            if (n < 0) {
                ESP_LOGE(TAG, "Recv failed, errno=%d", errno);
                sendto(sockfd, "", 0, 0, (const struct sockaddr*) &servaddr, sizeof(servaddr));
            } else {
                buffer[n] = '\0';
                ESP_LOGI(TAG, "Recv message: %s", buffer);
            }

            if (!std::strcmp(buffer, message_keys::GetDeviceStatus)) {
                ESP_LOGI(TAG, "GET_DEVICE_STATUS");

            } else if (!std::strcmp(buffer, message_keys::Ping)) {
                ESP_LOGI(TAG, "PING");
                sendto(
                    sockfd, message_keys::StatusSuccessfulConnection,
                    strlen(message_keys::StatusSuccessfulConnection), 0,
                    (const struct sockaddr*) &servaddr, sizeof(servaddr)
                );

            } else if (!std::strcmp(buffer, message_keys::GetDeviceTelemetry)) {
                ESP_LOGI(TAG, "GET_DEVICE_TELEMETRY");

                // Исходные данные
                float temperature = 24.1f;
                float humidity = 0.76f;
                float brightness = 0.56f;
                bool test = true;
                const char* ip_address = "192.168.1.1";  // или получайте реальный IP

                // Создаем JSON-объект
                cJSON* root = cJSON_CreateObject();
                if (root == NULL) {
                    // Обработка ошибки создания объекта
                    return;
                }

                // Создаем вложенный объект для IP-адреса
                cJSON* ip_object = cJSON_CreateObject();
                if (ip_object == NULL) {
                    cJSON_Delete(root);
                    return;
                }

                // Добавляем значения во вложенный объект
                cJSON_AddNumberToObject(ip_object, "temperature", temperature);
                cJSON_AddNumberToObject(ip_object, "humidity", humidity);
                cJSON_AddNumberToObject(ip_object, "brightness", brightness);
                cJSON_AddBoolToObject(ip_object, "test", test);

                // Добавляем вложенный объект в основной объект по ключу IP-адреса
                cJSON_AddItemToObject(root, client_ip, ip_object);

                // Сериализуем в строку без форматирования
                char* json_data = cJSON_PrintUnformatted(root);
                if (json_data == NULL) {
                    cJSON_Delete(root);
                    return;
                }

                // Отправляем данные
                sendto(
                    sockfd, json_data, strlen(json_data), 0, (const struct sockaddr*) &servaddr,
                    sizeof(servaddr)
                );

                // Освобождаем память
                free(json_data);
                cJSON_Delete(root);

            } else if (!std::strcmp(buffer, message_keys::Exit)) {
                ESP_LOGI(TAG, "EXIT");
                sendto(
                    sockfd, message_keys::Exit, strlen(message_keys::Exit), 0,
                    (const struct sockaddr*) &servaddr, sizeof(servaddr)
                );
                break;

            } else {
                continue;
            }
        }
    }
    ESP_LOGI(TAG, "close socket");
    close(sockfd);
}