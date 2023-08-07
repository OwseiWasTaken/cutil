#include "cutil.h"

int COUNT = 0;
int *MakeObject() {
	int *info = malloc(sizeof(int));
	*info = COUNT++;
	return info;
}

void print_node(linked_list *list, int index, void* obj) {
	printf("%d: <%d>\n", index, *(int*)obj);
}

void remove_odd(linked_list *list, int index, void* obj) {
	int info = *(int*)obj;
	if ( (info%2) == 1 ) {
		list_remove(list, index, true);
	}
}

int main() {
	linked_list *list = list_alloc();
	assert(list_append(list, MakeObject()));

	for (int i = 0; i < 20; i++) {
		assert(list_insert(list, MakeObject(), list->len));
	}


	//list_iter(list, print_node);
	//assert(list_remove(list, 5, true));
	//list_iter(list, print_node);

	list_iter(list, print_node);
	list_iter(list, remove_odd);
	list_iter(list, print_node);
	//for (int i = 0; i<list->len; i++) {
	//	int info = *(int*)list_get(list, i);
	//	//printf("%d: <%d> [%%2:%d]\n", i, info, (info%2) == 1);
	//	if ( (info%2) == 1 ) {
	//		list_remove(list, i, true);
	//		i--;
	//	}
	//}
	//list_iter_node(list, node_iter_print);


	list_free(list, true);
}

