#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "app_error.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "softdevice_handler.h"

#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "ble_dis.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT   0

#define DEVICE_NAME                       "BLELED CANDLE"
#define MANUFACTURER_NAME                 "Seth Black"
#define MODEL_NUM                         "SB-BLELED-CANDLE-v1"
#define MANUFACTURER_ID                   0x1122334455
#define ORG_UNIQUE_ID                     0x667788

#define APP_ADV_INTERVAL                  40
#define APP_ADV_TIMEOUT_IN_SECONDS        180 

#define MIN_CONN_INTERVAL                 MSEC_TO_UNITS(500, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL                 MSEC_TO_UNITS(1000, UNIT_1_25_MS)
#define SLAVE_LATENCY                     0
#define CONN_SUP_TIMEOUT                  MSEC_TO_UNITS(4000, UNIT_10_MS)

#define FIRST_CONN_PARAMS_UPDATE_DELAY    APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)
#define NEXT_CONN_PARAMS_UPDATE_DELAY     APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER)
#define MAX_CONN_PARAMS_UPDATE_COUNT      3


#define APP_TIMER_PRESCALER               0
#define APP_TIMER_MAX_TIMERS              10
#define APP_TIMER_OP_QUEUE_SIZE           4

#define TIMER_TEST_INTERVAL               APP_TIMER_TICKS(10, APP_TIMER_PRESCALER)

#define SCHED_MAX_EVENT_DATA_SIZE         sizeof(app_timer_event_t)
#define SCHED_QUEUE_SIZE                  10

static app_timer_id_t                     m_test_timer_id;
static int                                x;

static uint16_t                           m_conn_handle = BLE_CONN_HANDLE_INVALID;   /**< Handle of the current connection. */
static ble_gap_adv_params_t               m_adv_params;

static void test_timeout_handler(void * p_context)
{
    ++x;
}

static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    /*ble_bps_on_ble_evt(&m_bps, p_ble_evt);
    ble_bas_on_ble_evt(&m_bas, p_ble_evt);*/
    ble_conn_params_on_ble_evt(p_ble_evt);

    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            m_conn_handle               = BLE_CONN_HANDLE_INVALID;

            //advertising_start();
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISING)
            {
                // Go to system-off mode (this function will not return; wakeup will cause a reset).
                err_code = sd_power_system_off();
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            if (p_ble_evt->evt.gatts_evt.params.timeout.src == BLE_GATT_TIMEOUT_SRC_PROTOCOL)
            {
                err_code = sd_ble_gap_disconnect(m_conn_handle,
                                                 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            // No implementation needed.
            break;
    }
}

void main(void)
{
    uint32_t err_code = 0;

    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, NULL);

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

    err_code = app_timer_create(&m_test_timer_id, APP_TIMER_MODE_REPEATED, test_timeout_handler);
    APP_ERROR_CHECK(err_code);

    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);

    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN );
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);

    ble_advdata_t advdata;

    ble_uuid_t adv_uuids[] =
    {
        {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}
    };

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = adv_uuids;

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    m_adv_params.p_peer_addr = NULL;                           // Undirected advertisement.
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = APP_ADV_INTERVAL;
    m_adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;

    ble_dis_init_t   dis_init;
    ble_dis_sys_id_t sys_id;

    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, MANUFACTURER_NAME);
    ble_srv_ascii_to_utf8(&dis_init.model_num_str,     MODEL_NUM);

    sys_id.manufacturer_id            = MANUFACTURER_ID;
    sys_id.organizationally_unique_id = ORG_UNIQUE_ID;
    dis_init.p_sys_id                 = &sys_id;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);

    ble_conn_params_init_t connection_params_init;

    memset(&connection_params_init, 0, sizeof(connection_params_init));

    connection_params_init.p_conn_params                  = NULL;
    connection_params_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    connection_params_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    connection_params_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    connection_params_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    connection_params_init.disconnect_on_fail             = false;
    connection_params_init.evt_handler                    = on_conn_params_evt;
    connection_params_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&connection_params_init);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_test_timer_id, TIMER_TEST_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);

    while (true)
    {
        app_sched_execute();

        err_code = sd_app_evt_wait();
        APP_ERROR_CHECK(err_code);
    }
}
