#ifndef cutil_funcs
#define cutil_funcs 1

// IO
void StartChTerm (FILE *log) {
	if (_StartCh) return;
	setvbuf(stdin, LastKey, _IOFBF, MAXCHARLEN);
	_StartCh = true;

	struct termios term;
	tcgetattr(STDIN_FILENO, &term);
	if (log) fprintf(log, "this tty: %s\n", ttyname(STDIN_FILENO));

	//TODO try to save to ~/.tty_restore
	int rsts = (system("/bin/stty -g > /tmp/restore")); // must be able to copy file
	if (rsts && log) {
		fprintf(log, "can't save tty config '/tmp/restore'\n");
	} else if (log) {
		fprintf(log, "tty config saved to '/tmp/restore'\n");
	}

	char reconfig[36+10]; /* 36 = comamnd len, 10 = /dev/ttyNN */
	// -F for this tty
	// -echo to show chars
	// cbreak to keep ^C -> SIGINT
	// min 1 = feed buffer to program;
	sprintf(reconfig, "/bin/stty -F %s -echo cbreak min 1", ttyname(STDIN_FILENO));
	int x = system(reconfig); // must be able to set config
	if (log) fprintf(log, "tty tried to be reconfigured ($?=%d), so linefeed buffering is off\n", x);
}

//TODO: return -1 on error
void StopChTerm (FILE *log) {
	if (!_StartCh) return;
	setvbuf(stdin, stdinbuff, _IOLBF, BUFSIZ);
	_StartCh = false;

	int restore = system("/bin/stty $(/bin/cat /tmp/restore)"); // must be able to reset config
	if (restore) {assert(system("/bin/reset"));}
	if (log) fprintf(log, "\ntty config restored\n");

	if (system("/bin/rm -f /tmp/restore")) {
		if (log) fprintf(log, "couldn't delete file '/tmp/restore'\n");
	} else {
		if (log) fprintf(log, "file '/tmp/restore' deleted\n");
	}
}

inline bool IsChTermOn() { return _StartCh; }

int ChLen() {
	int i = 0;
	for (;LastKey[i] && i < MAXCHARLEN; i++);
	return i;
}

void GetCh () {
	// clean LastKey
	memset(LastKey, 0, MAXCHARLEN);
	// get first char, set lk
	getc(stdin);
	int ln = ChLen(); // clear buffer for \x1b keys
	for (int i = 1; i<ln; i++) getc(stdin);
}

bool CheckKey(char *CMP_KEY) {
	return strncmp(CMP_KEY, LastKey, MAXCHARLEN)==0;
}

// flush stdout
int flush() {return fflush(stdout);}

int GetFdCount () {
	int fdcount;
	// /self/ or getpid()
	DIR *dir = opendir("/proc/self/fd/");
	for (fdcount = 0; readdir(dir) != NULL; fdcount++);

	// print dir names
	//struct dirent *dp;
	//for (fdcount = 0; (dp = readdir(dir)) != NULL; fdcount++) {
	//	printf("%s\n", dp->d_name);
	//}

	closedir(dir);
	return fdcount - 3; // ., .., this dirfd
}

bool CheckValidFd(int fd) {
	int fdcount;
	char fdstr[CountDecimalDigits(fd)];
	sprintf(fdstr, "%i", fd);
	// /self/ or getpid()
	DIR *dir = opendir("/proc/self/fd/");

	// print dir names
	struct dirent *dp;
	for (fdcount = 0; (dp = readdir(dir)) != NULL; fdcount++) {
		if (!strcmp(dp->d_name, fdstr)) {
			closedir(dir);
			return false;
		}
	}

	closedir(dir);
	return true;
}

//TODO: get fd array, compare to array
//1 read, instead of N+1 reads
// (Max) N Compares instead of FDs*N
int GetValidFd() {
	int i;
	for (i = 0; !CheckValidFd(i); i++);
	if (i > MAXFDS) { return -1; } // TODO: set errno
	return i;
}

// Time
fmttime FmtTime (const time_t rn, const int UTF) {
	fmttime now;

	char *timetext = ctime(&rn);

	// string manipulation
	memcpy(now.weekday, (timetext), 3);
	now.weekday[4] = '\0';

	memcpy(now.month, (timetext+4), 3);
	now.month[3] = '\0';

	now.day = timetext[9]-'0';
	if (timetext[8] != ' ') {
		now.day += 10*(timetext[8]-'0');
	};

	// ctime manipulation
	now.minute = (rn%(60*60))/60;
	now.seccond= rn%60;
	now.year = 1970+(int)(rn/(24.0*3600.0)/365.25);
	now.hour = rn/3600%24+(UTF);
	if (now.hour < 1) {
		now.hour = 24+now.hour;
	}

	return now;
}

void FmtTimeToString (const fmttime now, char *buff) {
	sprintf(buff, "s:%d\nm:%d\nh:%d\nd:%d\ny:%d\n\nM:%s\nW:%s",
		now.seccond, now.minute, now.hour, now.day, now.year, now.month, now.weekday);
}

// Math
int ipow (const int base, const int power) {
	return (int)pow((double)base, (double)power);
}

int isquare (const int base) {
	return ipow(base, 2);
}

int isqrt(const int num) {
	return (int)sqrt((float)num);
}

int CountDecimalDigits( int v ) {
	int i;
	for (i = 0; v; i++) {v = v/10;}
	return i;
}

ppoint bhask (size_t a, size_t b, const size_t c) {
	ppoint p = {.x=0.0, .y=0.0};
	float delt = sqrt(isquare(b) - (4*a*c));
	b *= -1;
	a *= 2;
	p.x = (b + delt) / a;
	p.y = (b - delt) / a;
	return p;
}

// ANSII
void move (const int y, const int x) {
	printf(ESC"[%d;%dH", y, x);
}

void pmove (const point p) {
	printf(ESC"[%d;%dH", p.y, p.x);
}

void rmove(const int y, const int x) {
	printf("\x1b[%d%c\x1b[%d%c",
		abs(y), y<0?'A':'B',
		abs(x), x<0?'D':'C'
	);
}

inline void HideCursor () {
	puts(ESC"[?25l");
}

inline void ShowCursor () {
	puts(ESC"[?25h");
}

void ClearLine(const int y, const int x) {
	move(y, x);
	puts(ESC"[2K");
}

void pClearLine(const point p) {
	pmove(p);
	puts(ESC"[2K");
}

void TsRGB (char *buff, const color RGB) {
	sprintf(buff, ESC"[38;2;%d;%d;%dm", RGB.R, RGB.G, RGB.B);
}

void TRGB (char *buff, const byte R, const byte G, const byte B) {
	sprintf(buff, ESC"[38;2;%hhu;%hhu;%hhum", R, G, B);
}

void PRGB (const byte R, const byte G, const byte B) {
	printf(ESC"[38;2;%hhu;%hhu;%hhum", R, G, B);
}

upoint GetTerminalSize () {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	upoint p = {.x = (uint32_t)w.ws_col, .y = (uint32_t)w.ws_row};
	return p;
}

inline char* BoolToStr(bool b) {
	return b?"true":"false";
}

// structs
void _Print_Point(const point p) {
	printf("{%i,%i}", p.y, p.x);
}

color RGB(byte r, byte g, byte b) {
	color c = {.R=r, .G=g, .B=b};
	return c;
}

int fsleep (long double t) {
	return usleep((int) lroundl(1000000.0 * t));
}

queue_t *MakeQueue() {
	queue_t *q = malloc(sizeof(queue_t));
	q->first = 0;
	q->last = 0;
	memset(q->qu, 0, 255*sizeof(int));
	return q;
}

queue_t *InitQueue(queue_t *q) {
	q->first = 0;
	q->last = 0;
	memset(q->qu, 0, 255*sizeof(int));
	return q;
}

void ReallocQueue(queue_t *q) {
	// queue: [?, ?, ?, 1, 2, 3, ?, ?]
	// points:          f  -  l
	// queue: [1, 2, 3, ?, ?]
	// points: f  -  l

	// memmov if last > first
	//int t[255] = {0};
	//memmove(t, q->qu+q->first, q->last*sizeof(int));
	//memmove(q->qu, t, 255);

	memmove(q->qu, q->qu+q->first, q->last*sizeof(int));
	q->last -= q->first;
	q->first = 0;
}

// 0 = ok
// -1 = realloc but failed
int QueueAppend(queue_t *q, int item) {
	if (q->qu[q->last]) {
		ReallocQueue(q);
	}
	if (q->qu[q->last]) {
		return -1;
	}
	q->qu[q->last] = item;
	q->last++;
	return 0;
}

int QueuePop(queue_t *q) {
	int ret = q->qu[q->first];
	//q->first=q->first+1;
	q->first++;
	return ret;
}

int QueueRead(queue_t *q) {
	return q->qu[q->first];
}

int QueueSize(queue_t *q) {
	return q->last;
}

int QueueOffset(queue_t *q) {
	return q->first;
}

void _P_queue(queue_t *q) {
	printf("%d->%d\n", q->first, q->last);
	printf("queue {\n"TAB);
	for (byte i = 0; i<16; i++) {
		for (byte j = 0; j<16; j++) {
			//printf("<%d:%d>  ", 16*i+j, q->qu[16*i+j]);
			if (i*8+j == q->first) {
				PRGB(255,128,0);
				printf("%d", q->qu[i*16+j]);
				PRGB(255,255,255);
			} else if (i*8+j == q->last-1) {
				PRGB(255,127,128);
				printf("%d", q->qu[i*16+j]);
				PRGB(255,255,255);
			} else {
				printf("%d", q->qu[i*16+j]);
			}
			putc(',', stdout);
			putc(' ', stdout);
		}
		printf("\b\b \n"TAB); // remove ", " put " \n\t"
	}
	puts("\b\b}");
}

int randint(int min, int max) {
	return (rand() % (max-min+1))+min;
}

bool odds(int onein) {
	return !randint(0, onein);
}
char *revstr(char *dest, const char *src) {
	size_t n = strlen(src);
	for (size_t i = (n-=1);i+1; i--) dest[n-i] = src[i];
	return dest;
}

char *revnstr(char *dest, const char *src, size_t n) {
	for (size_t i = (n-=1);i+1; i--) dest[n-i] = src[i];
	return dest;
}

upoint UpointDiff(upoint a, upoint b) {
	upoint p = {.y=a.y - b.y, .x = a.x - b.x};
	return p;
}

upoint MakeU(uint32_t x, uint32_t y) {
	upoint p = {.y=y, .x=x};
	return p;
}

size_t min(size_t a, size_t b) {
	return a<b?a:b;
}

size_t max(size_t a, size_t b) {
	return a>b?a:b;
}

int node_print(node *n) {
	return printf("node{.obj=%p, .next=%p}\n",
		n->obj, n->next
	);
}

node *node_alloc(void *obj) {
	node *n = (node*)malloc(sizeof(node));
	n->obj = obj;
	n->next = NULL;
	return n;
}

// warning, can do an infinite loop
node *get_last_node(node *head) {
	for (;true;) {
		if (head->next == NULL) return head;
		head = head->next;
	}
	return NULL; // can't reach
}

// sets *len to the size of the list
node *get_last_node_len(node *head, int *len) {
	*len = -1;
	if (head == NULL) return NULL;
	for (int i = 0;true;i++) {
		if (head->next == NULL) {
			*len = i;
			return head;
		}
		head = head->next;
	}
	return NULL; // can't reach
}

node *node_append(node *head, void *obj) {
	node *tail = get_last_node(head);
	tail->next = node_alloc(obj);
	return tail->next;
}

void node_list_free(node *head, bool free_objs) {
	node *next;
	for (;true;) {
		next = head->next;
		if (free_objs) free(head->obj);
		free(head);
		head = next;
		if (head == NULL) return;
	}
}

void nodeListPrint(node *head) {
	printf("node List:\n");
	for (int i = 0; head!=NULL; i++) {
		printf("%i: %p\n", i, head->obj);
		head = head->next;
	}
}

// use user function to see if nodes are equal, insted of strncmp
node *nodeListSearchFn(
	node *head, void *key, size_t keylen,
	bool (*compare_function)(void *obj, void *key, size_t keylen)
) {
	for (;true;) {
		if (compare_function(head->obj, key, keylen)) return head;
		if (head->next == NULL) return NULL;
		head = head->next;
	}
	return NULL; // can't reach
}

linked_list *list_alloc() {
	linked_list *list = malloc(sizeof(linked_list));
	list->len = 0;
	list->head = NULL;
}

void list_free(linked_list *list, bool free_objs) {
	node_list_free(list->head, free_objs);
	free(list);
}

void list_reset(linked_list *list, bool free_objs) {
	node_list_free(list->head, free_objs);
	list->len = 0;
	list->head = NULL;
}

node *list_get_node(linked_list *list, int index) {
	if (index >= list->len) {
		errno = EFAULT;
		return NULL;
	}

	node *tail = list->head;
	for (int i = 0; i<index; i++) {
		if (tail->next == NULL) return NULL;
		tail = tail->next;
	}
	return tail;
}


void *list_get(linked_list *list, int index) {
	return list_get_node(list, index)->obj;
}

bool list_insert(linked_list *list, void *obj, int index) {
	if (index == 0) {
		if (list->head == NULL) {
			list->head = node_alloc(obj);
			list->len=1;
		} else {
			node *nd = node_alloc(obj);
			nd->next = list->head;
			list->head = nd;
			list->len++;
		}
		return true;
	} else if (index < list->len) {
		node *tail = list->head;
		for (int i = 1; i<index; i++) {
			if (tail->next == NULL) return NULL;
			tail = tail->next;
		}
		node *nd = node_alloc(obj);
		nd->next = tail->next;
		tail->next = nd;
		list->len++;
		return true;
	} else if (index == list->len) {
		if (list->head == NULL) {
			list->head = node_alloc(obj);
			list->len=1;
		} else {
			node_append(list->head, obj);
			list->len++;
		}
		return true;
	}
	return false;
}

bool list_append(linked_list *list, void *obj) {
	return list_insert(list, obj, list->len);
}

bool list_remove(linked_list *list, int index, bool free_obj) {
	if (list->len < index) return false;

	if (index == 0) {
		node *next = list->head->next;
		if (free_obj) {
			free(list->head->obj);
		}
		free(list->head);
		list->head = next;
		list->len--;
	} else {
		node *prev = list_get_node(list, index-1);
		node *next = prev->next->next;
		//node_print(prev);
		//node_print(prev->next);
		//node_print(next);
		if (free_obj) {
			free(prev->next->obj);
		}
		free(prev->next);
		prev->next = next;
		list->len--;
	}
}

void list_iter_node(
	linked_list *list,
	void (*forEach)(linked_list *list, int index, node *obj)
) {
	node *tail = list->head;
	if (tail == NULL) return;

	for (int i = 0;i<list->len;i++) {
		forEach(list, i, tail);
		if (tail->next != NULL) {
			tail = tail->next;
		} else {
			return;
		}
	}
}

void list_iter(
	linked_list *list,
	void (*forEach)(linked_list *list, int index, void *obj)
) {
	node *tail = list->head;
	if (tail == NULL) return;

	for (int i = 0;i<list->len;i++) {
		forEach(list, i, tail->obj);
		if (tail->next != NULL) {
			tail = tail->next;
		} else {
			return;
		}
	}
}

void node_iter_print(linked_list *list, int index, node *nd) {
	printf("%d: {.obj=%p, .next=%p}\n", index, nd->obj, nd->next);
}

int HashnodePrint(Hashnode *hnode) {
	return printf("Hashnode{.obj=%p, .key=%p, .keylen=%ld}\n",
		hnode->obj, hnode->key, hnode->keylen
	);
}

// string -> int
int _P_Hashnode_print(Hashnode *hnode) {
	return printf("\"%s\"=>%lX\n",
		(char*)hnode->key,
		*(size_t*)(hnode->obj)
	);
}

HashTable *HashTableAlloc(
	size_t size, bool isowner,
	size_t (*HashFunction)(byte *key, size_t keylen)
) {
	HashTable *table = malloc(sizeof(HashTable));
	node **arr = calloc(sizeof(node), size);
	for (size_t i = 0; i<size; i++) {
		//arr[i] = nodeAlloc(malloc(sizeof(Hashnode)));
		arr[i] = NULL;
	}
	table->size = size;
	table->isowner = isowner;
	table->array = arr;
	table->HashFunction = HashFunction;
	table->used = 0;
	table->entries = 0;
	return table;
}

void HashTableFree(HashTable *table) {
	for (size_t i = 0; i<table->size; i++) {
		node *nd = table->array[i];
		for (;nd!=NULL;) {
			// detele objects (if owner) and keys
			if (table->isowner) {
				free(((Hashnode*)nd->obj)->obj);
			}
			free(((Hashnode*)nd->obj)->key);
			nd = nd->next;
		}
		// delete nodes and Hashnodes
		if (table->array[i] != NULL) {
			nodeListFree(table->array[i], true);
		}
	}
	free(table->array);
	free(table);
}

// could speedup if, instead of using malloc than node_alloc
// pointed node to same space as
int HashTableAdd(
	HashTable *table,
	byte *key, size_t keylen, void *obj
) {
	table->entries++;
	int node_count = 0;
	size_t index = table->HashFunction(key, keylen);
	assert(index > 0); // hash funcion failed
	index = index%table->size;
	node *tail = GetLastnodeLen((table->array)[index], &node_count);

	Hashnode *hnode = malloc(sizeof(Hashnode));
	hnode->obj = obj;
	hnode->keylen = keylen;
	hnode->key = calloc(sizeof(byte), keylen);
	memcpy(hnode->key, key, keylen);

	if (tail == NULL) {
		table->used++;
		table->array[index] = nodeAlloc(hnode);
	} else {
		nodeAppend(tail, hnode);
	}
	return node_count+1;
}

void _P_HashTable(HashTable *table) {
	for (size_t i = 0; i<table->size; i++) {
		node *nd = table->array[i];
		printf("link %li\n", i);
		for (;nd!=NULL;) {
			Hashnode *hnode = (Hashnode*)nd->obj;;
			printf("\t");
			_P_Hashnode_print(hnode);
			nd = nd->next;
		}
	}
}

bool nodeListCompareHashnode(
	void *_obj, void *key, size_t keylen
) {
	Hashnode *obj = (Hashnode*)_obj;
	if (keylen != obj->keylen) return false;
	return strncmp((char*)obj->key, key, keylen)==0;
}

Hashnode *HashTableSearchnode(
	HashTable *table,
	byte *key, size_t keylen
) {
	size_t index = table->HashFunction(key, keylen);
	assert(index > 0); // hash funcion failed
	index = index%table->size;
	node *head = (table->array)[index];

	if (head == NULL) {
		printf("No such key %s\n", key);
		return NULL;
	}

	return ((Hashnode*)nodeListSearchFn(
		head, key, keylen,
		nodeListCompareHashnode
	)->obj);
}

void *HashTableSearch(
	HashTable *table,
	byte *key, size_t keylen
) {
	return HashTableSearchnode(table, key, keylen)->obj;
}

// is also colision chance
float HashTableLoadnodeFactor(HashTable *table) {
	return (float)table->used/(float)table->size;
}

float HashTableLoadEntryFactor(HashTable *table) {
	return (float)table->entries/(float)table->size;
}

void _P_HashTable_info(HashTable *table) {
	printf("load entry factor = %li/%li = %f\n",
		table->entries, table->size,
		HashTableLoadEntryFactor(table)
	);
	printf("load node factor = %li/%li = %f\n",
		table->used, table->size,
		HashTableLoadnodeFactor(table)
	);
	printf("entries per node = %li/%li = %f\n",
		table->entries, table->used,
		(float)table->entries/(float)table->used
	);
}

#endif
