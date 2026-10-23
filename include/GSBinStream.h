#ifndef GS_BIN_STREAM_H
#define GS_BIN_STREAM_H

#include <types.h>
#include <GSmemory.h>
#include <types.h>
extern "C" void* memcpy(void*, const void*, unsigned int);

union GSBinaryScalar {
    unsigned long word;
    float single;
    double real;
};

class GSBinReadStream {
public:
    GSBinReadStream(void* data, unsigned long size) {
        mTop = mCurrent = static_cast<u8*>(data);
        mSize = size;
    }
    unsigned char readU8() { return *mCurrent++; }
    unsigned short readU16() {
        unsigned short high = mCurrent[0];
        unsigned short low = mCurrent[1];
        unsigned short value = (high << 8) | low;
        mCurrent += 2;
        return value;
    }
    unsigned long readU32() {
        unsigned long value = (mCurrent[0] << 24) | (mCurrent[1] << 16) |
                              (mCurrent[2] << 8) | mCurrent[3];
        mCurrent += 4;
        return value;
    }
    bool readBool() { return readU8() != 0; }
    float readFloat() {
        GSBinaryScalar value;
        value.word = readU32();
        return value.single;
    }
    void read(void* data, unsigned long size) {
        memcpy(data, mCurrent, size);
        mCurrent += size;
    }
    void skip(unsigned long size) { mCurrent += size; }
    unsigned long getReadSize() const { return mCurrent - mTop; }
    void seek(unsigned long offset) { mCurrent = mTop + offset; }
private:
    u8* mTop;
    u8* mCurrent;
    unsigned long mSize;
};

class GSBinWriteStream {
public:
    GSBinWriteStream() {
        mCurrent = 0;
        mAllocated = 0;
        mTop = 0;
        mSize = 0;
        mWrite = false;
    }
    GSBinWriteStream(void* data, unsigned long size) {
        mAllocated = 0;
        mTop = mCurrent = static_cast<u8*>(data);
        mSize = size;
        mWrite = true;
    }
    ~GSBinWriteStream() { if (mAllocated) GSmemFree(mAllocated); }
    void writeU8(unsigned char value) {
        if (!mWrite) { mCurrent++; return; }
        *mCurrent++ = value;
    }
    void writeS8(signed char value) { writeU8(value); }
    void writeBool(bool value) { writeS8(value ? 1 : 0); }
    void writeU16(unsigned short value) {
        if (!mWrite) { mCurrent += 2; return; }
        *mCurrent++ = value >> 8;
        *mCurrent++ = value;
    }
    void writeU32(unsigned long value) {
        if (!mWrite) { mCurrent += 4; return; }
        *mCurrent++ = value >> 24;
        *mCurrent++ = (value >> 16) & 0xff;
        *mCurrent++ = (value >> 8) & 0xff;
        *mCurrent++ = value & 0xff;
    }
    void writeFloat(float single) {
        GSBinaryScalar value;
        value.single = single;
        writeU32(value.word);
    }
    void write(const void* data, unsigned long size) {
        if (mWrite) memcpy(mCurrent, data, size);
        mCurrent += size;
    }
    unsigned long getWriteSize() const { return mCurrent - mTop; }
    void seek(unsigned long offset) { mCurrent = mTop + offset; }
protected:
    u8* mTop;
    u8* mCurrent;
    unsigned long mSize;
    void* mAllocated;
    bool mWrite;
};

class GSBinWriteSizeStream : public GSBinWriteStream {
public:
    GSBinWriteSizeStream() {
        mCurrent = 0;
        mAllocated = 0;
        mTop = 0;
        mSize = 0x10000000;
        mWrite = false;
    }
};
#endif
