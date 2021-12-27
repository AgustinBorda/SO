#ifndef FMAP_H
#define FMAP_H
// descriptor of a in-memory
// file map
struct fmap {
  uint baseaddr;    // The address where the map begins
  uint size;        // The size of the mapped file
  struct file *f;   //The mapped file
  char present;
};
#endif
