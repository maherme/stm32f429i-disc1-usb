/********************************************************************************************************//**
* @file logger.c
*
* @brief File containing the APIs for logging.
*
* Public Functions:
*       - void log_error(char const* const format, ...)
*       - void log_info(char const* const format, ...)
*       - void log_debug(char const* const format, ...)
*       - void log_debug_array(char const* const label, void const* array, unit16_t const len)
*
* @note
*       For further information about functions refer to the corresponding header file.
**/

#include "logger.h"
#include "stm32f4xx.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

/***********************************************************************************************************/
/*                                       Static Function Prototypes                                        */
/***********************************************************************************************************/

static char const* const _get_log_level_string(log_level_t const log_level);
static void _log(log_level_t const log_level, char const* const format, va_list args);

/***********************************************************************************************************/
/*                                       Public API Definitions                                            */
/***********************************************************************************************************/

void log_error(char const* const format, ...)
{
    va_list args;
    va_start(args, format);
    _log(LOG_LEVEL_ERROR, format, args);
    va_end(args);
}

void log_info(char const* const format, ...)
{
    va_list args;
    va_start(args, format);
    _log(LOG_LEVEL_INFO, format, args);
    va_end(args);
}

void log_debug(char const* const format, ...)
{
    va_list args;
    va_start(args, format);
    _log(LOG_LEVEL_DEBUG, format, args);
    va_end(args);
}

void log_debug_array(char const* const label, void const* array, uint16_t const len)
{
    if(LOG_LEVEL_DEBUG > system_log_level)
        return;
    
    printf("[%s] %s[%d]: {", _get_log_level_string(LOG_LEVEL_DEBUG), label, len);
    for(uint16_t i = 0; i < len; i++){
        uint8_t val = *((uint8_t*)(array + i));
        printf("0x%02X", val);
        /* Add "," after all elements except the last one */
        if(i < (len - 1)){
            printf(", ");
        }
    }
    printf("}\n");
}

/***********************************************************************************************************/
/*                                       Weak Functions                                                    */
/***********************************************************************************************************/

int _write(int file, char* ptr, int len)
{
    for(int i = 0; i < len; i++){
        ITM_SendChar((*ptr++));
    }

    return len;
}

/***********************************************************************************************************/
/*                                       Static Function Definitions                                       */
/***********************************************************************************************************/

static char const* const _get_log_level_string(log_level_t const log_level)
{
    switch (log_level)
    {
    case LOG_LEVEL_ERROR:
        return "ERROR";
        break;
    case LOG_LEVEL_INFO:
        return "INFO";
        break;
    case LOG_LEVEL_DEBUG:
        return "DEBUG";
        break;
    default:
        return "VOID";
        break;
    }
}

static void _log(log_level_t const log_level, char const* const format, va_list args)
{
    if(log_level > system_log_level)
        return;
    
    printf("[%s] ", _get_log_level_string(log_level));
    vfprintf(stdout, format, args);
    printf("\n");
}