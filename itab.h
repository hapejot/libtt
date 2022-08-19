#ifndef ITAB_H
#define ITAB_H

typedef struct itab *t_itab;
typedef struct itab_iter* t_itab_iter;

unsigned itab_lines(struct itab *itab);
struct itab *itab_new(void);
int itab_entry_cmp(const void *aptr, const void *bptr);
void itab_insert(struct itab *itab, const char *key, void *value);
void *itab_read(struct itab *itab, const char *key);
void itab_dump(struct itab *itab);
struct itab_iter *itab_foreach(struct itab *tab);
struct itab_iter *itab_next(struct itab_iter *iter);
void *itab_value(struct itab_iter *iter);
const char *itab_key(struct itab_iter *iter);
/**
 * @brief clears the reference and frees all memory related to the table.
 * 
 * @param itab  refers to the variable pointing at the itab.
 */
t_itab itab_free(t_itab itab);
#endif // ITAB_H
