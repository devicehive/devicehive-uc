#ifndef __DEVICEHIVE_ENGINE_H__
#define __DEVICEHIVE_ENGINE_H__

#include "Message.h"


/** @ingroup Functions
@brief Initialize a DeviceHive Framework.
@details Call this function prior to using any other DeviceHive functions.
@param[in] SendBytesPtr pointer to a function that sends a sequence of bytes.
@param[in] RecvBytesPtr pointer to a function that receives a sequence of bytes.
*/
void InitializeDeviceHive(
    int (*SendBytesPtr)(const unsigned char*, int),
    int (*RecvBytesPtr)(unsigned char*, int));


/** @ingroup Functions
@brief Send custom bytes.
@param[in] data The data buffer.
@param[in] length The buffer length in bytes.
@return The number of bytes sent.
    Negative number in case of error.
*/
int SendBytes(const unsigned char * data, int length);

/** @ingroup Functions
@brief Receive custom bytes.
@param[out] data The data buffer.
@param[in] length The buffer length in bytes.
@return The number of bytes received.
    Negative number in case of error.
*/
int RecvBytes(unsigned char * data, int length);


/** @ingroup Functions
@brief Get the size of a string.
@param[in] s The string.
@return The buffer size in bytes.
*/
WORD GetStringSize(const char *s);

/** @ingroup Functions
@brief Send a string.
@param[in] s The string to send.
*/
void SendString(const char *s);

/** @ingroup Functions
@brief Receive a string.
@param[out] s The string to be received.
@param[in] maxLength The maximum length of a string buffer.
@return The number of bytes received or negative value in case of error.
*/
int RecvString(char *s, int maxLength);


/** @relates Parameter @ingroup Functions
@brief Get the size of parameter metadata.
@param[in] p The parameter metadata.
@return The buffer size in bytes.
*/
WORD GetParameterSize(const Parameter *p);

/** @relates Parameter @ingroup Functions
@brief Send parameter metadata.
@param[in] p The parameter metadata to send.
*/
void SendParameter(const Parameter *p);


/** @relates Equipment @ingroup Functions
@brief Get the equipment metadata size.
@param[in] e The equipment metadata.
@return The buffer size in bytes.
*/
WORD GetEquipmentSize(const Equipment *e);

/** @relates Equipment @ingroup Functions
@brief Send equipment metadata.
@param[in] e The equipment metadata to send.
*/
void SendEquipment(const Equipment *e);

/** @relates Notification @ingroup Functions
@brief Get the notification metadata size.
@param[in] n The notification metadata.
@return The buffer size in bytes.
*/
WORD GetNotificationSize(const Notification *n);

/** @relates Notification @ingroup Functions
@brief Send notification metadata.
@param[in] n The notification metadata to send.
*/
void SendNotification(const Notification *n);

/** @relates Command @ingroup Functions
@brief Get the command metadata size.
@param[in] c The command metadata.
@return The buffer size in bytes.
*/
WORD GetCommandSize(const Command *c);

/** @relates Command @ingroup Functions
@brief Send command metadata.
@param[in] c The command metadata to send.
*/
void SendCommand(const Command *c);

/** @relates MessageHeader @ingroup Functions
@brief Receive message header.
@details The function returns FALSE if the received data does not match
the message header definition or if timeout happens when RecvBytes() fails.
So it is a common practice to run it in a loop if waiting for a new message.
@param[in] pMsg Pointer to the message header structure to be received.
@return TRUE if the header was successfully received, FALSE otherwise.
*/
BOOL ReceiveMessageHeader(MessageHeader *pMsg);

/** @relates MessageHeader @ingroup Functions
@brief Validate message header.
@param[in] msgh The message header to check.
@return
    - MSG_CHECK_OK if the message header is OK
    - MSG_CHECK_SIGNATURE if the signature is invalid
    - MSG_CHECK_VERSION if the version is invalid
*/
int CheckHeader(const MessageHeader *msgh);

/** @relates MessageHeader @ingroup Functions
@brief Skip the message.
@details Also skips message's checksum byte.
@param[in] length The message data length in bytes.
*/
void SkipMessage(unsigned int length);

/** @relates RegData @ingroup Functions
@brief Get registration data size.
@param[in] rd The registration data.
@return The buffer size in bytes.
*/
WORD GetRegistrationDataSize(const RegData *rd);

/** @relates RegData @ingroup Functions
@brief Send registration data.
@param rd The registration data to send.
*/
void SendRegistrationData(const RegData *rd);


/** @relates NotificationData @ingroup Functions
@brief Get command result notification data size.
@param[in] nd The command result notification data.
@return The buffer size in bytes.
*/
WORD GetNotificationDataSize(const NotificationData *nd);

/** @relates NotificationData @ingroup Functions
@brief Send command result notification data.
@param[in] id The command identifier.
@param[in] status The command status text.
@param[in] result The command result text.
*/
void SendNotificationData(DWORD id, const char *status, const char *result);


/** @ingroup Constants
@brief The reset checksum constants.
*/
enum ResetChecksumFlag
{
    /** @brief Reset the "receive" checksum. */
    CHECKSUM_RX     = 0x01,

    /** @brief Reset the "send" checksum. */
    CHECKSUM_TX     = 0x2,

    /** @brief Reset both checksums. */
    CHECKSUM_ALL    = (CHECKSUM_TX|CHECKSUM_RX)
};

/** @ingroup Functions
@brief Send the checksum byte.
*/
void SendChecksum();

/** @ingroup Functions
@brief Receive and check the message checksum.
@details Reads the one byte and compares with calculated checksum.
@return The non-zero if checksum is valid.
*/
BOOL RecvAndValidateChecksum();

/** @ingroup Functions
@brief Reset the checksum.
@param[in] flag which specifies which checksum to reset.
    Should be one of ResetChecksumFlag constant.
*/
void ResetChecksum(int flag);


/** @ingroup Functions
@brief Send an array.
@param[in] it The array structure.
@param[in] SendItem name of a function used to send an item.
*/
#define SendArray(it, SendItem)                                 \
    do {                                                        \
        unsigned int x;                                         \
        SendBytes((unsigned char*)&it.Length, sizeof(WORD));    \
        for (x = 0; x < it.Length; ++x)                         \
            SendItem(&it.Items[x]);                             \
    } while (0)


/** @ingroup Functions
@brief The number of elements in static array.
*/
#define CountOf(arr) (sizeof(arr)/sizeof(arr[0]))

#endif
