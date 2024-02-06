#ifndef INTELLIGENTSTATIONFORGROWINGPLANTS_SSD1306_H
#define INTELLIGENTSTATIONFORGROWINGPLANTS_SSD1306_H

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "driver/i2c.h"
#include "stdint.h"
#include "ssd1306_fonts.h"
#include "string.h"

/**
 * @brief  I2C address.
 */
#define SSD1306_I2C_ADDRESS    ((uint8_t)0x3C)
#define SSD1306_WIDTH               128
#define SSD1306_HEIGHT              64

typedef void *ssd1306_handle_t;   /*handle of ssd1306*/

/**
 * @brief   device initialization
 *
 * @param   dev object handle of ssd1306
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t ssd1306_init(ssd1306_handle_t dev);

/**
 * @brief   Create and initialization device object and return a device handle
 *
 * @param   port     I2C port object handle
 * @param   dev_addr I2C device address of device
 *
 * @return
 *     - device object handle of ssd1306
 */
ssd1306_handle_t ssd1306_create(i2c_port_t port, uint16_t dev_addr);

/**
 * @brief   Delete and release a device object
 *
 * @param   dev object handle of ssd1306
 */
void ssd1306_delete(ssd1306_handle_t dev);

/**
 * @brief   draw point on (x, y)
 *
 * @param   dev object handle of ssd1306
 * @param   chXpos Specifies the X position
 * @param   chYpos Specifies the Y position
 * @param   chPoint fill point
 */
void ssd1306_fill_point(ssd1306_handle_t dev, uint8_t chXpos, uint8_t chYpos, uint8_t chPoint);

/**
 * @brief   display char on (x, y),and set size, mode
 *
 * @param   dev object handle of ssd1306
 * @param   chXpos Specifies the X position
 * @param   chYpos Specifies the Y position
 * @param   chSize char size
 * @param   chChr draw char
 * @param   chMode display mode
 */
void ssd1306_draw_char(ssd1306_handle_t dev, uint8_t chXpos,
                       uint8_t chYpos, uint8_t chChr, uint8_t chSize, uint8_t chMode);

/**
 * @brief   display number on (x, y),and set length, size, mode
 *
 * @param   dev object handle of ssd1306
 * @param   chXpos Specifies the X position
 * @param   chYpos Specifies the Y position
 * @param   chNum draw num
 * @param   chLen length
 * @param   chSize display size
 */

esp_err_t ssd1306_refresh_gram(ssd1306_handle_t dev);

/**
 * @brief   Clear screen
 *
 * @param   dev object handle of ssd1306
 * @param   chFill whether fill and fill char
 **/
void ssd1306_clear_screen(ssd1306_handle_t dev, uint8_t chFill);

/**
 * @brief   Displays a string on the screen
 *
 * @param   dev object handle of ssd1306
 * @param   chXpos Specifies the X position
 * @param   chYpos Specifies the Y position
 * @param   pchString Pointer to a string to display on the screen
 * @param   chSize char size
 * @param   chMode display mode
 **/
void ssd1306_draw_string(ssd1306_handle_t dev, uint8_t chXpos, uint8_t chYpos,
                         const uint8_t *pchString, uint8_t chSize, uint8_t chMode);

#ifdef __cplusplus
}
#endif

#endif //INTELLIGENTSTATIONFORGROWINGPLANTS_SSD1306_H
