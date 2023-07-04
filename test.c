#include "cutil.h"
#define bytes_in_size_t sizeof(size_t)

size_t hashfC(byte *key, size_t keylen) {
	if (!keylen) {return -1;}
	return *key;
	//TODO: make good ok function
	return *(size_t*)key & SIZE_MAX>>(bytes_in_size_t*keylen);
}

int main(void) {
	// 24 bytes in hash_table* in single malloc
	// size*48 bytes in
	// : 2 malloc calls (16byte) * size
	// : 1 calloc call (16 byte * size)

	hash_table *table = hash_table_alloc(26, true, hashfC);
	_P_hash_table_info(table);

	for (char l = 'a'; l<='z'; l++) {
		for (char k = 'a'; k<='z'; k++) {
			byte *obj = malloc(8);
			byte *key = malloc(2);
			key[0] = (byte)l;
			key[1] = (byte)k;

			// inverted cause little ending
			obj[7] = 0;
			obj[6] = '.';
			obj[5] = '.';
			obj[4] = '.';
			obj[3] = '.';
			obj[2] = ' ';
			obj[1] = (byte)l;
			obj[0] = (byte)k;

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

	//_P_hash_table(table);

	//printf("searching for \"d\"\n");
	//hash_node *hnode = hash_table_search_node(table, (byte*)"d", 1);
	//_P_hash_node_print(hnode);

	//printf("searching for \"b\"\n");
	//hash_node *hnode2 = hash_table_search_node(table, (byte*)"b", 1);
	//_P_hash_node_print(hnode2);

	_P_hash_table_info(table);

	hash_table_free(table);

	//node *head = node_alloc("hello");
	//node_append(head, "world");
	//node_list_print(head);
	//node *n = node_list_search(head, (byte *)"hello", 5);
	//node_print(n);
	//node_list_free(head, false);
}

