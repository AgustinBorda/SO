enum filetype { FILE_NONE, FILE_EXEC, FILE_DATA };

struct file {
    enum filetype type;
    char *name,
         *start, 
         *size;
};
