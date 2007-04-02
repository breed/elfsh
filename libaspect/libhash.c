/*
** libhash.c for libelfsh
** 
** Contain ELFsh internal hashtables library calls
**
** Started on  Fri Jan 24 20:26:18 2003 mayhem
** Last update Mon Aug 15 11:47:50 2005 mayhem
**
** $Id: libhash.c,v 1.26 2007-04-02 18:00:02 may Exp $
**
*/
#include "libaspect.h"


/* Hash tables of hash tables */
hash_t	*hash_hash = NULL;

/* Initialize the hash table */
int	hash_init(hash_t *h, char *name, int size, u_int type)
{
  NOPROFILER_IN();

  if (name && strstr(name, "commands"))
    fprintf(stderr, "Initializing hash %s \n", name);
  
  if (!hash_hash)
    {
      hash_hash = (hash_t *) calloc(sizeof(hash_t), 1);
      hash_init(hash_hash, "hashes", 51, ASPECT_TYPE_UNKNOW);
    }
  if (type >= aspect_type_nbr)
    {
      fprintf(stderr, "Unable to initialize hash table %s \n", name);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to initialize hash table", -1);
    }

  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 h->ent, size * sizeof(hashent_t), -1);
  h->size   = size;
  h->type   = type;
  h->elmnbr = 0;
  hash_add(hash_hash, name, h);
  NOPROFILER_ROUT(0);
}


/* Return a hash table by its name */
hash_t	*hash_find(char *name)
{
  return ((hash_t *) hash_get(hash_hash, name));
}

/* Return a hash table pointer by its name */
/* Overwrite existing table if there was one sharing that name, only
   if both tables have the same elements type */
int		hash_register(hash_t *table, char *name)
{
  hash_t	*h;
  int		sz;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  h = hash_get(hash_hash, name);
  if (h)
    {
      if (h->type == ASPECT_TYPE_UNKNOW)
	h->type = table->type;
      if (h->type != table->type)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Incompatible hash tables", -1);
      if (h->elmnbr)
	h = hash_empty(name);
      hash_merge(h, table);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }  
  XALLOC(__FILE__, __FUNCTION__, __LINE__, h, sizeof(hash_t), -1);
  sz = (table->size > table->elmnbr ? table->size : table->elmnbr);
  hash_init(h, name, sz, table->type);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Empty a hash table */
hash_t		*hash_empty(char *name)
{
  hash_t	*hash;
  char		*newname;
  int		size;
  char		type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  hash = hash_find(name);

  if (!hash)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);

  size    = hash->size;
  type    = hash->type;
  hash_del(hash_hash, name);
  hash_destroy(hash);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 newname, strlen(name) + 1, NULL);
  strcpy(newname, name);
  hash_init(hash, newname, size, type);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, hash);
}



/* Destroy a hash table */
void		hash_destroy(hash_t *h)
{
  char		**keys;
  int		idx;
  int		keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We should not destroy the elements as they might be in other hashes */
  keys = hash_get_keys(h, &keynbr);
  for (idx = 0; idx < keynbr; idx++)
    XFREE(__FILE__, __FUNCTION__, __LINE__, keys[idx]);
  hash_free_keys(keys);
  XFREE(__FILE__, __FUNCTION__, __LINE__, h->ent);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Add an entry to the hash table */
int		hash_add(hash_t *h, char *key, void *data)
{
  hashent_t	*actual;
  hashent_t	*newent;
  char		*backup;
  u_int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //Weaken the check : do not hash_get(h, key) check
  if (!h || !key || !data)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameters", -1);
  
  newent = NULL;
  for (index = 0, backup = key; *backup; backup++)
    index += *backup;
  index %= h->size;

  if (h->ent[index].key == NULL)
    {
      h->ent[index].key  = key;
      h->ent[index].data = data;
    }
  else
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	     newent, sizeof(hashent_t), -1);
      newent->key  = key;
      newent->data = data;
      actual = h->ent + index;
      while (actual->next)
	actual = actual->next;
      actual->next = newent;
    }
  h->elmnbr++;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Delete an entry from the hash table */
int		hash_del(hash_t *h, char *key)
{
  hashent_t	*actual;
  hashent_t	*todel;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check the first entry for this hash */
  //printf("before h = %p key = %p (%s) \n", h, key, (key ? key : ""));
  actual = hash_get_head(h, key);
  //printf("after  h = %p key = %p (%s) \n", h, key, (key ? key : ""));

  if (actual->key != NULL && !strcmp(actual->key, key))
    {
      if (actual->next)
	{
	  todel = actual->next;
	  *actual = *actual->next;
	  XFREE(__FILE__, __FUNCTION__, __LINE__, todel);
	}
      else
	bzero(actual, sizeof (hashent_t));
      h->elmnbr--;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Looking for the good entry in the list for this hash value */
  while (actual->next      != NULL && 
	 actual->next->key != NULL && 
	 strcmp(actual->next->key, key))
    actual = actual->next;

  /* Not found */
  if (!actual->next)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

  /* Found */
  todel = actual->next;
  actual->next = actual->next->next;
  XFREE(__FILE__, __FUNCTION__, __LINE__, todel);
  h->elmnbr--;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Retrieve the metadata for a given key */
void 		*hash_get(hash_t *h, char *key)
{
  hashent_t	*actual;

  actual = hash_get_head(h, key);
  while (actual      != NULL && 
	 actual->key != NULL && 
	 strcmp(actual->key, key))
    actual = actual->next;
  return (actual != NULL ? actual->data : NULL);
}

/* Retrieve the data pointer for a given key */
void 		*hash_select(hash_t *h, char *key)
{
  hashent_t	*actual;

  actual = hash_get_head(h, key);
  while (actual      != NULL && 
	 actual->key != NULL && 
	 strcmp(actual->key, key))
    actual = actual->next;
  return (actual != NULL ? &actual->data : NULL);
}



/* Change the metadata for an existing entry, giving its key */
int		hash_set(hash_t *h, char *key, void *data)
{
  hashent_t	*ent;

  ent = hash_get_ent(h, key);
  if (!ent)
    return (-1);
  ent->data = data;
  return (0);
}


/* Retrieve the -entry- for a given key */
hashent_t 	*hash_get_ent(hash_t *h, char *key)
{
  hashent_t	*actual;

  actual = hash_get_head(h, key);
  while (actual      != NULL && 
	 actual->key != NULL && 
	 strcmp(actual->key, key))
    actual = actual->next;
  return (actual);
}



/* Retreive a Hash entry head giving the key */
hashent_t	*hash_get_head(hash_t *h, char *backup)
{
  u_int		index;

  for (index = 0; *backup; backup++)
    index += *backup;
  return (&h->ent[index % h->size]);
}



/* Used to create arrays of keys for completion */
char		**hash_get_keys(hash_t *h, int *n)
{
  int		j, i;
  char		**keys;
  hashent_t	*entry;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  keys = NULL;
  for (i = j = 0; j < h->size; j++) 
    {
      entry = &h->ent[j];
      while (entry != NULL && entry->key != NULL) 
	{
	  
#if __DEBUG__
	  printf("hash[%u:%u] key = %s\n", j, i, entry->key);
#endif
	  XREALLOC(__FILE__, __FUNCTION__, __LINE__,
		   keys, keys, (i + 1) * sizeof(char *), NULL);
	  keys[i] = entry->key;
	  entry   = entry->next;
	  i++;
	}
    }
  
  XREALLOC(__FILE__, __FUNCTION__, __LINE__,
	   keys, keys, (i + 1) * sizeof(char *), NULL);
  
  keys[i] = NULL;
  if (n)
    *n = i;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, keys);
}



/* Free the keys returned by hash_get_keys() */
void		hash_free_keys(char **keys)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XFREE(__FILE__, __FUNCTION__, __LINE__, keys);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Print the hash table (DEBUG PURPOSE) */
void            hash_print(hash_t *h)
{
  hashent_t     *actual;
  int           index;

  puts(".::. Printing hash .::. ");
  for (index = 0; index < h->size; index++)
    {
      for (actual = &h->ent[index]; 
	   actual != NULL && actual->key != NULL; 
	   actual = actual->next)
	printf(" ENT [%u] key = %s ; data = %p ; next = %p\n",
	       index, actual->key, actual->data, actual->next);
   }
}

/* Apply func all entries */
int		hash_apply(hash_t      *h, 
			   void	       *ptr, 
			   int	       (*func)(hashent_t *ph, void *pptr))
{
  hashent_t     *actual;
  int           index;
  int		ret = 0;

  for (index = 0; index < h->size; index++)
    {
      for (actual = &h->ent[index];
           actual != NULL && actual->key != NULL;
           actual = actual->next)
	ret |= func (actual, ptr);
    }

  return ret;
}


/* Compare 2 hash tables */
/* Contributed by zorgon */
/* Can be used to compare ELF in memory and ELF in file */
int		hash_compare(hash_t *first, hash_t *two)
{
  int		index;
  int		m;
  hashent_t	*actual;
  hashent_t	*bis;

  if (first->size != two->size) 
    return (-1);
  
  for (m = index = 0; index < first->size; index++) 
    {
      for (actual = first->ent + index; 
	   actual != NULL && actual->key != NULL; 
	   actual = actual->next) 
	{
	  bis = hash_get_ent(two, actual->key);
	  if (actual->data != bis->data) 
	    {
	      printf("FIRST  key = %s ; data = %p", 
		     actual->key, actual->data);
	      printf("SECOND key = %s ; data = %p", 
		     bis->key, bis->data);
	      m++;
	    }
	  //misused API
	  //hash_destroy(bis);
	}
    }
  if (m)
    return (-1);
  return (0);
}


/* Merge hash tables in the first one */
/* We cannot use hash_get_keys() because we dont know the type of hashed objects */
int		hash_merge(hash_t	*dst,
			   hash_t	*src)
{
  hashent_t     *actual;
  int           index;
  int		ret;

  /* Make sure we dont inject element already presents */
  if (!src || !dst || src->elmnbr == 0)
    return (0);
  for (ret = index = 0; index < src->size; index++)
    for (actual = &src->ent[index];
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      if (!hash_get(dst, actual->key))
	ret += hash_add(dst, actual->key, actual->data);
  return ret;
}


/* Delete all elements of source hash in destination hash */
/* We cannot use hash_get_keys() because we dont know the type of hashed objects */
int		hash_unmerge(hash_t	*dst,
			     hash_t	*src)
{
  hashent_t	*actual;
  int           index;
  int		ret;
  
  /* Make sure we dont inject element already presents */
  if (!src || !dst || src->elmnbr == 0)
    return (0);
  for (ret = index = 0; index < src->size; index++)
    for (actual = &src->ent[index];
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      if (hash_get(dst, actual->key))
	ret += hash_del(dst, actual->key);
  return ret;
}


/* Return the hash size */
int		hash_size(hash_t *hash)
{
  return (hash ? hash->elmnbr : 0);
}

/* Return the only element of this hash */
void*		hash_get_single(hash_t *hash)
{
  char		**keys;
  int		idx;

  if (!hash || hash_size(hash) != 1)
    return (NULL);
  keys = hash_get_keys(hash, &idx);
  return (hash_get(hash, keys[0]));
}

/* Return an element of this hash */
/* The choice is non-deterministic */
void*		hash_get_one(hash_t *hash)
{
  char		**keys;
  int		index;

  if (!hash || !hash_size(hash))
    return (NULL);
  keys = hash_get_keys(hash, &index);
  return (hash_get(hash, keys[0]));
}
