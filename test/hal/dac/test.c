#include "hal.h"
#include "hal_dac.h"
#include "app.h"

#define RUNNING_AMPLITUDE_SET 0

static hal_dac_dev_t dac_dev = 0;
static dac_config_t dac_cfg =
{
    .resolution = DAC_ALIGN_12B_L,
    .trigger = DAC_TRIGGER_NONE,
    .use_output_buffer = 0,
};

dac_channel_t DAC_CHANNEL = DAC_CHANNEL_1;

int out = 0;  

typedef struct note_s {
  float freq;   // Hz
  int duration; // ms
}note_t;

note_t melody[] = {
  {261.63, 500}, // Dó
  {293.66, 500}, // Ré
  {329.63, 500}, // Mi
  {349.23, 500}, // Fá
  {349.23, 200},
  {349.23, 200},

  {261.63, 500}, // Dó
  {293.66, 500}, // Ré
  {261.63, 500}, // Dó
  {293.66, 500}, // Ré
  {293.66, 200},
  {293.66, 200},

  {261.63, 500}, // Dó
  {392.00, 500}, // Sol
  {349.23, 500}, // Fá
  {329.63, 500}, // Mi
  {329.63, 200},
  {329.63, 200},

  {261.63, 500}, // Dó
  {293.66, 500}, // Ré
  {329.63, 500}, // Mi
  {349.23, 500}, // Fá
  {349.23, 200},
  {349.23, 200}
};

void app_init(void)
{
    utl_dbg_mod_enable(UTL_DBG_MOD_APP);
    utl_dbg_mod_enable(UTL_DBG_MOD_DAC);
    UTL_DBG_PRINTF(UTL_DBG_MOD_APP, "Initializing app...\n");

    dac_dev = hal_dac_init(DAC_CHANNEL_1);
    if(dac_dev == 0)
    {
        UTL_DBG_PRINTF(UTL_DBG_MOD_APP, "Failed to open DAC port 0\n");
    }
    hal_dac_start(dac_dev);
}

bool app_loop(void)
{

#if RUNNING_AMPLITUDE_SET
    int max_val = (dac_cfg.resolution == DAC_ALIGN_12B_L || dac_cfg.resolution == DAC_ALIGN_12B_R) ? 2047 : 255;

    UTL_DBG_PRINTF(UTL_DBG_MOD_APP, "TYPE A VALID NUMERIC NUMBER [0, %d] (negative to quit): \n", max_val);

    if(scanf("%d", &out) != 1)
    {
        int c; 
        while ((c = getchar()) != '\n' && c != EOF)
        {}
    }

    if(out < 0)
    {
        hal_dac_stop(dac_dev);
        hal_dac_deinit(dac_dev);
        app_terminate_set();
    }
        

    if(out > max_val)
        UTL_DBG_PRINTF(UTL_DBG_MOD_APP, "Value out of range! Please try again.\n");
    else
        hal_dac_set_value(dac_dev, out);

#else

    hal_dac_set_value(dac_dev, 100);

    for(int i = 0; i < 24; i++)
    {
        hal_dac_set_freq(dac_dev, melody[i].freq);
        hal_cpu_sleep_ms(melody[i].duration);
    }

    UTL_DBG_PRINTF(UTL_DBG_MOD_APP, "SENOID AMPLITUDE IN DAC COUNTS: %d\n", hal_dac_get_value(dac_dev));

#endif

    return !app_terminate_get();
}
