#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tag.h"
#include "db.h"

void *aeon_tag_create(void *_dbhandle, char *_name, int name_length)
{
    aeon_tag *tag;
    char *name;

    tag = malloc(sizeof(aeon_tag));
    name = malloc(name_length);

    memcpy(name, _name, name_length);

    tag->dbhandle = (aeon_dbhandle *)_dbhandle;
    tag->tag_name = name;
    tag->tag_name_length = name_length;

    return tag;
}

void aeon_tag_free(void *_tag)
{
    aeon_tag *tag = (aeon_tag *)_tag;

    free(tag->tag_name);
    free(tag);
}

void aeon_tag_save(void *_tag)
{
    FILE *tag_database;
    aeon_tag *tag = (aeon_tag *)_tag;
    char *tag_database_location = malloc(tag->dbhandle->db_location_length + 4);

    strcpy(tag_database_location, tag->dbhandle->db_location);
    strcat(tag_database_location, "tags");

    tag_database = fopen(tag_database_location, "wb");
    fwrite(tag->tag_name, tag->tag_name_length, 1, tag_database);
    fclose(tag_database);
}
