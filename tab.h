/* Defines a very simple (naive) Table structure for storing and looking up
   key-value pairs with fixed, unique and consecutive integer indexes. Keys are
   strings and values are ints.

   0, "The key", 123
   1, "Second key", 4

   Any key can be found given its numeric index in the table and vice-versa,
   and any value can be found given either its index or its key.

   "What were you thinking?" note: I know using almost any other datastructure
   would have been a bit better, expecially a nice linked list or
   something.. But I've done those before and this was mainly for learning
   purposes so I thought I'd roll something a bit different.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  char *k;
  int v;
} Entry;

typedef struct {
  int next;
  int limit;
  Entry **entries;
} Table;

Table *mktab(void);
int insertkeyval(Table *tab, char *k, int v);
char *keyfromindex(Table *tab, int i);
int indexfromkey(Table *tab, char *k);
int valfromindex(Table *tab, int i);
int valfromkey(Table *tab, char *k);
void printtab(Table *tab);
