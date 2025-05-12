/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 15:47:04 by apintus           #+#    #+#             */
/*   Updated: 2025/01/31 13:20:09 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"

RPN::RPN()
{
}

RPN::RPN(const RPN &copy)
{
    *this = copy;
}

RPN::~RPN()
{
}

RPN &RPN::operator=(const RPN &assign)
{
    if (this != &assign)
    {
        _input = assign._input;
        _stack = assign._stack;
    }
    return (*this);
}

//Functions
void    RPN::calculate(const std::string &input)
{
    bool    isNumber = false;
    _input = input;
    
    for (size_t i = 0; i < _input.size(); i++)
    {
        if (isdigit(_input[i]) || ((_input[i] == '-' || _input[i] == '+') && isdigit(_input[i + 1])))
        {
            if (isNumber)
                throw InvalidExpressionException();
            
            int sign = 1;
            if (_input[i] == '+' || _input[i] == '-')
            {
                if (_input[i] == '-')
                    sign = -1;
                i++;
            }
            int nbr = input[i] - '0'; //conversion char->int
            _stack.push(nbr * sign);
            isNumber = true;
        }
        else
        {
            isNumber = false;
            if (_isOperator(_input[i]))
            {
                if (_stack.size() < 2)
                    throw InvalidExpressionException();
                double a =_stack.top();
                _stack.pop();
                double b = _stack.top();
                _stack.pop();
                if (_input[i] == '+')
                    _stack.push(b + a);
                else if (_input[i] == '-')
                    _stack.push(b - a);
                else if (_input[i] == '*')
                    _stack.push(b * a);
                else if (_input[i] == '/')
                {
                    if (a == 0)
                        throw DivisionByZeroExecption();
                    _stack.push(b / a);
                }
            }
            else if (iswspace(_input[i]))
                continue;
            else
                throw InvalidExpressionException();
        }
    }
    if (_stack.size() > 1)
        throw InvalidExpressionException();
    else
        std::cout << _stack.top() << std::endl;
}


bool    RPN::_isOperator(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

const char *RPN::InvalidExpressionException::what() const throw()
{
    return ("Invalid expression");
}

const char *RPN::DivisionByZeroExecption::what() const throw()
{
    return ("Division by zero");
}
