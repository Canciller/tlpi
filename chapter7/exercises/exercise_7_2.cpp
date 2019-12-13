#include <cstdio>
#include <unistd.h>

#define PAGESIZE sysconf(_SC_PAGESIZE)

#define ALLOC_MIN 4 * PAGESIZE
#define DEALLOC_MIN 1 * PAGESIZE

struct blk_t
{
    //size_t alloc_size;
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

void print_blks(const char* header)
{
    printf("-------------------%s---------------------\n", header);

    blk_t *curr = tail;
    while(curr) {
        /*
        printf("*(%p) = { alloc_size: %zu (%zu + %zu), size: %zu, previous: %p, next: %p }\n",
                curr, curr->alloc_size, curr->alloc_size - curr->size, curr->size, curr->size,
                curr->previous, curr->next);
                */

        printf("*(%p) = { size: %zu, previous: %p, next: %p }\n",
                curr, curr->size, curr->previous, curr->next);

        curr = curr->next;
    }

    printf("END\n\n");
}

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
    //blk->alloc_size = alloc_size;
    blk->size = alloc_size - sizeof(blk_t);

    if(alloc_size == required_size)
        return NULL;

    size_t size = required_size - sizeof(blk_t),
           remaining_size = alloc_size - required_size;

    if(remaining_size <= sizeof(blk_t))
        return NULL;

    blk_t *new_blk = BLK(PTR(blk) + required_size);

    //blk->alloc_size = required_size;
    blk->size = required_size - sizeof(blk_t);

    //new_blk->alloc_size = remaining_size;
    new_blk->size = remaining_size - sizeof(blk_t);

    return new_blk;
}

void merge_blks()
{
    print_blks("[free] after");

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

            curr->size += next->size + sizeof(blk_t);
            //curr->alloc_size += next->alloc_size;

            continue;
        }

        curr = curr->next;
    }

    print_blks("[free] before");
}

void free(void *ptr)
{
    add_blk(BLK_PART(ptr));
    merge_blks();
}

void *malloc(size_t size)
{
    print_blks("[malloc] before");

    size_t required_size = size + sizeof(blk_t),
           alloc_size = (size > ALLOC_MIN ? size : ALLOC_MIN) + sizeof(blk_t);

    blk_t *blk = NULL,
          *new_blk = NULL,
          *curr = tail;

    while(curr) {
        if(curr->size >= size) {
            remove_blk(curr);

            new_blk = resize_blk(curr, curr->size + sizeof(blk_t), required_size);
            add_blk(new_blk);

            print_blks("[malloc] list - after");

            return MEM_PART(curr);
        }

        curr = curr->next;
    }

    blk = BLK(sbrk(alloc_size));
    if(blk == BLK(-1))
        return NULL;

    new_blk = resize_blk(blk, alloc_size, required_size);
    add_blk(new_blk);

    print_blks("[malloc] sbrk - after");

    return MEM_PART(blk);
}

int main(int argc, char *argv[])
{
    void *p1 = malloc(PAGESIZE);
    void *p2 = malloc(3 * PAGESIZE);
    free(p2);
    free(p1);
    free(malloc(ALLOC_MIN));

    return 0;
}
