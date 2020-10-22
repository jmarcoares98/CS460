/************ uptable.c file ****************/
int uPtable(PROC* p)
{
    int i, j, uentry, * updir, * uptable;
    updir = (int*)0x4000;           // uPtable at 16KB of PA
    uptable = (int*)0x800000;       //uptable begins at 8MB
    uentry = 0 | 0x412;

    for (i = 0; i < 258; i++) {     // 1MB at 8MB has space for updirs of 258 entries
        for (j = 0; j < 2048; j++) {
            uptable[j] = updir[j];
        }
        uptable += 4096;
    }

    for (i = 0; i < 258; i++) {     // The first 258 entries of ptable ID map to 258MB PA; FLAG=0x412 AP = 01 Domanin = 0
        uptable[i] = uentry;
        uentry += 0x100000;
    }

    uptable = (int*)0x800000;       // PROC uptable begin at at 8MB
    for (i = 0; i < 258; i++) {     // Assume 256MB RAM + 2MB I/O space
        for (j = 2048; j < 4096; j++) {
            uptable[j] = 0;
        }

        if (i)   //exclude P0
            uptable[2048] = (0x800000 + i * (p->pid - 1) * 0x100000) | 0xC32;

        uptable += 4096;
    }
}