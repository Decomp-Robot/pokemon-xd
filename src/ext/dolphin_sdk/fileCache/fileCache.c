#include <dolphin/os.h>
#include <charPipeline/fileCache.h>

DSCache DODisplayCache;
u8 DOCacheInitialized;

static u8 AllocCacheNode(DSCacheNodePtr* cacheNode, char* name);
static void FreeCacheNode(DSCacheNodePtr* cacheNode);

// UNUSED: the game never calls this; dead-stripped at link.
DSCacheNodePtr DSAddCacheNode(DSCachePtr cache, char* name, Ptr data, Ptr OSFreeFunc) {
    DSCacheNodePtr cacheNode;

    cacheNode = NULL;
    if (!AllocCacheNode(&cacheNode, name)) {
        return NULL;
    }
    Strcpy(cacheNode->Name, name);
    cacheNode->Data = data;
    cacheNode->Free = (void (*)(Ptr *))OSFreeFunc;
    cacheNode->ReferenceCount = 0;
    DSInsertListObject(&cache->CacheNodeList, NULL, (Ptr)cacheNode);
    return cacheNode;
}

// UNUSED: the game never calls this; dead-stripped at link.
static u8 AllocCacheNode(DSCacheNodePtr* cacheNode, char* name) {
    if (*cacheNode) {
        FreeCacheNode(cacheNode);
    }

    *cacheNode = OSAlloc(sizeof(DSCacheNode));
    if (!*cacheNode) {
        return FALSE;
    }

    (*cacheNode)->Name = OSAlloc(Strlen(name) + 1);
    if (!(*cacheNode)->Name) {
        return FALSE;
    }

    return TRUE;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
static DSCacheNodePtr FindCacheNode(DSCachePtr cache, char* name, Ptr data) {
    DSCacheNodePtr cacheNode;

    cacheNode = (DSCacheNodePtr)cache->CacheNodeList.Head;
    if (data) {
        while (cacheNode) {
            if (data == cacheNode->Data) {
                return cacheNode;
            }
            cacheNode = (DSCacheNodePtr)cacheNode->Link.Next;
        }
    } else if (name) {
        while (cacheNode) {
            if (Strcmp(name, cacheNode->Name) == 0) {
                return cacheNode;
            }
            cacheNode = (DSCacheNodePtr)cacheNode->Link.Next;
        }
    }
    return NULL;
}

// UNUSED: the game never calls this; dead-stripped at link.
Ptr DSGetCacheObj(DSCachePtr cache, char* name) {
    DSCacheNodePtr cacheNode;

    cacheNode = FindCacheNode(cache, name, NULL);
    if (cacheNode) {
        cacheNode->ReferenceCount++;
        return cacheNode->Data;
    }
    return NULL;
}

// UNUSED: the game never calls this; dead-stripped at link.
static void FreeCacheNode(DSCacheNodePtr* cacheNode) {
    if (*cacheNode) {
        if ((*cacheNode)->Free) {
            (*cacheNode)->Free(&(*cacheNode)->Data);
        }
        OSFree((*cacheNode)->Name);
        OSFree(*cacheNode);
        *cacheNode = NULL;
    }
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
