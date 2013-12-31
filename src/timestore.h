#include "tag.h"
#include "db.h"
#include "btree.h"
#include "common.h"

#ifndef __AEON_TIMESTORE_H_
#define __AEON_TIMESTORE_H_

#define AEON_TIMESTORE_PAGE_SIZE 60
#define AEON_TIMESTORE_DATA_SIZE sizeof(double)

typedef struct
{
    aeon_time_t page_time;
    unsigned long size;
    aeon_pos_t page_location;
    unsigned int value_count;
    void *page_data;
} aeon_timestore_page;

typedef struct
{
    aeon_dbhandle *dbhandle;
    aeon_tag *tag;
    aeon_btree *index;
    char *timestore_location;
    int timestore_location_length;
    aeon_time_t last_time_saved;
    aeon_time_t first_time_saved;
    FILE *file;
    aeon_timestore_page *current_page;
    unsigned int page_count;
    aeon_pos_t current_page_location;
} aeon_timestore;

void *aeon_timestore_initialise(void *_tag);
void aeon_timestore_open(aeon_timestore *_timestore);
void aeon_timestore_update_header(aeon_timestore *_timestore);
void aeon_timestore_update_page_header(aeon_timestore *_timestore,
        aeon_timestore_page *_page);
unsigned long aeon_timestore_file_end(aeon_timestore *_timestore);
int aeon_timestore_add(void *_timestore, void *value, aeon_time_t time);
void aeon_timestore_free(void *_timestore);
void aeon_timestore_page_load(aeon_timestore *_timestore,
        aeon_timestore_page *_page, aeon_pos_t page_location);
int aeon_timestore_getvalue(aeon_timestore *_timestore,
        aeon_timestore_page *_page, int value_id, aeon_time_t *time,
        void *value);

#endif
