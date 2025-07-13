#include "hal.h"
#include "hal_dac.h"
#include "app.h"

static hal_dac_dev_t dac_dev = 0;
static dac_config_t dac_cfg =
{
    .resolution = DAC_ALIGN_12B_L,
    .trigger = DAC_TRIGGER_NONE,
    .use_output_buffer = 0,
};

dac_channel_t DAC_CHANNEL = DAC_CHANNEL_1;

int out = 0;  // use int para ler o valor com scanf

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
    int max_val = (dac_cfg.resolution == DAC_ALIGN_12B_L || dac_cfg.resolution == DAC_ALIGN_12B_R) ? 2047 : 255;

    UTL_DBG_PRINTF(UTL_DBG_MOD_APP, "TYPE A VALID NUMERIC NUMBER [0, %d] (negative to quit): \n", max_val);

    if(scanf("%d", &out) != 1)
    {
        // Entrada inválida: limpar buffer stdin e continuar
        int c; while ((c = getchar()) != '\n' && c != EOF);
        return true;
    }

    if(out < 0)
    {
        app_terminate_set();
        return false;  // indica que deve terminar o loop
    }

    if(out > max_val)
    {
        UTL_DBG_PRINTF(UTL_DBG_MOD_APP, "Value out of range! Please try again.\n");
        return true;
    }

    hal_dac_set_value(dac_dev, (uint16_t)out);

    UTL_DBG_PRINTF(UTL_DBG_MOD_APP, "SENOID AMPLITUDE IN DAC COUNTS: %d\n", hal_dac_get_value(dac_dev));

    return true;
}
