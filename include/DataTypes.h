#ifndef __DEVICEHIVE_DATATYPES_H__
#define __DEVICEHIVE_DATATYPES_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/** @defgroup Constants DeviceHive Constants
@brief Various constants related to binary protocol.
@details DeviceHive Framework for microcontrollers defines a number of constants to use with its data types and functions.
*/

/** @defgroup Types DeviceHive Types
@brief Various data structures and typedefs related to binary protocol.
@details DeviceHive declares several common types for implementer's convenience.
It is recommended to use them to avoid complications related to different platform enviroments where the code can be built.
*/

/** @defgroup Functions DeviceHive Functions
@brief Various functions related to binary protocol.
@details DeviceHive Framework for microcontrollers exposes its API as a set of global functions to work with its data structures.
*/


/** @ingroup Constants
@brief Parameter types.
@details This constants are used to identify data types.
*/
enum DataType
{
    DT_NULL         = 0,  /**< @brief The NULL value type. @details Has zero length. */
    DT_BYTE         = 1,  /**< @brief 1 byte unsigned integer. */
    DT_WORD         = 2,  /**< @brief 2 byte unsigned integer. */
    DT_DWORD        = 3,  /**< @brief 4 byte unsigned integer. */
    DT_QWORD        = 4,  /**< @brief 8 byte unsigned integer. */
    DT_SIGNED_BYTE  = 5,  /**< @brief 1 byte signed integer. */
    DT_SIGNED_WORD  = 6,  /**< @brief 2 byte signed integer. */
    DT_SIGNED_DWORD = 7,  /**< @brief 4 byte signed integer. */
    DT_SIGNED_QWORD = 8,  /**< @brief 8 byte signed integer. */
    DT_SINGLE       = 9,  /**< @brief 4 byte floating point number. */
    DT_DOUBLE       = 10, /**< @brief 8 byte floating point number. */
    DT_BOOLEAN      = 11, /**< @brief The boolean value type. @details The value is one byte length. */
    DT_GUID         = 12, /**< @brief The 16 byte GUID. */

    /** @brief The variable length string.
    @details The value is prefixed with 2-byte block indicating the length of the string.
    */
    DT_STRING       = 13,

    /** @brief The variable length binary data block.
    @details The value is prefixed with 2-byte block indicating the length of the binary data.
    */
    DT_BINARY       = 14,

    /** @brief The array.
    @details The value is prefixed with 2-byte block indicating the length of the array.
    */
    DT_ARRAY        = 15
};

/** @ingroup Constants
@brief Boolean values.
*/
enum Boolean
{
    FALSE, /**< @brief The `false` value. */
    TRUE   /**< @brief The `true` value. */
};

// TODO: use types from <stdint.h>

/** @ingroup Types
@brief 8-bit unsigned integer type.
*/
typedef unsigned char BYTE;

/** @ingroup Types
@brief 16-bit signed integer type.
*/
typedef unsigned short WORD;

/** @ingroup Types
@brief 32-bit unsigned integer type.
*/
typedef unsigned long DWORD;

/** @ingroup Types
@brief Boolean type.
*/
typedef unsigned char BOOL;


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif
