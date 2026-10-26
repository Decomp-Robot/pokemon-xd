#include <GSAPI/GSscript/chank.h>
#include <string.h>

bool CChankBlock::loadFromMemory(char* buffer, char* expectedTag) {
    data = buffer;
    char* chunkBuffer = data;
    if (chunkBuffer == 0) {
        return true;
    }
    if (expectedTag != 0 && memcmp(chunkBuffer, expectedTag, 4) != 0) {
        return false;
    }
    childCount = 0;
    char* child = data + sizeof(ChankHeader);
    char* parseEnd = data + ((ChankHeader*)chunkBuffer)->size;
    end = parseEnd;
    while (parseEnd > child) {
        child += ((ChankHeader*)child)->size;
        childCount++;
    }
    return true;
}

ChankHeader* CChank::searchSubChankID(const char* tag) {
    if (data == 0) {
        return 0;
    }
    char* child = data + sizeof(ChankHeader);
    int index = 0;
    while (index < childCount) {
        if (memcmp(child, tag, 4) == 0) {
            return (ChankHeader*)child;
        }
        child += ((ChankHeader*)child)->size;
        index++;
    }
    return 0;
}

ChankHeader* CChank::searchChankID(const char* tag, long id) {
    if (data == 0) {
        return 0;
    }
    char* child = data + sizeof(ChankHeader);
    int index = 0;
    while (index < childCount) {
        if (((ChankHeader*)child)->id == id && memcmp(child, tag, 4) == 0) {
            return (ChankHeader*)child;
        }
        child += ((ChankHeader*)child)->size;
        index++;
    }
    return 0;
}

CChank::CChank() {
    data = 0;
}
