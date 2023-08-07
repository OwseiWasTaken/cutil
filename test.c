#include "cutil.h"
#define bytes_in_size_t sizeof(size_t)

size_t hashfC(byte *key, size_t keylen) {
	if (!keylen) {return -1;}
	return *key;
	//TODO: make an actual hash function
	return *(size_t*)key & SIZE_MAX>>(bytes_in_size_t*keylen);
}

size_t hashfC2(byte *key, size_t keylen) {
	if (!keylen) {return -1;}
	return *key;
}

int main(void) {

	// make hash table for 2byte key
	// 26 nodes
	// hashtable IS owner of the values, and will shallow free them
	// hashing will be done with size_t hash_table(byte *key, size_t keylen)L
	hash_table *table = hash_table_alloc(26, true, hashfC);
	_P_hash_table_info(table);

	for (char l = 'a'; l<='z'; l++) {
		for (char k = 'a'; k<='z'; k++) {
			byte *obj = malloc(9);
			byte *key = malloc(2);
			key[0] = (byte)l;
			key[1] = (byte)k;

			obj[0] = (byte)l;
			obj[1] = (byte)k;
			obj[2] = ' ';
			obj[3] = 'I';
			obj[5] = 'N';
			obj[4] = 'F';
			obj[6] = 'O';
			obj[7] = '.';
			obj[8] = 0;

			hash_table_add(table, key, 2, obj);

			free(key);
		}
	}
	for (char l = 'a'; l<='z'; l++) {
		for (char k = 'a'; k<='z'; k++) {
			byte key[] = {l,k};
			char *s = (char*)hash_table_search(table, key, 2);
			printf( "%s=>%s\n", key, s);
		}
	}

	_P_hash_table_info(table);

	hash_table_free(table);

}

