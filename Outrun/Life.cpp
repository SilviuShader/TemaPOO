#include "pch.h"

using namespace std;

Life::Life(shared_ptr<GameObject> parent) :
    GameComponent(parent)
{
}

Life::~Life()
{
}
