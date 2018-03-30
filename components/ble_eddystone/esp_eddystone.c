/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


/****************************************************************************
*
* This file is used for eddystone receiver.
*
****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "esp_bt.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_defs.h"
#include "esp_gattc_api.h"
#include "esp_gap_ble_api.h"
#include "freertos/FreeRTOS.h"

#include "esp_eddystone_protocol.h"
#include "esp_eddystone_api.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static const char* TAG = "EDDYSTONE_BLE";

/* declare static functions */
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param);
static void esp_eddystone_show_inform(const esp_eddystone_result_t* res, lua_State *L);

static lua_State *L1;
char *uid = "UyiNJXcSzXwfBCQIvwcB";

static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30
};

void addTableInt (lua_State *L, char *index, int value) {
        lua_pushstring(L, index); /* key */
        lua_pushnumber(L, value);  /* value */
        lua_settable(L, -3);
}

void addTableString (lua_State *L, char *index, const char *value) {
        lua_pushstring(L, index); /* key */
        lua_pushstring(L, value);  /* value */
        lua_settable(L, -3);
}

static void esp_eddystone_show_inform(const esp_eddystone_result_t* res, lua_State *L)
{
    switch(res->common.frame_type)
    {
        case EDDYSTONE_FRAME_TYPE_UID: {
            ESP_LOGI(TAG, "Eddystone UID inform:");
            addTableString(L, "type", "UID");
            ESP_LOGI(TAG, "Measured power(RSSI at 0m distance):%d dbm", res->inform.uid.ranging_data);
//            addTableInt(L, "power", res->inform.uid.ranging_data);
            ESP_LOGI(TAG, "Namespace ID:0x");
            esp_log_buffer_hex(TAG, res->inform.uid.namespace_id, 10);
//            addTableInt(L, "namespaceId", res->inform.uid.namespace_id);
            ESP_LOGI(TAG, "Instance ID:0x");
            esp_log_buffer_hex(TAG, res->inform.uid.instance_id, 6);
//            addTableInt(L, "instanceId", res->inform.uid.instance_id);
            break;
        }
        case EDDYSTONE_FRAME_TYPE_URL: {
            ESP_LOGI(TAG, "Eddystone URL inform:");
            addTableString(L, "type", "URL");
            ESP_LOGI(TAG, "Measured power(RSSI at 0m distance):%d dbm", res->inform.url.tx_power);
//            addTableInt(L, "power", res->inform.url.tx_power);
            ESP_LOGI(TAG, "URL: %s", res->inform.url.url);
//            addTableString(L, "url", res->inform.url.url);
            break;
        }
        case EDDYSTONE_FRAME_TYPE_TLM: {
            ESP_LOGI(TAG, "Eddystone TLM inform:");
            addTableString(L, "type", "TLM");
            ESP_LOGI(TAG, "version: %d", res->inform.tlm.version);
//            addTableInt(L, "version", res->inform.tlm.version);
            ESP_LOGI(TAG, "battery voltage: %d mV", res->inform.tlm.battery_voltage);
//            addTableInt(L, "voltage", res->inform.tlm.battery_voltage);
            ESP_LOGI(TAG, "beacon temperature in degrees Celsius: %6.1f", res->inform.tlm.temperature);
//            addTableInt(L, "temterature", res->inform.tlm.temperature);
            ESP_LOGI(TAG, "adv pdu count since power-up: %d", res->inform.tlm.adv_count);
//            addTableInt(L, "adv_count", res->inform.tlm.adv_count);
            ESP_LOGI(TAG, "time since power-up: %d s", res->inform.tlm.time);
//            addTableInt(L, "time", res->inform.tlm.time);
            break;
        }
        default:
            break;
    }
}


static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param)
{
    switch(event)
    {
        case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
            uint32_t duration = 0;
            esp_ble_gap_start_scanning(duration);
            break;
        }
        case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT: {
            if(param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG,"Scan start failed");
            }
            else {
                ESP_LOGI(TAG,"Start scanning...");
            }
            break;
        }
        case ESP_GAP_BLE_SCAN_RESULT_EVT: {
            esp_ble_gap_cb_param_t* scan_result = (esp_ble_gap_cb_param_t*)param;
            switch(scan_result->scan_rst.search_evt)
            {
                case ESP_GAP_SEARCH_INQ_RES_EVT: {
                	esp_eddystone_result_t eddystone_res;
                    memset(&eddystone_res, 0, sizeof(eddystone_res));
                    esp_err_t ret = esp_eddystone_decode(scan_result->scan_rst.ble_adv, scan_result->scan_rst.adv_data_len, &eddystone_res);
                    if (ret) {
                        // error:The received data is not an eddystone frame packet or a correct eddystone frame packet.
                        // just return
                        return;
                    } else {   
                        // The received adv data is a correct eddystone frame packet.
                        // Here, we get the eddystone infomation in eddystone_res, we can use the data in res to do other things.
                        // For example, just print them:
                    	lua_getglobal(L1, uid);
                    	lua_newtable(L1);
                        ESP_LOGI(TAG, "--------Eddystone Found----------");
                        esp_log_buffer_hex("EDDYSTONE_BLE: Device address:", scan_result->scan_rst.bda, ESP_BD_ADDR_LEN);
//                        addTableInt(L1, "device", scan_result->scan_rst.bda);
                        ESP_LOGI(TAG, "RSSI of packet:%d dbm", scan_result->scan_rst.rssi);
//                        addTableInt(L1, "rssi", scan_result->scan_rst.rssi);
                        esp_eddystone_show_inform(&eddystone_res, L1);
                        lua_pcall(L1, 1, 0, 0);
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:{
            if(param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG,"Scan stop failed");
            }
            else {
                ESP_LOGI(TAG,"Stop scan successfully");
            }
            break;
        }
        default:
            break;
    }
}

void esp_eddystone_appRegister(void)
{
    esp_err_t status;
    
    ESP_LOGI(TAG,"Register callback");


    /*<! register the scan callback function to the gap module */
    if((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
        ESP_LOGE(TAG,"gap register error,error code = %x",status);
        return;
    }
}

void esp_eddystone_init(void)
{
    esp_bluedroid_init();
    esp_bluedroid_enable();
    esp_eddystone_appRegister();
}

int eddystone_init(lua_State *L) {
	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	esp_bt_controller_init(&bt_cfg);
	esp_bt_controller_enable(ESP_BT_MODE_BLE);

	esp_eddystone_init();
	return 0;
}

int eddystone_run(lua_State *L)
{
	//create new lua thread for callbacks
	lua_settop(L, 1);
	L1 = L;
	lua_pushvalue(L1, 1);
	lua_setglobal(L1, uid);

	/*<! set scan parameters */
	esp_ble_gap_set_scan_params(&ble_scan_params);

	return 0;
}

int eddystone_stop(lua_State *L) {
	esp_ble_gap_stop_scanning();
	return 0;
}

static const struct luaL_Reg eddystone [] = {
		{"init", eddystone_init},
        {"start", eddystone_run},
		{"stop", eddystone_stop},
        {NULL, NULL}  /* sentinel */
};

int luaopen_eddystone (lua_State *L) {
        luaL_newlib(L, eddystone);
        return 1;
}
