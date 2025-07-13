#include "hal.h"

static hal_serial_port_driver_t *drv = &HAL_SERIAL_PORT_DRIVER;

hal_serial_status_t hal_ser_init(hal_serial_port_conf_t *conf_var)
{
    drv->init(conf_var);
}

hal_serial_status_t hal_ser_deinit(hal_serial_port_conf_t *conf_var)
{
    drv->deinit(conf_var);
}

hal_serial_status_t hal_ser_configure_port(hal_serial_port_conf_t *conf_var)
{
    drv->configure_port(conf_var);
}

hal_serial_status_t hal_ser_open(uint8_t *port_name)
{
    drv->open(port_name);
}

hal_serial_status_t hal_ser_close(uint8_t *port_name)
{  
    drv->close(port_name);
}

hal_serial_status_t hal_ser_read(uint8_t *port_name, uint8_t *p_buf, size_t length)
{
    drv->read(port_name, p_buf, length);
}

hal_serial_status_t hal_ser_write(uint8_t *port_name, uint8_t *p_buf, size_t length)
{
    drv->write(port_name, p_buf, length);
}

void hal_ser_interrupt_set(uint8_t *port_name, hal_serial_port_interrupt_t fun)
{
    drv->interrupt_set(port_name, fun);
}