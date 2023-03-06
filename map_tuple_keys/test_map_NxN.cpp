#include<iostream>
#include<string>
#include<map>
#include<time.h>
using namespace std;


/*
In this file, we test std::map with a custom index class that models tuples of integers.
We suspect that reading from a filled std::map can become expensive for more complex keys
(and a more complex < operator), compared to maps that only use integers.
We enumerate tuples (j,k) of nonnegative integers by a bijection nr() to the nonnegative
integers based on Cantor's diagonal argument: Each key (j,k) sits on the antidiagonal line
of all nonnegative tuples with sum j+k. The number of elements on and below this antidiagonal
is equal to (j+k)*(j+k+1)/2, so that the enumeration
  nr(j,k)=(j+k)*(j+k+1)/2+j
yields the bijection
  (0,0) -> 0,
  (0,1) -> 1,
  (1,0) -> 2,
  (0,2) -> 3,
  ...
    
Simon Wardein, January 2020
Thorsten Raasch, March 2023
*/


class Key{
    public:
    
    int j, k;

    Key(int x, int y)
    {j=x;k=y;}

    
    long int nr()
    {
        return (j+k)*(j+k+1)/2+j;
    }
    bool operator<(const Key& vgl)
    {
        return((j < vgl.j) || ((j == vgl.j) && (k < vgl.k))); 
    }    
    bool operator!=(const Key& vgl)
    {
        return (j!=vgl.j || k!=vgl.k);
    }
    friend ostream &operator<< (ostream &ostr, const Key &o);
    
};

ostream &operator<< (ostream &ostr,const Key& o)
    {
    ostr << '(' << o.j << "," << o.k << ')';
    return ostr;   
    }

// lexicographical comparison of two tuples
struct Key_Compare
{
   bool operator() (const Key& lhs, const Key& rhs) const
   {
       return((lhs.j < rhs.j) || ((lhs.j == rhs.j) && (lhs.k < rhs.k)));
   }
};

// sorting Keys with nr()
struct Key_Compare2
{
   bool operator() (const Key& lhs, const Key& rhs) const
   {
       return((lhs.j+lhs.k)*(lhs.j+lhs.k+1)/2+lhs.j<(rhs.j+rhs.k)*(rhs.j+rhs.k+1)/2+rhs.j);
   }
};

int main(){
// upper bound for N
int N=500;

map<Key,float,Key_Compare> map_Key; //lexicograffical sorting
map<Key,float,Key_Compare2> map_Key2;//sorting with nr()
map<long int, float> map_int;       //standard map with long int 

//filling map
//dur1: Key with Key_compare
//dur2: Key with Key_compare2
//dur3: Integer with nr()

double dur1, dur2, dur3;
clock_t start;
start=clock();

Key It(0,0);
for(int j=0;j<N;j++){
    for(int k=0;k<N;k++)
    {
        It.j=j;
        It.k=k;
        map_Key[It]=1;        
    }
}

dur1=( clock() - start ) / (double) CLOCKS_PER_SEC;
start=clock();

for(int j=0;j<N;j++){
    for(int k=0;k<N;k++)
    {
        It.j=j;
        It.k=k;
        
        map_Key2[It]=1;        
    }
}

dur2=( clock() - start ) / (double) CLOCKS_PER_SEC;
start=clock();

for(int j=0;j<N;j++){
    for(int k=0;k<N;k++){
        It.j=j;
        It.k=k;
        
        map_int[It.nr()]=1;
    }
}

dur3=( clock() - start ) / (double) CLOCKS_PER_SEC;

//reading map

float r=0;

//dur4: Key with Key_compare
//dur5: Key with Key_compare2
//dur6: Integer with nr()

double dur4, dur5, dur6;
//clock_t start;
start=clock();


for(int j=1;j<N;j++){
    for(int k=1;k<N;k++)
    {
          It.j=j;
        It.k=k;
        r=map_Key[It];
    }
}

  dur4=( clock() - start ) / (double) CLOCKS_PER_SEC;
  start=clock();

for(int j=1;j<N;j++){
    for(int k=1;k<N;k++)
    {
        It.j=j;
        It.k=k;
        r=map_int[It.nr()];
    }
}

dur5=( clock() - start ) / (double) CLOCKS_PER_SEC;
start=clock();


for(int j=1;j<N;j++){
    for(int k=1;k<N;k++)
    {
        It.j=j;
        It.k=k;
        r=map_Key[It];
    }
}
dur6=( clock() - start ) / (double) CLOCKS_PER_SEC;



cout<<"\nwriting with Key and Key_compare: "<<dur1<<"s\n";
cout<<"writing with long int and nr():"<< dur2 <<"s\n";
cout<<"writing with Key and Key_compare2: "<<dur3<<"s\n";

cout<<"\nreading with Key and Key_compare: "<<dur4<<"s\n";
cout<<"reading with long int and nr():"<< dur5 <<"s\n";
cout<<"reading with Key and Key_compare2: "<<dur6<<"s\n";

return 0;
}
