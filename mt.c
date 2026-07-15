#undef _MT_DEBUG
#include "mt.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define _EXIT_FATAL(reason, ...) do { fprintf(stderr, reason __VA_ARGS__); exit(1); } while (0);
#define _FORMAT_POS "\x1b[33m%s\x1b[0m:\x1b[35m%d\x1b[0m(`\x1b[36m%s\x1b[0m')"
#define _FILENAME(fn) strrchr(fn, '\\') ? strrchr(fn, '\\') + 1 : fn

typedef struct _memtrace
{
	void* ptr;
	struct
	{
		int happened;
		int online;
		const char* infunction;
		const char* infile;
	} allocated, freed;
	struct _memtrace* next;
} _memtrace_t;

static _memtrace_t* head = NULL;
static void __mt_new_memtrace(void* ptr, int line, const char* funcname, const char* filename)
{
	_memtrace_t* mt = malloc(sizeof(_memtrace_t));
	if (!mt) _EXIT_FATAL("Unable to init mt debug struct\n");
	mt->ptr = ptr;

	mt->allocated.happened = 1;
	mt->allocated.online = line;
	mt->allocated.infunction = funcname;
	mt->allocated.infile = _FILENAME(filename);

	mt->freed.happened = 0;
	mt->freed.online = 0;
	mt->freed.infunction = NULL;
	mt->freed.infile = NULL;

	mt->next = head;
	head = mt;
}

void* _mt_malloc(size_t size, int line, const char* funcname, const char* filename)
{
	void* ptr = malloc(size);
	if (!ptr && size > 0) _EXIT_FATAL("Unable to allocate on " _FORMAT_POS "\n", , filename, line, funcname);
	__mt_new_memtrace(ptr, line, funcname, filename);
	return ptr;
}
void* _mt_calloc(size_t count, size_t size, int line, const char* funcname, const char* filename)
{
	void* ptr = calloc(count, size);
	if (!ptr && (count * size) > 0)
		_EXIT_FATAL("Unable to allocate on " _FORMAT_POS "\n", , filename, line, funcname);
	__mt_new_memtrace(ptr, line, funcname, filename);
	return ptr;
}
void* _mt_realloc(void* ptr, size_t size, int line, const char* funcname, const char* filename)
{
	if (!ptr) return _mt_malloc(size, line, funcname, filename);

	_memtrace_t* cur = head;
	while (cur && (cur->ptr != ptr || cur->freed.happened)) cur = cur->next;

	if (!cur) _EXIT_FATAL("Reallocating unallocated memory on " _FORMAT_POS "\n", , filename, line, funcname);

	void* newptr = realloc(ptr, size);
	if (!newptr && size > 0) _EXIT_FATAL("Unable to reallocate on " _FORMAT_POS "\n", , filename, line, funcname);

	cur->freed.happened = 1;
	cur->freed.online = line;
	cur->freed.infunction = funcname;
	cur->freed.infile = _FILENAME(filename);

	__mt_new_memtrace(newptr, line, funcname, filename);
	return newptr;
}
void _mt_free(void* ptr, int line, const char* funcname, const char* filename)
{
	if (!ptr) return;

	_memtrace_t* cur = head;
	while (cur && cur->ptr != ptr)
		cur = cur->next;

	if (!cur) _EXIT_FATAL("Freeing unallocated memory %p on " _FORMAT_POS "\n", , ptr, filename, line, funcname);
	if (cur->freed.happened) _EXIT_FATAL("Double freeing %p on " _FORMAT_POS "\n", , ptr, filename, line, funcname);

	cur->freed.happened = 1;
	cur->freed.online = line;
	cur->freed.infunction = funcname;
	cur->freed.infile = _FILENAME(filename);
	free(ptr);
}



void mt_log(void)
{
	_memtrace_t* cur = head;
	if (!cur) return;

	printf("-------------------------MT LOG-------------------------\n");
	while (cur)
	{
		if (cur->freed.happened)
		{
			printf(
				"\x1b[32m[v] %p\x1b[0m   ( " _FORMAT_POS " - " _FORMAT_POS " )\n"
				,
				cur->ptr,
				cur->allocated.infile, cur->allocated.online, cur->allocated.infunction,
				cur->freed.infile, cur->freed.online, cur->freed.infunction
			);
		}
		else
		{
			printf(
				"\x1b[31m[x] %p\x1b[0m   ( " _FORMAT_POS " - ... )\n"
				,
				cur->ptr,
				cur->allocated.infile, cur->allocated.online, cur->allocated.infunction
			);
		}
		cur = cur->next;
	}
	printf("--------------------------------------------------------\n");
}
