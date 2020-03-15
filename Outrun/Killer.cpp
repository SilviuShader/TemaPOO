#include "pch.h"

using namespace std;

Killer::Killer(shared_ptr<GameObject> parent) :
    GameComponent(parent)
{
}

Killer::~Killer()
{
}
