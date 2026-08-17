#include <gtest/gtest.h>
#include "storage/FileIO.hpp"
#include <filesystem>
#include <fstream>

using namespace HBR::Storage;

class FileIOTest : public ::testing::Test
{
protected:
    Path testDir;
    FileIO fileIO;

    void SetUp() override
    {
        // Setup isolated temporary directory for test runs
        testDir = std::filesystem::temp_directory_path() / "hbr_storage_gtest";
        std::filesystem::remove_all(testDir);
        std::filesystem::create_directories(testDir);
    }

    void TearDown() override
    {
        // Cleanup created test directory
        std::filesystem::remove_all(testDir);
    }
};

TEST_F(FileIOTest, WriteAndReadSuccess)
{
    Path filePath = testDir / "sample_data.bin";
    Buffer writeBuffer = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0xFF};

    STATUS writeStatus = fileIO.Write(filePath, writeBuffer);
    EXPECT_EQ(writeStatus, STATUS::OK);
    EXPECT_TRUE(std::filesystem::exists(filePath));

    Buffer readBuffer;
    STATUS readStatus = fileIO.Read(filePath, readBuffer);
    EXPECT_EQ(readStatus, STATUS::OK);
    EXPECT_EQ(readBuffer, writeBuffer);
}

TEST_F(FileIOTest, WriteCreatesNestedDirectories)
{
    Path nestedPath = testDir / "subfolder1" / "subfolder2" / "nested_file.txt";
    Buffer writeBuffer = {'N', 'e', 's', 't', 'e', 'd'};

    STATUS status = fileIO.Write(nestedPath, writeBuffer);
    EXPECT_EQ(status, STATUS::OK);
    EXPECT_TRUE(std::filesystem::exists(nestedPath));
}

TEST_F(FileIOTest, ReadNonExistentFile)
{
    Path nonExistentPath = testDir / "does_not_exist.bin";
    Buffer readBuffer;

    STATUS status = fileIO.Read(nonExistentPath, readBuffer);
    EXPECT_EQ(status, STATUS::FILE_NOT_FOUND);
}

TEST_F(FileIOTest, CopyFileSuccess)
{
    Path srcPath = testDir / "source.txt";
    Path destPath = testDir / "backup_folder" / "destination.txt";
    Buffer data = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};

    EXPECT_EQ(fileIO.Write(srcPath, data), STATUS::OK);

    STATUS copyStatus = fileIO.Copy(srcPath, destPath);
    EXPECT_EQ(copyStatus, STATUS::OK);
    EXPECT_TRUE(std::filesystem::exists(destPath));

    Buffer readBuffer;
    EXPECT_EQ(fileIO.Read(destPath, readBuffer), STATUS::OK);
    EXPECT_EQ(readBuffer, data);
}

TEST_F(FileIOTest, CopyNonExistentSource)
{
    Path srcPath = testDir / "missing_source.txt";
    Path destPath = testDir / "destination.txt";

    STATUS status = fileIO.Copy(srcPath, destPath);
    EXPECT_EQ(status, STATUS::FILE_NOT_FOUND);
}

TEST_F(FileIOTest, OverwriteExistingFile)
{
    Path filePath = testDir / "overwrite_test.txt";
    Buffer initialData = {1, 2, 3, 4, 5};
    Buffer newData     = {9, 8, 7};

    EXPECT_EQ(fileIO.Write(filePath, initialData), STATUS::OK);
    EXPECT_EQ(fileIO.Write(filePath, newData), STATUS::OK);

    Buffer readBuffer;
    EXPECT_EQ(fileIO.Read(filePath, readBuffer), STATUS::OK);
    EXPECT_EQ(readBuffer, newData);
}