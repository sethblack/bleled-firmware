#include "ble_candle.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"

static void on_connect(ble_candle_t * p_lbs, ble_evt_t * p_ble_evt) {
    p_lbs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

static void on_disconnect(ble_candle_t * p_lbs, ble_evt_t * p_ble_evt) {
    UNUSED_PARAMETER(p_ble_evt);
    p_lbs->conn_handle = BLE_CONN_HANDLE_INVALID;
}

static void on_write(ble_candle_t * p_lbs, ble_evt_t * p_ble_evt) {
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_lbs->candle_write_handler == NULL) {
        return;
    }

    if ((p_evt_write->handle == p_lbs->candle_char_handles.value_handle) &&
        (p_evt_write->len == 2)) {
        p_lbs->candle_write_handler(p_lbs, p_evt_write->data[0], p_evt_write->data[1]);
    }
}

void ble_candle_on_ble_evt(ble_candle_t * p_lbs, ble_evt_t * p_ble_evt) {
    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_lbs, p_ble_evt);
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_lbs, p_ble_evt);
            break;
            
        case BLE_GATTS_EVT_WRITE:
            on_write(p_lbs, p_ble_evt);
            break;
            
        default:
            break;
    }
}

static uint32_t candle_char_add(ble_candle_t * p_lbs, const ble_candle_init_t * p_lbs_init) {
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_lbs->uuid_type;
    ble_uuid.uuid = CANDLE_UUID_CHAR;
    
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 4;
    attr_char_value.max_len      = 4;
    uint8_t value[4]             = {0x00,0x00,0x00,0x00};
    attr_char_value.p_value      = value;
    
    return sd_ble_gatts_characteristic_add(p_lbs->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_lbs->candle_char_handles);
}

uint32_t ble_candle_service_init(ble_candle_t * p_lbs, const ble_candle_init_t * p_lbs_init) {
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    p_lbs->conn_handle          = BLE_CONN_HANDLE_INVALID;
    p_lbs->candle_write_handler = p_lbs_init->candle_write_handler;

    ble_uuid128_t base_uuid = {CANDLE_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_lbs->uuid_type);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
    
    ble_uuid.type = p_lbs->uuid_type;
    ble_uuid.uuid = CANDLE_UUID_BASE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_lbs->service_handle);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
    
    err_code = candle_char_add(p_lbs, p_lbs_init);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
    
    return NRF_SUCCESS;
}