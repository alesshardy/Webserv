/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:39:48 by apintus           #+#    #+#             */
/*   Updated: 2025/01/31 14:51:52 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMERGEME_HPP
# define PMERGEME_HPP

# include <iostream>
# include <vector>
# include <list>
# include <sys/time.h>
# include <sstream>

class PmergeMe
{
    private:
            std::vector<int>    _vector;
            std::list<int>      _list;
            long                _vectorTime;
            long                _listTime;
            long                _parsingTime;

            void    _parsing(const char **av);
            
            void    _mergeSortV(std::vector<int> &highs, int start, int end);
            void    _mergeV(std::vector<int> &highs, int left, int mid, int right);
            int     _binarySearchV(std::vector<int> &highs, int low);

            void    _mergeSortL(std::list<int> &highs, int start, int end);
            void    _mergeL(std::list<int> &highs, int left, int mid, int right);
            int     _binarySearchL(std::list<int> &highs, int low);
            
            std::list<int>::iterator    _advance(std::list<int>::iterator it, int n);
            
    public:
            PmergeMe();
            PmergeMe(const PmergeMe &copy);
            ~PmergeMe();
            
            PmergeMe &operator=(const PmergeMe &assign);
            
            void    parsing(char **av);
            void    displaySort();
            void    displayTime();
            void    sortVector();
            void    sortList();
            bool    isSorted();
            
            class InvalidArgumentException : public std::exception
            {
                public:
                        virtual const char * what() const throw();
            };
};

#endif