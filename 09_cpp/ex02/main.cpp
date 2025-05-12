/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 12:14:59 by apintus           #+#    #+#             */
/*   Updated: 2025/01/31 13:42:50 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"

int main(int ac, char **av)
{
    if (ac < 2)
    {
        std::cerr << "Exemple usage: ./PmergeMe 3 5 9 7 4" << std::endl;
        return (1);
    }
    try
    {
        PmergeMe merge;
        merge.parsing(av);
        if (merge.isSorted())
        {
            std::cout << "List already sorted !" << std::endl;
            return (0);
        }
        std::cout << "Before:" << std::endl;
        merge.displaySort();
        merge.sortVector();
        merge.sortList();
        std::cout << std::endl;
        std::cout << "After:" << std::endl;
        merge.displaySort();
        merge.displayTime();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}