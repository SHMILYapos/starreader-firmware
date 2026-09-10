/**
 * StarReader Firmware - Storage HAL Implementation
 */

#include "storage.h"
#include "../config.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#endif

HalStorage::HalStorage()
    : m_initialized(false)
    , m_cardPresent(false) {
    m_lastError[0] = '\0';
}

HalStorage::~HalStorage() {
    end();
}

bool HalStorage::begin() {
#ifdef ARDUINO
    // Initialize SD card on shared SPI bus
    SPI.begin(SD_PIN_SCK, SD_PIN_MISO, SD_PIN_MOSI, SD_PIN_CS);

    if (SD.begin(SD_PIN_CS)) {
        m_cardPresent = true;
        m_initialized = true;

        // Create system directory if it doesn't exist
        if (!directoryExists(SD_SYSTEM_DIR)) {
            createDirectory(SD_SYSTEM_DIR);
        }
        if (!directoryExists(SD_BOOK_CACHE_DIR)) {
            createDirectory(SD_BOOK_CACHE_DIR);
        }
        if (!directoryExists(SD_SLEEP_IMAGES_DIR)) {
            createDirectory(SD_SLEEP_IMAGES_DIR);
        }

        return true;
    } else {
        strcpy(m_lastError, "SD card mount failed");
        return false;
    }
#else
    m_initialized = true;
    m_cardPresent = true;
    return true;
#endif
}

void HalStorage::end() {
#ifdef ARDUINO
    SD.end();
#endif
    m_initialized = false;
    m_cardPresent = false;
}

bool HalStorage::isCardPresent() const {
    return m_cardPresent;
}

uint64_t HalStorage::cardSizeMB() const {
#ifdef ARDUINO
    if (m_cardPresent) {
        return SD.cardSize() / (1024 * 1024);
    }
#endif
    return 0;
}

uint64_t HalStorage::freeSpaceMB() const {
#ifdef ARDUINO
    if (m_cardPresent) {
        return SD.totalBytes() / (1024 * 1024);
    }
#endif
    return 0;
}

bool HalStorage::fileExists(const char* path) {
#ifdef ARDUINO
    if (!m_cardPresent) return false;
    return SD.exists(path);
#else
    return false;
#endif
}

size_t HalStorage::getFileSize(const char* path) {
#ifdef ARDUINO
    if (!m_cardPresent) return 0;
    File file = SD.open(path, FILE_READ);
    if (!file) return 0;
    size_t size = file.size();
    file.close();
    return size;
#else
    return 0;
#endif
}

bool HalStorage::deleteFile(const char* path) {
#ifdef ARDUINO
    if (!m_cardPresent) return false;
    return SD.remove(path);
#else
    return false;
#endif
}

bool HalStorage::directoryExists(const char* path) {
#ifdef ARDUINO
    if (!m_cardPresent) return false;
    File dir = SD.open(path);
    if (!dir) return false;
    bool isDir = dir.isDirectory();
    dir.close();
    return isDir;
#else
    return false;
#endif
}

bool HalStorage::createDirectory(const char* path) {
#ifdef ARDUINO
    if (!m_cardPresent) return false;
    return SD.mkdir(path);
#else
    return false;
#endif
}

int HalStorage::listDirectory(const char* path, FileEntry* entries, int maxEntries) {
#ifdef ARDUINO
    if (!m_cardPresent) return 0;

    File dir = SD.open(path);
    if (!dir || !dir.isDirectory()) return 0;

    int count = 0;
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) break;

        if (count < maxEntries) {
            strncpy(entries[count].name, entry.name(), sizeof(entries[count].name) - 1);
            entries[count].name[sizeof(entries[count].name) - 1] = '\0';
            entries[count].size = entry.size();
            entries[count].isDirectory = entry.isDirectory();
            entries[count].hidden = (entries[count].name[0] == '.');
            count++;
        }

        entry.close();
    }

    dir.close();
    return count;
#else
    return 0;
#endif
}

bool HalStorage::readFile(const char* path, uint8_t* buffer, size_t maxSize, size_t* bytesRead) {
#ifdef ARDUINO
    if (!m_cardPresent) return false;

    File file = SD.open(path, FILE_READ);
    if (!file) return false;

    size_t toRead = file.size();
    if (toRead > maxSize) toRead = maxSize;

    *bytesRead = file.read(buffer, toRead);
    file.close();
    return (*bytesRead > 0);
#else
    return false;
#endif
}

bool HalStorage::writeFile(const char* path, const uint8_t* data, size_t size) {
#ifdef ARDUINO
    if (!m_cardPresent) return false;

    File file = SD.open(path, FILE_WRITE);
    if (!file) return false;

    size_t written = file.write(data, size);
    file.close();
    return (written == size);
#else
    return false;
#endif
}

bool HalStorage::appendFile(const char* path, const uint8_t* data, size_t size) {
#ifdef ARDUINO
    if (!m_cardPresent) return false;

    File file = SD.open(path, FILE_APPEND);
    if (!file) return false;

    size_t written = file.write(data, size);
    file.close();
    return (written == size);
#else
    return false;
#endif
}

const char* HalStorage::getLastError() const {
    return m_lastError;
}
