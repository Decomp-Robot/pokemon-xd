#include <charPipeline/structures/List.h>

// UNUSED: the game never calls this; dead-stripped at link.
void DSInitList(DSListPtr list, Ptr obj, DSLinkPtr link) {
    list->Head = NULL;
    list->Tail = NULL;
    list->Offset = (Ptr)link - obj;
}

// UNUSED: the game never calls this; dead-stripped at link.
void DSInsertListObject(DSListPtr list, Ptr cursor, Ptr obj) {
    DSLinkPtr link;
    DSLinkPtr linkNext;
    DSLinkPtr linkPrev;

    link = (DSLinkPtr)(obj + list->Offset);
    if (list->Head) {
        if (!cursor) {
            linkPrev = (DSLinkPtr)(list->Tail + list->Offset);
            linkPrev->Next = obj;
            link->Prev = list->Tail;
            link->Next = NULL;
            list->Tail = obj;
        } else {
            linkNext = (DSLinkPtr)(cursor + list->Offset);
            if (cursor == list->Head) {
                list->Head = obj;
                link->Next = cursor;
                linkNext->Prev = obj;
            } else {
                linkPrev = (DSLinkPtr)(linkNext->Prev + list->Offset);
                link->Next = cursor;
                link->Prev = linkNext->Prev;
                linkNext->Prev = obj;
                linkPrev->Next = obj;
            }
        }
    } else {
        list->Tail = obj;
        list->Head = obj;
        link->Next = link->Prev = NULL;
    }
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
void* DSNextListObj(DSListPtr list, Ptr obj) {
    if (!list) {
        return NULL;
    }
    if (!obj) {
        return list->Head;
    }

    return ((DSLinkPtr)(obj + list->Offset))->Next;
}
