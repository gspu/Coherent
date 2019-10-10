/*
 * Space allocation for the dump driver.
 */

#define	_DDI_DKI	1

#include "dump.h"
#include "conf.h"

int		dm_ucnt = DUMP_USERS;
struct dm_str	dm_users [DUMP_USERS];
