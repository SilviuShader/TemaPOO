#pragma once

class FileManager
{
public:

    FileManager();
    ~FileManager();

    static std::shared_ptr<FileManager> GetInstance();

    int                                 ReadScore();
    void                                WriteScore(int);

private:

    static std::shared_ptr<FileManager> g_fileManager;
};