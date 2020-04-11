#pragma once

class FileManager
{
public:

    FileManager();
    ~FileManager();

    static std::shared_ptr<FileManager> GetInstance();

public:

    int  ReadScore();
    void WriteScore(int);
    void WriteLog();

    void PushToLog(std::string);

private:

    static std::shared_ptr<FileManager> g_fileManager;

private:

    std::string m_gameLog;
};