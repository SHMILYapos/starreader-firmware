/**
 * StarReader Firmware - Storage HAL
 * 
 * microSD card storage abstraction
 */

#ifndef STARREADER_HAL_STORAGE_H
#define STARREADER_HAL_STORAGE_H

#include <stdint.h>
#include <stddef.h>
#include "../config.h"

// File entry for directory listing
struct FileEntry {
    char name[64];
    uint32_t size;
    bool isDirectory;
    bool hidden;
};

class HalStorage {
public:
    HalStorage();
    ~HalStorage();

    bool begin();
    void end();

    // Card status
    bool isCardPresent() const;
    uint64_t cardSizeMB() const;
    uint64_t freeSpaceMB() const;

    // File operations
    bool fileExists(const char* path);
    size_t getFileSize(const char* path);
    bool deleteFile(const char* path);

    // Directory operations
    bool directoryExists(const char* path);
    bool createDirectory(const char* path);

    // List directory contents
    // Returns number of entries written to buffer
    int listDirectory(const char* path, FileEntry* entries, int maxEntries);

    // Read file into buffer
    bool readFile(const char* path, uint8_t* buffer, size_t maxSize, size_t* bytesRead);

    // Write buffer to file
    bool writeFile(const char* path, const uint8_t* data, size_t size);

    // Append to file
    bool appendFile(const char* path, const uint8_t* data, size_t size);

    // Utility
    const char* getLastError() const;

private:
    bool m_initialized;
    bool m_cardPresent;
    char m_lastError[128];
};

#endif // STARREADER_HAL_STORAGE_H
