typedef struct MSLStringInput {
    const char *cursor;
    int endOfString;
} MSLStringInput;

enum MSLInputOperation {
    MSL_INPUT_READ = 0,
    MSL_INPUT_UNREAD = 1,
    MSL_INPUT_QUERY_END = 2
};

#ifdef __cplusplus
extern "C" {
#endif
int __StringRead(void *context, int character, int operation);
#ifdef __cplusplus
}
#endif
