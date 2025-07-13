#include "hal.h"
#include "app.h"

static hal_dac_driver_t *drv =  &HAL_DAC_DRIVER;

hal_dac_dev_t hal_dac_init(dac_channel_t channel)
{
    drv->init(channel);
}

void hal_dac_deinit(hal_dac_dev_t dev)
{
    drv->deinit(dev);
}

void hal_dac_configure(hal_dac_dev_t dev, dac_config_t config)
{
    drv->configure(dev, config);
}

void hal_dac_start(hal_dac_dev_t dev)
{
    drv->start(dev);
}

void hal_dac_stop(hal_dac_dev_t dev)
{
    drv->stop(dev);
}

void hal_dac_set_value(hal_dac_dev_t dev, uint16_t value)
{
    drv->set_value(dev, value);
}

uint16_t hal_dac_get_value(hal_dac_dev_t dev)
{
    return(drv->get_value(dev));
}