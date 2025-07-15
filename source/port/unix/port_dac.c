#include <portaudio.h>

#include "hal.h"
#include "app.h"

#define SAMPLE_RATE 44100
#define FREQUENCY 100.0f
#define FRAMES_PER_BUFFER 64

#define PI 3.141592f

typedef struct Sine_s
{
    double phase;
    double phase_inc;
}Sine;

struct hal_dac_dev_s 
{
    uint16_t v_out;
    uint32_t sampling_rate;
    float amplitude;
    uint8_t frames;
    float freq;
    dac_channel_t channel;
    dac_config_t cfg;
    Sine sine;
    bool in_use;
    PaStream *stream;
};

static int callback(const void *in, void *out, unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags statusFlags, void *userData) 
{
    float *buff = (float*)out;
    hal_dac_dev_t aux = (hal_dac_dev_t)userData;

    (void)in; 
    (void)timeInfo;
    (void)statusFlags;

    for(unsigned long i = 0; i < frames; i++)
    {
        buff[i] = (float)(aux->amplitude * sin(aux->sine.phase));
        aux->sine.phase += aux->sine.phase_inc;
        if(aux->sine.phase >= 2.0 * PI)
            aux->sine.phase -= 2.0 * PI;
    }

    return paContinue;
}

static hal_dac_dev_t port_dac_init(dac_channel_t channel)
{
    hal_dac_dev_t aux = malloc(sizeof(struct hal_dac_dev_s));
    if(!aux)
    {
        UTL_DBG_PRINTF(UTL_DBG_MOD_DAC, "ALOCATION ERROR\n");
        return NULL;
    }

    aux->amplitude = 0;
    aux->channel = channel;
    aux->cfg.resolution = 0;
    aux->cfg.trigger = 0;
    aux->frames = FRAMES_PER_BUFFER;
    aux->cfg.use_output_buffer = 0;
    aux->freq = FREQUENCY;
    aux->sampling_rate = SAMPLE_RATE;
    aux->sine.phase = 0;
    aux->sine.phase_inc = 2 * PI * aux->freq / aux->sampling_rate;
    aux->v_out = 0;
    aux->in_use = true;

    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
        UTL_DBG_PRINTF(UTL_DBG_MOD_DAC, "PORTAUDIO INIT ERROR: %s\n", Pa_GetErrorText(err));
        free(aux);
        return NULL;
    }

    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice();
    if (outputParameters.device == paNoDevice) {
        UTL_DBG_PRINTF(UTL_DBG_MOD_DAC, "NO DEFAULT OUTPUT DEVICE FOUND!\n");
        free(aux);
        return NULL;
    }

    outputParameters.channelCount = 1;           
    outputParameters.sampleFormat = paFloat32;   
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&(aux->stream), NULL, &outputParameters, aux->sampling_rate, aux->frames, paNoFlag, callback, aux);
    if (err != paNoError)
    {
        UTL_DBG_PRINTF(UTL_DBG_MOD_DAC, "STREAM OPEN ERROR: %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        free(aux);
        return NULL;
    }

    UTL_DBG_PRINTF(UTL_DBG_MOD_DAC, "SUCCESSFULLY INITIALIZED BATUTA!!!\n");

    return aux;
}


static void port_dac_deinit(hal_dac_dev_t dev)
{
    if(Pa_CloseStream(dev->stream) != paNoError)
    {
        UTL_DBG_PRINTF(UTL_DBG_MOD_DAC, "VIX!!! #_#\n");
        return;
    }

    dev->in_use = false;
    free(dev);

    UTL_DBG_PRINTF(UTL_DBG_MOD_DAC, "PERIPHERAL SUCCESSFULLY CLOSED!!!\n");
}

static void port_dac_configure(hal_dac_dev_t dev, dac_config_t config)
{
    if(!dev)
    {
        UTL_DBG_PRINTF(UTL_DBG_MOD_DAC, "NOT VALID DEV PORT!!\n");
        return;
    }

    dev->cfg.resolution = config.resolution;
    dev->cfg.trigger = config.trigger;
    dev->cfg.use_output_buffer = config.use_output_buffer;
}

static void port_dac_start(hal_dac_dev_t dev)
{
    Pa_StartStream(dev->stream);
}

static void port_dac_stop(hal_dac_dev_t dev)
{
    if(Pa_StopStream(dev->stream) != paNoError)
    {
        UTL_DBG_PRINTF(UTL_DBG_MOD_DAC, "ERROR STOPING!!\n");
    }
}

static void port_dac_set_value(hal_dac_dev_t dev, uint16_t value)
{
    switch (dev->cfg.resolution)
    {
    case DAC_ALIGN_8B_R:
        dev->amplitude = (float)(value * 3.3/255.0);
        return;

    case DAC_ALIGN_12B_R:
        dev->amplitude = (float)(value * 3.3/2047.0);
        return;

    case DAC_ALIGN_12B_L:
        dev->amplitude = (float)(value * 3.3/2047.0);        
        return;

    default:
        dev->amplitude = 0;
        return;
    }
}

static void port_dac_set_freq(hal_dac_dev_t dev, uint16_t freq)
{
    if(!dev)
        return;

    dev->sine.phase_inc = 2 * PI * freq / dev->sampling_rate;
}

static uint16_t port_dac_get_value(hal_dac_dev_t dev)
{
    switch (dev->cfg.resolution)
    {
    case DAC_ALIGN_8B_R:  return (uint16_t)(dev->amplitude  * 255/3.3 );
    case DAC_ALIGN_12B_R: return (uint16_t)(dev->amplitude  * 2047/3.3);
    case DAC_ALIGN_12B_L: return (uint16_t)(dev->amplitude  * 2047/3.3);
    default:              return 0;
    }
}

hal_dac_driver_t HAL_DAC_DRIVER =
{
    .init = port_dac_init,
    .deinit = port_dac_deinit,
    .configure = port_dac_configure,
    .start = port_dac_start,
    .stop = port_dac_stop,
    .set_value = port_dac_set_value,
    .set_freq = port_dac_set_freq,
    .get_value = port_dac_get_value,
};