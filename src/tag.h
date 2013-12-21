#include "db.h"

#ifndef __AEON_TAG_H_
#define __AEON_TAG_H_

typedef struct
{
    aeon_dbhandle *dbhandle;
    char *tag_name;
    int tag_name_length;
} aeon_tag;

void *aeon_tag_create(void *_dbhandle, char *_name, int name_length);
void aeon_tag_free(void *_tag);
void aeon_tag_save(void *_tag);

#endif
