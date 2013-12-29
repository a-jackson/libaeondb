#include <string.h>
#include "query.h"
#include "tag.h"
#include "timestore.h"

void *aeon_query_create(void *_tag, unsigned long start_time,
        unsigned long end_time)
{
    aeon_tag *tag = (aeon_tag *) _tag;
    aeon_query *query = malloc(sizeof(aeon_query));
    unsigned long start_page_time = start_time
            - (start_time % AEON_TIMESTORE_PAGE_SIZE);
    unsigned long start_page_position;
    int start_time_result;

    query->tag = tag;
    query->dbhandle = tag->dbhandle;
    query->start_time = start_time;
    query->end_time = end_time;
    query->current_page = malloc(sizeof(aeon_timestore_page));
    query->current_page->page_data = NULL;
    query->current_page_value = 0;
    query->current_value = malloc(AEON_TIMESTORE_DATA_SIZE);
    query->current_value_time = 0;
    query->timestore = aeon_timestore_initialise(tag);

    if (end_time > query->timestore->last_time_saved)
    {
        query->end_time = query->timestore->last_time_saved;
    }

    if (start_time < query->timestore->first_time_saved)
    {
        query->start_time = query->timestore->first_time_saved;
    }

    start_time_result = aeon_btree_get_value(query->timestore->index,
            start_page_time, &start_page_position);

    if (start_time_result == 0)
    {
        aeon_query_free(query);
        return NULL ;
    }

    aeon_timestore_page_load(query->timestore, query->current_page,
            start_page_position);

    return query;
}

int aeon_query_move_next(void *_query)
{
    aeon_query *query = (aeon_query *) _query;
    unsigned long next_page_position;

    if (aeon_timestore_getvalue(query->timestore, query->current_page,
            query->current_page_value, &query->current_value_time,
            query->current_value) == 0)
    {
        return 0;
    }

    if (query->current_value_time >= query->end_time)
    {
        return 0;
    }

    query->current_page_value++;

    // Use a loop to handle empty pages.
    while (query->current_page_value >= query->current_page->value_count)
    {
        next_page_position = query->current_page->page_location
                + query->current_page->size;
        aeon_timestore_page_load(query->timestore, query->current_page,
                next_page_position);
        query->current_page_value = 0;
    }

    return 1;
}

int aeon_query_current_value(void *_query, unsigned long *time, void *value)
{
    aeon_query *query = (aeon_query *) _query;
    if (query->current_value == NULL )
    {
        return 0;
    }

    *time = query->current_value_time;
    memcpy(value, query->current_value, AEON_TIMESTORE_DATA_SIZE);
    return 1;
}

void aeon_query_free(void *_query)
{
    aeon_query *query = (aeon_query *) _query;

    aeon_timestore_free(query->timestore);
    free(query->current_page);
    free(query->current_value);
    free(query);
}
