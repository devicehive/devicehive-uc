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


Equipment eqp[] =
{
    {LED_EQP_NAME, LED_EQP_CODE, LED_EQP_TYPE},
    {BTN_EQP_NAME, BTN_EQP_CODE, BTN_EQP_TYPE}
};

Equipment * LedEqp = &(eqp[0]);
Equipment * BtnEqp = &(eqp[1]);

Parameter NotificationParams[] =
{
    {DT_STRING, NP_EQUIPMENT},
    {DT_BOOLEAN, NP_STATE}
};

Notification ntf[] =
{
    {D2G_EQUIPMENT, NOTIFY_EQUIPMENT, {NPEQ_COUNT, NotificationParams}},
};

Command cmds[] =
{
    {G2D_LED, LED_CMD_NAME, {NPEQ_COUNT, NotificationParams}}
};

void SendRegMessage()
{
    RegData rd;

    rd.DeviceID = DeviceID;
    rd.DeviceKey = DEVICE_KEY; //dk;
    rd.DeviceName = DEVICE_NAME; //dn;
    rd.DeviceClassName = DEVICE_CLASS_NAME;
    rd.DeviceClassVersion = DEVICE_CLASS_VERSION;
    rd.Equipment.Length = EQUIPMENT_LENGTH;
    rd.Equipment.Items = eqp;
    rd.Notifications.Length = NOTIFICATIONS_COUNT;
    rd.Notifications.Items = ntf;
    rd.Commands.Length = COMMANDS_COUNT;
    rd.Commands.Items = cmds;

    SendRegistrationData(&rd);
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
                    SendEquipmentNotification(LedEqp->Code, lcd.State);
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
    SendEquipmentNotification(LedEqp->Code, LED_PIN);
    SendEquipmentNotification(BtnEqp->Code, FALSE);

    while (TRUE)
    {
        BYTE btn = !BTN_PIN;
        TEST_PIN = btn;

        if (PrevBtn != btn)
        {
            SendEquipmentNotification(BtnEqp->Code, btn);
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
                            SendEquipmentNotification(LedEqp->Code, LED_PIN);
                            SendEquipmentNotification(BtnEqp->Code, btn);
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
