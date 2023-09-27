/** @file frzlib.h
 *
 * @brief Freeze frame library header file
 */

#ifndef FRZLIB_H_
#define FRZLIB_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    FRZLIB_TRIGGER_NONE,
    FRZLIB_TRIGGER_RISING_EDGE,
    FRZLIB_TRIGGER_FALLING_EDGE,
    FRZLIB_TRIGGER_ANY_EDGE,
    FRZLIB_TRIGGER_HIGH,
    FRZLIB_TRIGGER_LOW,
    FRZLIB_TRIGGER_MAX // Maximum number of entries
} frzlib_trigger_t;

typedef enum
{
    FRZLIB_STATUS_NOT_AVAILABLE,
    FRZLIB_STATUS_READY,
    FRZLIB_STATUS_ACTIVE,
    FRZLIB_STATUS_FROZEN,
    FRZLIB_STATUS_MAX // Maximum number of entries
} frzlib_status_t;

typedef enum
{
    FRZLIB_RETURN_SUCCESS = 0U,
    FRZLIB_RETURN_INVALID_SIZE,
    FRZLIB_RETURN_INVALID_SIGNALS,
    FRZLIB_RETURN_INVALID_POINTER,
    FRZLIB_RETURN_ALREADY_INIT,
    FRZLIB_RETURN_TABLE_FULL,
    FRZLIB_RETURN_INVALID_TRIGGER,
    FRZLIB_RETURN_SEQUENCE_ERROR,
    FRZLIB_RETURN_MAX // Maximum number of entries
} frzlib_return_t;

typedef struct
{
    uint16_t size;                 // Number of frames
    uint8_t num32;                 // Number of 32-bit signals
    uint32_t **pp_addr32;          // Pointer to address table for 32-bit signals
    uint32_t *p_data32;            // Pointer to 2-D data table for 32-bit signals
    bool *p_trigger;               // Pointer to trigger signal
    frzlib_trigger_t trigger_type; // Trigger type
    uint8_t decimation_factor;     // Set to 1 record every frame or N to record
                                   // every Nth frame
} frzlib_config_t;

typedef struct
{
    uint16_t index; // Index of current frame
    frzlib_status_t status;
    frzlib_config_t *p_config;
    uint8_t num32_used;
} frzlib_data_t;

frzlib_return_t frzlib_initialize(const frzlib_config_t *p_config);
frzlib_return_t frzlib_add32(const uint32_t *p_signal);
frzlib_return_t frzlib_activate(void);
frzlib_return_t frzlib_deactivate(void);
void frzlib_update(void);
bool frzlib_is_frozen(void);

#endif /* FRZLIB_H_ */

/*** end of file ***/
