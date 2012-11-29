#ifndef __CUSTOM_COMMANDS_H__
#define __CUSTOM_COMMANDS_H__

typedef struct tagEquipmentNotificationData
{
    char * EquipmentCode;
    BOOL State;
} EquipmentNotificationData;

typedef struct tagLedCommandData
{
    DWORD Code;
    char * Equipment;
    BOOL State;
} LedCommandData;

#endif
