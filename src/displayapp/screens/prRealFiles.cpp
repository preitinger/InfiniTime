#include "prRealFiles.h"
#include "prLog.h"

#include <memory>

namespace pr {

    // RealFile1::RealFile1()
    RealFile::RealFile(::Pinetime::Controllers::FS& fs, std::unique_ptr<lfs_file_t>&& file) : fs(fs), pFile(std::move(file)) {
    }

    RealFile::~RealFile() {
        auto& file = *pFile;
        int code = fs.FileClose(&file);
        assert(code >= 0);
        if (code < 0) NRF_LOG_ERROR("~RealFile: FileClose returned %d", code);
        log("file closed");
    }

    int RealFile::read(void* buff, uint32_t size) {
        auto& file = *pFile;
        return this->fs.FileRead(&file, static_cast<uint8_t*>(buff), size);
    };

    int RealFile::write(const void* buff, uint32_t size) {
        auto& file = *pFile;
        return this->fs.FileWrite(&file, static_cast<const uint8_t*>(buff), size);
    }
    void RealFile::seek(uint32_t pos) { 
        auto& file = *pFile;
        int code = fs.FileSeek(&file, pos);
        assert(code >= 0);
        if (code < 0) NRF_LOG_ERROR("seek returned %d", code);
    }
    

    RealFileFactory::RealFileFactory(Pinetime::Controllers::FS& fs)
    : fs(fs)
    {
    }

    // RealFileFactory::~RealFileFactory() {

    // }

    IFilePtr RealFileFactory::open(const char* name, int flags) {
        std::unique_ptr<lfs_file_t> file = std::make_unique<lfs_file_t>();
        auto code = fs.FileOpen(file.get(), name, flags);
        if (code < 0) {
            log("fs.FileOpen returned %d", code);
            return IFilePtr();
        }

        return std::make_unique<RealFile>(fs, std::move(file));
    }

} // namespace pr
