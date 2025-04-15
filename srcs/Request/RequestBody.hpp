/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBody.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 18:22:00 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/14 15:52:42 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTBODY_HPP
#define REQUESTBODY_HPP

#include <iostream>
#include "Request.hpp"

class RequestBody
{
    private:
        std::string             _bodyStr;
        // int                     _bodyFd;
        size_t      _currentSize;
        size_t      _maxBodySize;
        bool                    _isChunked;
        bool                    _isComplete;
    
    public:
        RequestBody(size_t maxBodySize, bool isChunked);
        ~RequestBody();


        void parseChunked(const std::string &rawData, size_t  &index);
        void parseContentLength(const std::string &rawData, size_t  &index, size_t contentLength);

        //getters
        const std::string &getBody() const {return _bodyStr;};
        bool isComplete() const {return _isComplete;};
        size_t getCurrentSize() const {return _currentSize;};
        
};

#endif