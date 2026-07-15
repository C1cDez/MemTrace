# MemTrace
Simple lib to track dynamically allocated memory.

## Usage

1. Copy `mt.h` and `mt.c` into your project dir
2. Include `mt.h` into your code file. 
3. To enable tracing, add `#define _MT_DEBUG` before `#include "mt.h"`.
4. To disable tracing remove `#define _MT_DEBUG`
5. To log information use `mt_log()`


## Examples

```c
#include <stdio.h>
#include <stdlib.h>

/* enable tracing */
#define _MT_DEBUG
#include "mt.h"

/* allocate memory somewhere */
int* create_array(int n)
{
    int* arr = calloc(n, sizeof(int));
    if (!arr) return NULL;
    for (int i = 0; i < n; i++) arr[i] = i;
    return arr;
}

int main()
{
    int* ptr = create_array(10);
    free(ptr);

    int* newptr = calloc(20, sizeof(int));
    /* do not free memory */
    /* free(newptr); */

    /* log */
    mt_log();
}
```

Output:
```txt
-------------------------MT LOG-------------------------
[x] 000001D196107260   ( main.c:20(`main') - ... )
[v] 000001D196115570   ( main.c:9(`create_array') - main.c:18(`main') )
--------------------------------------------------------
```
