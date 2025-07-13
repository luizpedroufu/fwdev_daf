#pragma once 

#ifdef __cplusplus
extern "C"
#endif

#define XMACRO_SERIAL_PORT_BAUD \
        X(BAUD_9600  , 9600  )  \
        X(BAUD_19200 , 19200 )  \
        X(BAUD_38400 , 38400 )  \
        X(BAUD_57600 , 57600 )  \
        X(BAUD_115200, 115200)  \
        X(BAUD_230400, 230400)  \
        X(BAUD_460800, 460800)  \
        X(BAUD_921600, 921600)  

typedef enum hal_serial_port_baudrate_e
{
   #define X(ENUM, BAUD) ENUM = BAUD,
        XMACRO_SERIAL_PORT_BAUD
    #undef X
}hal_serial_port_baudrate_t;

typedef enum hal_serial_port_parity_e
{
    PARITY_NONE = 0,
    PARITY_ODD = 1,
    PARITY_EVEN = 2,
}hal_serial_port_parity_t;

typedef enum hal_serial_port_DB_e
{
    DATA_5_BITS = 5,
    DATA_6_BITS = 6,
    DATA_7_BITS = 7,
    DATA_8_BITS = 8,
    DATA_9_BITS = 9,
}hal_serial_port_data_bits_t;

typedef enum hal_serial_port_SB_e
{
    STOP_BITS_1 = 1,
    STOP_BITS_2 = 2,
}hal_serial_port_SB_t;

typedef struct hal_serial_port_conf_s
{
    uint8_t *port_name;
    hal_serial_port_baudrate_t baudrate;
    hal_serial_port_parity_t parity;
    hal_serial_port_data_bits_t data_bits;
    hal_serial_port_SB_t stop_bits;
}hal_serial_port_conf_t;

typedef enum hal_serial_status_e
{
    HAL_SERIAL_OK = 0,
    HAL_SERIAL_BUSY,
    HAL_SERIAL_ERROR,
}hal_serial_status_t;

typedef void (*hal_serial_port_interrupt_t)(uint8_t c);

typedef struct hal_serial_port_driver_s
{
    hal_serial_status_t(*init)(hal_serial_port_conf_t *conf_var);
    hal_serial_status_t(*deinit)(hal_serial_port_conf_t *conf_var);
    hal_serial_status_t(*configure_port)(hal_serial_port_conf_t *conf_var);
    hal_serial_status_t(*open)(uint8_t *port_name);
    hal_serial_status_t(*close)(uint8_t *port_name);
    hal_serial_status_t(*read)(uint8_t *port_name, uint8_t *p_buf, size_t length);
    hal_serial_status_t(*write)(uint8_t *port_name, uint8_t *p_buf, size_t length);
    hal_serial_status_t(*interrupt_set)(uint8_t *port_name, hal_serial_port_interrupt_t fun);
}hal_serial_port_driver_t;


hal_serial_status_t hal_ser_init(hal_serial_port_conf_t *conf_var);

hal_serial_status_t hal_ser_deinit(hal_serial_port_conf_t *conf_var);

hal_serial_status_t hal_ser_configure_port(hal_serial_port_conf_t *conf_var);

hal_serial_status_t hal_ser_open(uint8_t *port_name);

hal_serial_status_t hal_ser_close(uint8_t *port_name);

hal_serial_status_t hal_ser_read(uint8_t *port_name, uint8_t *p_buf, size_t length);

hal_serial_status_t hal_ser_write(uint8_t *port_name, uint8_t *p_buf, size_t length);

void hal_ser_interrupt_set(uint8_t *port_name, hal_serial_port_interrupt_t fun);

#ifdef __cplusplus
}
#endif