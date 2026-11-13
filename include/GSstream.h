#ifndef GSSTREAM_H
#define GSSTREAM_H

#include <types.h>
#include <GSdvd.h>

/*
 * Text stream writing into a file of the GSdvd layer. A stream owns at most
 * one file and writes at its own byte position, so text accumulates from the
 * beginning of the file. open() replaces an existing file of the same name,
 * so a stream that has just been opened always starts on an empty file.
 */
class GSstreamFileOutput {
public:
    GSstreamFileOutput();
    ~GSstreamFileOutput();

    bool open(const char *fileName);
    bool is_open() const;
    int write(const char *buffer, long size);
    int writeString(const char *string);
    void writePrintf(const char *format, ...);

private:
    GSdvdFile *mFile;
    long mPosition;
};

#endif
