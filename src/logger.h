/********************************************************************************************************//**
* @file logger.h
*
* @brief Header file containing the prototypes of the APIs for logging.
*
* Public Functions:
*       - void log_error(char const* const format, ...)
*       - void log_info(char const* const format, ...)
*       - void log_debug(char const* const format, ...)
*       - void log_debug_array(char const* const label, void const* array, uint16_t const len)
*/

#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>

/**
 * @brief Enum for selecting the output level of the logger.
 */
typedef enum
{
    LOG_LEVEL_ERROR,    /**< @brief Error messages are logged */
    LOG_LEVEL_INFO,     /**< @brief Info and error messages are logged */
    LOG_LEVEL_DEBUG     /**< @brief Debug, info and error messages are logged */
}log_level_t;

/** @brief system_log_level variable should be defined and given the desired log level */
extern log_level_t system_log_level;

/***********************************************************************************************************/
/*                                       APIs Supported                                                    */
/***********************************************************************************************************/

/**
 * @brief function which prints information with error log level.
 * @param[in] format is a string with the information to be printed.
 * @return void.
 */
void log_error(char const* const format, ...);

/**
 * @brief function which prints information with info log level.
 * @param[in] format is a string with the information to be printed.
 * @return void.
 */
void log_info(char const* const format, ...);

/**
 * @brief function which prints information with debug log level.
 * @param[in] format is a string with the information to be printed.
 * @return void.
 */
void log_debug(char const* const format, ...);

/**
 * @brief function which prints bytes in hexadecimal format with debug log level.
 * @param[in] label is a label to be printed before the array of bytes.
 * @param[in] array is the array to be printed.
 * @param[in] len is the length of the string in bytes.
 * @return void.
 */
void log_debug_array(char const* const label, void const* array, uint16_t const len);

#endif /* LOGGER_H */