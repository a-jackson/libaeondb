#include "tag.h"
#include "db.h"
#include "btree.h"

#ifndef __AEON_TIMESTORE_H_
#define __AEON_TIMESTORE_H_

#define AEON_TIMESTORE_MAGIC_BYTE 0x0C
#define AEON_TIMESTORE_PAGE_SIZE 60
#define AEON_TIMESTORE_DATA_SIZE sizeof(double)

typedef struct
{
    unsigned long page_time;
    unsigned long size;
    unsigned long page_location;
    unsigned int value_count;
} aeon_timestore_page;

typedef struct
{
    aeon_dbhandle *dbhandle;
    aeon_tag *tag;
    aeon_btree *index;
    char *timestore_location;
    int timestore_location_length;
    unsigned long last_time_saved;
    unsigned long first_time_saved;
    FILE *file;
    aeon_timestore_page *current_page;
    unsigned int page_count;
    unsigned long current_page_location;
} aeon_timestore;

void *aeon_timestore_initialise(void *_tag);
void aeon_timestore_open(aeon_timestore *_timestore);
void aeon_timestore_update_header(aeon_timestore *_timestore);
void aeon_timestore_update_page_header(aeon_timestore *_timestore,
        aeon_timestore_page *_page);
unsigned long aeon_timestore_file_end(aeon_timestore *_timestore);
int aeon_timestore_add(void *_timestore, void *value, unsigned long time);
void aeon_timestore_free(void *_timestore);
void aeon_timestore_page_load(aeon_timestore *_timestore,
        aeon_timestore_page *_page, unsigned long page_location);
int aeon_timestore_getvalue(aeon_timestore *_timestore,
        aeon_timestore_page *_page, int value_id, unsigned long *time,
        void *value);

#endif
