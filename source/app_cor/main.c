#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>

#define SP_TEMP 24
#define HISTERESIS 2

typedef enum {
    STATE_RESET,
    STATE_IDLE,
    STATE_HEAT,
    STATE_COOL,
    STATE_MAX
} sm_state_t;

typedef struct {
    uint8_t temp;
    uint8_t sp;
    uint8_t hist;
    bool cooling;
    bool heating;
} sm_context_t;

static uint8_t random_range(uint8_t min, uint8_t max) {
    return rand() % (max - min + 1) + min;
}

static void print_status(const char *state, sm_context_t *ctx) {
    printf("\n");
    printf("STATE: %s\n", state);
    printf("TEMP: %" PRIu8 "\n", ctx->temp);
    printf("SETPOINT: %" PRIu8 "\n", ctx->sp);
    printf("COOL: %s\n", ctx->cooling ? "ON" : "OFF");
    printf("HEAT: %s\n", ctx->heating ? "ON" : "OFF");
}

// Corotina emulada com switch + estado estático
void app_sm_run(sm_context_t *ctx) {
    static sm_state_t state = STATE_RESET;

    switch (state) {
        case STATE_RESET:
            ctx->cooling = false;
            ctx->heating = false;
            state = STATE_IDLE;
            break;

        case STATE_IDLE:
            ctx->cooling = false;
            ctx->heating = false;
            ctx->temp = random_range(0, 50);

            print_status("IDLE", ctx);

            if (ctx->temp < (ctx->sp - ctx->hist))
                state = STATE_HEAT;
            else if (ctx->temp > (ctx->sp + ctx->hist))
                state = STATE_COOL;
            else
                state = STATE_IDLE;
            break;

        case STATE_HEAT:
            ctx->temp++;
            ctx->cooling = false;
            ctx->heating = true;

            print_status("HEAT", ctx);

            if (ctx->temp >= ctx->sp)
                state = STATE_IDLE;
            break;

        case STATE_COOL:
            ctx->temp--;
            ctx->cooling = true;
            ctx->heating = false;

            print_status("COOL", ctx);

            if (ctx->temp <= ctx->sp)
                state = STATE_IDLE;
            break;

        default:
            state = STATE_RESET;
            break;
    }
}

int main(void) {
    srand(time(NULL));

    sm_context_t ctx = {
        .temp = 50,
        .sp = SP_TEMP,
        .hist = HISTERESIS,
        .cooling = false,
        .heating = false
    };

    while (1) {
        app_sm_run(&ctx);
        sleep(1);
    }

    return 0;
}
