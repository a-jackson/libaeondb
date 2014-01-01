#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "common.h"

int aeon_initialise(void **_dbhandle, char *_location, int location_length)
{
    char *location;
    aeon_dbhandle *dbhandle;

    location = malloc(location_length);
    AEON_CHECK_ALLOC(location);
    dbhandle = malloc(sizeof(aeon_dbhandle));
    AEON_CHECK_ALLOC(dbhandle)

    memcpy(location, _location, location_length);

    dbhandle->db_location = location;
    dbhandle->db_location_length = location_length;

    *_dbhandle = dbhandle;

    return 1;
}
