#ifndef __DEVICEHIVE_EQUIPMENT_H__
#define __DEVICEHIVE_EQUIPMENT_H__

#include "DataTypes.h"

/** @ingroup Types
@brief The equipment metadata.
@details This structure defines the equipment metadata,
according to the binary protocol specification.
*/
typedef struct
{
    /** @brief The equipment name. */
    const char *Name;

    /** @brief The equipment code.
    @details Each piece of equipment should have a unique code in scope of a single device.
    Equipment commands use equipment code to refer a particular equipment.
    */
    const char *Code;

    /** @brief The equipment type.
    @details Type name can be used to logically group different types of equipment,
    e.g. sensors, switches, etc.
    */
    const char *TypeName;
} Equipment;


/** @related Equipment
@brief Initialize equipment metadata.
@param[in] name The equipment name.
@param[in] code The equipment code.
@param[in] type The equipment type.
*/
#define EQUIPMENT_INIT(name, code, type) \
    { name, code, type }


/** @ingroup Types
@brief Array of equipment.
@details Array of equipment structures with length specifier.
*/
typedef struct
{
    /** @brief The number of elements. */
    unsigned int Length;

    /** @brief The equipment items. */
    Equipment *Items;
} EquipmentArray;

#endif
