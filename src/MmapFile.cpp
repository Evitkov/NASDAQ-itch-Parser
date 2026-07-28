#include "MmapFile.h"
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

MmapFile::MmapFile(std::string filepath) {
#ifdef _WIN32
    //Open the file
    file_handle = CreateFileA(
        filepath.c_str(),       // Convert std::string to C-style string
        GENERIC_READ,           // We only want to read
        FILE_SHARE_READ,        // Let others read it too
        NULL,
        OPEN_EXISTING,          // The file must already exist
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file_handle == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open file: " << filepath << "\n";
        return;
    }

    //Get the file size
    LARGE_INTEGER size;
    if (!GetFileSizeEx(file_handle, &size)) {    // return is boolean and we pass reference to our size as param
        std::cerr << "Failed to get file size.\n";
        return;
    }
    file_size = static_cast<size_t>(size.QuadPart);

    //Create the mapping object
    mapping_handle = CreateFileMappingA(
        file_handle,
        NULL,
        PAGE_READONLY,          // Protect the memory from being modified
        0, 0,
        NULL
    );

    if (mapping_handle == NULL) {
        std::cerr << "Failed to create file mapping.\n";
        return;
    }

    //Map the view and get the pointer
    void* map_result = MapViewOfFile(
        mapping_handle,
        FILE_MAP_READ,          // Read-only view
        0, 0, 0
    );

    if (map_result == NULL) {
        std::cerr << "Failed to map view of file.\n";
        return;
    }

    // Cast the generic pointer to char pointer
    start_byte = static_cast<const char*>(map_result);

#else
    // Open
    fd = open(filepath.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "Failed to open file on Linux: " << filepath << "\n";
        return;
    }

    //Get file size
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        std::cerr << "Failed to get file size on Linux.\n";
        return;
    }
    file_size = static_cast<size_t>(sb.st_size);

    //Memory map
    void* map_result = mmap(
        nullptr,
        file_size,          // How much to map
        PROT_READ,          // Memory protection: Read-only
        MAP_PRIVATE,
        fd,
        0
    );

    if (map_result == MAP_FAILED) {
        std::cerr << "Failed to mmap file on Linux.\n";
        return;
    }

    start_byte = static_cast<const char*>(map_result);
#endif
}

MmapFile::~MmapFile() {
#ifdef _WIN32
    if (start_byte != nullptr) {
        UnmapViewOfFile(start_byte);
    }

    if (mapping_handle != NULL) {
        CloseHandle(mapping_handle);
    }

    if (file_handle != INVALID_HANDLE_VALUE && file_handle != NULL) {
        CloseHandle(file_handle);
    }
#else
    if (start_byte != nullptr && start_byte != (void*)-1) {
        munmap(const_cast<char*>(start_byte), file_size);
    }

    if (fd != -1) {
        close(fd);
    }
#endif
}

const char *MmapFile::get_start() const {
    return start_byte;
}

size_t MmapFile::get_length() const {
    return file_size;
}
