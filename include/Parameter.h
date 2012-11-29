#ifndef __DEVICEHIVE_PARAMETER_H__
#define __DEVICEHIVE_PARAMETER_H__

#include "DataTypes.h"

/** @ingroup Types
@brief The parameter metadata.
@details This structure defines a command or notification parameter metadata,
according to binary protocol specification. The content of the parameter
depends on its type and follows directly after the parameter definition
during the data transmission.
*/
typedef struct
{
    /** @brief The parameter type.
    @see DataType for allowed parameter types.
    */
    BYTE Type;

    /** @brief The parameter name.
    @details Maximum length is 256 bytes.
    Is case-sensitive.
    */
    const char *Name;
} Parameter;


/** @related Parameter
@brief Initialize parameter metadata.
@param[in] type The parameter type.
@param[in] name The parameter name.
*/
#define PARAMETER_INIT(type, name) \
    { type, name }


/** @ingroup Types
@brief Array of parameters.
@details This structure defines an array of parameters
according to binary protocol specification.
During the data transmission the content
of Items buffer follows directly the Length.
*/
typedef struct
{
    /** @brief The number of elements. */
    unsigned int Length;

    /** @brief The parameter elements. */
    Parameter *Items;
} ParameterArray;

#endif
