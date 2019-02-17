//
// Created by Никита on 15.03.2018.
//
#ifndef BIG_NUMBER_H
#define BIG_NUMBER_H


#include <iostream>
#include <stdio.h>
#include <exception>
#include <stdexcept>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <string.h>
#include <fstream>
#include <sstream>





typedef unsigned int base;
typedef unsigned long long doublebase;
typedef unsigned short halfbase;
typedef unsigned long int d_base;



class big_number
{
private:

    base* m_data;

    base m_len;

    base m_capacity;

    void add_base(base b, int shift = 0);

    void division(const big_number & right1, big_number & q, big_number &r) const;

    void Ox_input(const char *text);

    void dec_input(const char* text);

    void delete_leading_zeroes();

    static void count_and_delete_first_zeroes( big_number &a, int &count ); // но это не точно

    void shift_add ( const  big_number & a, int shift);

    bool shift_sub(  const big_number & right, int shift );

    static big_number base_mul(base a, base b);

    void base_div( base divider, base & remainder);

    void setLen( int len ); // но это не точно

    static void swap( big_number &a, big_number &b ); // и это не точно

    static big_number gcd( const big_number a, const big_number b );




void divbase( base divider, base & remainder);
bool shiftsub(  const big_number & right, int shift );
void shiftadd ( const big_number &a, int shift);
void div(const big_number & right1, big_number & q, big_number & r) const;




public:

    enum Filltype
    {
        ZERO = 0,
        RANDOM

    };

    big_number();

    big_number( int k, int odevity );

    big_number(const char* text);

    big_number( int num_for_conv, char* type ); // convert integer into big number

    explicit big_number(int lenght, Filltype filltype = ZERO);

    big_number( const big_number & a);


    static big_number left_right_dichotomus_rising( const big_number number, const big_number degree, const big_number mod );



    static big_number right_left_dichotomous_rising( big_number & number, big_number & degree, big_number & mod );

    static big_number barrett_mod( big_number number, big_number mod, big_number z );

    static big_number karatsuba_mul( big_number u, big_number v );

big_number poww(const big_number &y, const big_number &mod) const;


    int function2(big_number &r) const;
    static big_number Degree(big_number &x, big_number &y, big_number &mod);




    static bool miller_rabin_test( big_number number, int t);

    static big_number prime_number_generator( int bits_count );

    big_number left_base_shift( int base_count );

    big_number right_base_shift( int base_count );

    big_number create_z( int len );

    big_number rootsquaring();

    static int get_count_of_bits( const big_number number  );

    static int get_power_of_two( big_number number, big_number &r );

    static int get_len( big_number number );

    static void delete_zeroes(big_number &A);

    big_number ferma_method();

    void ferma_method_ext( int k );

    void dec_print();

    void hex_print();

    big_number & operator=( big_number const & a );

    big_number operator+( const big_number & right ) const;

    big_number operator -(const big_number &in);



void checkLength();

    big_number operator*( const big_number & right ) const;



    //big_number operator /(const big_number &right );

    big_number operator/ ( const big_number & right) const;



    big_number operator % ( const big_number & right );

    //big_number operator% ( const big_number & right1 ) const;





    big_number operator>>= ( const base &x );

    big_number operator<<= ( const base &x );

    big_number operator >>( int cout_bits_to_shift );



    base& operator[] ( int i );

    bool operator> ( const big_number& right) const;

    bool operator>=( const big_number& right) const;

    bool operator<( const big_number& right) const;

    bool operator<=(const big_number& right) const;

    bool operator==( const big_number& right) const;

    bool operator!=( const big_number& right) const;

    ~big_number()
    {
        delete [] m_data;
    }

};

#endif
