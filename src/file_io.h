#ifndef FILE_IO_H
#define FILE_IO_H

enum ReadEntireFileFlag {
    ReadFileEntireFile_NullTerminate = 0x1,
};

typedef struct EntireFile {
    size_t size;
    void* data;
} EntireFile;

#endif /* FILE_IO_H */
