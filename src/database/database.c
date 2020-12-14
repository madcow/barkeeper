#include "common.h"
#include "database.h"
#include "filesystem.h"
#include "hashtable.h"

int DAT_Init(void)
{
	Info("Initializing barcode database...");
	return 0;
}

int DAT_Query(const char *key, entry_t *out)
{
	UNUSED(key);
	UNUSED(out);
	return -1;
}

int DAT_Insert(const entry_t *ent)
{
	UNUSED(ent);
	return -1;
}

int DAT_Delete(int index)
{
	UNUSED(index);
	return -1;
}