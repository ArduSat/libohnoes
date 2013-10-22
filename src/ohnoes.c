/**
 * Oh noes!!
 * @author Francisco Soto <francisco@nanosatisfi.com>
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include <ohnoes.h>

#define OHNOES_MAX_HEALTH_CHECKS 15
#define OHNOES_TASK_NAME         "OHNOES!"
#define OHNOES_TASK_SIZE         512


/**
 * The ohnoes FreeRTOS task handle.
 */
static xTaskHandle _task_handle;

/**
 * How often are going to check for the health of the system?
 */
static int _frequency_seconds = 0;

/**
 * Function pointer to a cpu reset function.
 */
static ohnoes_cpu_reset _cpu_reset;

/**
 * Function pointer to a cpu reset function.
 */
static bool _paused = false;

/**
 * This table drives all the health check functions logic. If any of these
 * functions returns false it means we are on an unrecoverable state and we
 * reset the system.
 */
static ohnoes_health_check_fn _health_checks[OHNOES_MAX_HEALTH_CHECKS] = { NULL };

/**
 * This function resets the MCU/CPU when one of the healthy checks
 * functions returns false and we have an unrecoverable error.
 */
static void blue_screen_of_death (void) {
    printf("A fatal exception has 0E has ocurred at 0000:DEAD."
           "The current  application will be terminated.");
    _cpu_reset();
}

/**
 * The FreeRTOs OHNOES task.
 */
static void task_ohnoes (void *data) {
    while (true) {
        if (!_paused) {
            bool we_are_safe = true;

            // Check all the safeness level functions and if any of them signals
            // a problem, reset the system.
            for (unsigned int i = 0;
                 i < OHNOES_MAX_HEALTH_CHECKS && _health_checks[i] && we_are_safe;
                 i++) {
                we_are_safe = we_are_safe && _health_checks[i]();
            }

            if (!we_are_safe) {
                blue_screen_of_death();
            }
        }

        vTaskDelay(configTICK_RATE_HZ * _frequency_seconds);
    }
}

/**
 * Initializes the ohnoes health check library.
 * @param frequency_secs the seconds to wait between checks.
 * @param cpu_request a pointer to the function to call to reset the cpu/mcu.
 * @param priority the priority for the ohnoes FreeRTOS task.
 * @return 0 if initialized correctly, -1 otherwise.
 */
int ohnoes_init (int frequency_secs, ohnoes_cpu_reset cpu_reset, int priority) {

    if (cpu_reset == NULL || frequency_secs <= 0) {
        return -1;
    }

    _cpu_reset = cpu_reset;
    _frequency_seconds = frequency_secs;
    _paused = false;

    xTaskCreate(task_ohnoes, (signed char *) OHNOES_TASK_NAME, OHNOES_TASK_SIZE,
                NULL, priority, &_task_handle);

    return 0;
}

/**
 * Stops the checks and resets the library data structures.
 */
void ohnoes_destroy (void) {
    vTaskDelete(_task_handle);

    _cpu_reset = NULL;
    _frequency_seconds = 0;
    _paused = false;

    for (int i = 0; i < OHNOES_MAX_HEALTH_CHECKS; i++) {
        _health_checks[i] = NULL;
    }
}

/**
 * Pauses the health checks.
 */
void ohnoes_pause (void) {
    _paused = true;
}

/**
 * Unpauses the health checks.
 */
void ohnoes_unpause (void) {
    _paused = false;
}

/**
 * Registers a health checking function.
 * @param check the function pointer. If the function returns false ohnoes
 *              will reset the system.
 * @return 0 if registered correctly, -1 otherwise.
 */
int ohnoes_register_health_check (ohnoes_health_check_fn check) {
    for (int i = 0; i < OHNOES_MAX_HEALTH_CHECKS; i++) {
        if (_health_checks[i] == NULL) {
            _health_checks[i] = check;
            return 0;
        }
    }
    return -1;
}
