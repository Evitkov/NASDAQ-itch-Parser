#pragma once
#include <string>
#include <cstddef>

class MmapFile {
public:
    MmapFile(std::string filepath) ;
    //another cool thing is making all class vars imutable in the fn by declaring functions as constant themselves
    const char* get_start() const ;
    size_t get_length() const;
    ~MmapFile() ;



private:
    const char* start_byte = nullptr;
    size_t file_size = 0;
    //OS specific handles
#ifdef _WIN32
    void* file_handle = nullptr;     // Windows needs 2 handles
    void* mapping_handle = nullptr;
#else
    int fd = -1;                     // Linux just needs 1 file descriptor
#endif
};
