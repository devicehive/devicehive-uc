#ifndef __DEVICE_CONSTANTS_H__
#define __DEVICE_CONSTANTS_H__

#include "Guid.h"

#define DEVICE_KEY              "E2C_test"
#define DEVICE_NAME             "PIC Device"
#define DEVICE_CLASS_NAME       "SimpleDevice"
#define DEVICE_CLASS_VERSION    "1.0"

#define EQUIPMENT_LENGTH        2
#define NOTIFICATIONS_COUNT     1
#define COMMANDS_COUNT          1
#define NPEQ_COUNT              2
#define LED_CMD_COUNT           1

//const GUID DeviceID =           {0x7201BE97, 0x4677, 0x4A57, {0xAA, 0xB1, 0x89, 0x56, 0x39, 0xAF, 0x8B, 0xD5}};
#define DEVICE_ID               "7201be97-4677-4a57-aab1-895639af8bd5"

#define NP_EQUIPMENT            "equipment"
#define NP_STATE                "state"
#define NOTIFY_EQUIPMENT        "equipment"
#define LED_CMD_NAME            "UpdateLedState"

#define LED_EQP_NAME            "LED #1"
#define LED_EQP_CODE            "LED1"
#define LED_EQP_TYPE            "Controllable LED"

#define BTN_EQP_NAME            "Button"
#define BTN_EQP_CODE            "BTN"
#define BTN_EQP_TYPE            "Button"

#define ERR_UNKNOWN_CMD         "Unknown command"
#define ERR_FAIL                "Failed"
#define ERR_INVALID_VALUE       "Invalid data. 1-byte Boolean value expected."
#define ERR_EMPTY_EQUIPMENT     "Failed to receive the equipment name."
#define ERR_INVALID_EQUIPMENT   "The specified equipment not supported."
#define ERR_TIMEOUT             "Failed to retrieve LED state due to timeout."
#define ERR_CHECKSUM            "Invalid checksum."

#define RESULT_OK               "OK"
#define ST_SUCCESS              "Led command successfully executed"

#define D2G_EQUIPMENT           (MIN_CUSTOM_INTENT)
#define G2D_LED                 (MIN_CUSTOM_INTENT + 1)

typedef struct tagEquipmentNotification
{
    char * EquipmentCode;
    BOOL State;
} EquipmentNotification;

#endif
