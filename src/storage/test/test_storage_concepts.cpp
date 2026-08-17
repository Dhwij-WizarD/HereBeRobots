#include <gtest/gtest.h>
#include "storage/StorageConcepts.hpp"
#include "storage/FileIO.hpp"

using namespace HBR::Storage;

// ─── Custom Mock Types for Testing Constraints ────────────────────────────────

struct ValidMockAdapter
{
    using PathType   = Path;
    using BufferType = Buffer;

    STATUS Read(const PathType&, BufferType&) const { return STATUS::OK; }
    STATUS Write(const PathType&, const BufferType&) const { return STATUS::OK; }
    STATUS Copy(const PathType&, const PathType&) const { return STATUS::OK; }
};

struct IncompleteMockAdapter
{
    using PathType   = Path;
    using BufferType = Buffer;

    // Missing Write and Copy
    STATUS Read(const PathType&, BufferType&) const { return STATUS::OK; }
};

struct WrongReturnTypeAdapter
{
    using PathType   = Path;
    using BufferType = Buffer;

    // Returns int instead of HBR::Storage::STATUS
    int Read(const PathType&, BufferType&) const { return 0; }
    int Write(const PathType&, const BufferType&) const { return 0; }
    int Copy(const PathType&, const PathType&) const { return 0; }
};

// ─── Concept Verification Tests ────────────────────────────────────────────────

TEST(StorageConceptsTest, FileIOSatisfiesAllConcepts)
{
    static_assert(Reader<FileIO>, "FileIO must satisfy Reader concept");
    static_assert(Writer<FileIO>, "FileIO must satisfy Writer concept");
    static_assert(Copier<FileIO>, "FileIO must satisfy Copier concept");
    static_assert(FileStorageAdapter<FileIO>, "FileIO must satisfy FileStorageAdapter concept");
    SUCCEED();
}

TEST(StorageConceptsTest, ValidMockSatisfiesConcepts)
{
    static_assert(Reader<ValidMockAdapter>);
    static_assert(Writer<ValidMockAdapter>);
    static_assert(Copier<ValidMockAdapter>);
    static_assert(FileStorageAdapter<ValidMockAdapter>);
    SUCCEED();
}

TEST(StorageConceptsTest, IncompleteMockFailsConcepts)
{
    static_assert(Reader<IncompleteMockAdapter>);
    static_assert(!Writer<IncompleteMockAdapter>);
    static_assert(!Copier<IncompleteMockAdapter>);
    static_assert(!FileStorageAdapter<IncompleteMockAdapter>);
    SUCCEED();
}

TEST(StorageConceptsTest, WrongReturnTypeFailsConcepts)
{
    static_assert(!Reader<WrongReturnTypeAdapter>);
    static_assert(!Writer<WrongReturnTypeAdapter>);
    static_assert(!Copier<WrongReturnTypeAdapter>);
    static_assert(!FileStorageAdapter<WrongReturnTypeAdapter>);
    SUCCEED();
}

// Concept-constrained generic function execution
template <FileStorageAdapter StorageT>
STATUS PerformStorageWorkflow(const StorageT& storage, const Path& p, const Buffer& b)
{
    STATUS s = storage.Write(p, b);
    if (s != STATUS::OK) return s;

    Buffer outBuffer;
    s = storage.Read(p, outBuffer);
    if (s != STATUS::OK) return s;

    return storage.Copy(p, p);
}

TEST(StorageConceptsTest, ConstrainedFunctionCallWithValidMock)
{
    ValidMockAdapter mock;
    Path testPath = "dummy.txt";
    Buffer testData = {0x01, 0x02, 0x03};

    EXPECT_EQ(PerformStorageWorkflow(mock, testPath, testData), STATUS::OK);
}