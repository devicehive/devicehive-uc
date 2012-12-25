#include "DeviceHiveEngine.h"

static BYTE g_sendChecksum = 0;
static BYTE g_recvChecksum = 0;

static int (*g_sendBytes)(const unsigned char*, int);
static int (*g_recvBytes)(unsigned char*, int);

#if 1 // send/recv

void InitializeDeviceHive(int (*SendBytesPtr)(const unsigned char*, int), int (*RecvBytesPtr)(unsigned char*, int))
{
    g_sendBytes = SendBytesPtr;
    g_recvBytes = RecvBytesPtr;
}

int SendBytes(const unsigned char * data, int length)
{
    int i, r = g_sendBytes(data, length);

    // update checksum
    for (i = 0; i < r; ++i)
        g_sendChecksum += data[i];

    return r;
}

int RecvBytes(unsigned char * data, int length)
{
    int i, r = g_recvBytes(data, length);

    // update checksum
    for (i = 0; i < r; ++i)
        g_recvChecksum += data[i];

    return r;
}

#endif // // send/recv


#if 1 // String

static int strlen(const char *s)
{
    int n = 0;
    while (s[n] != 0)
        ++n;
    return n;
}

WORD GetStringSize(const char *s)
{
    return sizeof(WORD) + strlen(s);
}

void SendString(const char *s)
{
    WORD len = strlen(s);
    SendBytes((unsigned char*)&len, sizeof(len));
    SendBytes((unsigned char*)s, len);
}

int RecvString(char *s, int maxLength)
{
    WORD len = 0;
    maxLength -= 1; // reserve space for NULL
    if (RecvBytes((unsigned char *)&len, sizeof(len)) == sizeof(len))
    {
        unsigned int n = (len < maxLength)
            ? len : maxLength;

        if (RecvBytes((unsigned char*)s, n) != n)
            return -1;

        // ignore the rest of string
        for (; n < len; --len)
        {
            BYTE tmp;
            RecvBytes(&tmp,
                sizeof(tmp));
        }

        s[n] = 0; // NULL-terminated
        return n;
    }
    else
        return -1;
}

#endif // String


#if 1 // Parameter

WORD GetParameterSize(const Parameter *p)
{
    return sizeof(p->Type)
        + GetStringSize(p->Name);
}

void SendParameter(const Parameter *p)
{
    SendBytes(&p->Type, sizeof(p->Type));
    SendString(p->Name);
}

#endif // Parameter


#if 1 // Equipment

WORD GetEquipmentSize(const Equipment *e)
{
    return GetStringSize(e->Name)
        + GetStringSize(e->Code)
        + GetStringSize(e->TypeName);
}

void SendEquipment(const Equipment *e)
{
    SendString(e->Name);
    SendString(e->Code);
    SendString(e->TypeName);
}

#endif // Equipment


#if 1 // Notification

WORD GetNotificationSize(const Notification *n)
{
    unsigned int i;

    WORD rv = sizeof(n->Intent) + GetStringSize(n->Name) + sizeof(WORD);
    for (i = 0; i < n->Parameters.Length; ++i)
        rv += GetParameterSize(&(n->Parameters.Items[i]));

    return rv;
}

void SendNotification(const Notification *n)
{
    SendBytes((unsigned char*)&n->Intent, sizeof(n->Intent));
    SendString(n->Name);
    SendArray(n->Parameters, SendParameter);
}

#endif // Notification


#if 1 // Command

WORD GetCommandSize(const Command *c)
{
    unsigned int i;

    WORD n = sizeof(c->Intent) + GetStringSize(c->Name) + sizeof(WORD);
    for (i = 0; i < c->Parameters.Length; ++i)
        n += GetParameterSize(&(c->Parameters.Items[i]));

    return n;
}

void SendCommand(const Command *c)
{
    SendBytes((unsigned char*)&c->Intent, sizeof(c->Intent));
    SendString(c->Name);
    SendArray(c->Parameters, SendParameter);
}

#endif // Command


#if 1 // MessageHeader

BOOL ReceiveMessageHeader(MessageHeader * pMsg)
{
    int r;

    // read signature
    for (;;)
    {
        ResetChecksum(CHECKSUM_RX);
        r = RecvBytes(&pMsg->Signature1, sizeof(BYTE));
        if (r != sizeof(BYTE))
            return FALSE;
        if (pMsg->Signature1 == DEVICEHIVE_SIGNATURE1)
        {
            r = RecvBytes(&pMsg->Signature2, sizeof(BYTE));
            if (r != sizeof(BYTE))
                return FALSE;

            if (pMsg->Signature2 != DEVICEHIVE_SIGNATURE2)
            {
                // TODO: check if  Signature2 is equal to SIGNATURE1!!!
            }
            else
                break; // read the rest of header
        }
    }

    RecvBytes(((BYTE*)pMsg) + 2, sizeof(MessageHeader) - 2);
    return CheckHeader(pMsg) == MSG_CHECK_OK;
}

int CheckHeader(const MessageHeader *msgh)
{
    if (msgh->Signature1 != DEVICEHIVE_SIGNATURE1)
        return MSG_CHECK_SIGNATURE;
    if (msgh->Signature2 != DEVICEHIVE_SIGNATURE2)
        return MSG_CHECK_SIGNATURE;
    if (msgh->Version != DEVICEHIVE_VERSION)
        return MSG_CHECK_VERSION;

    return MSG_CHECK_OK;
}

void SkipMessage(unsigned int length)
{
    unsigned int x;
    for (x = 0; x < length+1; ++x) // including checksum byte
    {
        BYTE ch;
        if (RecvBytes(&ch, sizeof(ch)) != sizeof(ch))
            break;
    }
}

#endif // MessageHeader


#if 1 // RegistrationData

WORD GetRegistrationDataSize(const RegData *rd)
{
    WORD rv = sizeof(rd->DeviceID)
        + GetStringSize(rd->DeviceKey)
        + GetStringSize(rd->DeviceName)
        + GetStringSize(rd->DeviceClassName)
        + GetStringSize(rd->DeviceClassVersion)
        + 3*sizeof(WORD); // array sizes

    int x;
    for (x = 0; x < rd->Equipment.Length; ++x)
        rv += GetEquipmentSize(&(rd->Equipment.Items[x]) );
    for (x = 0; x < rd->Notifications.Length; ++x)
        rv += GetNotificationSize(&(rd->Notifications.Items[x]));
    for (x = 0; x < rd->Commands.Length; ++x)
        rv += GetCommandSize(&(rd->Commands.Items[x]));

    return rv;
}

void SendRegistrationData(const RegData *rd)
{
    // Send header
    MessageHeader msgh = MESSAGE_HEADER_INIT(D2G_REGISTER);
    msgh.Length = GetRegistrationDataSize(rd);

    ResetChecksum(CHECKSUM_TX);
    SendBytes((unsigned char*)&msgh, sizeof(msgh));
    SendBytes((unsigned char*)&rd->DeviceID, sizeof(GUID));
    SendString(rd->DeviceKey);
    SendString(rd->DeviceName);
    SendString(rd->DeviceClassName);
    SendString(rd->DeviceClassVersion);
    SendArray(rd->Equipment, SendEquipment);
    SendArray(rd->Notifications, SendNotification);
    SendArray(rd->Commands, SendCommand);
    SendChecksum();
}

WORD GetRegistration2DataSize(const char *rd)
{
    return GetStringSize(rd);
}

void SendRegistration2Data(const char *rd)
{
    // Send header
    MessageHeader msgh = MESSAGE_HEADER_INIT(D2G_REGISTER2);
    msgh.Length = GetRegistration2DataSize(rd);

    ResetChecksum(CHECKSUM_TX);
    SendBytes((unsigned char*)&msgh, sizeof(msgh));
    SendString(rd);
    SendChecksum();
}

#endif // RegistrationData


#if 1 // NotificationData

WORD GetNotificationDataSize(const NotificationData *nd)
{
    return sizeof(nd->CommandID)
        + GetStringSize(nd->Status)
        + GetStringSize(nd->Result);
}

void SendNotificationData(DWORD id, const char *status, const char *result)
{
    MessageHeader msgh = MESSAGE_HEADER_INIT(D2G_NOTIFY);
    NotificationData nd;
    nd.CommandID = id;
    nd.Status = status;
    nd.Result = result;
    msgh.Length = GetNotificationDataSize(&nd);

    ResetChecksum(CHECKSUM_TX);
    SendBytes((unsigned char*)&msgh, sizeof(msgh));
    SendBytes((unsigned char*)&nd.CommandID, sizeof(DWORD));
    SendString(nd.Status);
    SendString(nd.Result);
    SendChecksum();
}

#endif // NotificationData


#if 1 // checksum

void SendChecksum()
{
    BYTE cs = 0xFF - g_sendChecksum;
    SendBytes(&cs, sizeof(cs));
}

BOOL RecvAndValidateChecksum()
{
    BYTE cs = 0;
    RecvBytes(&cs, sizeof(cs));
    return g_recvChecksum == 0xFF;
}

void ResetChecksum(int flag)
{
    if (flag & CHECKSUM_RX)
        g_recvChecksum = 0;

    if (flag & CHECKSUM_TX)
        g_sendChecksum = 0;
}

#endif // // checksum
