#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BLOCKSIZE 16

typedef struct {
  char *k;
  int v;
} Entry;

typedef struct {
  Entry **entries;
  Entry **next;
  Entry **limit;
  int count;
} Table;

Table *mktab(void) {
  /* Create a new Table and returns a pointer to it. NULL pointer returned if
     memory allocation fails.
  */
  Table *t = (Table *) malloc(sizeof(Table));
  if ((t->entries = (Entry **) malloc(BLOCKSIZE * (sizeof(Entry *))))) {
    t->next = t->entries;
    t->limit = t->entries+BLOCKSIZE;
    t->count = 0;
    printf("Table initiliased with...\nt->entries=%p\nt->next=%p\nt->limit=%p\n", t->entries, t->next, t->limit);
    return t;
  } else {
    return NULL;
  }
}

int indexfromkey(Table *tab, char *k) {
  /* Find the index of an Entry from its key. -1 is returned if the key is not
     found.
  */
  int i;
  Entry **e;

  for (i = 0, e = tab->entries; e != tab->next; i++, e++) {
    if (strcmp(k, (*e)->k) == 0)
      return i;
  }
  return -1; // Not found.
}

char *keyfromindex(Table *tab, int i) {
  /* Find the key of an Entry from its index in the table. NULL pointer is
     retured if the index is not in the table.
  */
  Entry **e;

  if ((e = (tab->entries + i)) < tab->next)
    return (*e)->k;
  else
    return NULL;
}

int valfromkey(Table *tab, char *k) {
  /* Find the value of an Entry from its key. -1 is returned if the key is not
     found.
  */
  Entry **e;
  
  for (e = tab->entries; e < tab->next; e++) {
    if (strcmp(k, (*e)->k) == 0)
      return (*e)->v;
  }

  return -1; // Not found.
}

int valfromindex(Table *tab, int i) {
  /* Find the value of an Entry from its index in the table. -1 is returned if
     the index is not in the table.
  */
  Entry **e;

  if ((e = (tab->entries + i)) < tab->next)
    return (*e)->v;
  else
    return -1;
}

int insertkeyval(Table *tab, char *k, int v) {
  /* Insert a key-value pair into the into the variable table, or update the
     value of an existing Entry with the supplied key if such an Entry
     exists. Returns the index of the inserted or updated Entry, or -1 if more
     memory has to be allocated and this allocation fails.
  */
  int i;

  if ((i = indexfromkey(tab, k)) >= 0) {
    // Key already in the table, so update it's value.
    (*(tab->entries + i))->v = v;
    return i;
  } else {
    // New entry to be inserted:
    if(tab->next == tab->limit) {
      // Try to allocate more memory..
      int newsize = (tab->count + BLOCKSIZE) * sizeof(Entry *);
      if ((tab->entries = (Entry **) realloc(tab->entries, newsize)) == NULL) {
	return -1;
      } else {
	printf("Allocated more memory to the table.\n");
	// Need to re-assign pointers as tab->entries base address may have
	// changed (if the block was moved in reallocation).
	tab->limit = tab->entries + BLOCKSIZE;
	tab->next = tab->entries + tab->count;
      }
    }

    // Construct new entry struct:
    Entry *new = (Entry *) malloc(sizeof(Entry));
    new->k = (char *) malloc(strlen(k));
    new->v = v;
    strcpy(new->k, k);

    // Insert it, return and increment next counter.
    *(tab->next++) = new;
    return tab->count++;
  }
}

void printtab(Table *tab) {
  /* Print the contents of the Table.
  */
  int i = 0;
  Entry **e = tab->entries;
  
  for (i = 0, e = tab->entries; i < tab->count; i++, e++) {
    printf("%4d: \"%s\", %d\n", i++, (*e)->k, (*e)->v);
  }
}

void test(void) {
  Table *mytab = mktab();
  char key[16];
  int val;
  int i;

  for (i = 2*BLOCKSIZE, val = 10123; i > 1; i--, val++) {
    sprintf(key, "l%3d", i);
    printf("Inserting (%s, %d)\n", key, val);
    insertkeyval(mytab, key, val);
  }

  for (i = 1; i < 5*BLOCKSIZE; i*=2) {
    sprintf(key, "l%3d", i);
    printf("Looking up \"%s\"... Index is %d, value is %d\n", key, indexfromkey(mytab, key), valfromkey(mytab, key));
  }

  printtab(mytab);
}

// int main(void) { test(); return 0; }
