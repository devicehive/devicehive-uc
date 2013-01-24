#ifndef __DEVICEHIVE_COMMAND_H__
#define __DEVICEHIVE_COMMAND_H__

#include "DataTypes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/** @ingroup Types
@brief The command metadata.
@details This structure describes command metadata,
according to the binary protocol specification.
*/
typedef struct
{
    /** @brief The command intent.
    @details Valid values are G2D_REQREG, D2G_REGISTER, D2G_NOTIFY and custom intents (>256)
    */
    WORD        Intent;

    /** @brief The command name. */
    const char *Name;

    /** @brief Array of command parameters.
    @details When transmitting data, command parameter definitions follow immediately the Command structure.
    */
    ParameterArray Parameters;
} Command;


/** @relates Command
@brief Initialize command metadata.
@param[in] intent The command intent.
@param[in] name The command name.
@param[in] params The command parameters.
*/
#define COMMAND_INIT(intent, name, params) \
    { intent, name, {CountOf(params), params}}


/** @ingroup Types
@brief Array of commands.
@details Array of Command structures with leading length specifier.
*/
typedef struct
{
    /** @brief The number of elements. */
    unsigned int Length;

    /** @brief The command items. */
    Command *Items;
} CommandArray;


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif
