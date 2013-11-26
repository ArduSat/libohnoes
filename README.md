libohnoes
========

This library is basically a panic mode button for embedded systems. In embedded application system
resets are usually very fast and not very disruptive as most applications are simply tasks waiting for a
given event to happen. So when something goes very wrong and it is very hard to recover from it (like
running out of memory) resetting the microcontroller and re-initializing the application is a good way
to get back to a known good state.

We built this library for our satellite firmware.

*libohnoes* is a (for now) FreeRTOS dependent library that runs a monitoring task periodically checking
the health of your application and when something goes very wrong it resets the microcontroller so it
returns to a known state.

The health-checking functions have to be registered using the libohnoes interface, they should return a
boolean true (C99), if they return false, libohnoes will reset the system.

## Requirements

* FreeRTOS

# Types

`typedef bool (*ohnoes_health_check_fn)(void);`

Health checking callback. Takes no parameters and returns a boolean.

`typedef void (*ohnoes_cpu_reset)(void);`

CPU reset function, this is heavily microcontroller/architecture dependent so you should provide it.
Takes no parameters and returns nothing.

# Functions

`int ohnoes_init (int frequency_secs, ohnoes_cpu_reset cpu_reset, int priority)`

Initializes the ohnoes health check library. *frequency_secs* is the number of seconds to wait between health
checks, *cpu_reset* is the function that will reset the cpu and *priority* is the priority for the FreeRTOS task.

`void ohnoes_destroy (void)`

Destroys the ohnoes task and removes all the registered health checking functions from internal data structures.

`void ohnoes_pause (void)`

Stops monitoring. The task still exists and is scheduled to run, just does nothing and yields to other tasks.

`void ohnoes_unpause (void)`

Resumes monitoring after a call to pause.

/**
 * Registers a health checking function.
 * @param check the function pointer. If the function returns false ohnoes
 *              will reset the system.
 * @return 0 if registered correctly, -1 otherwise.
 */
`int ohnoes_register_health_check (ohnoes_health_check_fn check)`

Registers a health check function. The function must take no parameters and return a boolean. `true` means everything
is fine, `false` means something is very wrong and it will make libohnoes reset the system. Even if there are several
registered functions, if one of them returns false, it will reset the system so make sure the functions do check for
really unrecoverable situations.
