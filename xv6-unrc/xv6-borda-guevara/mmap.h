// descriptor of a in-memory
// file map
struct filemap {
  uint baseaddr;    // The address where the map begins
  uint size;        // The size of the mapped file
  char readable;    
  char writable;    // Permissions of the file
  struct inode *ip; // Inode pointer
};
