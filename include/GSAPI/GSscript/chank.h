#ifndef GSAPI_GSSCRIPT_CHANK_H
#define GSAPI_GSSCRIPT_CHANK_H

struct ChankHeader {
    char tag[4];
    unsigned long size;
    long id;
    unsigned long unknown;
};

class CChank {
public:
    char* data;
    char* end;
    int childCount;

    CChank();
    virtual ~CChank() {}
    ChankHeader* searchSubChankID(const char* tag);
    ChankHeader* searchChankID(const char* tag, long id);
};

class CChankBlock : public CChank {
public:
    virtual ~CChankBlock() {}
    bool loadFromMemory(char* buffer, char* expectedTag);
};

#endif
