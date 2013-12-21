#ifndef __AEONDB_H_
#define __AEONDB_H_

void *aeon_initialise(char *_location, int location_length);
void *aeon_tag_create(void *_dbhandle, char *_name, int name_length);
void aeon_tag_save(void *_tag);

#endif
