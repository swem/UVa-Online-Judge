#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <sstream>
using namespace std ;

#define MaxN 10000

//
// need to include <sstream>, <iomanip>, <cstdlib>, <algorithm>, <string>
//
class bigint {
	friend ostream& operator<<(ostream& os, const bigint& b) {
		using std::ostringstream ;
		ostringstream buffer ;
		buffer << b.v[b.size-1] ;
		for(int i=b.size-2; i>=0; i--)
			buffer << setw(Ndigit) << setfill('0') << b.v[ i ] ;
		os << buffer.str() ;
		return os;
	}
	friend istream& operator>>(istream& is, bigint& b) {
		string	input ;
		int		i = 0 ;
		is >> input ;
		input.push_back( '\0' ) ;
		while( input[ 0 ] == '0' && input[ 1 ] != '\0' ) 
			input.erase( 0, 1 ) ;
		memset(b.v, 0, sizeof(b.v));
		for( 	i=input.length() - Ndigit - 1, b.size = 0 ; i >= 0 ; 
				i-=Ndigit, (b.size) += 1 ) {
			b.v[ b.size ] = atoi( input.c_str() + i ) ;
			input[ i ] = '\0' ;
		}
		if( i + Ndigit != 0 ) {
			b.v[ b.size ] = atoi( input.c_str() ) ;
			(b.size) += 1 ;
		}
		return is;
	}

	public:
		int v[10];
		int size;
		static const int N, Ndigit;
		bigint(int x) {
			memset(v, 0, sizeof(v));
			v[0] = x % N , size = 1 , x /= N ;
			while( x != 0 ) v[ size ] = x % N, size++, x /= N ;
		}
		bigint() {
			memset(v, 0, sizeof(v));
			size = 1;
		}

		int compare(const bigint& x ) {
			if( size != x.size ) return size - x.size ;
			for( int i = size - 1 ; i >= 0 ; i-- ) {
				if( v[ i ] != x.v[ i ] ) return v[ i ] - x.v[ i ] ;
			}
			return 0 ;
		}

		int compare( int x ) {
			bigint bigintX( x ) ;
			return compare( bigintX ) ;
		}

		bool operator>( const bigint &x ) {
			return compare( x ) > 0 ;
		}

		bigint& operator=( const bigint& x ) {
			memset(v, 0, sizeof(v));
			copy( x.v, x.v + x.size, v ) ;
			size = x.size ;
			return *this ;
		}

		bigint& operator=( int x ){
			memset(v, 0, sizeof(v));
			v[0] = x % N , size = 1 , x /= N ;
			while( x != 0 ) v[ size ] = x % N, size++, x /= N ;
			return *this ;
		}

		bigint operator+(const bigint& x) {
			bigint sum = 0 ;
			long long int carry = 0 ;
			int i = 0 ;
			for( i = 0; i < x.size || i < size ; i++) {
				carry += v[ i ] + x.v[i] ;
				if( carry >= N ) sum.v[ i ] = carry - N , carry = 1 ;
				else sum.v[ i ] = carry, carry = 0 ;
			}
			if( carry > 0 ) sum.v[ i ] = carry, i += 1 ;
			sum.size = i;
			return sum ;
		}

		bigint& operator+=(const bigint& x) {
			*this = *this + x ;
			return *this;
		}

		bigint operator*(int x) const {
			bigint mul = 0 ;
			if( x == 0 ) return mul ;
			long long int i = 0, carry = 0 ;
			for(i=0; i<size; i++) {
				carry += (long long int)v[i] * x ;
				mul.v[i] = carry % N ;
				carry /= N ;
			}
			while( carry>0 ) mul.v[i] = carry % N, carry /= N, i++ ;
			mul.size = i ;
			return mul;
		}

		bigint& operator*=(int x) {
			*this = *this * x ;
			return *this;
		}

		bigint operator*(const bigint& x) const {
			bigint mul = 0 ;
			int i = 0 ;
			if( x.size <= size ) {
				for(i=x.size - 1; i>=0 ; i--) {
					mul *= N ;
					mul += *this * x.v[ i ] ;
				}
			}
			else {
				for(i=size - 1; i>=0 ; i--) {
					mul *= N ;
					mul += x * v[ i ] ;
				}
			}
			return mul ;
		}

		bigint& operator*=(const bigint& x) {
			*this = *this * x ;
			return *this;
		}

		bigint operator-(const bigint& x ) {
			bigint sub ;
			long long int i = 0, carry = 0 ;
			for( i = 0 ; i < x.size ; i++ ) {
				carry = v[ i ] - x.v[ i ] + carry ;
				if( carry < 0 ) sub.v[ i ] = carry + N, carry = -1 ;
				else sub.v[ i ] = carry, carry = 0 ;
			}
			for( ; i < size ; i++ ) {
				carry = v[ i ] + carry ;
				if( carry < 0 ) sub.v[ i ] = carry + N, carry = -1 ;
				else sub.v[ i ] = carry, carry = 0 ;
			}
			sub.size = size ;
			while( sub.v[ sub.size - 1 ] == 0 && sub.size > 1 )
				sub.size -= 1 ;
			return sub ;
		}

		bigint& operator-=(const bigint& x ) {
			*this = *this - x ;
			return *this ;
		}

		void divideBy2() {
			long long int i = 0, carry = 0 ;
			for( i = size - 1 ; i >= 0 ; i-- ) {
				carry = v[ i ] + carry * N ;
				v[ i ] = carry / 2 ;
				carry %= 2 ;
			}
			if( v[ size - 1 ] == 0 && size > 1 ) size-- ;
		}

		bigint operator/(const bigint& x ) {
			bigint quotient ;
			bigint remainder = *this ;
			bigint quotientPart( 1 ) ;
			bigint Xpow2s = x ;

			while( remainder.compare( Xpow2s ) > 0 ) {
				Xpow2s += Xpow2s, quotientPart += quotientPart ;
			}

			while( remainder.compare( x ) >= 0 )
			{
				if( remainder.compare( Xpow2s ) >= 0 ) 
					remainder -= Xpow2s, quotient += quotientPart ;
				Xpow2s.divideBy2(), quotientPart.divideBy2() ;
			}

			return quotient ;
		}

		bigint operator%(const bigint& x ) {
			bigint quotient ;
			bigint remainder = *this ;
			bigint quotientPart( 1 ) ;
			bigint Xpow2s = x ;

			while( remainder.compare( Xpow2s ) > 0 ) {
				Xpow2s += Xpow2s, quotientPart += quotientPart ;
			}

			while( remainder.compare( x ) >= 0 )
			{
				if( remainder.compare( Xpow2s ) >= 0 ) 
					remainder -= Xpow2s, quotient += quotientPart ;
				Xpow2s.divideBy2(), quotientPart.divideBy2() ;
			}

			return remainder ;
		}
};

const int bigint::N = 1000000000;
const int bigint::Ndigit=9;

int main()
{
	bigint	hanoi3[ MaxN + 1 ] ;
	bigint	hanoi4[ MaxN + 1 ] ;
	int		n = 0 ;
	bigint	temp ;
	int i = 0 ;
	int j = 0 ;
	int k = 0 ;

	hanoi3[ 1 ] = 1 ;
	for( i = 2 ; i <= MaxN ; i++ )
	{
		hanoi3[ i ] = hanoi3[ i - 1 ] * 2 + 1 ;
	}

	hanoi4[ 1 ] = 1 ;
	for( i = 2 ; i <= MaxN ; i++ )
	{
		hanoi4[ i ] = hanoi4[ i - 1 ] * 2 + hanoi3[ 1 ] ;

		for( k = i - 2 ; k >= 1 ; k-- )
		{
			temp = hanoi4[ k ] * 2 + hanoi3[ i - k ] ;
			if( hanoi4[ i ] > temp )
			{
				hanoi4[ i ] = temp ;
			}
			else
			{
				break ;
			}
		}
	}

	while( cin >> n )
	{
		cout << hanoi4[ n ] << endl ;
	}

	return 0 ;
}

