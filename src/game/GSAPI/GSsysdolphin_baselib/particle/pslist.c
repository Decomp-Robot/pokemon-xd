#include <GSAPI/GSsysdolphin_baselib/particle/particle.h>
#include <hsd/memory.h>
#include <hsd/assert.h>
#include <string.h>

static _psParticle *activeParticle[PS_NUM_LINK];
static int sortDirty[PS_NUM_LINK];
static _psParticle *activeParticleXLU[PS_NUM_LINK];
static _psParticle *freeParticle;

int _psLinkInit(int count)
{
    int i;
    _psParticle *pp;
    for (i = 0; i < PS_NUM_LINK; i++) {
        activeParticle[i] = NULL;
        sortDirty[i] = 0;
        activeParticleXLU[i] = NULL;
    }
    freeParticle = NULL;
    for (i = count - 1; i >= 0; i--) {
        pp = HSD_Alloc(sizeof(_psParticle));
        memset(pp, 0, sizeof(_psParticle));
        if (pp == NULL)
            return -1;
        pp->next = freeParticle;
        freeParticle = pp;
    }
    return i;
}

void _psListClear(void)
{
    _psParticle *pp, *next;
    int i;
    for (pp = freeParticle; pp != NULL; pp = next) {
        next = pp->next;
        HSD_Free(pp);
    }
    freeParticle = NULL;
    for (i = 0; i < PS_NUM_LINK; i++) {
        for (pp = activeParticle[i]; pp != NULL; pp = next) {
            next = pp->next;
            HSD_Free(pp);
        }
        activeParticle[i] = NULL;
        sortDirty[i] = 0;
        activeParticleXLU[i] = NULL;
    }
}

_psParticle *_psListNew(_psParticle *parent, int linkNo)
{
    _psParticle *pp;
    if (freeParticle == NULL) {
        freeParticle = HSD_Alloc(sizeof(_psParticle));
        memset(freeParticle, 0, sizeof(_psParticle));
    }
    pp = freeParticle;
    if (pp == NULL)
        return NULL;
    numActiveParticles++;
    if (numActiveParticles > numPeakParticles)
        numPeakParticles = numActiveParticles;
    freeParticle = pp->next;
    if (parent == NULL) {
        pp->next = activeParticle[linkNo];
        activeParticle[linkNo] = pp;
    } else {
        pp->next = parent->next;
        parent->next = pp;
    }
    sortDirty[linkNo] = 1;
    return pp;
}

void _psListDelete(_psParticle *pp, _psParticle *parent)
{
    sortDirty[pp->linkNo] = 1;
    if (parent == NULL) {
        HSD_ASSERTLINE(136, activeParticle[pp->linkNo] == pp);
        activeParticle[pp->linkNo] = pp->next;
    } else {
        HSD_ASSERTLINE(139, parent->next == pp);
        parent->next = pp->next;
    }
    pp->next = freeParticle;
    freeParticle = pp;
    numActiveParticles--;
}

_psParticle *_psListGetFirst(int linkNo)
{
    HSD_ASSERTLINE(152, linkNo >= 0 && linkNo < PS_NUM_LINK);
    return activeParticle[linkNo];
}

_psParticle *particleSort(int linkNo, _psParticle **zWriteHead,
                          _psParticle **xluHead)
{
    struct {
        _psParticle *head;
        _psParticle *tail;
    } bucket[2 * PS_SORT_ORDER_COUNT];
    _psParticle *pp = activeParticle[linkNo];
    _psParticle *next;
    _psParticle *head, *zHead, *xHead, *zTail, *xTail;
    int index, i;

    if (!sortDirty[linkNo]) {
        if (pp == NULL) {
            *zWriteHead = NULL;
            *xluHead = NULL;
            return NULL;
        }
        if (pp->flags & PS_FLAG_Z_WRITE)
            *zWriteHead = pp;
        else
            *zWriteHead = NULL;
        *xluHead = activeParticleXLU[linkNo];
        return pp;
    }
    sortDirty[linkNo] = 0;
    if (pp == NULL) {
        activeParticleXLU[linkNo] = NULL;
        *zWriteHead = NULL;
        *xluHead = NULL;
        return NULL;
    }
    /* Keep equal-order runs together, preserving their order within a bucket. */
    memset(bucket, 0, sizeof(bucket));
    index = ((pp->flags >> PS_SORT_ORDER_SHIFT) & (PS_SORT_ORDER_MASK >> PS_SORT_ORDER_SHIFT)) +
            ((((pp->flags & PS_FLAG_Z_WRITE) >> PS_Z_WRITE_SHIFT) ^ 1) * PS_SORT_ORDER_COUNT);
    bucket[index].head = pp;
    for (next = pp->next; next != NULL; next = next->next) {
        if ((pp->flags ^ next->flags) & (PS_SORT_ORDER_MASK | PS_FLAG_Z_WRITE)) {
            bucket[index].tail = pp;
            index = ((next->flags >> PS_SORT_ORDER_SHIFT) & (PS_SORT_ORDER_MASK >> PS_SORT_ORDER_SHIFT)) +
                    ((((next->flags & PS_FLAG_Z_WRITE) >> PS_Z_WRITE_SHIFT) ^ 1) * PS_SORT_ORDER_COUNT);
            if (bucket[index].head == NULL)
                bucket[index].head = next;
            else
                bucket[index].tail->next = next;
        }
        pp = next;
    }
    bucket[index].tail = pp;
    zTail = NULL;
    zHead = NULL;
    xTail = NULL;
    xHead = NULL;
    for (i = 0; i < PS_SORT_ORDER_COUNT; i++) {
        if (bucket[i].head != NULL) {
            if (zHead == NULL)
                zHead = bucket[i].head;
            else
                zTail->next = bucket[i].head;
            zTail = bucket[i].tail;
        }
    }
    for (i = PS_SORT_ORDER_COUNT; i < 2 * PS_SORT_ORDER_COUNT; i++) {
        if (bucket[i].head != NULL) {
            if (xHead == NULL)
                xHead = bucket[i].head;
            else
                xTail->next = bucket[i].head;
            xTail = bucket[i].tail;
        }
    }
    head = NULL;
    /* The Z-write group leads into the XLU suffix of the same list. */
    if (zTail != NULL) {
        head = zHead;
        zTail->next = xHead;
    }
    if (xTail != NULL) {
        if (head == NULL)
            head = xHead;
        xTail->next = NULL;
    }
    activeParticle[linkNo] = head;
    activeParticleXLU[linkNo] = xHead;
    *zWriteHead = zHead;
    *xluHead = xHead;
    return head;
}
