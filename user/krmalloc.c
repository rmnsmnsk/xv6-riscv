#include "kernel/types.h"
#include "user/user.h"

// The original xv6 K&R allocator, renamed so it can be tested next to SFL.
typedef long Align;

union kr_header {
  struct {
    union kr_header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union kr_header KrHeader;

static KrHeader base;
static KrHeader *freep;

void
kr_free(void *ap)
{
  KrHeader *bp, *p;

  bp = (KrHeader *)ap - 1;
  for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

static KrHeader *
morecore(uint nu)
{
  char *p;
  KrHeader *hp;

  if (nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(KrHeader));
  if (p == SBRK_ERROR)
    return 0;
  hp = (KrHeader *)p;
  hp->s.size = nu;
  kr_free((void *)(hp + 1));
  return freep;
}

void *
kr_malloc(uint nbytes)
{
  KrHeader *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(KrHeader) - 1) / sizeof(KrHeader) + 1;
  if ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    if (p->s.size >= nunits) {
      if (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      return (void *)(p + 1);
    }
    if (p == freep)
      if ((p = morecore(nunits)) == 0)
        return 0;
  }
}
