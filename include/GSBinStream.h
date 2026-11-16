#ifndef GS_BIN_STREAM_H
#define GS_BIN_STREAM_H

#include <types.h>
#include <GSmemory.h>

class GSBinReadStream {
public:
    GSBinReadStream(void* data, unsigned long size);
    u8 readU8();
    unsigned short readU16();
    unsigned long readU32();
    bool readBool();
    float readFloat();
    void read(void* data, unsigned long size);
    void skip(unsigned long size);
    unsigned long getReadSize() const;
    void seek(unsigned long offset);
private:
    u8* mTop;
    u8* mCurrent;
    unsigned long mSize;
};

class GSBinWriteStream {
public:
    GSBinWriteStream();
    GSBinWriteStream(void* data, unsigned long size);
    ~GSBinWriteStream();
    void writeU8(u8 value);
    void writeS8(s8 value);
    void writeBool(bool value);
    void writeU16(unsigned short value);
    void writeU32(unsigned long value);
    void writeFloat(float single);
    void write(const void* data, unsigned long size);
    unsigned long getWriteSize() const;
    void seek(unsigned long offset);
protected:
    u8* mTop;
    u8* mCurrent;
    unsigned long mSize;
    void* mAllocated;
    bool mWrite;
};

class GSBinWriteSizeStream : public GSBinWriteStream {
public:
    GSBinWriteSizeStream();
    ~GSBinWriteSizeStream();
};

#endif
