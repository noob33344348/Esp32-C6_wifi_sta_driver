#ifndef WIFI_STA_H
#define WIFI_STA_H

#include "esp_err.h"

#define LOG true

/**
 *
 * @brief Initialize WiFi in station (STA) mode.
 *
 * Set up the WiFi interface.
 * You can use the event group to wait for a connection and IP address assignment.
 *
 * Important!
 * You must call wifi_deinit before calling this function again.
 *
 *
 * @return
 *  - ESP_OK on success
 *  - Other errors on failure. See esp_err.h for error codes.
 */
esp_err_t my_wifi_init(void);

/**
 * @brief Connect to a WiFi network
 *
 * @return
 *  - ESP_OK on success
 *  - Other errors on failure. See esp_err.h for error codes.
 */
esp_err_t my_wifi_start(void);

/**
 *
 * @brief Disable WiFi
 *
 * @return
 *  - ESP_OK on success
 *  - Other errors on failure. See esp_err.h for error codes.
 */
esp_err_t my_wifi_disconnect(void);

/**
 * @brief Attempt to reconnect WiFi
 *
 * @return
 *  - ESP_OK on success
 *  - Other errors on failure. See esp_err.h for error codes.
 */
esp_err_t my_wifi_reconnect(void);

/**
 *
 * @brief Deinitialize WiFi
 *
 * @return
 *  - ESP_OK on success
 *  - Other errors on failure. See esp_err.h for error codes.
 */
esp_err_t my_wifi_deinit(void);

/**
 *
 * @brief Get WiFi status
 *
 * @return
 *  - True: connected
 *  - False: not connected
 */
bool my_wifi_status(void);
#endif
