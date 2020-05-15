#include "pch.h"

using namespace std;

FileManager* FileManager::g_fileManager = nullptr;

FileManager* FileManager::GetInstance()
{
    if (g_fileManager == nullptr)
        g_fileManager = new FileManager();

    return g_fileManager;
}

void FileManager::DeleteInstance()
{
    if (g_fileManager != nullptr)
    {
        delete g_fileManager;
        g_fileManager = nullptr;
    }
}

int FileManager::ReadScore()
{
    ifstream fin("data.poo", ios::binary);

    if (!fin.is_open())
        return 0;

    int value = 0;
    fin.read((char*)&value, sizeof(int));

    fin.close();

    return value;
}

void FileManager::WriteScore(int score)
{
    ofstream fout("data.poo", ios::binary);
    fout.write((const char*)&score, sizeof(int));
    fout.close();
}

void FileManager::WriteLog()
{
    ofstream fout("log.txt");
    fout << m_gameLog;
    fout.close();
}

void FileManager::PushToLog(string str)
{
    time_t t = time(nullptr);
    if (char dateStr[100] = { NULL }; strftime(dateStr, sizeof(dateStr), "[%Y-%b-%d %H:%M:%S] ", localtime(&t)))
    {
        string logLine = string(dateStr) + str + "\n";
        m_gameLog = m_gameLog.append(logLine);
#ifdef _DEBUG
        cout << logLine;
#endif // _DEBUG
    }
}

FileManager::FileManager() :
    m_gameLog("")
{
}

FileManager::~FileManager()
{
}