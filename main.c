#include <stdint.h>
#include <stdbool.h>

#include "app_error.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "softdevice_handler.h"

#define APP_TIMER_PRESCALER               0
#define APP_TIMER_MAX_TIMERS              10
#define APP_TIMER_OP_QUEUE_SIZE           4

#define BATTERY_LEVEL_MEAS_INTERVAL      APP_TIMER_TICKS(10, APP_TIMER_PRESCALER)

#define SCHED_MAX_EVENT_DATA_SIZE         sizeof(app_timer_event_t)
#define SCHED_QUEUE_SIZE                  10

static app_timer_id_t                     m_test_timer_id;
static int                                x;

static void test_timeout_handler(void * p_context)
{
    ++x;
}

void main(void)
{
    uint32_t err_code = 0;

    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, NULL);

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

    err_code = app_timer_create(&m_test_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                test_timeout_handler);
    APP_ERROR_CHECK(err_code);

    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);

    err_code = app_timer_start(m_test_timer_id, BATTERY_LEVEL_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    while (true)
    {
        app_sched_execute();
        err_code = sd_app_evt_wait();
        APP_ERROR_CHECK(err_code);
    }
}
