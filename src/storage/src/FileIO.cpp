#include "../include/FileIO/FileIO.hpp"
#include <filesystem>
#include <fstream>
#include <system_error>

namespace HBR::Storage
{

STATUS FileIO::Read(const PathType & path, BufferType & out_buffer) const
{
    std::error_code ec;
    if (!std::filesystem::exists(path, ec) || !std::filesystem::is_regular_file(path, ec))
    {
        return STATUS::FILE_NOT_FOUND;
    }

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return STATUS::READ_ERROR;
    }

    const auto fileSize = file.tellg();
    if (fileSize < 0)
    {
        return STATUS::READ_ERROR;
    }

    file.seekg(0, std::ios::beg);
    out_buffer.resize(static_cast<size_t>(fileSize));

    if (fileSize > 0 && !file.read(reinterpret_cast<char*>(out_buffer.data()), fileSize))
    {
        return STATUS::READ_ERROR;
    }

    return STATUS::OK;
}

STATUS FileIO::Write(const PathType & path, const BufferType & data) const
{
    // Auto-create directory structure if missing
    if (path.has_parent_path())
    {
        std::error_code ec;
        std::filesystem::create_directories(path.parent_path(), ec);
        if (ec)
        {
            return STATUS::WRITE_ERROR;
        }
    }

    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file.is_open())
    {
        return STATUS::WRITE_ERROR;
    }

    if (!data.empty())
    {
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        if (!file.good())
        {
            return STATUS::WRITE_ERROR;
        }
    }

    return STATUS::OK;
}

STATUS FileIO::Copy(const PathType & src, const PathType & dest) const
{
    std::error_code ec;
    if (!std::filesystem::exists(src, ec))
    {
        return STATUS::FILE_NOT_FOUND;
    }

    // Auto-create destination directories if needed
    if (dest.has_parent_path())
    {
        std::filesystem::create_directories(dest.parent_path(), ec);
        if (ec)
        {
            return STATUS::COPY_ERROR;
        }
    }

    std::filesystem::copy_file(
        src, 
        dest, 
        std::filesystem::copy_options::overwrite_existing, 
        ec
    );

    if (ec)
    {
        return STATUS::COPY_ERROR;
    }

    return STATUS::OK;
}

} // namespace HBR::Storage