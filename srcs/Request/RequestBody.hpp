/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBody.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpassin <tpassin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 18:22:00 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/03 19:30:07 by tpassin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTBODY_HPP
#define REQUESTBODY_HPP

#include <iostream>
#include "Request.hpp"

class RequestBody{
    private:
    
    public:
        RequestBody();
        RequestBody(RequestBody const & copy);
        RequestBody & operator=(RequestBody const & rhs);
        ~RequestBody();
};

#endif