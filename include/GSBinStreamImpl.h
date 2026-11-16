#ifndef GS_BIN_STREAM_IMPL_H
#define GS_BIN_STREAM_IMPL_H

#include <GSBinStream.h>
#include <types.h>
#include <GSmemory.h>
extern "C" void* memcpy(void*, const void*, unsigned int);

union GSBinaryScalar {
    unsigned long word;
    float single;
    double real;
};

inline unsigned long GSBinWriteStream::getWriteSize() const { return mCurrent - mTop; }
inline void GSBinWriteStream::writeFloat(float single) {
    GSBinaryScalar value;
    value.single = single;
    writeU32(value.word);
}
inline void GSBinWriteStream::writeU32(unsigned long value) {
    if (!mWrite) { mCurrent += 4; return; }
    *mCurrent++ = value >> 24;
    *mCurrent++ = value >> 16;
    *mCurrent++ = value >> 8;
    *mCurrent++ = value;
}
inline void GSBinWriteStream::writeBool(bool value) { writeS8(value ? 1 : 0); }
inline void GSBinWriteStream::writeS8(s8 value) { writeU8(value); }
inline void GSBinWriteStream::writeU8(u8 value) {
    if (!mWrite) { mCurrent++; return; }
    *mCurrent++ = value;
}
inline void GSBinWriteStream::writeU16(unsigned short value) {
    if (!mWrite) { mCurrent += 2; return; }
    *mCurrent++ = value >> 8;
    *mCurrent++ = value & 0xff;
}
inline GSBinWriteSizeStream::~GSBinWriteSizeStream() {}
inline GSBinWriteStream::~GSBinWriteStream() { if (mAllocated) GSmemFree(mAllocated); }
inline GSBinWriteSizeStream::GSBinWriteSizeStream() {
    mCurrent = 0;
    mAllocated = 0;
    mTop = 0;
    mSize = 268435456;
    mWrite = false;
}
inline GSBinWriteStream::GSBinWriteStream() {
    mCurrent = 0;
    mAllocated = 0;
    mTop = 0;
    mSize = 0;
    mWrite = false;
}
inline float GSBinReadStream::readFloat() {
    GSBinaryScalar value;
    value.word = readU32();
    return value.single;
}
inline unsigned long GSBinReadStream::readU32() {
    unsigned long value = (mCurrent[0] << 24) | (mCurrent[1] << 16) |
                          (mCurrent[2] << 8) | mCurrent[3];
    mCurrent += 4;
    return value;
}
inline bool GSBinReadStream::readBool() { return readU8() != 0; }
inline u8 GSBinReadStream::readU8() { return *mCurrent++; }
inline unsigned short GSBinReadStream::readU16() {
    unsigned short high = mCurrent[0];
    unsigned short low = mCurrent[1];
    unsigned short value = (high << 8) | low;
    mCurrent += 2;
    return value;
}
inline GSBinReadStream::GSBinReadStream(void* data, unsigned long size) {
    mTop = mCurrent = static_cast<u8*>(data);
    mSize = size;
}
inline GSBinWriteStream::GSBinWriteStream(void* data, unsigned long size) {
    mAllocated = 0;
    mTop = mCurrent = static_cast<u8*>(data);
    mSize = size;
    mWrite = true;
}

inline void GSBinReadStream::read(void* data, unsigned long size) {
    memcpy(data, mCurrent, size);
    mCurrent += size;
}
inline void GSBinReadStream::skip(unsigned long size) { mCurrent += size; }
inline unsigned long GSBinReadStream::getReadSize() const { return mCurrent - mTop; }
inline void GSBinReadStream::seek(unsigned long offset) { mCurrent = mTop + offset; }
inline void GSBinWriteStream::write(const void* data, unsigned long size) {
    if (mWrite) memcpy(mCurrent, data, size);
    mCurrent += size;
}
inline void GSBinWriteStream::seek(unsigned long offset) { mCurrent = mTop + offset; }
#endif
