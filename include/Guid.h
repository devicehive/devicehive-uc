#ifndef __DEVICEHIVE_GUID_H__
#define __DEVICEHIVE_GUID_H__

#include "DataTypes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/** @ingroup Types
@brief The %GUID structure.
@details Global Unique IDentifier as documented in MSDN.
*/
typedef struct
{
    /** @brief Data bytes 1. */
    DWORD Data1;

    /** @brief Data bytes 2. */
    WORD Data2;

    /** @brief Data bytes 3 */
    WORD Data3;

    /** @brief Data bytes 4. */
    BYTE Data4[8];
} GUID;


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif
