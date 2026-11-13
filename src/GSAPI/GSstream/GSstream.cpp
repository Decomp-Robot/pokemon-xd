#include <GSstream.h>
#include <string.h>

extern "C" {
#include <stdio.h>
}

void GSstreamFileOutput::writePrintf(const char *format, ...)
{
    char buffer[512];
    va_list args;

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    writeString(buffer);
}

int GSstreamFileOutput::writeString(const char *string)
{
    long length = strlen(string);

    write(string, length);
    return 1;
}

int GSstreamFileOutput::write(const char *buffer, long size)
{
    if (GSdvdWrite(mFile, (void *)buffer, size, mPosition) < 0) {
        return 0;
    }
    mPosition += size;
    return 1;
}

bool GSstreamFileOutput::is_open() const
{
    return mFile != NULL;
}

bool GSstreamFileOutput::open(const char *fileName)
{
    if (mFile != NULL) {
        return false;
    }
    if (GSdvdOpenTest(fileName)) {
        if (GSdvdRemove(fileName) == 0) {
            return false;
        }
    }
    mFile = GSdvdCreate(fileName);
    mPosition = 0;
    return is_open();
}

GSstreamFileOutput::~GSstreamFileOutput()
{
    if (mFile != NULL) {
        GSdvdClose(mFile);
    }
}

GSstreamFileOutput::GSstreamFileOutput()
{
    mFile = NULL;
}
