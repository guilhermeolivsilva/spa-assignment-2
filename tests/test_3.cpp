int func() {
    int i = 0;
    int j = 10;
    i += j;
    if (i < 5) {
        if (i < 3) {
            return 0;
        }
        if (i > 10) {
            return 1;
        }
        return 4;
    }
    if (i >= 6) {
        if (i >= 10) {
            return 2;
        }
        if (i < 4) {
            return 3;
        }
        return 5;
    }
    return 6;
}
