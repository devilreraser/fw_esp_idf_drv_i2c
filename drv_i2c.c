/* *****************************************************************************
 * File:   drv_i2c.c
 * Author: XX
 *
 * Created on YYYY MM DD
 * 
 * Description: ...
 * 
 **************************************************************************** */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "drv_i2c.h"

#include <string.h>

#include "sdkconfig.h"
#include "driver/i2c.h"
#include "esp_log.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define TAG "drv_i2c"

#define I2C_TX_BUF_SIZE             0 /*!< I2C master doesn't need buffer */
#define I2C_RX_BUF_SIZE             0 /*!< I2C master doesn't need buffer */
#define I2C_MODE                    I2C_MODE_MASTER

#define I2C_INTERFACE_COUNT         I2C_NUM_MAX

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#if CONFIG_DRV_I2C_0_SELECT_I2C1
#define DRV_I2C_0_PORT  I2C_NUM_0
#elif  CONFIG_DRV_I2C_0_SELECT_I2C2
#define DRV_I2C_0_PORT  I2C_NUM_1
#else
#define DRV_I2C_0_PORT  -1
#endif

#if CONFIG_DRV_I2C_2_SELECT_I2C1
#define DRV_I2C_1_PORT  I2C_NUM_2
#elif  CONFIG_DRV_I2C_1_SELECT_I2C2
#define DRV_I2C_1_PORT  I2C_NUM_1
#else
#define DRV_I2C_1_PORT  -1
#endif

#define DRV_I2C_0_SDA_GPIO      CONFIG_DRV_I2C_0_SDA_GPIO
#define DRV_I2C_0_SCL_GPIO      CONFIG_DRV_I2C_0_SCL_GPIO

#define DRV_I2C_1_SDA_GPIO      CONFIG_DRV_I2C_1_SDA_GPIO
#define DRV_I2C_1_SCL_GPIO      CONFIG_DRV_I2C_1_SCL_GPIO

#define DRV_I2C_0_FREQUENCY     CONFIG_DRV_I2C_0_FREQUENCY
#define DRV_I2C_1_FREQUENCY     CONFIG_DRV_I2C_1_FREQUENCY


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

static i2c_port_t i2c_port[I2C_INTERFACE_COUNT] = 
{
    DRV_I2C_0_PORT, 
    #if I2C_INTERFACE_COUNT > 1
    DRV_I2C_1_PORT,
    #endif
};
static int i2c_sda_pin[I2C_INTERFACE_COUNT] = 
{
    DRV_I2C_0_SDA_GPIO, 
    #if I2C_INTERFACE_COUNT > 1
    DRV_I2C_1_SDA_GPIO,
    #endif
};
static int i2c_scl_pin[I2C_INTERFACE_COUNT] = 
{
    DRV_I2C_0_SCL_GPIO, 
    #if I2C_INTERFACE_COUNT > 1
    DRV_I2C_1_SCL_GPIO,
    #endif
};

static int i2c_frequency[I2C_INTERFACE_COUNT] = 
{
    DRV_I2C_0_FREQUENCY, 
    #if I2C_INTERFACE_COUNT > 1
    DRV_I2C_1_FREQUENCY,
    #endif
};


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
esp_err_t drv_i2c_initialize_interface(drv_i2c_e_index_t i2c_index)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);


    if (i2c_index >= I2C_INTERFACE_COUNT) 
    {
        ESP_LOGE(TAG, "Interface Index %d not supported initialize", i2c_index);
        return ESP_ERR_INVALID_ARG;
    }
    else
    {
        i2c_driver_install(i2c_port[i2c_index], I2C_MODE, I2C_RX_BUF_SIZE, I2C_TX_BUF_SIZE, 0);

        i2c_config_t conf = {
            .mode = I2C_MODE,
            .sda_io_num = i2c_sda_pin[i2c_index],
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_io_num = i2c_scl_pin[i2c_index],
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = i2c_frequency[i2c_index],
            // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
        };
        esp_err_t error = i2c_param_config(i2c_index, &conf);
        if (error != ESP_OK)
        {
            ESP_LOGE(TAG, "Interface Index %d failure initialize", i2c_index);
        }
        else
        {
            ESP_LOGI(TAG, "Interface Index %d success initialize", i2c_index);
        }
        return error;
    }
}

esp_err_t drv_i2c_master_write_to_register( drv_i2c_e_index_t i2c_index, uint8_t device_address,
                                            const uint8_t* register_address, size_t register_address_size,
                                            const uint8_t* write_data_buffer, size_t write_data_size,
                                            TickType_t ticks_to_wait)
{
    if (i2c_index >= I2C_INTERFACE_COUNT) 
    {
        ESP_LOGE(TAG, "Interface Index %d not supported write", i2c_index);
        return ESP_ERR_INVALID_ARG;
    }
    else
    {  
        uint8_t* write_buffer = malloc(register_address_size + write_data_size);
        if (write_buffer == NULL)
        {
            ESP_LOGE(TAG, "Interface Index %d failure write", i2c_index);
            return ESP_ERR_NO_MEM;
        }
        memcpy(write_buffer, register_address, register_address_size);
        memcpy(write_buffer + register_address_size, write_data_buffer, write_data_size);
        size_t write_size = register_address_size + write_data_size;

        esp_err_t err;    
        err = i2c_master_write_to_device(   i2c_port[i2c_index], device_address,
                                            write_buffer, write_size,
                                            ticks_to_wait);
        free(write_buffer);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Interface Index %d failure write", i2c_index);
        }
        else
        {
            ESP_LOGD(TAG, "Interface Index %d success write", i2c_index);
        }
        return err;
    }
}

esp_err_t drv_i2c_master_read_from_register(drv_i2c_e_index_t i2c_index, uint8_t device_address,
                                            const uint8_t* register_address, size_t register_address_size,
                                            uint8_t* read_data_buffer, size_t read_data_size,
                                            TickType_t ticks_to_wait)
{
    if (i2c_index >= I2C_INTERFACE_COUNT) 
    {
        ESP_LOGE(TAG, "Interface Index %d not supported read", i2c_index);
        return ESP_ERR_INVALID_ARG;
    }
    else
    {        
        esp_err_t err;      
        err = i2c_master_write_read_device( i2c_port[i2c_index], device_address,
                                            register_address, register_address_size,
                                            read_data_buffer, read_data_size,
                                            ticks_to_wait);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Interface Index %d failure read", i2c_index);
        }
        else
        {
            ESP_LOGD(TAG, "Interface Index %d success read", i2c_index);
        }
        return err;
    }
}
