#include "DeviceHiveEngine.h"

#include "uart.h"
#include <msp430.h>

#define RLED_PIN    BIT0 ///< @brief The red LED pin number.
#define GLED_PIN    BIT6 ///< @brief The green LED pin number.
#define BTN_PIN     BIT3 ///< @brief The push button pin number.

#define DEVICE_ID               "c73ccf23-8bf5-4c2c-b330-ead36f469d1a"
#define DEVICE_KEY              "uC_test"
#define DEVICE_NAME             "MSP430 Device"
#define DEVICE_CLASS_NAME       "SimpleDevice"
#define DEVICE_CLASS_VERSION    "1.1"


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

#define ST_SUCCESS              "Success"
#define ST_FAIL                 "Failed"
#define RESULT_OK               "OK"

#define D2G_EQUIPMENT           (256)
#define G2D_LED                 (257)


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
    SendRegistration2Data("{"
        "id:\"" DEVICE_ID "\","
        "key:\"" DEVICE_KEY "\","
        "name:\"" DEVICE_NAME "\","
        "deviceClass:{"
            "name:\"" DEVICE_CLASS_NAME"\","
            "version:\"" DEVICE_CLASS_VERSION "\"},"
        "equipment:["
            "{code:\"" LED0_EQP_CODE "\",name:\"" LED0_EQP_NAME "\",type:\"" LED0_EQP_TYPE "\"},"
            "{code:\"" LED1_EQP_CODE "\",name:\"" LED1_EQP_NAME "\",type:\"" LED1_EQP_TYPE "\"},"
            "{code:\""  BTN_EQP_CODE "\",name:\""  BTN_EQP_NAME "\",type:\""  BTN_EQP_TYPE "\"}"
            "],"
        "commands:["
            "{intent:257,name:\"UpdateLedState\",params:{equipment:str,state:bool}}"
            "],"
        "notifications:["
            "{intent:256,name:\"equipment\",params:{equipment:str,state:bool}}"
            "]"
        "}");
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
                    else
                        SendNotificationData(cmdId, ST_FAIL, "The specified equipment not supported");
                }
            }
        }
    }
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

    SendRegMessage();

    g_prevBtnState = !(P1IN&BTN_PIN);
    SendEquipmentNotification(BTN_EQP_CODE, g_prevBtnState);
    SendEquipmentNotification(LED0_EQP_CODE, (P1OUT&RLED_PIN)!=0);
    SendEquipmentNotification(LED1_EQP_CODE, (P1OUT&GLED_PIN)!=0);

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
                        break;
                }
            }
        }
    }
}
