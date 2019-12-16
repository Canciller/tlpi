#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <unistd.h>

#define PAGESIZE sysconf(_SC_PAGESIZE)

#define ALLOC_MIN 3 * PAGESIZE
#define DEALLOC_MIN 1 * PAGESIZE

#define DEBUG

struct blk_t
{
#ifdef DEBUG
    size_t alloc_size;
#endif
    size_t size;
    blk_t *previous,
          *next;
};

blk_t *head = NULL,
      *tail = NULL;

#define PTR(ptr) (char *)(ptr)
#define BLK(ptr) (blk_t *)(ptr)

#define BLK_PART(ptr) BLK(PTR(ptr) - sizeof(blk_t))
#define MEM_PART(blk) PTR(blk) + sizeof(blk_t)

#ifdef DEBUG
void print_blk(blk_t *blk)
{
    if(!blk) {
        printf("(nil)\n");
        return;
    };

    printf("*(%p) = { alloc_size: %zu (%zu + %zu), size: %zu, previous: %p, next: %p, end: %p }\n",
            blk, blk->alloc_size, blk->alloc_size - blk->size, blk->size, blk->size,
            blk->previous, blk->next, MEM_PART(blk) + blk->size);
}

void print_blks(const char *format, ...) {

    static size_t count = 0;
    static char buff[512] = {};

    sprintf(buff, "START %04zu - %s\n", ++count, format);

    va_list args;
    va_start(args, format);
    vprintf(buff, args);
    va_end(args);

    printf("sbrk(0) = %p\n", sbrk(0));
    printf("tail = ");
    print_blk(tail);
    printf("head = ");
    print_blk(head);
    blk_t *curr = tail;
    while(curr) {
        print_blk(curr);
        curr = curr->next;
    }

    printf("END\n\n");
}
#else
#define print_blks(...)
#endif

void add_blk(blk_t *blk)
{
    if(blk == NULL)
        return;

    if(head == NULL) {
        head = blk;
        head->previous = NULL;
        head->next = NULL;
        tail = head;

        return;
    }

    if(blk > head) {
        head->next = blk;
        blk->next = NULL;
        blk->previous = head;
        head = blk;

        return;
    }

    if(blk < tail) {
        tail->previous = blk;
        blk->next = tail;
        blk->previous = NULL;
        tail = blk;

        return;
    }

    blk_t *curr = tail;

    while(curr) {
        if(blk > curr && blk < curr->next) {
            blk->previous = curr;
            blk->next = curr->next;

            if(curr->next)
                curr->next->previous = blk;

            curr->next = blk;

            return;
        }

        curr = curr->next;
    }
}

void remove_blk(blk_t *blk)
{
    if(blk == head) {
        if(head->previous)
            head->previous->next = NULL;

        head = head->previous;

        if(blk == tail)
            tail = head;

        return;
    }

    if(blk == tail) {
        if(tail->next)
            tail->next->previous = NULL;

        tail = tail->next;

        return;
    }

    blk_t *curr = tail;
    while(curr) {
        if(curr == blk) {
            if(curr->previous)
                curr->previous->next = curr->next;
            if(curr->next)
                curr->next->previous = curr->previous;

            return;
        }

        curr = curr->next;
    }
}

blk_t* resize_blk(blk_t *blk, size_t alloc_size, size_t required_size)
{
#ifdef DEBUG
    blk->alloc_size = alloc_size;
#endif
    blk->size = alloc_size - sizeof(blk_t);

    if(alloc_size == required_size)
        return NULL;

    size_t size = required_size - sizeof(blk_t),
           remaining_size = alloc_size - required_size;

    if(remaining_size <= sizeof(blk_t))
        return NULL;

    blk_t *new_blk = BLK(PTR(blk) + required_size);

#ifdef DEBUG
    blk->alloc_size = required_size;
    new_blk->alloc_size = remaining_size;
#endif

    blk->size = required_size - sizeof(blk_t);
    new_blk->size = remaining_size - sizeof(blk_t);

    return new_blk;
}

void merge_blks()
{
    blk_t *curr = tail,
          *next = NULL;

    while(curr) {
        next = BLK(MEM_PART(curr) + curr->size);
        if(next == curr->next) {
            curr->next = next->next;

            if(curr->next)
                curr->next->previous = curr;
            else
                head = curr;

#ifdef DEBUG
            curr->alloc_size += next->alloc_size;
#endif
            curr->size += next->size + sizeof(blk_t);

            continue;
        }

        curr = curr->next;
    }

    // return memory to os

    char *pb = PTR(sbrk(0));
    if(pb == PTR(-1))
        return;

    if(pb == MEM_PART(head) + head->size) {
        if(head->size <= DEALLOC_MIN) {
            printf("[free] returning memory to os\n\n");
            if(sbrk(-head->size - sizeof(blk_t)) == (void *)(-1))
                return;

            if(head == tail) {
                head = tail = NULL;
                return;
            }

            if(head->previous) {
                head->previous->next = NULL;
                head = head->previous;
                return;
            }
        }
    }
}

void free(void *ptr)
{
    if(ptr == NULL)
        return;

    blk_t *blk = BLK_PART(ptr);


    add_blk(blk);

    print_blks("free(%p) before", blk);

    merge_blks();

    print_blks("free(%p) after", blk);
}

void *malloc(size_t size)
{
    size_t required_size = size + sizeof(blk_t),
           alloc_size = (size > ALLOC_MIN ? required_size : ALLOC_MIN + sizeof(blk_t));

    blk_t *blk = NULL,
          *new_blk = NULL,
          *curr = tail;

    while(curr) {
        if(curr->size >= size) {
            print_blks("malloc(%zu) before", size);

            remove_blk(curr);
            new_blk = resize_blk(curr, curr->size + sizeof(blk_t), required_size);
            add_blk(new_blk);

            print_blks("malloc(%zu) after", size);
            printf("malloc(%zu) return %p\n\n", size, curr);

            return MEM_PART(curr);
        }

        curr = curr->next;
    }

    printf("[malloc] sbrk(0) = %p\n\n", sbrk(0));

    blk = BLK(sbrk(alloc_size));
    if(blk == BLK(-1))
        return NULL;

    print_blks("malloc(%zu) before, using sbrk", size);

    new_blk = resize_blk(blk, alloc_size, required_size);
    add_blk(new_blk);

    print_blks("malloc(%zu) after, using sbrk", size);
    printf("malloc(%zu) return %p\n\n", size, blk);

    return MEM_PART(blk);
}

int main(int argc, char *argv[])
{
    printf("ALLOC_MIN: %zu\n", ALLOC_MIN);
    printf("DEALLOC_MIN: %zu\n\n", DEALLOC_MIN);

    void *p1 = malloc(1);
    char *str = (char *) malloc(10);

    free(p1);
    free(str);

    void *p2 = malloc(20);
    void *p3 = malloc(20);
    void *p4 = malloc(100);

    free(p3);
    free(p4);
    free(p2);

    free(malloc(1));

    void *p5 = malloc(2 * PAGESIZE);
    void *p6 = malloc(ALLOC_MIN);
    free(p5);
    free(p6);

    void *ptr[10] = {};
    for(size_t i = 0; i < 10; ++i) {
        ptr[i] = malloc(PAGESIZE);
    }

    for(int i = 9; i >= 0; --i) {
        free(ptr[i]);
    }

    return 0;
}
