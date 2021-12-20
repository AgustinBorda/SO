// descriptor of a in-memory
// file map
struct fmap {
  uint baseaddr;    // The address where the map begins
  uint size;        // The size of the mapped file
  char readable;   
  char writable;
  char present;
  struct inode *ip; //Inode pointer
};
