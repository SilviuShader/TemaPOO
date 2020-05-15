#pragma once

class GraphicsException : public std::exception
{
public:

    GraphicsException(std::initializer_list<IUnknown*>);
    virtual ~GraphicsException() throw() {}

private:

    std::string GetErrorString(std::initializer_list<IUnknown*>&);
};