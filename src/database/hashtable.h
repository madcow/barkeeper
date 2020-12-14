#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef struct table_s table_t;

struct table_s
{
	long     hashsize;
	entry_t  **data;
};

int   Table_Init(table_t *tab);
long  Table_Insert(table_t *tab, const entry_t *ent);
int   Table_Lookup(const table_t *tab, const char *key, entry_t *ent);
int   Table_Delete(table_t *tab, const char *key);
void  Table_Serialize(const table_t *tab, int fd);
void  Table_Free(table_t *tab);

#endif // HASHTABLE_H
