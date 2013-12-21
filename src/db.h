#ifndef __AEONDB_H_
#define __AEONDB_H_

typedef struct
{
    char *db_location;
    int db_location_length;
} aeon_dbhandle;

void *aeon_initialise(char *_location, int location_length);

#endif
