/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBody.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 18:21:35 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/11 18:32:05 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestBody.hpp"

RequestBody::RequestBody(unsigned long long maxBodySize, bool isChunked)
{   
    _bodyStr = "";
    _bodyFd = -1;
    _currentSize = 0;
    _maxBodySize = maxBodySize;
    _isChunked = isChunked;
}

RequestBody::~RequestBody()
{
}

void RequestBody::initBody(unsigned long long maxBodySize, bool isChunked)
{
    _maxBodySize = maxBodySize;
    _isChunked = isChunked;
}
