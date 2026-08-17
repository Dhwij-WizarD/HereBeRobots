#pragma once

#include "StorageConcepts.hpp"

namespace HBR::Storage
{

class FileIO
{
public:
    using PathType   = Path;
    using BufferType = Buffer;

    FileIO() = default;
    ~FileIO() = default;

    /**
     * @brief Reads entire file contents into a byte buffer.
     */
    STATUS Read(const PathType & path, BufferType & out_buffer) const;

    /**
     * @brief Writes byte buffer contents to a file. Overwrites if file exists.
     */
    STATUS Write(const PathType & path, const BufferType & data) const;

    /**
     * @brief Copies file from source path to destination path.
     */
    STATUS Copy(const PathType & src, const PathType & dest) const;
};

// Verify at compile time that FileIO satisfies all constraints
static_assert(Reader<FileIO>);
static_assert(Writer<FileIO>);
static_assert(Copier<FileIO>);
static_assert(FileStorageAdapter<FileIO>);

} // namespace HBR::Storage