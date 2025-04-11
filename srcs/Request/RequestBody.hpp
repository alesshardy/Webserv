/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBody.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 18:22:00 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/11 18:45:08 by apintus          ###   ########.fr       */
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
        int                     _bodyFd;
        unsigned long long      _currentSize;
        unsigned long long      _maxBodySize;
        bool                    _isChunked;
        bool                    _isComplete;
    
    public:
        RequestBody(unsigned long long maxBodySize, bool isChunked);
        ~RequestBody();

        void initBody(unsigned long long maxBodySize, bool ischunked);

        // void parseChunked(const std::string &rawData, unsigned long long  &index);
        // void parseContentLength(const std::string &rawData, unsigned long long  &index, unsigned long long contentLength);

        //getters
        // const std::string &getBody() const;
        
};

#endif