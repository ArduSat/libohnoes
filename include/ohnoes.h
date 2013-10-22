/**
 * Oh noes!!
 *
 * @author Francisco Soto <francisco@nanosatisfi.com>
 ******************************************************************************/

#ifndef _OHNOES_H_INCLUDED_
#define _OHNOES_H_INCLUDED_

#include <stdbool.h>

typedef bool (*ohnoes_health_check_fn)(void);
typedef void (*ohnoes_cpu_reset)(void);

int ohnoes_init (int frequency_secs, ohnoes_cpu_reset cpu_reset, int priority);
void ohnoes_pause (void);
void ohnoes_unpause (void);
void ohnoes_destroy (void);
int ohnoes_register_health_check (ohnoes_health_check_fn check);

#endif /* _OHNOES_H_INCLUDED_ */
