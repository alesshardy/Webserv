/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 12:20:57 by apintus           #+#    #+#             */
/*   Updated: 2025/01/31 12:36:35 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BITCOINEXCHANGE_HPP
# define BITCOINEXCHANGE_HPP

# include <iostream>
# include <map>
# include <fstream>
# include <sstream>
# include <algorithm>

// Container by assiociation usful here to associate value with key (date) to find quickly the good value

class BitcoinExchange
{
  private:
            std::map<std::string, double>   _data;
            
            void    _isValidDate(const std::string &date);
            double  _isValidValue(const std::string &str);
  public:  
            BitcoinExchange();
            BitcoinExchange(const BitcoinExchange &copy);
            ~BitcoinExchange();
            
            BitcoinExchange &operator=(const BitcoinExchange &assign);

            //Main function
            void    exchange(const char *fileName);
            
            //Convert Functions
            int     ft_stoi(const std::string &str);
            double  ft_stod(const std::string &str);

            //Exceptions
            class NoDataException : public std::exception
            {
                public:
                        virtual const char* what() const throw();
            };

            class InvalidDataException : public std::exception
            {
                public:
                        virtual const char* what() const throw();
            };

            class EmptyFileException : public std::exception
            {
                public:
                        virtual const char* what() const throw();
            };
            
            class NoPreviousException : public std::exception
            {
                public:
                        virtual const char* what() const throw();
            };

            class ValueOutOfRangeException : public std::exception
            {
                public:
                        virtual const char* what() const throw();
            };

            class NegativeValueException : public std::exception
            {
                public:
                        virtual const char* what() const throw();
            };

            class InvalidDateException : public std::exception
            {
                public:
                        virtual const char* what() const throw();
            };

            class InvalidValueException : public std::exception
            {
                public:
                        virtual const char* what() const throw();
            };

            class InvalidArgumentException : public std::exception
            {
                public:
                        virtual const char* what() const throw();
            };

            class EmptyFieldException : public std::exception
            {
                public:
                        virtual const char* what() const throw();
            };
};

#endif