/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 12:21:11 by apintus           #+#    #+#             */
/*   Updated: 2025/01/31 12:02:24 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

BitcoinExchange::BitcoinExchange()
{
    std::string     line;
    std::ifstream   file("data.csv");

    if (!file.is_open())
        throw NoDataException();
    //Check Empty file
    if (file.peek() == std::ifstream::traits_type::eof())
        throw EmptyFileException();
    //Check file header
    std::getline(file, line);
    if (line != "date,exchange_rate")
        throw InvalidDataException();
    
    while (std::getline(file, line))
    {
        std::string         date;
        double              value;
        std::stringstream   ss(line);

        std::getline(ss, date, ',');
        ss >> value;
        if (ss.fail() | ss.bad())
            throw InvalidDataException();
        try {
            _isValidDate(date);
        }
        catch (std::exception &e){
            throw InvalidDataException();
        }
        _data[date] = value;
    }
    file.close();
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &copy)
{
    *this = copy;
}

BitcoinExchange::~BitcoinExchange()
{
}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &assign)
{
    if (this != &assign)
        _data = assign._data;
    return (*this);
}

//Functions
void    BitcoinExchange::_isValidDate(const std::string &date)
{
    if (date.size() != 10 || date[4] != '-' || date[7] != '-')
        throw InvalidDateException();
    //Convert char->int
    int year = ft_stoi(date.substr(0, 4));
    int month = ft_stoi(date.substr(5, 2));
    int day = ft_stoi(date.substr(8, 2));

    if (month < 1 || month > 12 || day < 1 || day > 31)
        throw InvalidDateException();
    //years bissextile
    if (month == 2)
    {
        if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
        {
            if (day > 29)
                throw InvalidDateException();
        }
        else
        {
            if (day > 28)
               throw InvalidDateException();
        }
    }
    else if (month == 4 || month == 6 || month == 9 || month == 11)
    {
        if (day > 30)
            throw InvalidDateException();
    }
    
    //future
    time_t  now = time(0);
    tm *ltm = localtime(&now);
    if (year > 1900 + ltm->tm_year)
        throw InvalidDateException();
    if (year == 1900 + ltm->tm_year && month > 1 + ltm->tm_mon)
        throw InvalidDateException();
    if (year == 1900 + ltm->tm_year && month == 1 + ltm->tm_mon && day > ltm->tm_mday)
        throw InvalidDateException();

}

double  BitcoinExchange::_isValidValue(const std::string &str)
{
    if (str.empty() || std::count(str.begin(), str.end(), '.') > 1)
        throw InvalidValueException();
    
    //Convert char->double
    double ret = ft_stod(str);

    if (ret < 0)
        throw NegativeValueException();
    if (ret > 1000)
        throw ValueOutOfRangeException();
    return (ret);
}

//Convert functions
int     BitcoinExchange::ft_stoi(const std::string &str)
{
    int                 ret;
    std::stringstream   ss(str);
    
    ss >> ret;
    if (ss.fail() || ss.bad())
        throw InvalidArgumentException();
    return (ret);
}

double     BitcoinExchange::ft_stod(const std::string &str)
{
    double                 ret;
    std::stringstream   ss(str);
    
    ss >> ret;
    if (ss.fail() || ss.bad())
        throw InvalidArgumentException();
    return (ret);
}

//Main function
void    BitcoinExchange::exchange(const char *fileName)
{
    std::string line;
    std::ifstream   file(fileName);

    if (!file.is_open())
        throw NoDataException();
    //Check Empty file
    if (file.peek() == std::ifstream::traits_type::eof())
        throw EmptyFileException();
    //Check file header
    std::getline(file, line);
    if (line != "date | value")
        throw InvalidDataException();

    while (std::getline(file, line))
    {
        std::string         date;
        std::string         amount;
        std::stringstream   ss(line);

        std::getline(ss, date, '|');
        date.erase(std::remove(date.begin(), date.end(), ' '), date.end());
        
        std::getline(ss, amount);
        amount.erase(std::remove(amount.begin(), amount.end(), ' '), amount.end());
        
        bool dateValidFlag = true;
        try {
            if (line.empty())
                throw EmptyFieldException();
            _isValidDate(date);
        }
        catch (std::exception &e){
            std::cerr << "Error: " << e.what() << " => " << date << std::endl;
            dateValidFlag = false;
        }
        
        if (dateValidFlag)
        {
            try{
                double value = _isValidValue(amount);
                if (this->_data.count(date))
                    std::cout << date << " => " << value << " = " << value * this->_data[date] << std::endl;
                else
                {
                    std::map<std::string, double>::const_iterator it = this->_data.upper_bound(date);
                    if (it == this ->_data.begin())
                        throw NoPreviousException();
                    else
                    {
                        --it;
                        std::cout << date << " => " << value << " = " << value * it->second << std::endl;
                    }
                } 
            }
            catch (std::exception &e){
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }
    file.close();
}


//Exceptions
const char *BitcoinExchange::NoDataException::what() const throw()
{
    return ("No data found");
}

const char *BitcoinExchange::InvalidDataException::what() const throw()
{
    return ("Invalid data found");
}

const char *BitcoinExchange::EmptyFileException::what() const throw()
{
    return("Empty file");
}

const char *BitcoinExchange::NoPreviousException::what() const throw()
{
    return ("No previous data found");
}

const char *BitcoinExchange::ValueOutOfRangeException::what() const throw()
{
    return ("too large number.");
}

const char *BitcoinExchange::NegativeValueException::what() const throw()
{
    return ("not a positive number.");
}

const char *BitcoinExchange::InvalidDateException::what() const throw()
{
    return ("Invalid date");
}

const char *BitcoinExchange::InvalidValueException::what() const throw()
{
    return ("Invalid value");
}

const char *BitcoinExchange::InvalidArgumentException::what() const throw()
{
    return ("Invalid argument");
}

const char *BitcoinExchange::EmptyFieldException::what() const throw()
{
    return ("Empty field");
}