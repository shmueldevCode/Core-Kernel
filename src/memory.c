#include <stdint.h>
#include <stddef.h>
#include <core.h>

struct limine_hhdm_response { uint64_t revision; uint64_t offset; };
struct limine_hhdm_request { uint64_t id[4]; uint64_t revision; struct limine_hhdm_response *response; };
struct limine_memmap_entry { uint64_t base; uint64_t length; uint64_t type; };
struct limine_memmap_response { uint64_t revision; uint64_t entry_count; struct limine_memmap_entry **entries; };
struct limine_memmap_request { uint64_t id[4]; uint64_t revision; struct limine_memmap_response *response;};

#define LIMINE_MAGIC 0xc7b1dd30df4c8b88, 0x0a82e883a194f07b

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_req = { .id = { LIMINE_MAGIC, 0x48dcf1cb8ad2b852, 0x63984e959a98244b},};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_req = { .id = { LIMINE_MAGIC, 0x67cf3d9d378a806f, 0xe304acdfc50c3c62}, };

#define MEM_USABLE 0
#define PAGE_SIZE  4096
#define HEAP_SIZE  (4 * 1024 * 1024)

void *memset(void *d, int c, size_t n);

static uint64_t hhdm;
static uint64_t total_bytes;

struct free_page {
    struct free_page *next;
};

static struct free_page *free_list = NULL;
static uint64_t free_pages = 0;

static uint8_t *heap_start;
static uint64_t heap_size;

static void page_free(void *ptr)
{
    struct free_page *p = ptr;
    p->next = free_list;
    free_list = p;
    free_pages++;
}

static void *page_alloc(void)
{
    if (!free_list) return NULL;
    struct free_page *p = free_list;
    free_list = p->next;
    free_pages--;
    memset(p, 0, PAGE_SIZE);
    return p;
}

struct block{
    uint64_t size;
    uint64_t free;
    struct block *next;
    uint64_t pad;
};

#define ALIGN16(x) (((x) + 15) & ~(uint64_t)15)

static struct block *heap_head = NULL;

static void heap_init(void)
{
    if (!heap_start) return;
    heap_head = (struct block *)heap_start;
    heap_head->size = heap_size - sizeof(struct block);
    heap_head->free = 1;
    heap_head->next = NULL;
}

static void *mem_alloc(uint64_t size)
{
    if (size == 0 || !heap_head) return NULL;
    size = ALIGN16(size);

    for (struct block *b = heap_head; b; b = b->next)
    {
        if (!b->free || b->size < size) continue;

        if (b->size >=size + sizeof(struct block) + 16)
        {
            struct block *n = (struct block *)((uint8_t *)(b + 1) + size);
            n->size = b->size - size - sizeof(struct block);
            n->free = 1;
            n->next = b->next;
            b->size = size;
            b->next = n;
        }
        b->free = 0;
        return b + 1;
    }
    return NULL;
}

static void mem_free(void *ptr)
{
    if (!ptr) return;
    struct block *b = (struct block *)ptr -1;
    b->free = 1;

    for (struct block *c = heap_head; c; c = c->next)
        while (c->free && c->next && c->next->free)
        {
            c->size += sizeof(struct block) + c->next->size;
            c->next = c->next->next;
        }
}

void memory_init(void)
{
    if (!hhdm_req.response || !memmap_req.response) return;

    hhdm = hhdm_req.response->offset;
    struct limine_memmap_response *mm = memmap_req.response;

    struct limine_memmap_entry *biggest = NULL;
    for (uint64_t i = 0; i < mm->entry_count; i++) {
        struct limine_memmap_entry *e = mm->entries[i];
        if (e->type != MEM_USABLE) continue;
        total_bytes += e->length;
        if (!biggest || e->length > biggest->length)
            biggest = e;
    }

    for (uint64_t i = 0; i < mm->entry_count; i++) {
        struct limine_memmap_entry *e = mm->entries[i];
        if (e->type != MEM_USABLE) continue;

        uint64_t start = (e->base + PAGE_SIZE - 1) & ~(uint64_t)(PAGE_SIZE - 1);
        uint64_t end   = (e->base + e->length)     & ~(uint64_t)(PAGE_SIZE - 1);

        if (e == biggest && end - start > HEAP_SIZE) {
            heap_start = (uint8_t *)(start + hhdm);
            heap_size  = HEAP_SIZE;
            start += HEAP_SIZE;
        }

        for (uint64_t a = start; a < end; a += PAGE_SIZE)
            page_free((void *)(a + hhdm));
        
    }
    heap_init();
}

static uint64_t memory_total(void)     { return total_bytes; }
static uint64_t memory_available(void) { return free_pages * PAGE_SIZE; }

CoreMemory Memory = {
    .alloc      = mem_alloc,
    .free       = mem_free,
    .total      = memory_total,
    .available  = memory_available,
    .page_alloc = page_alloc,
    .page_free  = page_free,
};