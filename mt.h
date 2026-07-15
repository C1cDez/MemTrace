#ifndef _MT_H_
#define _MT_H_

#ifdef _MT_DEBUG

void* _mt_malloc(size_t size, int line, const char* funcname, const char* filename);
void* _mt_calloc(size_t count, size_t size, int line, const char* funcname, const char* filename);
void* _mt_realloc(void* ptr, size_t size, int line, const char* funcname, const char* filename);
void _mt_free(void* ptr, int line, const char* funcname, const char* filename);

#define malloc(size)          _mt_malloc(size, __LINE__, __func__, __FILE__)
#define calloc(count, size)   _mt_calloc(count, size, __LINE__, __func__, __FILE__)
#define realloc(ptr, size)    _mt_realloc(ptr, size, __LINE__, __func__, __FILE__)
#define free(ptr)             _mt_free(ptr, __LINE__, __func__, __FILE__)

#endif

void mt_log(void);

#endif
