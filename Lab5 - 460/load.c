/************ load.c file ****************/
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;


int load(char *filename, PROC *p)
{
	int    i, ino = 0, InodesBeginBlock;
	char* cp, temp[64], buf[1024], buf2[1024], * name[2];
    GD* gp; INODE* ip; DIR* dp;

    // Read in the group descriptor block(block 2) to access the group 0 descriptor.From the group descriptor's bg_inode_table entry, 
    // find the INODEs begin block number, call it the InodesBeginBlock.
    getblock(2, buf);
    gp = (GD*)buf;
    InodesBeginBlock = (int)gp->bg_inode_table;

    // Read in InodeBeginBlock to get the INODE of root directory / , which is second inode(ino = 2) in the INODE Table
    getblock(InodesBeginBlock, buf);
    ip = (INODE*)buf + 1;

    // Tokenize the pathname into component stringsand let the number of components be n
    name[0] = "bin";
    name[1] = "filename";

    // Start from the root INODE in(3),  search for name[0] in its data block(s). Each data block of a DIR INODE contains
    // dir_entry structures of the form
    for (i = 0; i < 2; i++) {
        ino = search(ip, name[i]) - 1;

        if (ino < 0)
            return 0;
        // Use the inode number (ino) to compute the disk block containing the INODE and its offset in that block by the
        // Mailman's algorithm 
        getblock(InodesBeginBlock + (ino / 8), buf);
        ip = (INODE*)buf + ((int)ino % 8);
    }
}

int search(INODE* ip, char* name)
{
	int    i, iblk;
	char* cp, temp[64], buf[1024];
    DIR* dp;

    for (i = 0; i < 12; i++) {
        // if (i_block[i]==0) BREAK;
        if (ip->i_block[i] == 0)
            break;

        getblock(ip->i_block[i], buf);
        dp = ((DIR*)buf);
        cp = buf;

        // 4. print file names in the root directory /
        while (cp < buf + 1024) {
            strcpy(temp, dp->name);
            temp[dp->name_len] = 0; //set the last one to null

            printf("%s ", dp->name);

            if (strcmp(dp->name, name) == 0) {
                printf("found %s\n", name);
                return(dp->inode);
            }

            cp += dp->rec_len;
            dp = (DIR*)cp;
        }
    }
}