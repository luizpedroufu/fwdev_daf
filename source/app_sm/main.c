#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>

#include "app_sm.h"

#define INITIAL_TEMP 50
#define SP_TEMP 24
#define HISTERESIS 2

sm_state_table_t sm_state_table[] =
{
	#define X(idx, enum, func, name) { func },
		XMACRO_STATE_MACHINE
	#undef X
};

const char *state_names[] =
{
    #define X(idx, enum, func, name) name,
        XMACRO_STATE_MACHINE
    #undef X
};

sm_state_var_t state_var =
{
    .sp = SP_TEMP,
    .hist = HISTERESIS,
    .cooling = false,
    .heating = false,
    .state = STATE_RESET,
};

static uint8_t random_range(uint8_t min, uint8_t max) 
{
    return rand() % (max - min + 1) + min;
}

static void app_sm_run(sm_state_var_t *st)
{
    st->state = sm_state_table[st->state].func(st);
}

sm_state_t sm_state_idle(sm_state_var_t *st)
{
    sm_state_t next_state = STATE_IDLE;

    st->cooling = false;
    st->heating = false;

    printf("\n");
    printf("STATE: %s\n", state_names[st->state]);
    printf("TEMP: %" PRIu8 "\n", st->temp);
    printf("SETPOINT: %" PRIu8 "\n", st->sp);
    printf("COOL: %s\n", st->cooling ? "ON" : "OFF");
    printf("HEAT: %s\n", st->heating ? "ON" : "OFF");

    st->temp = random_range(0, 50);
    
    if(st->temp < (st->sp + st->hist))
        next_state = STATE_HEAT;
    else if(st->temp > (st->sp - st->hist))
        next_state = STATE_COOL;
    else
        next_state = STATE_IDLE;

    return next_state;
}

sm_state_t sm_state_cool(sm_state_var_t *st)
{
    sm_state_t next_state = STATE_COOL;
    st->temp--;
    st->cooling = true;
    st->heating = false;
    
    printf("\n");
    printf("STATE: %s\n", state_names[st->state]);
    printf("TEMP: %" PRIu8 "\n", st->temp);
    printf("SETPOINT: %" PRIu8 "\n", st->sp);
    printf("COOL: %s\n", st->cooling ? "ON" : "OFF");
    printf("HEAT: %s\n", st->heating ? "ON" : "OFF");

    if(st->temp <= st->sp)
        next_state = STATE_IDLE;

    return next_state;
}

sm_state_t sm_state_heat(sm_state_var_t *st)
{
    sm_state_t next_state = STATE_HEAT;
    st->temp++;
    st->cooling = false;
    st->heating = true;
    
    printf("\n");
    printf("STATE: %s\n", state_names[st->state]);
    printf("TEMP: %" PRIu8 "\n", st->temp);
    printf("SETPOINT: %" PRIu8 "\n", st->sp);
    printf("COOL: %s\n", st->cooling ? "ON" : "OFF");
    printf("HEAT: %s\n", st->heating ? "ON" : "OFF");

    if(st->temp >= st->sp)

        next_state = STATE_IDLE;

    return next_state;
}

sm_state_t sm_state_reset(sm_state_var_t *st)
{
    sm_state_t next_state = STATE_IDLE;
    st->heating = false;
    st->cooling = false;
    return next_state;
}

int main(void)
{
    while (1)
    {
        app_sm_run(&state_var);
        sleep(1);
    }
    
    return 0;
}