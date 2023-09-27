#include "frzlib.h"
#include <stddef.h>

frzlib_data_t g_frzlib_cb = {.status = FRZLIB_STATUS_NOT_AVAILABLE};

frzlib_return_t frzlib_initialize(const frzlib_config_t *p_config)
{
    // Pointers to data frames
    uint32_t(*p_frame32)[p_config->num32];

    // Check for invalid configuration parameters

    if (g_frzlib_cb.status != FRZLIB_STATUS_NOT_AVAILABLE)
    {
        return FRZLIB_RETURN_ALREADY_INIT;
    }

    if (0U == p_config->size)
    {
        return FRZLIB_RETURN_INVALID_SIZE;
    }

    if (0U == p_config->num32)
    {
        return FRZLIB_RETURN_INVALID_SIGNALS;
    }

    if ((p_config->num32 != 0U) &&
        ((NULL == p_config->pp_addr32) || (NULL == p_config->p_data32)))
    {
        return FRZLIB_RETURN_INVALID_POINTER;
    }

    if (NULL == p_config->p_trigger)
    {
        return FRZLIB_RETURN_INVALID_POINTER;
    }

    if (p_config->trigger_type >= FRZLIB_TRIGGER_MAX)
    {
        return FRZLIB_RETURN_INVALID_TRIGGER;
    }

    // All configuration parameters are valid. Perform initialization.
    p_frame32 = p_config->p_data32;

    for (uint16_t frame = 0U; frame < p_config->size; frame++)
    {
        for (uint8_t idx32 = 0U; idx32 < p_config->num32; idx32++)
        {
            (*p_frame32)[idx32] = 0UL;
        }

        p_frame32++;
    }

    g_frzlib_cb.p_config = p_config;

    // Decimation factor of 0 is treated as 1
    if (0U == g_frzlib_cb.p_config->decimation_factor)
    {
        g_frzlib_cb.p_config->decimation_factor = 1U;
    }

    g_frzlib_cb.index = 0U;
    g_frzlib_cb.num32_used = 0U;
    g_frzlib_cb.status = FRZLIB_STATUS_READY;

    return FRZLIB_RETURN_SUCCESS;
}

frzlib_return_t frzlib_add32(const uint32_t *p_signal)
{
    if (g_frzlib_cb.status != FRZLIB_STATUS_READY)
    {
        return FRZLIB_RETURN_SEQUENCE_ERROR;
    }

    if (g_frzlib_cb.num32_used >= g_frzlib_cb.p_config->num32)
    {
        return FRZLIB_RETURN_TABLE_FULL;
    }

    (g_frzlib_cb.p_config->pp_addr32)[g_frzlib_cb.num32_used] = p_signal;
    g_frzlib_cb.num32_used++;

    return FRZLIB_RETURN_SUCCESS;
}

frzlib_return_t frzlib_activate(void)
{
    if (g_frzlib_cb.status != FRZLIB_STATUS_READY)
    {
        return FRZLIB_RETURN_SEQUENCE_ERROR;
    }

    g_frzlib_cb.status = FRZLIB_STATUS_ACTIVE;

    return FRZLIB_RETURN_SUCCESS;
}

frzlib_return_t frzlib_deactivate(void)
{
    if (g_frzlib_cb.status != FRZLIB_STATUS_ACTIVE)
    {
        return FRZLIB_RETURN_SEQUENCE_ERROR;
    }

    g_frzlib_cb.status = FRZLIB_STATUS_READY;

    return FRZLIB_RETURN_SUCCESS;
}

void frzlib_update(void)
{
    static bool prev_trigger = false;
    static uint8_t counter = 0U;
    bool trigger;
    bool freeze_condition;

    // Pointers to data frames
    uint32_t(*p32)[g_frzlib_cb.p_config->num32];

    // Do not update if not active
    if (g_frzlib_cb.status != FRZLIB_STATUS_ACTIVE)
    {
        return;
    }

    // Do not update for frames to be skipped
    if (counter < (g_frzlib_cb.p_config->decimation_factor - 1U))
    {
        counter++;
        return;
    }

    p32 = (uint32_t(*)[g_frzlib_cb.p_config->num32])
              g_frzlib_cb.p_config->p_data32 +
          g_frzlib_cb.index;

    // Record the frame
    for (uint8_t idx32 = 0U; idx32 < g_frzlib_cb.num32_used; idx32++)
    {
        (*p32)[idx32] = *((g_frzlib_cb.p_config->pp_addr32)[idx32]);
    }

    trigger = *(g_frzlib_cb.p_config->p_trigger);

    switch (g_frzlib_cb.p_config->trigger_type)
    {
    case FRZLIB_TRIGGER_RISING_EDGE:
        freeze_condition = trigger && !prev_trigger;
        break;

    case FRZLIB_TRIGGER_FALLING_EDGE:
        freeze_condition = !trigger && prev_trigger;
        break;

    case FRZLIB_TRIGGER_ANY_EDGE:
        freeze_condition =
            (trigger && !prev_trigger) || (!trigger && prev_trigger);
        break;

    case FRZLIB_TRIGGER_HIGH:
        freeze_condition = trigger;
        break;

    case FRZLIB_TRIGGER_LOW:
        freeze_condition = !trigger;
        break;

    case FRZLIB_TRIGGER_NONE:
    default:
        freeze_condition = false;
        break;
    }

    if (freeze_condition)
    {
        g_frzlib_cb.status = FRZLIB_STATUS_FROZEN;
    }
    else
    {
        g_frzlib_cb.index =
            (g_frzlib_cb.index + 1U) % g_frzlib_cb.p_config->size;
        prev_trigger = trigger;
        counter = 0U;
    }
}

bool frzlib_is_frozen(void)
{
    return (FRZLIB_STATUS_FROZEN == g_frzlib_cb.status);
}

/*** end of file ***/
