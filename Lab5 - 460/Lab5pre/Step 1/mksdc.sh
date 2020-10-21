# Crete a flat disk image file of 1024 1 kb blocks
dd if=/dev/zero of=sdimage bs=1024 count=1024

# format sdimage as an EXT2 file system with 1024 1KB blocks
mke2fs -b 1024 sdimage 1024

# mount sdimage on /mnt
sudo mount -o loop sdimage /mnt

# mkdir bin dev etc user on the mounted sdimage
sudo mkdir /mnt/bin

# umount it
sudo umount /mnt