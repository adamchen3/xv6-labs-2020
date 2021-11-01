// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

void freerange(void *pa_start, void *pa_end);
void kfree2(void *, int);
void *stealmemory(int);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

int 
mycpuid()
{
  int id;
  push_off();
  id = cpuid();
  pop_off();
  return id;
}

void
kinit()
{
  // initlock(&kmem.lock, "kmem");
  for (int i = 0; i < NCPU; i++) {
    initlock(&cpus[i].lock, "kmem");
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);

  int id = 0;
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    if (id >= NCPU) {
      id = 0;
    }
    kfree2(p, id);
    id++;
    // kfree(p);
  }
}

void kfree2(void *pa, int cpuid)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
  struct cpu *cpu = &cpus[cpuid];

  acquire(&cpu->lock);
  r->next = cpu->freelist;
  cpu->freelist = r;
  release(&cpu->lock);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  int cpuid = mycpuid();
  kfree2(pa, cpuid);
  // struct run *r;

  // if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
  //   panic("kfree");

  // // Fill with junk to catch dangling refs.
  // memset(pa, 1, PGSIZE);

  // r = (struct run*)pa;

  // acquire(&kmem.lock);
  // r->next = kmem.freelist;
  // kmem.freelist = r;
  // release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  int cpuid = mycpuid();

  struct cpu *cpu = &cpus[cpuid];
  acquire(&cpu->lock);
  r = cpu->freelist;
  if(r)
    cpu->freelist = r->next;
  release(&cpu->lock);

  if (!r)
    // stealing memory from other cpu
    r = stealmemory(cpuid);

  // acquire(&kmem.lock);
  // r = kmem.freelist;
  // if(r)
  //   kmem.freelist = r->next;
  // release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

void *
stealmemory(int cpuid)
{
  int id = cpuid + 1;
  struct run *r = 0;
  for (int i = 0; i < NCPU; i++) {
    if (id >= NCPU) {
      id = 0;
    }

    if (cpuid == id) {
      break;
    }

    struct cpu *cpu = &cpus[id];

    acquire(&cpu->lock);
    r = cpu->freelist;
    if (r) {
      cpu->freelist = r->next;
      release(&cpu->lock);
      break;
    }
    release(&cpu->lock);

    id++;
  }
  return (void *)r;
}