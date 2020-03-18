#include "pch.h"

using namespace std;

shared_ptr<FileManager> FileManager::g_fileManager = nullptr;

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

shared_ptr<FileManager> FileManager::GetInstance()
{
    if (g_fileManager == nullptr)
        g_fileManager = make_shared<FileManager>();

    return g_fileManager;
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
