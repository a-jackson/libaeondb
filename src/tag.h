#include "db.h"

#ifndef __AEON_TAG_H_
#define __AEON_TAG_H_

#define AEON_TAG_MAGIC_BYTE 0x0B
#define AEON_TAG_DB_NAME "tags"

typedef struct
{
    aeon_dbhandle *dbhandle;
    unsigned int tag_id;
    unsigned int tag_name_length;
    char *tag_name;
} aeon_tag;

typedef struct
{
    char magic_byte;
    unsigned int tag_count;
} aeon_tag_header;

typedef struct
{
    aeon_dbhandle *dbhandle;
    aeon_tag_header *header;
    aeon_tag **tags;
    char *tag_db_location;
} aeon_tag_db;

void *aeon_tag_create(void *_tagdb, char *_name, int name_length);
void *aeon_tags_load(void *_dbhandle);
void aeon_tag_save(void *_tag, void *_tagdb);
void aeon_tag_free(void *_tag);
void aeon_tag_database_initialise(char *tag_database_location);
aeon_tag_header aeon_tag_header_load(FILE *file);
void aeon_tag_header_update(aeon_tag_header *header, FILE *file);


#endif
