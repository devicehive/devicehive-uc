#include "DeviceHiveEngine.h"

#include "uart.h"
#include <msp430.h>

#define RLED_PIN    BIT0 ///< @brief The red LED pin number.
#define GLED_PIN    BIT6 ///< @brief The green LED pin number.
#define BTN_PIN     BIT3 ///< @brief The push button pin number.

#define DEVICE_KEY              "uC_test"
#define DEVICE_NAME             "MSP430 Device"
#define DEVICE_CLASS_NAME       "SimpleDevice"
#define DEVICE_CLASS_VERSION    "1.1"

// {C73CCF23-8BF5-4C2C-B330-EAD36F469D1A}
const GUID DeviceID =           { 0xc73ccf23, 0x8bf5, 0x4c2c, { 0xb3, 0x30, 0xea, 0xd3, 0x6f, 0x46, 0x9d, 0x1a } };

#define NP_EQUIPMENT            "equipment"
#define NP_STATE                "state"
#define NOTIFY_EQUIPMENT        "equipment"
#define LED_CMD_NAME            "UpdateLedState"

#define LED0_EQP_NAME           "red LED"
#define LED0_EQP_CODE           "LED_R"
#define LED0_EQP_TYPE           "Controllable LED"

#define LED1_EQP_NAME           "green LED"
#define LED1_EQP_CODE           "LED_G"
#define LED1_EQP_TYPE           "Controllable LED"

#define BTN_EQP_NAME            "Button"
#define BTN_EQP_CODE            "BTN"
#define BTN_EQP_TYPE            "Button"

#define ERR_UNKNOWN_CMD         "Unknown command"
#define ERR_FAIL                "Failed"
#define ERR_INVALID_VALUE       "Invalid data. 1-byte Boolean value expected."
#define ERR_EMPTY_EQUIPMENT     "Failed to receive the equipment name."
#define ERR_INVALID_EQUIPMENT   "The specified equipment not supported."
#define ERR_TIMEOUT             "Failed to retrieve LED state due to timeout."

#define RESULT_OK               "OK"
#define ST_SUCCESS              "Led command successfully executed"

#define D2G_EQUIPMENT           (MIN_CUSTOM_INTENT)
#define G2D_LED                 (MIN_CUSTOM_INTENT + 1)

static /*const*/ Equipment EQUIPMENTS[] =
{
    EQUIPMENT_INIT(LED0_EQP_NAME, LED0_EQP_CODE, LED0_EQP_TYPE),
    EQUIPMENT_INIT(LED1_EQP_NAME, LED1_EQP_CODE, LED1_EQP_TYPE),
    EQUIPMENT_INIT(BTN_EQP_NAME, BTN_EQP_CODE, BTN_EQP_TYPE)
};

static /*const*/ Parameter NOTIFICATION_PARAMS[] =
{
    PARAMETER_INIT(DT_STRING, NP_EQUIPMENT),
    PARAMETER_INIT(DT_BOOLEAN, NP_STATE)
};

static /*const*/ Notification NOTIFICATIONS[] =
{
    NOTIFICATION_INIT(D2G_EQUIPMENT, NOTIFY_EQUIPMENT, NOTIFICATION_PARAMS)
};

static /*const*/ Command COMMANDS[] =
{
    COMMAND_INIT(G2D_LED, LED_CMD_NAME, NOTIFICATION_PARAMS)
};


static int strcmp(const char *s1, const char *s2)
{
     while (*s1 && (*s1 == *s2))
     {
         ++s1;
         ++s2;
     }

     return ((*s1 < *s2)
         ? -1 : (*s1 > *s2));
}

/** @brief Send device registration metadata. */
static void SendRegMessage()
{
    RegData rd;

    rd.DeviceID = DeviceID;
    rd.DeviceKey = DEVICE_KEY;
    rd.DeviceName = DEVICE_NAME;
    rd.DeviceClassName = DEVICE_CLASS_NAME;
    rd.DeviceClassVersion = DEVICE_CLASS_VERSION;
    rd.Equipment.Length = CountOf(EQUIPMENTS);
    rd.Equipment.Items = EQUIPMENTS;
    rd.Notifications.Length = CountOf(NOTIFICATIONS);
    rd.Notifications.Items = NOTIFICATIONS;
    rd.Commands.Length = CountOf(COMMANDS);
    rd.Commands.Items = COMMANDS;

    SendRegistrationData(&rd);
}

/** @brief Send equipment notification. */
static void SendEquipmentNotification(const char *EquipmentCode, BOOL state)
{
    MessageHeader msgh = MESSAGE_HEADER_INIT(D2G_EQUIPMENT);
    msgh.Length = sizeof(BOOL) + GetStringSize(EquipmentCode);

    ResetChecksum(CHECKSUM_TX);
    SendBytes((unsigned char*)&msgh, sizeof(msgh));
    SendString(EquipmentCode);
    SendBytes(&state, sizeof(state));
    SendChecksum();
}

/** @brief Process the input command. */
static void ProcessCommand(const MessageHeader *msgh)
{
    DWORD cmdId = 0;
    if (RecvBytes((BYTE*)&cmdId, sizeof(cmdId)) == sizeof(cmdId))
    {
        char eqp[64];
        if (RecvString(eqp, sizeof(eqp)) > 0)
        {
            BOOL state = FALSE;
            if (RecvBytes(&state, sizeof(state)) == sizeof(state))
            {
                if (RecvAndValidateChecksum())
                {
                    if (0 == strcmp(eqp, LED0_EQP_CODE))
                    {
                        if (state != 0)
                            P1OUT |= RLED_PIN;
                        else
                            P1OUT &= ~RLED_PIN;
                        SendNotificationData(cmdId, ST_SUCCESS, RESULT_OK);
                        SendEquipmentNotification(LED0_EQP_CODE, state);
                    }
                    else if (0 == strcmp(eqp, LED1_EQP_CODE))
                    {
                        if (state != 0)
                            P1OUT |= GLED_PIN;
                        else
                            P1OUT &= ~GLED_PIN;
                        SendNotificationData(cmdId, ST_SUCCESS, RESULT_OK);
                        SendEquipmentNotification(LED1_EQP_CODE, state);
                    }
                }
            }
            else
                SendNotificationData(cmdId, ERR_INVALID_VALUE, ERR_FAIL);
        }
        else
            SendNotificationData(cmdId, ERR_EMPTY_EQUIPMENT, ERR_FAIL);
    }
    else
        SendNotificationData(0, ERR_TIMEOUT, ERR_FAIL);
}

/** @brief The button state cache. */
static BOOL g_prevBtnState = 0;


/** @brief Initialize device. */
static void Setup()
{
    // stop watchdog timer
    WDTCTL = WDTPW + WDTHOLD;

    // setup P1
    P1DIR = (GLED_PIN|RLED_PIN); // all other pins are 'input'
    P1OUT &= ~(GLED_PIN|RLED_PIN); // switch it off
    P1REN |= BTN_PIN;  // (!) important for revision 1.5
    //P1IFG &= ~BTN_PIN; // clear interrupt flag
    //P1IES |= BTN_PIN;  // wait for 'down'
    //P1IE |= BTN_PIN;   // enable button interrupt

    UartInit(UART_BAUD_9600);
}


/** @brief The application entry point. */
void main(void)
{
    Setup();
    InitializeDeviceHive(&UartSendBytes, &UartRecvBytes);

    // Initial notifications
    SendEquipmentNotification(LED0_EQP_CODE, (P1OUT&RLED_PIN)!=0);
    SendEquipmentNotification(LED1_EQP_CODE, (P1OUT&GLED_PIN)!=0);
    SendEquipmentNotification(BTN_EQP_CODE, g_prevBtnState);
    while (TRUE)
    {
        BYTE btn = !(P1IN&BTN_PIN);

        if (g_prevBtnState != btn)
        {
            SendEquipmentNotification(BTN_EQP_CODE, btn);
            g_prevBtnState = btn;
        }

        if (IFG2&UCA0RXIFG)
        {
            MessageHeader msgh;
            if (ReceiveMessageHeader(&msgh))
            {
                switch (msgh.Intent)
                {
                    case G2D_REQREG:
                    {
                        // no body
                        if (RecvAndValidateChecksum())
                        {
                            SendRegMessage();
                            SendEquipmentNotification(LED0_EQP_CODE, (P1OUT&RLED_PIN)!=0);
                            SendEquipmentNotification(LED1_EQP_CODE, (P1OUT&GLED_PIN)!=0);
                            SendEquipmentNotification(BTN_EQP_CODE, g_prevBtnState);
                        }
                    } break;
                    case G2D_LED:
                        ProcessCommand(&msgh);
                        break;
                    default:
                        SkipMessage(msgh.Length);
                        //SendNotificationMessage(0, ERR_UNKNOWN_CMD, ERR_FAIL);
                        break;
                }
            }
        }
    }
}
