#ifndef __DEVICE_CONSTANTS_H__
#define __DEVICE_CONSTANTS_H__

#include "Guid.h"

#define DEVICE_KEY              ((char*)"E2C_test")
#define DEVICE_NAME             ((char*)"PIC Device")
#define DEVICE_CLASS_NAME       ((char*)"SimpleDevice")
#define DEVICE_CLASS_VERSION    ((char*)"1.0")

#define EQUIPMENT_LENGTH        2
#define NOTIFICATIONS_COUNT     1
#define COMMANDS_COUNT          1
#define NPEQ_COUNT              2
#define LED_CMD_COUNT           1

const GUID DeviceID =           {0x7201BE97, 0x4677, 0x4A57, {0xAA, 0xB1, 0x89, 0x56, 0x39, 0xAF, 0x8B, 0xD5}};

#define NP_EQUIPMENT            ((char*)"equipment")
#define NP_STATE                ((char*)"state")
#define NOTIFY_EQUIPMENT        ((char*)"equipment")
#define LED_CMD_NAME            ((char*)"UpdateLedState")

#define LED_EQP_NAME            ((char*)"LED #1")
#define LED_EQP_CODE            ((char*)"LED1")
#define LED_EQP_TYPE            ((char*)"Controllable LED")

#define BTN_EQP_NAME            ((char*)"Button")
#define BTN_EQP_CODE            ((char*)"BTN")
#define BTN_EQP_TYPE            ((char*)"Button")

#define ERR_UNKNOWN_CMD         ((char*)"Unknown command")
#define ERR_FAIL                ((char*)"Failed")
#define ERR_INVALID_VALUE       ((char*)"Invalid data. 1-byte Boolean value expected.")
#define ERR_EMPTY_EQUIPMENT     ((char*)"Failed to receive the equipment name.")
#define ERR_INVALID_EQUIPMENT   ((char*)"The specified equipment not supported.")
#define ERR_TIMEOUT             ((char*)"Failed to retrieve LED state due to timeout.")
#define ERR_CHECKSUM            ((char*)"Invalid checksum.")

#define RESULT_OK               ((char*)"OK")
#define ST_SUCCESS              ((char*)"Led command successfully executed")

#define D2G_EQUIPMENT           (MIN_CUSTOM_INTENT)
#define G2D_LED                 (MIN_CUSTOM_INTENT + 1)

typedef struct tagEquipmentNotification
{
    char * EquipmentCode;
    BOOL State;
} EquipmentNotification;

#endif
