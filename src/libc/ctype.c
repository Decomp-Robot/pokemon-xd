extern signed char __lower_map[];

int tolower(int c) {
    if (c == -1) {
        return -1;
    }
    return (unsigned char)__lower_map[c & 0xFF];
}

