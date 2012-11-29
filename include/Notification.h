#ifndef __DEVICEHIVE_NOTIFICATION_H__
#define __DEVICEHIVE_NOTIFICATION_H__

#include "DataTypes.h"

/** @ingroup Types
@brief The notification metadata.
@details This structure representrs a notification metadata description
according to binary protocol specification.
*/
typedef struct
{
    /** @brief The notification intent. */
    WORD        Intent;

    /** @brief The notification name. */
    const char *Name;

    /** @brief Array of notification parameters. */
    ParameterArray Parameters;
} Notification;


/** @relates Notification
@brief Initialize notification metadata.
@param[in] intent The notification intent.
@param[in] name The notification name.
@param[in] params The notification parameters.
*/
#define NOTIFICATION_INIT(intent, name, params) \
    { intent, name, {CountOf(params), params}}


/** @ingroup Types
@brief Array of notifications.
@details This structure represents an array of notifications with leading length specifier.
During the transmission, notification elements follow directly after the length specifier.
*/
typedef struct
{
    /** @brief The number of elements. */
    unsigned int Length;

    /** @brief The notification items. */
    Notification *Items;
} NotificationArray;

#endif
