#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>

/*
 As one of the core ingredients of the AMSTeL library, we will use a C++
 implementation of infinite scalar-valued sequences over a countable index set
 with only finitely many nontrivial entries. Mathematically speaking, a minimal
 implementation provides something like c_0, the space of all finitely
 supported, real- or complex-valued sequences over the natural numbers.
 
 In this design test program, we will work out a minimal C++ implementation of
 such objects, which will be called InfiniteVector. In order to use as much
 well-tested code from the standard library as possible, we intend to derive
 the class InfiniteVector from std::map and endow it with some standard linear
 algebra functionality (BLAS level 1 and 2, essentially). Using std::map
 as a protected base class leaves open the option to exchange the internal
 implementation of InfiniteVector, e.g., by a hashed map like std::unordered_map
 or even a custom container class (just change the template parameter CONTAINER)
 
 Main design goals and problems:
 1) Read and write access should be at most of O(N) complexity, N being the
    number of nonzero entries. This is ensured by the standard implementation of
    std::map as a red/black tree. We are aware of the fact that read access via
    std::map::operator [] alone may insert new, unwanted entries into the
    infinite vector. Therefore, since we want to use the notation [] (and not
    methods like std::map::at() which have been available only since C++11),
    we will expose read and write access to the protected base class std:: map
    access to it via custom access functions like get_coefficient()
    and set_coefficient().
 2) We want the class InfiniteVector to be an STL-compliant associative
    container class, meaning that all algorithms that can be applied to
    std::map itself should also work for InfiniteVector.
    For example, comparison of two instances of InfiniteVector should be
    feasible with std::equal from <algorithm> (which was the main reason to
    write this design test program). This requires the existence of suitable
    forward iterator classes by which one can walk through the nontrivial
    entries.
 3) We enable the user to exchange the base container class by a third
    template argument CONTAINER, defaulting to std::map<I,C>.
 
 Thorsten Raasch, November 2018 and March 2023
 */

using std::cout;
using std::endl;

// forward declaration of InfiniteVector iterators
template <class C, class I, class CONTAINER> class InfiniteVectorConstIterator;

template <class C, class I=int, class CONTAINER=std::map<I,C> >
class InfiniteVector
  : protected CONTAINER
{
public:
  friend class InfiniteVectorConstIterator<C,I,CONTAINER>;
  typedef InfiniteVectorConstIterator<C,I,CONTAINER> const_iterator;
  
  typedef typename CONTAINER::value_type value_type;
  
  InfiniteVector()
  : CONTAINER()
  {
  }
  
  InfiniteVector(const CONTAINER& source)
  : CONTAINER(source)
  {
  }
  
  const_iterator begin() const
  {
    return const_iterator(*this, CONTAINER::begin());
  }
  
  const_iterator end() const
  {
    return const_iterator(*this, CONTAINER::end());
  };
  
  size_t size() const
  {
    return CONTAINER::size();
  };
  
  bool operator == (const InfiniteVector<C,I,CONTAINER>& v) const
  {
#if 1
    // this implementation is desirable (using code from <algorithm>),
    // but did not compile under macOS until modifying InfiniteVectorConstIterator::operator *
    return (size()==v.size()) && std::equal(this->begin(), this->end(), v.begin());
#else
    const_iterator it(begin()), vit(v.begin());
    do
    {
      if (it==end())
        return (vit==v.end());
      if (vit==v.end())
        return false;
    } while (*it++ == *vit++);
    return false;
#endif
  }
};

template<class C, class I, class CONTAINER>
std::ostream& operator << (std::ostream& os, const InfiniteVector<C,I,CONTAINER>& v)
{
  if (v.begin() == v.end())
  {
    os << "0" << std::endl;
  }
  else
  {
    for (typename InfiniteVector<C,I,CONTAINER>::const_iterator it(v.begin());
         it != v.end(); ++it)
    {
      os << it.index() << ": " << it.value() << std::endl;
    }
  }
  
  return os;
}

template <class C, class I, class CONTAINER>
class InfiniteVectorConstIterator
: protected CONTAINER::const_iterator
{
public:
  typedef typename CONTAINER::const_iterator::iterator_category iterator_category;
  typedef typename CONTAINER::const_iterator::difference_type difference_type;
  typedef typename CONTAINER::const_iterator::value_type value_type;
  typedef typename CONTAINER::const_iterator::pointer pointer;
  typedef typename CONTAINER::const_iterator::reference reference;
  
private:
  const InfiniteVector<C,I,CONTAINER>& _container;
  
public:
  InfiniteVectorConstIterator(const InfiniteVector<C,I,CONTAINER>& container,
                              typename CONTAINER::const_iterator state)
  : CONTAINER::const_iterator(state), _container(container)
  {
  }

  bool operator == (const InfiniteVectorConstIterator<C,I,CONTAINER>& it) const
  {
    return (static_cast<typename CONTAINER::const_iterator>(*this)
            == static_cast<typename CONTAINER::const_iterator>(it));
  }
  
  bool operator != (const InfiniteVectorConstIterator<C,I,CONTAINER>& it) const
  {
    return !(*this == it);
  }
  
  InfiniteVectorConstIterator<C,I,CONTAINER>& operator ++ ()
  {
    CONTAINER::const_iterator::operator ++ ();
    return *this;
  }
 
  InfiniteVectorConstIterator<C,I,CONTAINER> operator ++ (int step)
  {
    InfiniteVectorConstIterator<C,I,CONTAINER> r(*this);
    CONTAINER::const_iterator::operator ++ (step);
    return r;
  }
  
  InfiniteVectorConstIterator<C,I,CONTAINER>& operator -- ()
  {
    CONTAINER::const_iterator::operator -- ();
    return *this;
  }
  
  InfiniteVectorConstIterator<C,I,CONTAINER> operator -- (int step)
  {
    InfiniteVectorConstIterator<C,I,CONTAINER> r(*this);
    CONTAINER::const_iterator::operator -- (step);
    return r;
  }
  
  const I& index() const
  {
    return (CONTAINER::const_iterator::operator * ()).first;
  }

  const C& value() const
  {
    return (CONTAINER::const_iterator::operator * ()).second;
  }

  const reference operator * () const
  {
    return CONTAINER::const_iterator::operator * ();
  }
  
  const pointer operator -> () const
  {
    return CONTAINER::const_iterator::operator -> ();
  }
};

template <class C, class I=int, class CONTAINER=std::map<I,C> >
class AnotherInfiniteVector
: public CONTAINER
{
public:
  // Note that the class std::map already has an operator ==, which we
  // override here to check whether std::equal works
  bool operator == (const AnotherInfiniteVector<C,I>& v) const
  {
    return (this->size() == v.size()) && std::equal(this->begin(),this->end(),v.begin());
  }
};

template <class C, class I=int, class CONTAINER=std::map<I,C> >
class YetAnotherInfiniteVector
: protected CONTAINER
{
public:
  // Note that the class std::map already has an operator ==, which we
  // override here to check whether std::equal works
  bool operator == (const YetAnotherInfiniteVector<C,I>& v) const
  {
    return (this->size() == v.size()) && std::equal(this->begin(),this->end(),v.begin());
  }
};

template <typename Pair>
struct second_equal_to
: std::unary_function<const Pair&, bool>
{
  second_equal_to(const typename Pair::second_type& value)
  : value_(value) { }
  
  bool operator()(const Pair& p) const
  {
    return p.second == value_;
  }
  
private:
  typename Pair::second_type value_;
};

int main()
{
  InfiniteVector<double,int> v; // default CONTAINER=std::map<I,C>
  InfiniteVector<double,int,std::unordered_map<int,double> > z; // custom choice of CONTAINER
  
  // test default constructor and operator <<
  cout << "- a zero vector v:" << endl
    << v;
  cout << "- a zero vector z, using std::unordered_map as CONTAINER:" << endl
    << z;
  
  // test constructor from std::map
  std::map<int,double> wmap;
  wmap[42]=23.0;
  wmap[123]=23.0;
  InfiniteVector<double,int> w(wmap);
  cout << "- a vector w created via std::map :" << endl
    << w;

  // test constructor from custom CONTAINER, e.g., from std::unordered_map
  std::unordered_map<int,double> umap;
  umap[42]=23.0;
  umap[123]=23.0;
  InfiniteVector<double,int,std::unordered_map<int,double> > u(umap);
  cout << "- a vector u created via std::unordered_map :" << endl
    << u;

  // test operator == on AnotherInfiniteVector
  AnotherInfiniteVector<double,int> a,b; // container class is public
  a[1]=2.5;
  b[2]=2.5;
  cout << "- are the vectors a and b equal?" << endl;
  if (a==b)
    cout << "  ... yes!" << endl;
  else
    cout << "  ... no!" << endl;

  // test operator == on YetAnotherInfiniteVector
  YetAnotherInfiniteVector<double,int> ay,by; // container class is protected
  cout << "- are the vectors ay and by equal?" << endl;
  if (ay==by)
    cout << "  ... yes!" << endl;
  else
    cout << "  ... no!" << endl;
  
  // test operator ==
  cout << "- are the vectors v and w equal?" << endl;
  if (v==w)
    cout << "  ... yes!" << endl;
  else
    cout << "  ... no!" << endl;
  
  // test std::count_if() algorithm for std::map
  const double number=23.0;
  cout << "- wmap contains "
  << std::count_if(wmap.begin(), wmap.end(), second_equal_to<std::map<int,double>::value_type>(number))
  << " times the number " << number << endl;
  
  // test std::count_if() algorithm for InfiniteVector
  cout << "- w contains "
  << std::count_if(w.begin(), w.end(), second_equal_to<InfiniteVector<double,int>::value_type>(number))
  << " times the number " << number << endl;

  // test std::count_if() algorithm for InfiniteVector with custom container
  cout << "- u contains "
  << std::count_if(u.begin(), u.end(), second_equal_to<InfiniteVector<double,int,std::unordered_map<int,double> >::value_type>(number))
  << " times the number " << number << endl;

  return 0;
}
