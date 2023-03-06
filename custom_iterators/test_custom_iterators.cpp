#include <iostream>
#include <vector>
#include <iterator>

/*
 The goal of this design test program is to implement a class with
 custom STL-compatible iterators and to check under which conditions
 generic algorithms from <algorithm> like std::equal do compile.
 The reason to setup such a test was that std::equal did not work properly
 in (an old version of) InfiniteVector::operator == () when compiling with
 CLang under macOS.
 
 Thorsten Raasch, November 2018
 */

using std::cout;
using std::endl;

// forward declaration for PrimeIterator
template<int N> class PrimeIterator;

template<int N>
class PrimeContainer
{
public:
  friend class PrimeIterator<N>;
  typedef PrimeIterator<N> iterator;

  PrimeContainer ()
  : _sieve(N)
  {
    // run Eratosthenes' sieve algorithm
    _sieve[0] = false; // 1 is not prime
    for (int k=2; k<=N; k++)
      _sieve[k-1] = true; // mark potential primes
    
    for (int k=2; k*k<=N;)
      {
        // unmark all multiples of k
        for (int j=2; j*k<=N; j++)
          _sieve[j*k-1] = false;
        
        // advance k to next prime
        for (k++; !_sieve[k-1] && k*k<N; k++);
      }
    
    // store maximal prime
    for (_maxprime=N; _maxprime>2 && !_sieve[_maxprime-1]; _maxprime--);
    
    // store number of primes
    _nprimes=0;
    for (int k=1; k<=N; k++)
      if (_sieve[k-1])
        _nprimes++;
  }
  
  bool is_prime(const int k) const
  {
    return _sieve[k-1];
  }
  
  // the list of primes begins with 2
  iterator begin() const
  {
    return iterator(*this, 2);
  }
  
  iterator end() const
  {
    return iterator(*this, N+1);
  }
  
  size_t size() const
  {
    return _nprimes;
  }
  
private:
  std::vector<bool> _sieve; // sieve for natural numbers 1,...,N
  int _maxprime;
  size_t _nprimes;
};

template<int N>
std::ostream& operator << (std::ostream& os, const PrimeContainer<N>& p)
{
#if 0
  // implementation without using the nested iterator class
  for (int k=1; k<=N; k++)
  {
    if (p.is_prime(k))
      os << k << " ";
  }
  os << std::endl;
#else
  // implementation with the nested iterator class
  for (typename PrimeContainer<N>::iterator it(p.begin()); it!=p.end(); ++it)
  {
    os << *it << " ";
  }
#endif
  return os;
}

template <int N>
class PrimeIterator
{
public:
  typedef std::forward_iterator_tag iterator_category;
  typedef int difference_type;
  typedef int value_type;
  typedef value_type& reference;
  typedef value_type* pointer;
private:
  const PrimeContainer<N>& _container; // parent container
  int _k; // current state (= current prime)
public:
  PrimeIterator(const PrimeContainer<N>& container, const int k)
  : _container(container), _k(k)
  {
  }
  
  bool operator == (const PrimeIterator<N>& it) const
  {
    return (_k== it._k);
  }

  bool operator != (const PrimeIterator<N>& it) const
  {
    return !(*this == it);
  }
  
  PrimeIterator<N>& operator ++ ()
  {
    if (_k==_container._maxprime)
    {
      // jump to end()
      _k=N+1;
    }
    else // _k<_container._maxprime
    {
      // jump to next prime (there is at least one)
      _k++;
      for (; !(_container._sieve[_k-1]); _k++);
    }
    return *this;
  }
  
  reference operator * ()
  {
    return _k;
  }
  
  pointer operator -> ()
  {
    return &_k;
  }
};

int main()
{
  const int M=23;
  PrimeContainer<M> p;
  cout << "- the primes from 2 to " << M << ":" << endl;
  cout << p << endl;
  cout << "- these are " << p.size() << " prime numbers" << endl;
  
  const int M1=M-1;
  PrimeContainer<M1> q;
  cout << "- the primes from 2 to " << M1 << ":" << endl;
  cout << q << endl;
  cout << "- these are " << q.size() << " prime numbers" << endl;

  // use std::equal to test whether these sets of prime numbers are equal
  cout << "- these two sets of primes are ";
  if ((p.size()==q.size()) && std::equal(p.begin(), p.end(), q.begin()))
    cout << "equal!" << endl;
  else
    cout << "different!" << endl;
}
