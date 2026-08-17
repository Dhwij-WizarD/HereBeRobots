#pragma once

#include <concepts>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace HBR::Storage
{

// Standardized return statuses for storage operations
enum class STATUS
{
    OK = 0,
    FILE_NOT_FOUND,
    READ_ERROR,
    WRITE_ERROR,
    COPY_ERROR,
    INVALID_PATH
};

using Buffer = std::vector<uint8_t>;
using Path   = std::filesystem::path;

// ─── Call-site concepts ────────────────────────────────────────────────────────
// Each adapter type carries its associated path and buffer types as nested aliases
// (PathType / BufferType), making these concepts self-contained and flexible.

template<typename T>
concept Reader =
  requires(T reader, const typename T::PathType & path, typename T::BufferType & buffer)
    {
    typename T::PathType;
    typename T::BufferType;
  { reader.Read(path, buffer) } -> std::same_as<STATUS>;
};

template<typename T>
concept Writer =
  requires(T writer, const typename T::PathType & path, const typename T::BufferType & buffer)
    {
    typename T::PathType;
    typename T::BufferType;
  { writer.Write(path, buffer) } -> std::same_as<STATUS>;
};

template<typename T>
concept Copier =
  requires(T copier, const typename T::PathType & src, const typename T::PathType & dest)
    {
    typename T::PathType;
  { copier.Copy(src, dest) } -> std::same_as<STATUS>;
};

// Composite concept for a full-featured File Storage Adapter
template<typename T>
concept FileStorageAdapter = Reader<T> && Writer<T> && Copier<T>;

// ─── Generic Storage Manager Concept ──────────────────────────────────────────
// High-level application interface concept capable of operating on any generic file storage.

template<typename T>
concept StorageApp =
  requires(T app, const Path & path, const Buffer & buffer)
    {
  { app.Save(path, buffer) }  -> std::same_as<STATUS>;
  { app.Load(path, buffer) }  -> std::same_as<STATUS>;
  { app.Duplicate(path, path) } -> std::same_as<STATUS>;
};

// ─── Proxy stubs ───────────────────────────────────────────────────────────────
// Minimal verification stubs to static_assert concepts in isolation.

struct ProxyFileAdapter
{
    using PathType   = Path;
    using BufferType = Buffer;

    STATUS Read(const PathType &, BufferType &) const { return STATUS::OK; }
    STATUS Write(const PathType &, const BufferType &) const { return STATUS::OK; }
    STATUS Copy(const PathType &, const PathType &) const { return STATUS::OK; }
};

static_assert(Reader<ProxyFileAdapter>);
static_assert(Writer<ProxyFileAdapter>);
static_assert(Copier<ProxyFileAdapter>);
static_assert(FileStorageAdapter<ProxyFileAdapter>);

} // namespace HBR::Storage