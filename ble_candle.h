#ifndef __BLE_CANDLE_SERVICE_H__
#define __BLE_CANDLE_SERVICE_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

#define CANDLE_UUID_BASE 0x7D, 0x20, 0xA1, 0xE4, 0x43, 0x8A, 0x4E, 0xEC, 0xB2, 0x47, 0x1A, 0x0A, 0x89, 0xBE, 0x0F, 0xFF
#define CANDLE_UUID_SERVICE 0x1010
#define CANDLE_UUID_CHAR 0x1020

typedef struct ble_candle_s ble_candle_t;

typedef void (*ble_candle_write_handler_t) (ble_candle_t * p_candle, uint8_t brightness, uint8_t speed);

typedef struct {
    ble_candle_write_handler_t candle_write_handler;
} ble_candle_init_t;

typedef struct ble_candle_s {
    uint16_t                    service_handle;
    ble_gatts_char_handles_t    candle_char_handles;
    uint8_t                     uuid_type;
    uint16_t                    conn_handle;
    ble_candle_write_handler_t  candle_write_handler;
} ble_candle_t;

uint32_t ble_candle_service_init(ble_candle_t * p_lbs, const ble_candle_init_t * p_lbs_init);
void ble_candle_on_ble_evt(ble_candle_t * p_lbs, ble_evt_t * p_ble_evt);

#endif