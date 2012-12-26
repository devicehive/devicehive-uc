#include <htc.h>
#include "usart_16f88.h"
#include "DeviceHiveEngine.h"
#include "DeviceConstants.h"
#include "CustomCommands.h"

__CONFIG(0x3738);

#define LED_PIN     RA0
#define TEST_PIN    RA1
#define BTN_PIN     RB4

#define TRUE    1
#define FALSE   0

BOOL PrevBtn;

void Setup()
{
    ANSEL = 0b00000000;
    TRISA = 0b00000000;
    TRISB = 0b00010000;

    UsartInit(USART_BAUD_38400); // This is the fastest speed PIC16F88 can provide

    PrevBtn = FALSE;
}


//Equipment eqp[] =
//{
//    {LED_EQP_NAME, LED_EQP_CODE, LED_EQP_TYPE},
//    {BTN_EQP_NAME, BTN_EQP_CODE, BTN_EQP_TYPE}
//};
//
//Equipment * LedEqp = &(eqp[0]);
//Equipment * BtnEqp = &(eqp[1]);
//
//Parameter NotificationParams[] =
//{
//    {DT_STRING, NP_EQUIPMENT},
//    {DT_BOOLEAN, NP_STATE}
//};
//
//Notification ntf[] =
//{
//    {D2G_EQUIPMENT, NOTIFY_EQUIPMENT, {NPEQ_COUNT, NotificationParams}},
//};
//
//Command cmds[] =
//{
//    {G2D_LED, LED_CMD_NAME, {NPEQ_COUNT, NotificationParams}}
//};
//
void SendRegMessage()
{
//    RegData rd;
//
//    rd.DeviceID = DeviceID;
//    rd.DeviceKey = DEVICE_KEY; //dk;
//    rd.DeviceName = DEVICE_NAME; //dn;
//    rd.DeviceClassName = DEVICE_CLASS_NAME;
//    rd.DeviceClassVersion = DEVICE_CLASS_VERSION;
//    rd.Equipment.Length = EQUIPMENT_LENGTH;
//    rd.Equipment.Items = eqp;
//    rd.Notifications.Length = NOTIFICATIONS_COUNT;
//    rd.Notifications.Items = ntf;
//    rd.Commands.Length = COMMANDS_COUNT;
//    rd.Commands.Items = cmds;
//
//    SendRegistrationData(&rd);

    SendRegistration2Data("{"
        "id:\"" DEVICE_ID "\","
        "key:\"" DEVICE_KEY "\","
        "name:\"" DEVICE_NAME "\","
        "deviceClass:{"
            "name:\"" DEVICE_CLASS_NAME"\","
            "version:\"" DEVICE_CLASS_VERSION "\"},"
        "equipment:["
            "{code:\"" LED_EQP_CODE "\",name:\"" LED_EQP_NAME "\",type:\"" LED_EQP_TYPE "\"},"
            "{code:\"" BTN_EQP_CODE "\",name:\"" BTN_EQP_NAME "\",type:\"" BTN_EQP_TYPE "\"}"
            "],"
        "commands:["
            "{intent:257,name:\"UpdateLedState\",params:{equipment:str,state:bool}}"
            "],"
        "notifications:["
            "{intent:256,name:\"equipment\",params:{equipment:str,state:bool}}"
            "]"
        "}");
}

void SendEquipmentNotification(const char * EquipmentCode, BOOL state)
{
    WORD len = sizeof(BOOL) + GetStringSize(EquipmentCode);

    MessageHeader msgh = MESSAGE_HEADER_INIT(D2G_EQUIPMENT);
    msgh.Length = len;

    ResetChecksum(CHECKSUM_TX);
    SendBytes((unsigned char*)&msgh, sizeof(msgh));
    SendString(EquipmentCode);
    SendBytes((unsigned char*)&state, sizeof(state));
    SendChecksum();
}

void ProcessCommand(MessageHeader * msgh)
{
    LedCommandData lcd = {FALSE, 0};

    if (RecvBytes((BYTE*)&lcd.Code, sizeof(DWORD)) > 0)
    {
        char buf[50];
        if (RecvString(buf, 49) > 0)
        {
            lcd.State = FALSE;

            if (RecvBytes(&lcd.State, sizeof(lcd.State)) && (lcd.State == TRUE || lcd.State == FALSE))
            {
                if (RecvAndValidateChecksum())
                {
                    LED_PIN = lcd.State;
                    SendNotificationData(lcd.Code, ST_SUCCESS, RESULT_OK);
                    SendEquipmentNotification(LED_EQP_CODE, lcd.State);
                }
                else
                {
                    SendNotificationData(lcd.Code, ERR_CHECKSUM, ERR_FAIL);
                }
            }
            else
            {
                SendNotificationData(lcd.Code, ERR_INVALID_VALUE, ERR_FAIL);
            }
        }
        else
        {
            SendNotificationData(lcd.Code, ERR_EMPTY_EQUIPMENT, ERR_FAIL);
        }
    }
    else
    {
        SendNotificationData(0, ERR_TIMEOUT, ERR_FAIL);
    }
}

void main()
{
    Setup();

    InitializeDeviceHive(&UsartSendBytes, &UsartRecvBytes);

    // Initial notifications
    SendRegMessage();
    SendEquipmentNotification(LED_EQP_CODE, LED_PIN);
    SendEquipmentNotification(BTN_EQP_CODE, FALSE);

    while (TRUE)
    {
        BYTE btn = !BTN_PIN;
        TEST_PIN = btn;

        if (PrevBtn != btn)
        {
            SendEquipmentNotification(BTN_EQP_CODE, btn);
            PrevBtn = btn;
        }

        if (RCIF)
        {
            MessageHeader msgh;

            if (ReceiveMessageHeader(&msgh))
            {
                switch (msgh.Intent)
                {
                    case G2D_REQREG:
                        if (RecvAndValidateChecksum())
                        {
                            SendRegMessage();
                            SendEquipmentNotification(LED_EQP_CODE, LED_PIN);
                            SendEquipmentNotification(BTN_EQP_CODE, btn);
                        }
                        break;
                    case G2D_LED:
                        ProcessCommand(&msgh);
                        break;
                    default:
                        SkipMessage(msgh.Length);
                        break;
                }

            }
        }
    }
}
