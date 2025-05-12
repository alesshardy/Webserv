/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 15:47:12 by apintus           #+#    #+#             */
/*   Updated: 2025/01/31 12:52:35 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RPN_HPP
# define RPN_HPP

# include <iostream>
# include <iomanip>
# include <stack>

// Container stack usful here to apply the polish method of calcul we need stack (FILO) 

class RPN
{
  private:
            std::string			_input;
            std::stack<double>	_stack;
			bool				_isOperator(char c);
  public:  
			RPN();
			RPN(const RPN &copy);
			~RPN();
			
			RPN &operator=(const RPN &assign);
			
			void	calculate(const std::string &input);
			
			//Exceptions
			class InvalidExpressionException : public std::exception
			{
				public:
						virtual const char* what() const throw();	
			};

			class	DivisionByZeroExecption : public std::exception
			{
				public:
						virtual const char* what() const throw();	
			};
};

#endif