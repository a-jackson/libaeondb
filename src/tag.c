#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tag.h"
#include "db.h"

void *aeon_tag_create(void *_tagdb, char *_name, uint32_t name_length)
{
    aeon_tag_db *tagdb = (aeon_tag_db *) _tagdb;
    aeon_tag *tag;
    char *name;

    tag = malloc(sizeof(aeon_tag));
    name = malloc(name_length);

    memcpy(name, _name, name_length);

    tag->dbhandle = tagdb->dbhandle;
    tag->tag_name = name;
    tag->tag_name_length = name_length;

    return tag;
}

void aeon_tag_db_free(void *_tagdb)
{
    aeon_tag_db *tagdb = (aeon_tag_db *) _tagdb;
    int i;
    for (i = 0; i < tagdb->header->tag_count; i++)
    {
        aeon_tag_free(tagdb->tags[i]);
    }

    free(tagdb->tags);
    free(tagdb->header);
    free(tagdb);
}

void aeon_tag_free(void *_tag)
{
    aeon_tag *tag = (aeon_tag *) _tag;

    free(tag->tag_name);
    free(tag);
}

void *aeon_tags_load(void *_dbhandle)
{
    aeon_dbhandle *dbhandle = (aeon_dbhandle *) _dbhandle;
    char *tagdb_location;
    FILE *file;
    aeon_tag_db *tag_db;
    int i;

    tagdb_location =
            malloc(
                    sizeof(char)
                            * (dbhandle->db_location_length
                                    + strlen(AEON_TAG_DB_NAME)));
    strcpy(tagdb_location, dbhandle->db_location);
    strcat(tagdb_location, AEON_TAG_DB_NAME);

    tag_db = malloc(sizeof(aeon_tag_db));
    tag_db->dbhandle = dbhandle;
    tag_db->tag_db_location = tagdb_location;
    tag_db->header = malloc(sizeof(aeon_tag_header));

    file = fopen(tagdb_location, "rb");
    if (file == 0)
    {
        aeon_tag_database_initialise(tagdb_location);
        file = fopen(tagdb_location, "rb");
    }

    fread(&tag_db->header->tag_count, sizeof(aeon_count_t), 1, file);

    if (tag_db->header->tag_count > 0)
    {
        tag_db->tags = malloc(sizeof(aeon_tag *) * tag_db->header->tag_count);
        for (i = 0; i < tag_db->header->tag_count; i++)
        {
            tag_db->tags[i] = malloc(sizeof(aeon_tag));
            tag_db->tags[i]->dbhandle = dbhandle;
            fread(&tag_db->tags[i]->tag_id, sizeof(uint32_t), 1, file);
            fread(&tag_db->tags[i]->tag_name_length, sizeof(uint32_t), 1,
                    file);
            tag_db->tags[i]->tag_name = malloc(
                    sizeof(char) * tag_db->tags[i]->tag_name_length);
            fread(tag_db->tags[i]->tag_name, sizeof(char),
                    tag_db->tags[i]->tag_name_length, file);
        }
    }

    fclose(file);
    return tag_db;
}

void *aeon_tag_get(void *_tagdb, char *tag)
{
    aeon_tag_db *tagdb = (aeon_tag_db *) _tagdb;
    int i;

    for (i = 0; i < tagdb->header->tag_count; i++)
    {
        if (strcmp(tag, tagdb->tags[i]->tag_name) == 0)
        {
            return tagdb->tags[i];
        }
    }

    return NULL ;
}

void aeon_tag_save(void *_tag, void *_tagdb)
{
    FILE *tag_database;
    aeon_tag **existing_tags;
    int i;
    aeon_tag_db *tag_db = (aeon_tag_db *) _tagdb;
    aeon_tag *tag = (aeon_tag *) _tag;

    existing_tags = tag_db->tags;
    tag_db->tags = malloc(sizeof(aeon_tag *) * (tag_db->header->tag_count + 1));
    for (i = 0; i < tag_db->header->tag_count; i++)
    {
        tag_db->tags[i] = existing_tags[i];
    }

    tag_db->tags[i] = tag;
    tag_db->header->tag_count++;
    tag->tag_id = tag_db->header->tag_count;
    free(existing_tags);

    tag_database = fopen(tag_db->tag_db_location, "r+b");
    aeon_tag_header_update(tag_db->header, tag_database);

    fseek(tag_database, 0, SEEK_END);
    fwrite(&tag->tag_id, sizeof(uint32_t), 1, tag_database);
    fwrite(&tag->tag_name_length, sizeof(uint32_t), 1, tag_database);
    fwrite(tag->tag_name, sizeof(char), tag_db->tags[i]->tag_name_length,
            tag_database);
    fflush(tag_database);
    fclose(tag_database);
}

void aeon_tag_database_initialise(char *tag_database_location)
{
    FILE *tag_database = fopen(tag_database_location, "wb");
    aeon_tag_header header;
    header.tag_count = 0;

    aeon_tag_header_update(&header, tag_database);
    fclose(tag_database);
}

aeon_tag_header aeon_tag_header_load(FILE *file)
{
    aeon_tag_header header;
    fseek(file, 0, SEEK_SET);
    fread(&header.tag_count, sizeof(uint32_t), 1, file);
    return header;
}

void aeon_tag_header_update(aeon_tag_header *header, FILE *file)
{
    fseek(file, 0, SEEK_SET);
    fwrite(&header->tag_count, sizeof(uint32_t), 1, file);
    fflush(file);
}
