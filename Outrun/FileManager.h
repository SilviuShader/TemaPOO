#pragma once

class FileManager
{
public:

    static FileManager* GetInstance();
    static void         DeleteInstance();

public:

    int  ReadScore();
    void WriteScore(int);
    void WriteLog();

    void PushToLog(std::string);

private:

    FileManager();
    ~FileManager();

    FileManager(const FileManager&)           = delete;
    FileManager operator=(const FileManager&) = delete;

private:

    static FileManager* g_fileManager;

private:

    std::string m_gameLog;
};