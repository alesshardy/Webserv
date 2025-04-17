#ifndef REQUESTBODY_HPP
#define REQUESTBODY_HPP

#include <iostream>
#include "Request.hpp"

class RequestBody
{
    private:
        std::string             _tmpFilePath;
        int                     _fd;
        
        // std::string             _bodyStr;
        size_t                  _currentSize;
        size_t                  _maxBodySize;
        bool                    _isChunked;
        bool                    _isComplete;
    
    public:
        RequestBody(size_t maxBodySize, bool isChunked);
        ~RequestBody();


        void parseChunked(const std::string &rawData, size_t  &index);
        void parseContentLength(const std::string &rawData, size_t  &index, size_t contentLength);

        //getters
        std::string readBody() const;
        // const std::string &getBody() const {return _bodyStr;};
        bool isComplete() const {return _isComplete;};
        size_t getCurrentSize() const {return _currentSize;};
        
};

#endif