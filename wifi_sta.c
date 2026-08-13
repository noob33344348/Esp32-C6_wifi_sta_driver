/**
 * The documentation follows most of the steps that can be found at:
 * https://docs.espressif.com/projects/esp-idf/en/v5.2/esp32c6/api-guides/wifi.html
 */

#include "wifi_sta.h"
#include "environment.h"

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs_flash.h"


#define MAX_TRIES 5

// Private global variables
static const char* TAG = "wifi_sta";

static bool status = false; // true connected, false otherwise
static bool disconnecting = false;
static uint8_t tries = 0;

esp_err_t try_wifi_connect()
{
    tries++;
    esp_err_t ret = esp_wifi_connect();
    #if LOG
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Connection failed: %s", esp_err_to_name(ret));
    }
    #endif

    return ret;
}

// EVENT HANDLERS

static void wifi_event(void *event_handler_arg,
                       esp_event_base_t event_base,
                       int32_t event_id,
                       void *event_data)
{

    switch(event_id)
    {
        // Step 4 - Wi-Fi Connected Phase
        case WIFI_EVENT_STA_START:
            status = false;
            disconnecting = false;
            try_wifi_connect();
            break;

        case WIFI_EVENT_STA_CONNECTED:
            tries = 0;
            status = true;
            break;

        // Step 6 or Failed Step 3|4
        case WIFI_EVENT_STA_DISCONNECTED:
            status = false;
            if (!disconnecting && tries < MAX_TRIES)
            {
                try_wifi_connect();
            }
            else
            {
                // Closing TCP/UDP sockets?
            }
            break;

    }
}

static void ip_event(void *event_handler_arg,
                       esp_event_base_t event_base,
                       int32_t event_id,
                       void *event_data)
{
    switch(event_id)
    {
        // Step 5 & 7
        case IP_EVENT_STA_GOT_IP:
            ip_event_got_ip_t *event_ip = (ip_event_got_ip_t *)event_data;
            if(event_ip->ip_changed)
            {
                // Step 7 - Wi-Fi IP Change Phase
                // Closing TCP/UDP sockets?
            }
            else
            {
                // Step 5 - Wi-Fi 'Got IP' Phase
                // Creating TCP/UDP sockets?
            }
            break;
    }
}

// INTERFACE FUNCTIONS

// Step 1 & 2
esp_err_t my_wifi_init(void)
{
    esp_err_t ret;

    // Step 0 - Flash memory Init
    ret = nvs_flash_init();
    if(ret != ESP_OK)
    {
        #if LOG
        ESP_LOGE(TAG, "Error initializing the flash memory: %s", esp_err_to_name(ret));
        #endif
        return ret;
    }

    // Step 1 - Wi-Fi/LwIP Init Phase

    // s1.1
    ret = esp_netif_init();
    if(ret != ESP_OK)
    {
        #if LOG
        ESP_LOGE(TAG, "Error initializing the TCP/IP stack: %s", esp_err_to_name(ret));
        #endif
        return ret;
    }

    // s1.2
    ret = esp_event_loop_create_default();
    if(ret != ESP_OK)
    {
        #if LOG
        ESP_LOGE(TAG, "Error creating event loop: %s", esp_err_to_name(ret));
        #endif
        return ret;
    }

    // s1.3
    esp_netif_create_default_wifi_sta(); // Aborts in case of errors

    // Register WiFi event handler
    ret = esp_event_handler_register(WIFI_EVENT,  ESP_EVENT_ANY_ID, wifi_event, NULL);
    #if LOG
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error setting wifi event handler: %s", esp_err_to_name(ret));
    }
    #endif
    // Register IP event handler
    ret = esp_event_handler_register(IP_EVENT,  ESP_EVENT_ANY_ID, ip_event, NULL);
    #if LOG
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error setting ip event handler: %s", esp_err_to_name(ret));
    }
    #endif

    // s1.4
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if(ret != ESP_OK)
    {
        #if LOG
        ESP_LOGE(TAG, "Error allocating resources for WiFi: %s", esp_err_to_name(ret));
        #endif
        return ret;
    }

    // Enable maximum power-saving mode
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);

    // Step 2 - Wi-Fi Configuration Phase

    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if(ret != ESP_OK)
    {
        #if LOG
        ESP_LOGE(TAG, "Error setting station mode: %s", esp_err_to_name(ret));
        #endif
        return ret;
    }

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_STA_SSID,
            .password = CONFIG_WIFI_STA_PASSWORD,
            .threshold.authmode = AUTH_MODE,
            .scan_method = WIFI_FAST_SCAN
        },
    };
    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if(ret != ESP_OK)
    {
        #if LOG
        ESP_LOGE(TAG, "Error setting wifi configs: %s", esp_err_to_name(ret));
        #endif
        return ret;
    }

    return ESP_OK;
}

// Step 3 - Wi-Fi Start Phase
esp_err_t my_wifi_start(void)
{
    esp_err_t ret;
    ret = esp_wifi_start();
    if(ret != ESP_OK)
    {
        #if LOG
        ESP_LOGE(TAG, "Error starting wifi: %s", esp_err_to_name(ret));
        #endif
        return ret;
    }

    return ESP_OK;
}

esp_err_t my_wifi_reconnect(void)
{
    disconnecting = false;
    return try_wifi_connect();
}

// Step 6 - Wi-Fi Disconnect Phase
esp_err_t my_wifi_disconnect(void)
{
    esp_err_t ret;

    disconnecting = true;

    ret = esp_wifi_disconnect();
    if(ret != ESP_OK)
    {
        #if LOG
        ESP_LOGE(TAG, "Error disconnecting: %s", esp_err_to_name(ret));
        #endif
        return ret;
    }

    return ESP_OK;
}

// Step 8 - Wi-Fi Deinit Phase
esp_err_t my_wifi_deinit(void)
{
    esp_err_t ret;

    if(!disconnecting)
    {
        disconnecting = true;

        // Continue even if it failes because it could be already disconnected
        my_wifi_disconnect();
    }


    ret = esp_wifi_stop();
    if(ret != ESP_OK)
    {
        #if LOG
        ESP_LOGE(TAG, "Error stopping wifi: %s", esp_err_to_name(ret));
        #endif
        return ret;
    }

    ret = esp_wifi_deinit();
    if(ret != ESP_OK)
    {
        #if LOG
        ESP_LOGE(TAG, "Error unloading wifi: %s", esp_err_to_name(ret));
        #endif
        return ret;
    }

    return ESP_OK;
}

bool my_wifi_status(void)
{
    return status;
}
