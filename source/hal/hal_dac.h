/*
 * hal_dac.h
 *
 * Author: Pedro Bittencourt
 * GitHub: @luizpedrobt
 * Email: luizpedrob65@gmail.com
 *
 * JORNADA SENIOR EM 3 MESES!!!
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dac_resolution_e
{
    DAC_ALIGN_8B_R = 0,
    DAC_ALIGN_12B_R,
    DAC_ALIGN_12B_L,
}dac_resolution_t;

typedef enum dac_channel_e
{
    DAC_CHANNEL_1 = 0,
    DAC_CHANNEL_2,
}dac_channel_t;

typedef enum dac_trigger_e
{
    DAC_TRIGGER_NONE = 0,
    DAC_TRIGGER_TIMER6,
    DAC_TRIGGER_TIMER2,
    DAC_TRIGGER_SOFTWARE,
}dac_trigger_t;

typedef struct dac_config_e
{
    dac_resolution_t resolution;
    dac_trigger_t trigger;
    uint8_t use_output_buffer;  
}dac_config_t;

typedef struct hal_dac_dev_s* hal_dac_dev_t;

typedef struct hal_dac_driver_t
{
    hal_dac_dev_t (*init)(dac_channel_t channel);
    void (*deinit)(hal_dac_dev_t dev);
    void (*configure)(hal_dac_dev_t dev, dac_config_t config);
    void (*start)(hal_dac_dev_t dev);
    void (*stop)(hal_dac_dev_t dev);
    void (*set_value)(hal_dac_dev_t dev, uint16_t value);
    void (*set_freq)(hal_dac_dev_t dev, float freq);
    uint16_t (*get_value)(hal_dac_dev_t dev);
}hal_dac_driver_t;

hal_dac_dev_t hal_dac_init(dac_channel_t channel);
void hal_dac_deinit(hal_dac_dev_t dev);
void hal_dac_configure(hal_dac_dev_t dev, dac_config_t config);
void hal_dac_start(hal_dac_dev_t dev);
void hal_dac_stop(hal_dac_dev_t dev);
void hal_dac_set_value(hal_dac_dev_t dev, uint16_t value);
void hal_dac_set_freq(hal_dac_dev_t dev, float freq);
uint16_t hal_dac_get_value(hal_dac_dev_t dev);

#ifdef __cplusplus
}
#endif
