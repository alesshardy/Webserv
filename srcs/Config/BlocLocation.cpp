#include "BlocLocation.hpp"

// Constructors and destructors
BlocLocation::BlocLocation()
{
    _autoIndex = false;
}

BlocLocation::BlocLocation(const BlocLocation &copy)
{
    *this = copy;
}


BlocLocation::~BlocLocation()
{
}

BlocLocation &BlocLocation::operator=(const BlocLocation &assign)
{
    if (this != &assign)
    {
        _root = assign._root;
        _alias = assign._alias;
        _index = assign._index;
        _allowMethod = assign._allowMethod;
        _uploadPath = assign._uploadPath;
        _autoIndex = assign._autoIndex;
    }
    return (*this);
}

//
