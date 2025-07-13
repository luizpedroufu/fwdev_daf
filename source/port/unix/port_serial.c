#include <libserialport.h>
#include <pthread.h>

#include "hal.h"
#include "app.h"

struct sp_port *port;

typedef enum sp_return sp_return_t;

static hal_serial_port_interrupt_t rx_cbk = NULL;

static void* rx_thread_func(void *arg)
{
    (void)arg;

    uint8_t byte;
    while(1)
    {
        uint16_t r = (uint16_t)sp_blocking_read(port, &byte, 1, 100);
        if(r > 0 && rx_cbk)
        {
            rx_cbk(byte);
        }
    }
    return NULL;
}

hal_serial_status_t port_serial_init(hal_serial_port_conf_t *conf_var)
{
    return HAL_SERIAL_OK;
}

hal_serial_status_t port_serial_deinit(hal_serial_port_conf_t *conf_var)
{
    return HAL_SERIAL_OK;
}

hal_serial_status_t port_serial_configure_port(hal_serial_port_conf_t *conf_var)
{
    sp_return_t ret_val;

    ret_val = sp_get_port_by_name((const char*)conf_var->port_name, &port);
    if(ret_val != SP_OK)
        return HAL_SERIAL_ERROR;
    
    ret_val = sp_set_baudrate(port, conf_var->baudrate);
    if(ret_val != SP_OK)
        return HAL_SERIAL_ERROR;

    ret_val = sp_set_bits(port, conf_var->data_bits);
    if(ret_val != SP_OK)
        return HAL_SERIAL_ERROR;

    switch(conf_var->parity)
    {
        case PARITY_NONE: ret_val = sp_set_parity(port, SP_PARITY_NONE); break;
        case PARITY_ODD:  ret_val = sp_set_parity(port, SP_PARITY_ODD); break;
        case PARITY_EVEN: ret_val = sp_set_parity(port, SP_PARITY_EVEN); break;
        default:          ret_val = sp_set_parity(port, SP_PARITY_NONE); break;
    }
    if (ret_val != SP_OK) 
        return HAL_SERIAL_ERROR;

    return HAL_SERIAL_OK;
}

hal_serial_status_t port_serial_open(uint8_t *port_name)
{
    sp_return_t ret_val;
    ret_val = sp_open(port, SP_MODE_READ_WRITE);
    if(ret_val != SP_OK)
        return HAL_SERIAL_ERROR;

    return HAL_SERIAL_OK;
}

hal_serial_status_t port_serial_close(uint8_t *port_name)
{  
    sp_return_t ret_val;
    ret_val = sp_close(port);
    if(ret_val != SP_OK)
        return HAL_SERIAL_ERROR;

    return HAL_SERIAL_OK;
}

hal_serial_status_t port_serial_read(uint8_t *port_name, uint8_t *p_buf, size_t length)
{
    if(sp_nonblocking_read(port, (void*)p_buf, length) != SP_OK)
    {
        return HAL_SERIAL_ERROR;
    }
    return HAL_SERIAL_OK;
}

hal_serial_status_t port_serial_write(uint8_t *port_name, uint8_t *p_buf, size_t length)
{
    if(sp_nonblocking_write(port, (void*)p_buf, length) != SP_OK)
    {
        return HAL_SERIAL_ERROR;
    }
    return HAL_SERIAL_OK;
}

void port_serial_interrupt_set(uint8_t *port_name, hal_serial_port_interrupt_t fun)
{
    (void)port_name;
    rx_cbk = fun;

    pthread_t thread;
    pthread_create(&thread, NULL, rx_thread_func, NULL);
    pthread_detach(thread);
}

hal_serial_port_driver_t HAL_SERIAL_PORT_DRIVER=
{
    .init = port_serial_init,
    .deinit = port_serial_deinit,
    .configure_port = port_serial_configure_port,
    .open = port_serial_open,
    .close = port_serial_close,
    .read = port_serial_read,
    .write = port_serial_write,
    .interrupt_set = port_serial_interrupt_set,
};