/* *****************************************************************************
 * File:   drv_i2c.h
 * Author: XX
 *
 * Created on YYYY MM DD
 * 
 * Description: ...
 * 
 **************************************************************************** */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
 #include "driver/i2c.h"
   
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    DRV_I2C_INDEX_0,
    DRV_I2C_INDEX_1

} drv_i2c_e_index_t;


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
esp_err_t drv_i2c_initialize_interface(drv_i2c_e_index_t i2c_index);
esp_err_t drv_i2c_master_write_to_register( drv_i2c_e_index_t i2c_index, uint8_t device_address,
                                            const uint8_t* register_address, size_t register_address_size,
                                            const uint8_t* write_data_buffer, size_t write_data_size,
                                            TickType_t ticks_to_wait);
esp_err_t drv_i2c_master_read_from_register(drv_i2c_e_index_t i2c_index, uint8_t device_address,
                                            const uint8_t* register_address, size_t register_address_size,
                                            uint8_t* read_data_buffer, size_t read_data_size,
                                            TickType_t ticks_to_wait);


#ifdef __cplusplus
}
#endif /* __cplusplus */


