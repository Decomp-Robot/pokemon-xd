#include "scanf.h"

int __StringRead(void *context, int character, int operation)
{
    MSLStringInput *input = (MSLStringInput *)context;

    switch (operation) {
    case MSL_INPUT_READ: {
        int value = *(const unsigned char *)input->cursor;
        if ((char)value == '\0') {
            input->endOfString = 1;
            return -1;
        }
        input->cursor++;
        return value;
    }
    case MSL_INPUT_UNREAD:
        if (input->endOfString == 0) {
            input->cursor--;
        } else {
            input->endOfString = 0;
        }
        return character;
    case MSL_INPUT_QUERY_END:
        return input->endOfString;
    default:
        return 0;
    }
}
