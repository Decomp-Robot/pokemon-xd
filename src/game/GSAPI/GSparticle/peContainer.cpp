#include <peContainer.h>
#include <peEmitter.h>
#include <peBank.h>
#include <particleEngine.h>
#include <GSmem.h>
#include <dolphin/os.h>
#include <string.h>

namespace {
enum {
    PE_RENDER_BUFFER_ALIGNMENT = 32,
    PE_RENDER_UNUSED_INDEX_BYTE = 0xFF
};
}

peContainer::peContainer(peEmitterNode *parent, unsigned long nodeSize, u16 maxNodes)
{
    emitter = parent;
    nextContainer = 0;
    nodeAllocation = 0;
    freeNodes = 0;
    nodes = 0;
    nodeCount = 0;
    capacity = maxNodes;
    changed = false;
    started = false;
    bool separateTransforms = (emitter->emitter->flags & PE_EMITTER_LOCAL_VELOCITY) != 0;
    u32 size = maxNodes * nodeSize;
    if (separateTransforms)
        size += maxNodes * sizeof(peNodeTransform);
    nodeAllocation = GSmemAllocClr(size);
    if (!nodeAllocation)
        return;
    size = GSmemGetSize(nodeAllocation);
    g_particle_debug_stats.containerBytes += size;
    g_particle_debug_stats.totalBytes += size;
    freeNodes = (peNode *)nodeAllocation;
    peNode *node;
    char *cursor = (char *)freeNodes;
    peNode *previous = 0;
    for (u32 i = 0; i < maxNodes; ++i) {
        node = (peNode *)cursor;
        cursor += nodeSize;
        if (separateTransforms) {
            node->transform = (peNodeTransform *)cursor;
            cursor += sizeof(peNodeTransform);
        } else {
            node->transform = &parent->mWorldTransform;
        }
        node->previous = previous;
        if (previous)
            previous->next = node;
        previous = node;
    }
    changed = true;
}

peContainer::~peContainer()
{
    if (nodeAllocation) {
        u32 size = GSmemGetSize(nodeAllocation);
        g_particle_debug_stats.containerBytes -= size;
        g_particle_debug_stats.totalBytes -= size;
        GSmemFree(nodeAllocation);
        nodeAllocation = 0;
    }
}

void peContainer::Start()
{
    if (started)
        return;
    started = true;
}

void peContainer::PreUpdate()
{
}

void peContainer::SetLayerVisibility(GSgfxLayerID id, bool visible)
{
    GSgfxLayer *layer = GSgfxLayerFindByID(id);
    if (layer)
        layer->SetLayerVisibility(visible);
}

u16 peContainer::CreateNodes(u16 count)
{
    peNode *first = freeNodes;
    if (!first)
        return 0;
    peNode *last = first;
    u16 created = 1;
    while (last->next && created < count) {
        last = last->next;
        ++created;
    }
    if (last->next)
        last->next->previous = 0;
    freeNodes = last->next;
    peNode *activeHead = nodes;
    last->next = activeHead;
    if (activeHead)
        activeHead->previous = last;
    first->previous = 0;
    nodes = first;
    nodeCount += created;
    changed = true;
    return created;
}

void peContainer::RemoveNode(peNode *node)
{
    peNode *next = node->next;
    peNode *previous = node->previous;
    if (next)
        next->previous = previous;
    if (previous)
        previous->next = next;
    else
        nodes = next;
    node->previous = 0;
    peNode *freeHead = freeNodes;
    node->next = freeHead;
    if (freeHead)
        freeHead->previous = node;
    freeNodes = node;
    --nodeCount;
    changed = true;
}

void peContainer::RemoveAll()
{
    peNode *tail = nodes;
    if (!tail)
        return;
    while (tail->next)
        tail = tail->next;
    peNode *freeHead = freeNodes;
    tail->next = freeHead;
    if (freeHead)
        freeHead->previous = tail;
    freeNodes = nodes;
    nodes = 0;
    nodeCount = 0;
    changed = true;
}

void peContainer::MoveNode(peNode *node, peNode *before)
{
    peNode *next = node->next;
    peNode *previous = node->previous;
    if (next)
        next->previous = previous;
    if (previous)
        previous->next = next;
    else
        nodes = next;
    peNode *insertionPrevious = before->previous;
    before->previous = node;
    node->next = before;
    node->previous = insertionPrevious;
    if (insertionPrevious)
        insertionPrevious->next = node;
    else
        nodes = node;
}

peRenderContainer::peRenderContainer(peEmitterNode *parent, unsigned long nodeSize, u16 maxNodes,
                                     GSgfxVF *format, GSgfxPrimType primitive, u8 vertices,
                                     u8 renderStride, bool doubleBuffer)
    : peContainer(parent, nodeSize, maxNodes)
{
    mSwapState = false;
    mBufferIndex = 0;
    mRenderDataSize = 0;
    u32 dataSize = maxNodes * renderStride;
    memset(&mDisplayLists[0], 0, sizeof(GSgfxDL));
    memset(&mDisplayLists[1], 0, sizeof(GSgfxDL));
    mRenderData[0] = 0;
    mRenderData[1] = 0;
    mRenderTokens[0] = 0;
    mRenderTokens[1] = 0;
    u32 bufferSize = (dataSize + PE_RENDER_BUFFER_ALIGNMENT - 1) & ~(PE_RENDER_BUFFER_ALIGNMENT - 1);
    char *data = (char *)GSmemAllocAlignClr(doubleBuffer ? bufferSize * 2 : bufferSize, PE_RENDER_BUFFER_ALIGNMENT);
    if (!data)
        return;
    GSgfxDLCreate(&mDisplayLists[0], primitive, format, maxNodes * vertices);
    if ((mDisplayLists[0].mFlags & GS_DISPLAY_LIST_ALLOCATED) != GS_DISPLAY_LIST_ALLOCATED) {
        if (data)
            GSmemFree(data);
        return;
    }
    if (doubleBuffer)
        GSgfxDLCreate(&mDisplayLists[1], primitive, format, maxNodes * vertices);
    u32 size = GSmemGetSize(data);
    g_particle_debug_stats.nodeBytes += mDisplayLists[0].mSize + mDisplayLists[1].mSize;
    g_particle_debug_stats.renderDataBytes += size;
    g_particle_debug_stats.totalBytes += size + mDisplayLists[0].mSize + mDisplayLists[1].mSize;
    if ((mDisplayLists[1].mFlags & GS_DISPLAY_LIST_ALLOCATED) == GS_DISPLAY_LIST_ALLOCATED) {
        DCFlushRange(data, ((dataSize + PE_RENDER_BUFFER_ALIGNMENT - 1) & ~(PE_RENDER_BUFFER_ALIGNMENT - 1)) * 2);
        data = (char *)OSCachedToUncached(data);
        mRenderData[0] = data;
        mRenderData[1] = data + bufferSize;
    } else {
        DCFlushRange(data, dataSize);
        mRenderData[0] = (char *)OSCachedToUncached(data);
    }
    mRenderDataSize = dataSize;
    char *vertexData = mDisplayLists[0].VertexData(0);
    memset(vertexData, PE_RENDER_UNUSED_INDEX_BYTE, maxNodes * (vertices * mDisplayLists[0].VertexStride()));
    mDisplayLists[0].Invalidate();
    if ((mDisplayLists[1].mFlags & GS_DISPLAY_LIST_ALLOCATED) == GS_DISPLAY_LIST_ALLOCATED) {
        vertexData = mDisplayLists[1].VertexData(0);
        memset(vertexData, PE_RENDER_UNUSED_INDEX_BYTE, maxNodes * (vertices * mDisplayLists[1].VertexStride()));
        mDisplayLists[1].Invalidate();
    }
}

peRenderContainer::~peRenderContainer()
{
    if ((mDisplayLists[0].mFlags & GS_DISPLAY_LIST_ALLOCATED) == GS_DISPLAY_LIST_ALLOCATED) {
        g_particle_debug_stats.nodeBytes -= mDisplayLists[0].mSize;
        g_particle_debug_stats.totalBytes -= mDisplayLists[0].mSize;
        GSgfxDLFree(&mDisplayLists[0]);
        if ((mDisplayLists[1].mFlags & GS_DISPLAY_LIST_ALLOCATED) == GS_DISPLAY_LIST_ALLOCATED) {
            g_particle_debug_stats.nodeBytes -= mDisplayLists[1].mSize;
            g_particle_debug_stats.totalBytes -= mDisplayLists[1].mSize;
            GSgfxDLFree(&mDisplayLists[1]);
        }
        if (mRenderData[0]) {
            mRenderData[0] = (char *)OSUncachedToCached(mRenderData[0]);
            u32 size = GSmemGetSize(mRenderData[0]);
            g_particle_debug_stats.renderDataBytes -= size;
            g_particle_debug_stats.totalBytes -= size;
            if (mRenderData[0]) {
                GSmemFree(mRenderData[0]);
                mRenderData[0] = 0;
            }
        }
    }
}

void peRenderContainer::PreUpdate()
{
    if (started && (mDisplayLists[0].mFlags & GS_DISPLAY_LIST_ALLOCATED) == GS_DISPLAY_LIST_ALLOCATED) {
        if (mRenderTokens[mBufferIndex]) {
            GSgfxTokenWaitForClear(mRenderTokens[mBufferIndex]);
            mRenderTokens[mBufferIndex] = 0;
        }
        if ((mDisplayLists[1].mFlags & GS_DISPLAY_LIST_ALLOCATED) == GS_DISPLAY_LIST_ALLOCATED && mSwapState) {
            mSwapState = false;
            if (changed) {
                memcpy(mDisplayLists[mBufferIndex].mData,
                       mDisplayLists[!mBufferIndex].mData, mDisplayLists[0].mSize);
                mDisplayLists[mBufferIndex].Invalidate();
                changed = false;
            }
        }
    }
}

void peRenderContainer::MarkForSwap()
{
    if ((mDisplayLists[0].mFlags & GS_DISPLAY_LIST_ALLOCATED) == GS_DISPLAY_LIST_ALLOCATED) {
        g_bank_token = g_current_layer->tokenMark();
        mRenderTokens[mBufferIndex] = g_bank_token;
        if ((mDisplayLists[1].mFlags & GS_DISPLAY_LIST_ALLOCATED) == GS_DISPLAY_LIST_ALLOCATED) {
            mBufferIndex ^= 1;
            mSwapState = true;
        }
    }
}

void peRenderContainer::ResolveMultipleSwap()
{
    if ((mDisplayLists[0].mFlags & GS_DISPLAY_LIST_ALLOCATED) == GS_DISPLAY_LIST_ALLOCATED &&
        (mDisplayLists[1].mFlags & GS_DISPLAY_LIST_ALLOCATED) == GS_DISPLAY_LIST_ALLOCATED &&
        mSwapState) {
        mBufferIndex ^= 1;
        mSwapState = false;
    }
}

void peRenderContainer::InitSwap()
{
    if ((mDisplayLists[0].mFlags & GS_DISPLAY_LIST_ALLOCATED) == GS_DISPLAY_LIST_ALLOCATED) {
        mRenderTokens[0] = 0;
        mDisplayLists[0].Invalidate();
        if ((mDisplayLists[1].mFlags & GS_DISPLAY_LIST_ALLOCATED) == GS_DISPLAY_LIST_ALLOCATED) {
            mRenderTokens[1] = 0;
            mDisplayLists[1].Invalidate();
        }
    }
}

void peRenderContainer::ResetSwap()
{
}
