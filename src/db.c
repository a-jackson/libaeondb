#include <stdlib.h>
#include <string.h>
#include "db.h"

void *aeon_initialise(char *_location, int location_length)
{
    char *location;
    aeon_dbhandle *dbhandle;

    location = malloc(location_length);
    dbhandle = malloc(sizeof(aeon_dbhandle));

    memcpy(location, _location, location_length);
    dbhandle->db_location = location;
    dbhandle->db_location_length = location_length;

    return dbhandle;
}
