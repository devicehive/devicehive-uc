#ifndef __DEVICEHIVE_MESSAGE_H__
#define __DEVICEHIVE_MESSAGE_H__

#include "Parameter.h"
#include "Guid.h"
#include "Equipment.h"
#include "Notification.h"
#include "Command.h"

/** @ingroup Constants
@brief The message intents.
*/
enum Intent
{
    /** @brief The registration request.
    @details A command sent to device. The device should respond by sending a registration response.
    */
    G2D_REQREG          = 0,

    /** @brief The registration response.
    @details Notification sent by the device in response to registration request.
    */
    D2G_REGISTER        = 1,

    /** @brief The command result notification.
    @details This message is sent in response to any command message.
    */
    D2G_NOTIFY          = 2,

    /** @brief Custom device-specific intent.
    This is the minimum intent number for user-specific intents.
    */
    MIN_CUSTOM_INTENT   = 256
};


/** @ingroup Constants
@brief The message format constants.
*/
enum MessageFormat
{
    /** @brief DeviceHive signature (first byte). */
    DEVICEHIVE_SIGNATURE1   = 0xC5,

    /** @brief DeviceHive signature (second byte). */
    DEVICEHIVE_SIGNATURE2   = 0xC3,

    /** @brief DeviceHive protocol version. */
    DEVICEHIVE_VERSION      = 1
};


/** @ingroup Constants
@brief The message header check result.
*/
enum MessageCheckResult
{
    /** @brief Header check success. */
    MSG_CHECK_OK,

    /** @brief Wrong message signature. */
    MSG_CHECK_SIGNATURE,

    /** @brief Incorrect version. */
    MSG_CHECK_VERSION,

    /** @brief Invalid checksum. */
    MSG_CHECK_CHECKSUM
};


/** @ingroup Types
@brief The message header.
@details This structure describes message header according to binary protocol specification.
It is a block of data sent or received over data lines by the device.
*/
typedef struct
{
    /** @brief The signature (first byte).
    @details Should always be #DEVICEHIVE_SIGNATURE1.
    */
    BYTE Signature1;

    /** @brief The signature (second byte).
    @details Should always be #DEVICEHIVE_SIGNATURE1.
    */
    BYTE Signature2;

    /** @brief The protocol version.
    @details Should always be #DEVICEHIVE_VERSION.
    */
    BYTE Version;

    /** @brief The header flags.
    @details Should be zero.
    */
    BYTE Flags;

    /** @brief The message length.
    @details Length of the message data, not including the size of the header and checksum.
    */
    WORD Length;

    /** @brief The message intent.
    @details See the binary protocol definition for all available intents.
    @see Intent
    */
    WORD Intent;
} MessageHeader;


/** @relates MessageHeader
@warning The "Length" field must be initialized later!
@brief Initialize the message header.
@param[in] intent The message intent.
*/
#define MESSAGE_HEADER_INIT(intent)                 \
    { DEVICEHIVE_SIGNATURE1, DEVICEHIVE_SIGNATURE2, \
      DEVICEHIVE_VERSION, 0, 0, intent }


/** @ingroup Types
@brief The registration data.
@details This structure represents a registration response data
for #D2G_REGISTER intent, according to binary protocol specification.
*/
typedef struct
{
    /** @brief Device UID.
    @details The device implementer should generate this key and use it for a device.
    Each device in all DeviceHive network should have a unique ID.
    */
    GUID DeviceID;

    /** @brief Device key.
    @details The security key for the device. It should be a long string containing secure key data.
    Usually, it is a string representation of GUID.
    The device implementer should create this key and use it for registration purpose.
    */
    const char *DeviceKey;

    /** @brief Device name. */
    const char *DeviceName;

    /** @brief Name of the device class. */
    const char *DeviceClassName;

    /** @brief Version of the device class.
    @details Although it is a string, a version-like number is expected, e.g. "1.004"
    */
    const char *DeviceClassVersion;

    /** @brief Equipment metadata. */
    EquipmentArray Equipment;

    /** @brief Notification metadata. */
    NotificationArray Notifications;

    /** @brief Command metadata. */
    CommandArray Commands;
} RegData;


/** @ingroup Types
@brief The command result notification data.
@details Represents a notification structure according to binary protocol specification.
This structure is sent by device to notify the command result.
Notifications are sent with #D2G_NOTIFY intent.
*/
typedef struct
{
    /** @brief Command ID.
    @details ID of the command which caused the notification.
    Can be 0 if there was no command which triggered it.
    */
    DWORD CommandID;

    /** @brief Command status.
    @details Status is a string with text describing the command status.
    */
    const char *Status;

    /** @brief Command result.
    @details Text description of a notification result.
    It is related to command result notifications and is related, by meaning,
    to the command which caused a notification.
    */
    const char *Result;
} NotificationData;


/** @ingroup Types
@brief The command data.
@details This structure represents a command received by the device.
It contains only a common part for all the commands.
Command data following this structure depends on the type of the command.
During the data transmission, command parameters are transmitted directly after this structure.
*/
typedef struct
{
    /** @brief Command ID
    @details This ID is automatically generated by server to identify the command.
    Implementers should consider it to send notifications with the corresponding ID after the command is executed.
    */
    DWORD CommandID;
} CommandData;

#endif
