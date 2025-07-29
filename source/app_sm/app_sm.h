
#pragma once

#define XMACRO_STATE_MACHINE                       \
        X(0, STATE_IDLE , sm_state_idle , "IDLE ") \
        X(1, STATE_COOL , sm_state_cool , "COOL ") \
        X(2, STATE_HEAT , sm_state_heat , "HEAT ") \
        X(3, STATE_RESET, sm_state_reset, "RESET")

typedef enum sm_state_e
{
    #define X(idx, enum, func, name) enum = idx,
        XMACRO_STATE_MACHINE
    #undef X
}sm_state_t;

typedef struct sm_state_var_s
{
    sm_state_t state;
    uint8_t sp;
    uint8_t temp;
    uint8_t hist;
    bool cooling;
    bool heating;
}sm_state_var_t;

typedef sm_state_t (*sm_func_t)(sm_state_var_t *st);

typedef struct sm_state_table_s
{
	sm_func_t func;
}sm_state_table_t;

#define X(idx, enum, func, name) sm_state_t func(sm_state_var_t *st);
    XMACRO_STATE_MACHINE
#undef X
