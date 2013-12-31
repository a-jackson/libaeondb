#ifndef __AEONDB_H_
#define __AEONDB_H_

#include <stdint.h>

typedef int64_t aeon_time_t;
typedef uint64_t aeon_pos_t;
typedef uint64_t aeon_size_t;
typedef uint32_t aeon_count_t;


void *aeon_initialise(char *_location, int location_length);
void *aeon_query_create(void *_tag, aeon_time_t start_time, aeon_time_t end_time);
int aeon_query_move_next(void *_query);
int aeon_query_current_value(void *_query, aeon_time_t *time, void *value);
void aeon_query_free(void *_query);
void *aeon_tag_create(void *_tagdb, char *_name, uint32_t name_length);
void *aeon_tags_load(void *_dbhandle);
void *aeon_tag_get(void *_tagdb, char *tag);
void aeon_tag_save(void *_tag, void *_tagdb);
void aeon_tag_db_free(void *_tagdb);
void *aeon_timestore_initialise(void *_tag);
int aeon_timestore_add(void *_timestore, void *value, aeon_time_t time);
void aeon_timestore_free(void *_timestore);

#endif
