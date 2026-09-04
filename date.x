
/* arquivo date.x */
program DATE_PROG {
    version DATE_VERS {
        long bin_date(void) = 1;
        string str_date(long) = 2;
    } = 1;
} = 0x31234567;