#pragma once

#include "prInterfaces.h"
#include "components/fs/FS.h"

namespace pr {

class RealFile : public IFile {
public:
    RealFile(::Pinetime::Controllers::FS& fs, std::unique_ptr<lfs_file_t>&& file);
    ~RealFile() override;
    int read(void* buff, uint32_t size) override;
    int write(void* buff, uint32_t size) override;
    void seek(uint32_t pos) override;

private:
    Pinetime::Controllers::FS& fs;
    std::unique_ptr<lfs_file_t> pFile;
};

class RealFileFactory : public IFileFactory {
public:
    RealFileFactory(Pinetime::Controllers::FS& fs);
    ~RealFileFactory() = default;
    IFilePtr open(const char* name, int flags) override;

private:
    Pinetime::Controllers::FS& fs;
};

} // namespace pr
