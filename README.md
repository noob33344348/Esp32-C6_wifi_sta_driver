# Esp32-C6_wifi_sta_driver
A simple wifi station-mode driver for Esp32-C6.

## Features:
- Initialize WiFi interface.
- Connect to a WiFi network.
- Disconnect from the WiFi network.
- Attempt to reconnect to the WiFi network.
- Get WiFi status.
- Deinitialize WiFi interface.

### Important macros:
- MAX_TRIES: the number of tries the driver tries when connecting (or reconnecting) to a WiFi network;
after that number it considers the network unreachable.
- LOG: If set to true logging is enabled.
- TAG: Tag used in the logging (if enabled).

### Example:
```cpp
esp_err_t ret;

// Wifi connection
ret = my_wifi_init();
if(ret != ESP_OK)
{
    #if DEBUG
    ESP_LOGE(TAG, "Failed: %s", esp_err_to_name(ret));
    #endif
    manage_failure(ret);
    return;
}

ret = my_wifi_start();
if(ret != ESP_OK)
{
    #if DEBUG
    ESP_LOGE(TAG, "Failed: %s", esp_err_to_name(ret));
    #endif
    manage_failure(ret);
    return;
}

// Main loop
while (1)
{
    // Try to reconnect until it works
    while(my_wifi_status() == false)
    {
        ret = my_wifi_reconnect();
        if(ret != ESP_OK)
        {
            #if DEBUG
            ESP_LOGE(TAG, "Failed: %s", esp_err_to_name(ret));
            #endif
            manage_failure(ret);
            return;
        }
    }

    // Actual code...
}
```
