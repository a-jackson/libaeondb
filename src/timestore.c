#include <stdio.h>
#include <string.h>
#include "timestore.h"
#include "btree.h"
#include "tag.h"

void *aeon_timestore_initialise(void *_tag)
{
    aeon_tag *tag = (aeon_tag *) _tag;
    aeon_timestore *timestore = malloc(sizeof(aeon_timestore));
    char tag_id[11];
    int tag_length;
    char *timestore_location;
    char *index_location;

    tag_length = sprintf(tag_id, "%d", tag->tag_id);

    timestore_location = malloc(
            sizeof(char) * (tag->dbhandle->db_location_length + tag_length));
    memcpy(timestore_location, tag->dbhandle->db_location,
            tag->dbhandle->db_location_length);
    strcat(timestore_location, tag_id);

    index_location = malloc(
            sizeof(char)
                    * (tag->dbhandle->db_location_length + tag_length + 6));
    memcpy(index_location, tag->dbhandle->db_location,
            tag->dbhandle->db_location_length);
    strcat(index_location, tag_id);
    strcat(index_location, ".index");

    timestore->dbhandle = tag->dbhandle;
    timestore->tag = tag;
    timestore->index = aeon_btree_initialise(100, index_location,
            strlen(index_location) + 1);
    timestore->timestore_location_length =
            (int) (strlen(timestore_location) + 1);
    timestore->timestore_location = timestore_location;
    timestore->last_time_saved = 0;
    timestore->first_time_saved = 0;
    timestore->current_page = NULL;
    timestore->current_page_location = 0;
    timestore->page_count = 0;

    aeon_btree_open(timestore->index, 0);
    aeon_timestore_open(timestore);

    return timestore;
}

void aeon_timestore_open(aeon_timestore *_timestore)
{
    char magic_byte = AEON_TIMESTORE_MAGIC_BYTE;

    _timestore->file = fopen(_timestore->timestore_location, "r+b");
    if (_timestore->file == 0)
    {
        _timestore->file = fopen(_timestore->timestore_location, "wb");
        aeon_timestore_update_header(_timestore);
        fclose(_timestore->file);
    }

    _timestore->file = fopen(_timestore->timestore_location, "r+b");
    fread(&magic_byte, sizeof(char), 1, _timestore->file);
    fread(&_timestore->first_time_saved, sizeof(unsigned long), 1,
            _timestore->file);
    fread(&_timestore->last_time_saved, sizeof(unsigned long), 1,
            _timestore->file);
    fread(&_timestore->page_count, sizeof(unsigned int), 1, _timestore->file);
    fread(&_timestore->current_page_location, sizeof(unsigned long), 1,
            _timestore->file);

    if (_timestore->page_count > 0)
    {
        _timestore->current_page = malloc(sizeof(aeon_timestore_page));
        fseek(_timestore->file, _timestore->current_page_location, SEEK_SET);
        fread(&_timestore->current_page->page_time, sizeof(unsigned long), 1,
                _timestore->file);
        fread(&_timestore->current_page->size, sizeof(unsigned long), 1,
                _timestore->file);
        fread(&_timestore->current_page->value_count, sizeof(unsigned int), 1,
                _timestore->file);
        _timestore->current_page->page_location =
                _timestore->current_page_location;
    }
    else
    {
        _timestore->current_page = NULL;
    }
}

void aeon_timestore_update_header(aeon_timestore *_timestore)
{
    char magic_byte = AEON_TIMESTORE_MAGIC_BYTE;
    fseek(_timestore->file, 0, SEEK_SET);
    fwrite(&magic_byte, sizeof(char), 1, _timestore->file);
    fwrite(&_timestore->first_time_saved, sizeof(unsigned long), 1,
            _timestore->file);
    fwrite(&_timestore->last_time_saved, sizeof(unsigned long), 1,
            _timestore->file);
    fwrite(&_timestore->page_count, sizeof(unsigned int), 1, _timestore->file);
    fwrite(&_timestore->current_page_location, sizeof(unsigned long), 1,
            _timestore->file);
    fflush(_timestore->file);
}

void aeon_timestore_update_page_header(aeon_timestore *_timestore,
        aeon_timestore_page *_page)
{
    fseek(_timestore->file, _page->page_location, SEEK_SET);
    fwrite(&_timestore->current_page->page_time, sizeof(unsigned long), 1,
            _timestore->file);
    fwrite(&_timestore->current_page->size, sizeof(unsigned long), 1,
            _timestore->file);
    fwrite(&_timestore->current_page->value_count, sizeof(unsigned int), 1,
            _timestore->file);
    fflush(_timestore->file);
}

unsigned long aeon_timestore_file_end(aeon_timestore *_timestore)
{
    fpos_t pos;
    fseek(_timestore->file, 0, SEEK_END);
    fgetpos(_timestore->file, &pos);
    return pos.__pos;
}

int aeon_timestore_add(void *_timestore, void *value, unsigned long time)
{
    unsigned long page_time;
    unsigned long current_page_time;
    unsigned long page_location;
    aeon_timestore *timestore = (aeon_timestore *) _timestore;

    if (time < timestore->last_time_saved)
    {
        return -1;
    }

    page_time = time - (time % AEON_TIMESTORE_PAGE_SIZE);
    if (timestore->current_page == NULL )
    {
        // Need to create the first page.
        timestore->current_page = malloc(sizeof(aeon_timestore_page));
        page_location = aeon_timestore_file_end(_timestore);
        timestore->current_page->page_location = page_location;
        timestore->current_page->page_time = page_time;
        timestore->current_page->value_count = 0;
        timestore->current_page->size = (sizeof(unsigned long) * 2)
                + sizeof(unsigned int);
        aeon_timestore_update_page_header(_timestore, timestore->current_page);
        aeon_btree_insert(timestore->index, page_time, page_location);

        timestore->first_time_saved = time;
        timestore->page_count++;
        timestore->current_page_location = page_location;
    }

    current_page_time = timestore->current_page->page_time;

    // Need to make empty pages to fill the time between last page and target.
    while (page_time > current_page_time)
    {
        current_page_time += AEON_TIMESTORE_PAGE_SIZE;
        page_location = aeon_timestore_file_end(_timestore);
        timestore->current_page->page_location = page_location;
        timestore->current_page->page_time = current_page_time;
        timestore->current_page->value_count = 0;
        timestore->current_page->size = (sizeof(unsigned long) * 2)
                + sizeof(unsigned int);
        aeon_timestore_update_page_header(_timestore, timestore->current_page);
        aeon_btree_insert(timestore->index, current_page_time, page_location);

        timestore->page_count++;
        timestore->current_page_location = page_location;
    }

    // Add value to end of file.
    fseek(timestore->file, 0, SEEK_END);
    fwrite(&time, sizeof(unsigned long), 1, timestore->file);
    fwrite(value, AEON_TIMESTORE_DATA_SIZE, 1, timestore->file);
    fflush(timestore->file);

    timestore->current_page->value_count++;
    timestore->current_page->size += sizeof(unsigned long)
            + AEON_TIMESTORE_DATA_SIZE;
    timestore->last_time_saved = time;

    aeon_timestore_update_page_header(timestore, timestore->current_page);
    aeon_timestore_update_header(timestore);

    return 1;
}

void aeon_timestore_free(void *_timestore)
{
    aeon_timestore *timestore = (aeon_timestore *) _timestore;
    if (timestore->file != NULL )
    {
        fclose(timestore->file);
    }

    free(timestore->current_page);
    free(timestore->timestore_location);
    aeon_btree_free(timestore->index);

    free(timestore);
}

void aeon_timestore_page_load(aeon_timestore *_timestore,
        aeon_timestore_page *_page, unsigned long page_location)
{
    _page->page_location = page_location;
    fseek(_timestore->file, _page->page_location, SEEK_SET);
    fread(&_page->page_time, sizeof(unsigned long), 1, _timestore->file);
    fread(&_page->size, sizeof(unsigned long), 1, _timestore->file);
    fread(&_page->value_count, sizeof(unsigned int), 1, _timestore->file);
}

int aeon_timestore_getvalue(aeon_timestore *_timestore,
        aeon_timestore_page *_page, int value_id, unsigned long *time,
        void *value)
{
    int value_position;

    if (value_id >= _page->value_count || value_id < 0)
    {
        return 0;
    }

    value_position = _page->page_location + sizeof(unsigned long)
            + sizeof(unsigned long) + sizeof(unsigned int);
    value_position += value_id
            * (sizeof(unsigned long) + AEON_TIMESTORE_DATA_SIZE);

    fseek(_timestore->file, value_position, SEEK_SET);
    fread(time, sizeof(unsigned long), 1, _timestore->file);
    fread(value, AEON_TIMESTORE_DATA_SIZE, 1, _timestore->file);
    return 1;
}
