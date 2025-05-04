#ifndef REQUESTBODY_HPP
#define REQUESTBODY_HPP

#include <iostream>
#include "Request.hpp"
#include <map>  

class Request;

class RequestBody
{
    friend class CgiRequest;
    private:
        Request                 *_request;
        std::string             _tmpFilePath;
        int                     _fd;
        size_t                  _currentSize;
        size_t                  _maxBodySize;
        bool                    _isChunked;
        bool                    _isComplete;
        bool                    _isTmp;
        bool                    _isNew;
    
    public:
        RequestBody(size_t maxBodySize, bool isChunked, Request *request);
        ~RequestBody();

        void parseChunked(const std::string &rawData, size_t  &index);
        void parseContentLength(const std::string &rawData, size_t  &index, size_t contentLength);

        //getters
        bool isComplete() const {return _isComplete;};
        size_t getCurrentSize() const {return _currentSize;};
        int getFd () const {return _fd;};
        std::string getTmpFilePath() const{return _tmpFilePath;};
        bool getIsNew() const {return _isNew;};
        
        //print
        std::string readBody() const;

        void defineBodyDestination();
};

#endif