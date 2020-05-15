#include "pch.h"
#include "GraphicsException.h"

using namespace std;

GraphicsException::GraphicsException(initializer_list<IUnknown*> ls) : 
    exception(GetErrorString(ls).c_str())
{
}

string GraphicsException::GetErrorString(initializer_list<IUnknown*>& ls)
{
    stringstream ss;
    ss << "Graphics exception: ";
    for (auto& it : ls)
        ss << static_cast<const void*>(it) << " ";

    return ss.str();
}
