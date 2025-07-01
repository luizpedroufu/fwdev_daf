
#include <signal.h>
#include <time.h> // Here is defined SIGEV_* constants 
#include <unistd.h>
#include <pthread.h>

#include "hal.h"
#include "app.h"

extern char *main_app_name_get(void);

static bool port_cpu_init_flag = false;

static volatile uint32_t port_tick_count = 0;
volatile bool terminate_flag = false;

pthread_t systick_thread;

static pthread_mutex_t critical_section_mutex;
static pthread_mutexattr_t port_cpu_cs_attr;

void* systick_thread_function(void *arg);

static void port_cpu_sigint_handler(int sig_num)
{
    UTL_DBG_PRINTF(UTL_DBG_MOD_PORT, "CTRL+C pressed!\n");
    app_terminate_set();
}

static void port_cpu_systick_cbk(void *context) // Ponteiro genérico | callback do systick
{
    port_tick_count++;
}

static void port_cpu_init(void)
{
    pthread_mutexattr_init(&port_cpu_cs_attr);
    pthread_mutexattr_settype(&port_cpu_cs_attr, PTHREAD_MUTEX_RECURSIVE_NP);

    pthread_mutex_init(&critical_section_mutex, &port_cpu_cs_attr);

    signal(SIGINT, port_cpu_sigint_handler);

    if(pthread_create(&systick_thread, NULL, systick_thread_function, NULL) != 0)
        UTL_DBG_PRINTF(UTL_DBG_MOD_PORT, "Fail initializing 1 ms systick!\n");

    port_cpu_init_flag =  true;
}

static void port_cpu_deinit(void)
{

    pthread_join(&systick_thread, NULL); 
    port_cpu_init_flag = false;

}

static void port_cpu_reset(void)
{
    char *app_name = main_app_name_get();
    char *rst_cmd_msg = malloc(strlen(app_name) + 32);

    UTL_DBG_PRINTF(UTL_DBG_MOD_PORT, "Restarting: %s | 5 s last...\n", app_name);

    sprintf(rst_cmd_msg, "/bin/bash -c 'sleep 5; %s '", app_name);
    system(rst_cmd_msg);
    free(rst_cmd_msg);

    app_terminate_set();
}

static void port_cpu_watchdog_refresh(void)
{
    // NOT IMPLEMENTED
}

static void port_cpu_id_get(uint8_t *id)
{
    memcpy(id, "STM32U565CI", HAL_CPU_ID_SIZE);
}

static uint32_t port_cpu_random_seed_get(void)
{
    uint32_t ret_val = (uint32_t)rand();
    UTL_DBG_PRINTF(UTL_DBG_MOD_PORT, "Seed generated: %d\n", ret_val);
    return ret_val;
}

static uint32_t port_cpu_critical_section_enter(hal_cpu_cs_level_t level)
{
    
    pthread_mutex_lock(&critical_section_mutex);
    return 0;
}

static void port_cpu_critical_section_leave(uint32_t last_level)
{
    pthread_mutex_unlock(&critical_section_mutex);
}

static void port_cpu_low_power_enter(void)
{
    // NOT IMPLEMENTED
}

static void port_cpu_sleep_ms(uint32_t slp_ms)
{
    struct timespec sleep_time =
    {
        .tv_sec = 0,
        .tv_nsec = slp_ms * 1e6,
    };

    nanosleep(&sleep_time, NULL);
}

static uint32_t port_cpu_time_get_ms(void)
{
    return port_tick_count;
}

void* systick_thread_function(void *arg)
{
    struct timespec sleep_time =
    {
        .tv_sec = 0,
        .tv_nsec = 1e6,
    };

    while (port_cpu_init_flag)
    {
        port_cpu_systick_cbk(arg);
        nanosleep(&sleep_time, NULL);
    }
    return NULL;
}

hal_cpu_driver_t HAL_CPU_DRIVER =
{
    .init = port_cpu_init,
    .deinit = port_cpu_deinit,
    .reset = port_cpu_reset,
    .watchdog_refresh = port_cpu_watchdog_refresh,
    .id_get = port_cpu_id_get,
    .random_seed_get = port_cpu_random_seed_get,
    .critical_section_enter = port_cpu_critical_section_enter,
    .critical_section_leave = port_cpu_critical_section_leave,
    .low_power_enter = port_cpu_low_power_enter,
    .sleep_ms = port_cpu_sleep_ms,
    .time_get_ms = port_cpu_time_get_ms,
};