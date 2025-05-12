/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:39:50 by apintus           #+#    #+#             */
/*   Updated: 2025/01/31 15:17:57 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"

PmergeMe::PmergeMe(): _vectorTime(0), _listTime(0), _parsingTime(0)
{
}

PmergeMe::PmergeMe(const PmergeMe &copy)
{
    *this = copy;
}

PmergeMe::~PmergeMe()
{
}

PmergeMe &PmergeMe::operator=(const PmergeMe &assign)
{
    if (this != &assign)
    {
        _vector = assign._vector;
        _list = assign._list;
        _vectorTime = assign._vectorTime;
        _listTime = assign._listTime;
        _parsingTime = assign._parsingTime;
    }
    return (*this);
}

//Convert function
int ft_stoi(const std::string &str)
{
    int                 ret;
    std::stringstream   ss(str);

    ss >> ret;
    if (ss.fail() || ss.bad())
		throw std::invalid_argument("Invalid argument");
    return (ret);
}

//Parsing
void    PmergeMe::parsing(char **av)
{
    struct timeval start;
    struct timeval end;
    int i = 1;
    int value = 0;
    
    gettimeofday(&start, NULL);
    while (av[i])
    {
        value = ft_stoi(av[i]);
        if (value < 0)
        {
            throw InvalidArgumentException();
            return ;
        }
        _vector.push_back(value);
        _list.push_back(value);
        i++;
    }
    gettimeofday(&end, NULL);
    _parsingTime = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
}

//Display (acommenter l' un des deux pour conformer affichage)
void    PmergeMe::displaySort()
{
    std::cout << "Vector: ";
    for (std::vector<int>::iterator it = _vector.begin(); it != _vector.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "List: ";
    for (std::list<int>::iterator it = _list.begin(); it != _list.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

void    PmergeMe::displayTime()
{
    std::cout << "Time to parse arguments " << _parsingTime << " µs " << std::endl;
    std::cout << "Time to process a range of " << _vector.size() << " elements with std::vector : " << _vectorTime << " µs " << std::endl;
    std::cout << "Time to process a range of " << _list.size() << " elements with std::list : " << _listTime << " µs " << std::endl;
}
//Algo
bool    PmergeMe::isSorted()
{
    std::vector<int> vector = _vector;
    std::list<int> list = _list;

    for (size_t i = 0; i < vector.size() - 1; i++)
    {
        if (vector[i] > vector[i + 1])
            return (false);
    }

    //Pas de size() sur list
    for (std::list<int>::iterator it = list.begin(); it != list.end(); ++it)
    {
        if (*it > *_advance(it, 1) && _advance(it, 1) != list.end())
            return (false);
    }
    return (true);
}

/************************************************************************************************************************************/
//VECTOR
void    PmergeMe::_mergeV(std::vector<int> &big, int left, int mid, int right)
{
    int sizeL = mid - left + 1; // size left tab
    int sizeR = right - mid; // size right tab
    std::vector<int> L(sizeL); 
    std::vector<int> R(sizeR);
    
    //field the tab
    for (int i = 0; i < sizeL; i++)
        L[i] = big[left + i];
    for (int j = 0; j < sizeR; j++)
        R[j] = big[mid + 1 + j];

    int i = 0;
    int j = 0;
    int k = left;

    while (i < sizeL && j < sizeR)
    {
        if (L[i] <= R[j])
        {
            big[k] = L[i];
            i++;
        }
        else
        {
            big[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < sizeL)
    {
        
        big[k] = L[i];
        i++;
        k++;
    }
    while (j < sizeR)
    {
        
        big[k] = R[j];
        j++;
        k++;
    }
}

void    PmergeMe::_mergeSortV(std::vector<int> &big, int start, int end)
{
    if (start < end)
    {
        int mid = (start + end) / 2;
        _mergeSortV(big, start, mid);
        _mergeSortV(big, mid + 1, end);
        _mergeV(big, start, mid, end);
    }
}

int PmergeMe::_binarySearchV(std::vector<int> &big, int small)
{
    int start = 0;
    int end = big.size() - 1;
    int mid;

    while (start <= end)
    {
        mid = (start + end) / 2;
        if (big[mid] == small)
            return (mid);
        if (big[mid] < small)
            start = mid +1;
        else
            end = mid - 1;
    }
    return (start);
}

void    PmergeMe::sortVector()
{
    struct timeval start;
    struct timeval end;
    std::vector<int>    big;
    std::vector<int>    small;
    
    gettimeofday(&start, NULL);
    
    // Pre Sort in 2
    int size = _vector.size();
    if (size % 2)
    {
        size--;
        big.push_back(_vector[size]);
    }
    for (int i = 0; i < size; i += 2)
    {
        if (_vector[i] > _vector[i + 1])
        {
            big.push_back(_vector[i]);
            small.push_back(_vector[i + 1]);
        }
        else
        {
            big.push_back(_vector[i + 1]);
            small.push_back(_vector[i]);
        }
    }
    
    // sorte big nb by fusion
    _mergeSortV(big, 0, big.size() - 1);

    // insert small in big wtih binarieSearch
    int insertPos;
    for (size_t i = 0; i < small.size(); i++)
    {
        insertPos = _binarySearchV(big, small[i]);
        big.insert(big.begin() + insertPos, small[i]);
    }
    
    _vector = big;

    gettimeofday(&end,NULL);
    _vectorTime = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
}

/************************************************************************************************************************************/
//LIST
void    PmergeMe::_mergeL(std::list<int> &big, int left, int mid, int right)
{
    int sizeL = mid - left + 1;
    int sizeR = right - mid;
    std::list<int> L;
    std::list<int> R;
    
    //field the tab
    std::list<int>::iterator it = _advance(big.begin(), left);
    for (int i = 0; i < sizeL; i++)
    {
       L.push_back(*it);
       ++it; 
    }
    it = _advance(big.begin(), mid + 1);
    for (int i = 0; i < sizeR; i++)
    {
        R.push_back(*it);
        ++it;
    }
    
    int i = 0;
    int j = 0;
    it = _advance(big.begin(), left);
    std::list<int>::iterator itL = L.begin();
    std::list<int>::iterator itR = R.begin();

    while (i < sizeL && j < sizeR)
    {
        if (*itL <= *itR)
        {
            *it = *itL;
            ++itL;
            i++;
        }
        else
        {
            *it = *itR;
            ++itR;
            j++;
        }
        ++it;
    }
    while (i < sizeL)
    {
        *it = *itL;
        itL++;
        it++;
        i++;
    }
    while (j < sizeR)
    {
        *it = *itR;
        itR++;
        it++;
        j++;
    }
}

void    PmergeMe::_mergeSortL(std::list<int> &big, int start, int end)
{
    if (start < end)
    {
        int mid = start  + (end - start) / 2;
        _mergeSortL(big, start, mid);
        _mergeSortL(big, mid + 1, end);
        _mergeL(big, start, mid, end);
    }
}

int PmergeMe::_binarySearchL(std::list<int> &big, int small)
{
    int start = 0;
    int end = big.size() - 1;
    int mid;
    
    while (start <= end)
    {
        mid = (start + end) / 2;
        std::list<int>::iterator it = big.begin();
        std::advance(it, mid);
        if (*it == small)
            return (mid);
        if (*it < small)
            start = mid + 1;
        else
            end = mid - 1;
    }
    return (start);
}

void    PmergeMe::sortList()
{
    struct timeval start;
    struct timeval end;
    std::list<int>  big;
    std::list<int>  small;
 
    gettimeofday(&start, NULL);
    
    int size = _list.size();
    if (size % 2)
    {
        size--;
        big.push_back(*_list.rbegin());
    }
    
    std::list<int>::iterator it = _list.begin();
    for (int i = 0; i < size; i += 2)
    {
        if (*it > *_advance(it, 1))
        {
            big.push_back(*it);
            small.push_back(*_advance(it, 1));
        }
        else
        {
            big.push_back(*_advance(it, 1));
            small.push_back(*it);
        }
        it = _advance(it, 2);
    }

    // sort big by fusion
    _mergeSortL(big, 0, big.size() - 1);

    // insert small in big wtih binarieSearch
    int insertPos;
    for (std::list<int>::iterator it = small.begin(); it != small.end(); ++it)
    {
        insertPos =  _binarySearchL(big, *it);
        std::list<int>::iterator itB = big.begin();
        std::advance(itB, insertPos);
        big.insert(itB, *it);
    }

    _list = big;
    
    gettimeofday(&end, NULL);
    _listTime = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
}

//Iterator advance
std::list<int>::iterator    PmergeMe::_advance(std::list<int>::iterator it, int n)
{
    std::advance(it, n);
    return (it);
}

//Exceptions
const char* PmergeMe::InvalidArgumentException::what() const throw()
{
    return ("Error");
}
