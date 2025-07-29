#include "hal.h"
#include "hal_dac.h"
#include "app.h"

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

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
    {REST, 750}, {REST, 375}, {REST, 375}, {REST, 375}, // 1
    {NOTE_E4, 375}, {NOTE_A4, 375}, {NOTE_C5, 375},
    {NOTE_B4, 375}, {NOTE_A4, 375}, {NOTE_C5, 375}, {NOTE_A4, 375},
    {NOTE_B4, 375}, {NOTE_A4, 375}, {NOTE_F4, 375}, {NOTE_G4, 375},
    {NOTE_E4, 1500}, {NOTE_E4, 375}, {NOTE_A4, 375}, {NOTE_C5, 375},

    {NOTE_B4, 375}, {NOTE_A4, 375}, {NOTE_C5, 375}, {NOTE_A4, 375},
    {NOTE_C5, 375}, {NOTE_A4, 375}, {NOTE_E4, 375}, {NOTE_DS4, 375},

    {NOTE_D4, 1500}, {NOTE_D4, 375}, {NOTE_F4, 375}, {NOTE_GS4, 375},
    {NOTE_B4, 1500}, {NOTE_D4, 375}, {NOTE_F4, 375}, {NOTE_GS4, 375},
    {NOTE_A4, 1500}, {NOTE_C4, 375}, {NOTE_C4, 375}, {NOTE_G4, 375},
    {NOTE_F4, 375}, {NOTE_E4, 375}, {NOTE_G4, 375}, {NOTE_F4, 375},
    {NOTE_F4, 375}, {NOTE_E4, 375}, {NOTE_E4, 375}, {NOTE_GS4, 375},

    {NOTE_A4, 1500}, {REST, 375}, {NOTE_A4, 375}, {NOTE_A4, 375}, {NOTE_GS4, 375},
    {NOTE_G4, 1500}, {NOTE_B4, 375}, {NOTE_A4, 375}, {NOTE_F4, 375},
    {NOTE_E4, 1500}, {NOTE_E4, 375}, {NOTE_G4, 375}, {NOTE_E4, 375},
    {NOTE_D4, 1500}, {NOTE_D4, 375}, {NOTE_D4, 375}, {NOTE_F4, 375}, {NOTE_DS4, 375},

    {NOTE_E4, 1500}, {REST, 375}, {NOTE_E4, 375}, {NOTE_A4, 375}, {NOTE_C5, 375},

    // Repetição da parte 2
    {NOTE_B4, 375}, {NOTE_A4, 375}, {NOTE_C5, 375}, {NOTE_A4, 375},
    {NOTE_B4, 375}, {NOTE_A4, 375}, {NOTE_F4, 375}, {NOTE_G4, 375},
    {NOTE_E4, 1500}, {NOTE_E4, 375}, {NOTE_A4, 375}, {NOTE_C5, 375},
    {NOTE_B4, 375}, {NOTE_A4, 375}, {NOTE_C5, 375}, {NOTE_A4, 375},
    {NOTE_C5, 375}, {NOTE_A4, 375}, {NOTE_E4, 375}, {NOTE_DS4, 375},

    {NOTE_D4, 1500}, {NOTE_D4, 375}, {NOTE_F4, 375}, {NOTE_GS4, 375},
    {NOTE_B4, 1500}, {NOTE_D4, 375}, {NOTE_F4, 375}, {NOTE_GS4, 375},
    {NOTE_A4, 1500}, {NOTE_C4, 375}, {NOTE_C4, 375}, {NOTE_G4, 375},
    {NOTE_F4, 375}, {NOTE_E4, 375}, {NOTE_G4, 375}, {NOTE_F4, 375},
    {NOTE_F4, 375}, {NOTE_E4, 375}, {NOTE_E4, 375}, {NOTE_GS4, 375},

    {NOTE_A4, 1500}, {REST, 375}, {NOTE_A4, 375}, {NOTE_A4, 375}, {NOTE_GS4, 375},
    {NOTE_G4, 1500}, {NOTE_B4, 375}, {NOTE_A4, 375}, {NOTE_F4, 375},
    {NOTE_E4, 1500}, {NOTE_E4, 375}, {NOTE_G4, 375}, {NOTE_E4, 375},
    {NOTE_D4, 1500}, {NOTE_D4, 375}, {NOTE_D4, 375}, {NOTE_F4, 375}, {NOTE_DS4, 375},

    {NOTE_E4, 1500}
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

    for(int i = 0; i < 129; i++)
    {
        hal_dac_set_freq(dac_dev, melody[i].freq);
        hal_cpu_sleep_ms(melody[i].duration);
    }

    UTL_DBG_PRINTF(UTL_DBG_MOD_APP, "SENOID AMPLITUDE IN DAC COUNTS: %d\n", hal_dac_get_value(dac_dev));

#endif

    return !app_terminate_get();
}
