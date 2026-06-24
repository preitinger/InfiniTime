#pragma once

#include <memory>

namespace pr {

// 1. Das Interface für das Dateisystem (perfekt zum Mocken)
class IFile {
public:
    virtual ~IFile() = default;
    virtual int read(void* buff, uint32_t size) = 0;
    virtual int write(void* buff, uint32_t size) = 0;
    virtual void seek(uint32_t pos) = 0;
};

using IFilePtr = std::unique_ptr<IFile>;


class IFileFactory {
public:
    enum Flags {
        RDONLY = 1,
        WRONLY = 2,
        RDWR = 3,
        CREAT = 0x0100,
        EXCL = 0x0200,
        TRUNC = 0x0400,
        APPEND = 0x0800,
    };
    virtual IFilePtr open(const char* name, int flags) = 0;
};

using IFileFactoryPtr = std::unique_ptr<IFileFactory>;

} // namespace pr
