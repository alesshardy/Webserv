#include "BlocLocation.hpp"

// Constructors and destructors
BlocLocation::BlocLocation()
{
    _autoIndex = false;
    _hasTmpCgiExtension = false;
    _hasTmpReturnDirective = false;
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
        _cgiExtension = assign._cgiExtension;
        _returnDirectives = assign._returnDirectives;
    }
    return (*this);
}

//
