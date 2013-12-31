#include "tag.h"
#include "db.h"
#include "timestore.h"
#include "btree.h"
#include "common.h"

#ifndef __AEON_QUERY_H_
#define __AEON_QUERY_H_

typedef struct
{
    aeon_tag *tag;
    aeon_dbhandle *dbhandle;
    aeon_timestore *timestore;
    int current_page_value;
    aeon_timestore_page *current_page;
    aeon_time_t start_time;
    aeon_time_t end_time;
    aeon_time_t current_value_time;
    void *current_value;
} aeon_query;

void *aeon_query_create(void *_tag, aeon_time_t start_time, aeon_time_t end_time);
int aeon_query_move_next(void *_query);
int aeon_query_current_value(void *_query, aeon_time_t *time, void *value);
void aeon_query_free(void *_query);

#endif
