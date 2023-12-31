#ifndef CORE_HASHMAP_H
#define CORE_HASHMAP_H

#include "../memory/alloc.h"

#include "types.h"
#include "vector.h"

typedef struct entry
{
	// Size in bytes of the key
	uint64_t key_size;
	byte* key;
	byte* value;
	struct entry* next;
} entry;

typedef struct
{
	uint64_t size;
	vector(entry) buckets;
} _hashmap;

#define hashmap(_key, _value) _hashmap*

// =================================================================
// HASH FUNCTIONS:

uint64_t  _hash64(byte* _data, uint64_t size);
uint64_t* _hash128(byte* _data, uint64_t size);
uint64_t  getblock64(const uint64_t* p, uint64_t i);
uint64_t  fmix64(uint64_t k);

#define hash64(_data)  _hash64(_data, sizeof(_data))
#define hash128(_data) _hash128(_data, sizeof(_data))

// =================================================================

entry* entry_new(void* key, void* value);
//     Takes in a function pointer to free the data inside the entry for the key and value
void   _entry_free(entry* current_entry, void (*key_free)(void*), void (*value_free)(void*));

hashmap(void*, void*) hashmap_default();
hashmap(void*, void*) _hashmap_new(entry* entries, uint64_t num_entries);
void                  hashmap_free(hashmap(void*, void*) hmap);
void                  hashmap_insert(hashmap(void*, void*) hmap, byte* key, byte* value, uint64_t key_size);
void                  _hashmap_inserts(hashmap(void*, void*) hmap, void* keys[], void* values[], uint64_t num_entries, uint64_t key_sizes[]);
void                  __hashmap_inserts(hashmap(void*, void*) hmap, void* keys[], void* values[], uint64_t num_entries, uint64_t key_sizes);
//                    This expects an array of key sizes that is the same length as the entries
void                  _hashmap_insert_entries(hashmap(void*, void*) hmap, entry entries[], uint64_t num_entries, uint64_t key_sizes[]);
void                  __hashmap_insert_entries(hashmap(void*, void*) hmap, entry entries[], uint64_t num_entries, uint64_t key_sizes);
void                  hashmap_remove(hashmap(void*, void*) hmap, byte* key, uint64_t key_size);
//					  Takes in a function pointer to free the data inside the entry for the key and value
void                  _hashmap_free(hashmap(void*, void*) hmap, void (*key_free)(void*), void (*value_free)(void*));
void*			      hashmap_traverse(hashmap(void*, void*) hmap);
void*                 hashmap_get(hashmap(void*, void*) hmap, byte* key, uint64_t key_size);
bool                  hashmap_compare_keys(byte* key1, byte* key2, uint64_t key_size);

#define hashmap_inserts(_hmap, _keys, _values, _key_sizes) _hashmap_inserts(_hmap, _keys, _values, sizeof(_keys) / sizeof(_keys[0]), _key_sizes)
#define hashmap_insertss(_hmap, _keys, _values, _key_sizes) __hashmap_inserts(_hmap, _keys, _values, sizeof(_keys) / sizeof(_keys[0]), _key_sizes)
#define hashmap_insert_entries(_hmap, _entries, _key_sizes) _hashmap_insert_entries(_hmap, _entries, sizeof(_entries) / sizeof(_entries[0]), _key_sizes)
#define hashmap_insert_entriess(_hmap, _entries, _key_sizes) __hashmap_insert_entries(_hmap, _entries, sizeof(_entries) / sizeof(_entries[0]), _key_sizes)
#define hashmap_new(_entries) _hashmap_new(_entries, sizeof(_entries) / sizeof(_entries[0]))
#define entry_free(_entry, _free_fn) _entry_free(_entry, _free_fn, _free_fn)
#define hashmap_free(_hmap, _free_fn) _hashmap_free(hmap, _free_fn, _free_fn);

#endif // CORE_HASH_H