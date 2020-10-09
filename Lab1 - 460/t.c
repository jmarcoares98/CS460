/*******************************************************
*                  @t.c file                          *
*******************************************************/

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

u16 ino;
u32 *up;
INODE* ip;
GD* gp;
DIR* dp;

int prints(char* s)
{
    while (*s) {
        putc(*s);
        s++;
    }
}

int gets(char* s)
{
    while ((*s = getc()) != '\r') {
        putc(*s);
        s++;
    }
    *s = 0;
}


u16 NSEC = 2;
char buf1[BLK], buf2[BLK];

u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);

    // readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

u16 search(INODE *ip, char *name)
{
    int i;
    char temp[256], * cp;

     //search for name in the data block of INODE; 
    for (i = 0; i < 12; i++) {
        getblk((u16)ip->i_block[i], buf2);
        cp = buf2;
        dp = (DIR*)cp;

        while (cp < buf2 + BLK) {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;

            //return its inumber if found
            if (strcmp(temp, name) == 0)
                return (u16)dp->inode - 1;

            cp += dp->rec_len;
            dp = (DIR*)cp;
        }

    }

    //else error();
    error();
}

main()
{ 
    int i, blk;
    char* names[2];
    names[0] = "boot";
    names[1] = "mtx";

    getblk(2, buf1);
    gp = (GD*)buf1;
    blk = gp->bg_inode_table;

    getblk(blk, buf1);
    ip = (INODE*)buf1 + 1;

//1. Write YOUR C code to get the INODE of /boot/mtx
//   INODE *ip --> INODE
    for (i = 0; i < 2; i++) {
        ino = search(ip, names[i]);
        getblk(blk + (ino / 8), buf1);
        ip = (INODE*)buf1 + (ino % 8);
    }

//   if INODE has indirect blocks: get i_block[12] int buf2[  ]
    if ((u16)ip->i_block[12])
        getblk((u16)ip->i_block[12], buf2);

//2. setes(0x1000);  // MTX loading segment = 0x1000
    setes(0x1000);

//3. load 12 DIRECT blocks of INODE into memory
   for (i = 0; i < 12; i++) {
      getblk((u16)ip->i_block[i], 0);
      putc('*');
      inces();
   }

//4. load INDIRECT blocks, if any, into memory
   if ((u16)ip->i_block[12]) {
     up = (u32 *)buf2;      
     while(*up){
        getblk((u16)*up, 0); putc('.');
        inces();
        up++;
     }
  }

  prints("\n\rgo?"); getc();
}  
