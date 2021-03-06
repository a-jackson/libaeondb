#include <stdio.h>
#include "db.h"
#include "common.h"

#ifndef __AEON_TAG_H_
#define __AEON_TAG_H_

#define AEON_TAG_DB_NAME "tags"

typedef struct
{
    aeon_dbhandle *dbhandle;
    uint32_t tag_id;
    uint32_t tag_name_length;
    char *tag_name;
} aeon_tag;

typedef struct
{
    uint32_t tag_count;
} aeon_tag_header;

typedef struct
{
    aeon_dbhandle *dbhandle;
    aeon_tag_header *header;
    aeon_tag **tags;
    char *tag_db_location;
} aeon_tag_db;

int aeon_tag_create(void **_tag, void *_tagdb, char *_name, uint32_t name_length);
int aeon_tags_load(void **_tagdb, void *_dbhandle);
int aeon_tag_get(void **_tag, void *_tagdb, char *tag);
int aeon_tag_save(void *_tag, void *_tagdb);
void aeon_tag_free(void *_tag);
void aeon_tag_database_initialise(char *tag_database_location);
aeon_tag_header aeon_tag_header_load(FILE *file);
void aeon_tag_header_update(aeon_tag_header *header, FILE *file);
void aeon_tag_db_free(void *_tagdb);

#endif
