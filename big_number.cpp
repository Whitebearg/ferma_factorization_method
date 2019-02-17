//
// Created by Никита on 15.03.2018.
//

#include "big_number.h"

#define KARATSUBA_BORDER 2

const int bitsCount = sizeof( base ) * 8;

const base Null = 0;

using namespace std;

void big_number::add_base(base b, int shift)
{
    base overflow = 0;

    m_data[shift] += b;

    if(m_data[shift] < b)
    {
        overflow = 1;

        for(int i = shift+1; i < m_len; i++ )
        {
            m_data[i] += overflow;

            if (m_data[i] != 0)
            {
                overflow = 0;
                break;
            }
        }
    }

    if (overflow == 1)
    {
        if (m_capacity == m_len)
        {

            m_capacity = ceil(m_len*1.5);

            base * new_data = new base [m_capacity];

            memcpy(new_data,m_data,sizeof(base)*m_len);

            delete [] m_data;

            m_data = new_data;

        }

        m_data[m_len] = 1;

        m_len += 1;

    }
}


void big_number::division(const big_number & right1, big_number & q, big_number & r) const
{
    //KHNUT

    doublebase b = 1;

    b <<= sizeof(base)*8;

    big_number left(m_len+1);

    left = *this;

    big_number right(right1.m_len);

    right = right1;

    if(right1.m_len == 1)
    {
        base remainder = 0;

        left.base_div(right.m_data[0],remainder);

        q = left;

        r.m_data[0] = remainder;

        r.m_len = 1;

        return;
    }
    //D1


    base d=1;

    if(right.m_data[right.m_len - 1] < 0x80000000) //2147483648
    {
        d = (base) (b / (right.m_data[right.m_len - 1] + 1 ) );

        big_number d_tmp(1);

        d_tmp.m_data[0] = d;

        left = left * d_tmp;

        right = right * d_tmp;
    }

    left.m_len = m_len + 1;


    //D2 - j = (..)

    int n = right.m_len;
    int m = left.m_len - right.m_len-1;

    for(int j = m; j >= 0 ;j--)
    {
        //D3

        doublebase q_cap = ( (doublebase)left.m_data[j+n] * b + left.m_data[j+n-1] ) / right.m_data[n-1];

        doublebase r_cap = ( (doublebase)left.m_data[j+n] * b + left.m_data[j+n-1] ) % right.m_data[n-1];

        if(q_cap == b || (q_cap*right.m_data[n-2]) > (b*r_cap + left.m_data[j+n-2]) )
        {
            q_cap -= 1;

            r_cap += right.m_data[n-1];

            if(r_cap < b)
            {
                if(q_cap == b || (q_cap*right.m_data[n-2]) > (b*r_cap + left.m_data[j+n-2]) )
                {
                    q_cap -= 1;

                    r_cap += right.m_data[n-1];
                }
            }
        }

        //D4

        big_number tmp(1);

        tmp.m_data[0] = (base)q_cap;

        bool borrow = left.shift_sub(right*tmp,j); // left has been changed.



        //D5

        q.m_data[j] = (base)q_cap;

        if (borrow)
        {
            //printf("BORROW\n");

            q.m_data[j] -= 1;

            left.shift_add(right,j);
        }

    }

    //D8

    for(int i = 0; i < right.m_len; i++)
        r.m_data[i] = left.m_data[i];

    base remainder=0;
    r.base_div(d,remainder);

}

void big_number::Ox_input(const char *text)
{
    int text_lenght = strlen(text);

    m_len = ((text_lenght - 2) + sizeof(base)*2 -1 ) / ( sizeof(base)*2 );

    m_capacity = m_len;

    m_data = new base [ m_len ]; // text = 0xFFFFF ; 8F -> 1 int; => len = (a-2)/8!!!

    for( int i = 0; i < m_len ; i++)
        m_data[i]=0;

    int current_base = 0, current_symbol = 0;

    for(int i = text_lenght - 1; i > 1 ; i--)
    {

        if ( current_symbol == sizeof(base)*2 )
        {
            current_symbol = 0;

            current_base++;
        }


        // fill m_data with blocks of 8 chars (tmp)

        base tmp = 0;

        if( (text[i]>='0') && (text[i]<='9') )
            tmp = text[i]-'0';

        if((text[i]>='A') && (text[i]<='F'))
            tmp = text[i]-'A' + 10;

        tmp <<= ( 4*current_symbol );

        m_data[current_base] = m_data[current_base] | tmp;

        current_symbol++;
    }

    delete_leading_zeroes();

}

void big_number::delete_leading_zeroes()
{
    assert(m_len != 0 && "WRONG BIG NUMBER"); // wtf?
    for (int i = m_len - 1 ; i > 0 && (m_data[i] == 0 ); i--)
        m_len--;
}


big_number big_number::base_mul(base a, base b)
{
    halfbase* p_a = reinterpret_cast<halfbase*>(&a);

    halfbase* p_b = reinterpret_cast<halfbase*>(&b);

    base result, tmp;

    base owerflow = 0;

    halfbase* p_tmp = reinterpret_cast<halfbase*>(&tmp);

    result = p_a[0] * p_b[0];

    //STEP 1 FINISHED

    //STEP 2

    tmp = (base)p_a[1] * p_b[0] + (base)p_a[0] * p_b[1];

    if ( tmp < ((base)p_a[1] * p_b[0]) )
        owerflow += ( 1 << ( sizeof(base)*4 ) );

    result += ((base)(p_tmp[0])) << ( sizeof(base)*4 );

    if ( result < ( ((base)p_tmp[0]) << ( sizeof(base)*4 ) ) )
        owerflow += 1;

    owerflow += p_tmp[1];

    //STEP 2 FINISHED

    //STEP 3

    owerflow += p_a[1] * p_b[1];

    big_number c(2);

    c.m_data[0] = result;

    c.m_data[1] = owerflow;

    c.delete_leading_zeroes();

    return c;
}


void big_number::base_div( base divider, base & remainder)
{
    doublebase dividing = 0;

    for(int i = m_len-1; i >=0; i--)
    {
        dividing <<= (sizeof(base) * 8 );

        dividing += m_data[i];

        base q = dividing / divider;

        m_data[i] = q;

        dividing = dividing % divider;
    }

    remainder = base(dividing);

    delete_leading_zeroes();
}


void big_number::shift_add ( const big_number &a, int shift)  // only for this programm
{
    int overflow = 0;


    for(int i = 0 ; i < a.m_len; i++)
    {
        m_data[i+shift] += a.m_data[i] + overflow;
        if(m_data[i+shift] < a.m_data[i])
            overflow = 1;
        else
            overflow = 0;
    }


    for(int i = a.m_len + shift; i < m_len && overflow; i++)
    {

        m_data[i] += overflow;
        if( m_data[i] < overflow)
            overflow = 1;
        else
            overflow = 0;
    }
}


bool big_number::shift_sub(  const big_number & right, int shift )
{
    int borrow = 0;

    base tmp=0;

    for (int i = 0; i < right.m_len; i++)
    {
        assert(i+shift < m_len);
        tmp = m_data[i+shift] - right.m_data[i] - borrow;
        if (tmp > m_data[i+shift])
            borrow = 1;
        else
            borrow = 0;
        m_data[i+shift] = tmp;
    }

    for(int i = right.m_len + shift; i < m_len && borrow == 1; i++)
    {
        tmp = m_data[i] - borrow;
        if (tmp > m_data[i])
            borrow = 1;
        else
            borrow = 0;
        m_data[i] = tmp;
    }

    delete_leading_zeroes();

    if (borrow == 1)
        return true;
    else
        return false;

}

big_number::big_number( int k, int odevity )
{

    int mask = 1;

    m_len = ( k + 31 ) / 32;

    m_capacity = m_len;

    m_data = new base [ m_len ];

    for(int i = 0; i < m_len; i++) m_data[i] = ( (rand() - rand()) << 16) | ( (rand() - rand()));

    m_data[ 0 ] |= 1;

    mask = mask << k - 1;

    m_data[ m_len - 1 ] |= mask;

    int shift = 32 - k % 32;

    m_data[ m_len - 1 ] = m_data[ m_len - 1 ] << shift >> shift;

}

big_number::big_number(const char* text)
{
    if(strlen(text) == 0)
        throw "INVALID USER";

    if(strlen(text) > 1)
        if(text[0] == '0' && text[1] == 'x')
        {
            Ox_input(text);
            return;
        }

    dec_input(text);

}


big_number::big_number()
{
    m_data = NULL;

    m_len = 0;

    m_capacity = 0;
}

big_number::big_number(int lenght, Filltype filltype)
{

    m_data = new base [lenght];

    m_capacity = lenght;

    m_len = lenght;

    switch(filltype)
    {
        case ZERO: memset(m_data, 0, lenght*sizeof(base)); break;

        case RANDOM: for(int i=0; i< m_len; i++) m_data[i] = ( (rand() - rand()) << 16) | ( (rand() - rand())); break;

        default: assert(!"Invalid Filltype");
    }
}

big_number::big_number( const big_number & a) /*: m_len(a.m_len), m_capacity(a.m_capacity)*/
{
    m_capacity = a.m_capacity;

    m_len = a.m_len;

    m_data = new base[m_capacity];

    for (int i = 0; i < m_len; i++ )

        m_data[i] = a.m_data[i];

    //m_data = new base [a.m_capacity];

    //memcpy(m_data, a.m_data, m_len*(sizeof(base)));
}

big_number::big_number( int num_for_conv, char* type )
{
    m_len = 1;

    m_capacity = 1;

    m_data = new base[ 1 ];

    m_data[ 0 ] = 0;

    m_data[ 0 ] = num_for_conv;
}


big_number& big_number:: operator=( big_number const & a )
{

    if (m_capacity < a.m_len)
    {
        delete [] m_data;

        m_data = new base [a.m_len];

        m_capacity = a.m_len;

    }

    m_len = a.m_len;

    memcpy(m_data, a.m_data, m_len*(sizeof(base)));

    return *this;

}

big_number big_number:: operator+(  const big_number & right ) const
{

    int maxlen = std::max(m_len,right.m_len);
    int minlen = std::min(m_len,right.m_len);

    big_number result(maxlen+1);

    int overflow = 0;


    for (int i = 0; i < minlen; i++)
    {
        result.m_data[i] = m_data[i] + right.m_data[i] + overflow;
        if (result.m_data[i] <= m_data[i])
            overflow = 1;
        else overflow = 0;
    }

    if (m_len > right.m_len)
    {
        for (int i = minlen; i < maxlen; i++)
        {
            result.m_data[i] = m_data[i] + overflow;
            if (result.m_data[i] < m_data[i])
                overflow = 1;
            else overflow = 0;
        }
    }

    else
    {
        for (int i = minlen; i < maxlen; i++)
        {
            result.m_data[i] = right.m_data[i] + overflow;
            if (result.m_data[i] < right.m_data[i])
                overflow = 1;
            else overflow = 0;
        }
    }

    if (overflow == 1)
        result.m_data[result.m_len-1] = 1;

    else
        result.m_len -= 1;

    return result;

}


big_number big_number::operator -(const big_number &in)
{
    int len = m_len;

    big_number Result(len);

    char loan = 0;

    int j = 0;
    for( ; j < in.m_len; ++j )
    {
        Result.m_data[j] = m_data[j] - in.m_data[j] - loan;

        if( Result.m_data[j] >= m_data[j] )
            loan = 1;
        else
            loan = 0;
    }

    for( ; loan != 0 && j < m_len; ++j )
    {
        Result.m_data[j] = m_data[j] - loan;

        if( Result.m_data[j] > m_data[j] )
            loan = 1;
        else
            loan = 0;
    }

    for( ; j < m_len; ++j )
        Result.m_data[j] = m_data[j];

    while( Result.m_data[Result.m_len - 1] == 0 && Result.m_len > 1 )
    {
        Result.m_len--;
    }

    return Result;

}


big_number big_number::operator*( const big_number & right ) const
{
    big_number result(m_len + right.m_len);

    for (int i = 0; i < m_len ; i++)
        for(int j = 0; j < right.m_len ; j++)
        {
            big_number tmp = big_number::base_mul(m_data[i], right.m_data[j]);
            result.shift_add(tmp, i+j);
        }

    result.delete_leading_zeroes();
    return result;
}


void big_number::divbase( base divider, base & remainder)
{
    d_base dividing = 0;

    for(int i = m_len-1; i >=0; i--)
    {
        dividing <<= (sizeof(base) * 8 );

        dividing += m_data[i];

        base q = dividing / divider;

        m_data[i] = q;

        dividing = dividing % divider;
    }

    remainder = base(dividing);

    delete_leading_zeroes();
}

bool big_number::shiftsub(  const big_number & right, int shift )
{
    int borrow = 0;

    base tmp=0;

    for (int i = 0; i < right.m_len; i++)
    {
        assert(i+shift < m_len);
        tmp = m_data[i+shift] - right.m_data[i] - borrow;
        if (tmp > m_data[i+shift])
            borrow = 1;
        else
            borrow = 0;
        m_data[i+shift] = tmp;
    }

    for(int i = right.m_len + shift; i < m_len && borrow == 1; i++)
    {
        tmp = m_data[i] - borrow;
        if (tmp > m_data[i])
            borrow = 1;
        else
            borrow = 0;
        m_data[i] = tmp;
    }

    delete_leading_zeroes();

    if (borrow == 1)
        return true;
    else
        return false;

}


void big_number::shiftadd ( const big_number &a, int shift)
{
    int overflow = 0;

    for(int i = 0 ; i < a.m_len; i++)
    {
        m_data[i+shift] += a.m_data[i] + overflow;
        if(m_data[i+shift] < a.m_data[i])
            overflow = 1;
        else
            overflow = 0;
    }

    for(int i = a.m_len + shift; i < m_len && overflow; i++)
    {

        m_data[i] += overflow;
        if( m_data[i] < overflow)
            overflow = 1;
        else
            overflow = 0;
    }

}


void big_number::div(const big_number & right1, big_number & q, big_number & r) const
{
    d_base b = 1;
    b <<= sizeof(base)*8;


    big_number left(m_len+1);
    left = *this;
    big_number right(right1.m_len);
    right = right1;

    if(right1.m_len == 1)
    {
        base remainder = 0;

        left.divbase(right.m_data[0],remainder);

        q = left;

        r.m_data[0] = remainder;

        r.m_len = 1;

        return;
    }

    //1

    base d=1;

    if(right.m_data[right.m_len - 1] < 0x80000000)
    {
        d = (base) (b / (right.m_data[right.m_len - 1] + 1 ) );

        big_number d_tmp(1);

        d_tmp.m_data[0] = d;

        left = left * d_tmp;

        right = right * d_tmp;
    }

    left.m_len = m_len + 1;


    //2

    int n = right.m_len;
    int m = left.m_len - right.m_len-1;

    for(int j = m; j >= 0 ;j--)
    {
        //3

        d_base q_cap = ( (d_base)left.m_data[j+n] * b + left.m_data[j+n-1] ) / right.m_data[n-1];

        d_base r_cap = ( (d_base)left.m_data[j+n] * b + left.m_data[j+n-1] ) % right.m_data[n-1];

        if(q_cap == b || (q_cap*right.m_data[n-2]) > (b*r_cap + left.m_data[j+n-2]) )
        {
            q_cap -= 1;

            r_cap += right.m_data[n-1];

            if(r_cap < b)
            {
                if(q_cap == b || (q_cap*right.m_data[n-2]) > (b*r_cap + left.m_data[j+n-2]) )
                {
                    q_cap -= 1;

                    r_cap += right.m_data[n-1];
                }
            }
        }

        //4

        big_number tmp(1);

        tmp.m_data[0] = (base)q_cap;

        bool borrow = left.shiftsub(right*tmp,j);

        //5

        q.m_data[j] = (base)q_cap;

        if (borrow)
        {

            q.m_data[j] -= 1;

            left.shiftadd(right,j);
        }

    }

    //8

    for(int i = 0; i < right.m_len; i++)
        r.m_data[i] = left.m_data[i];

    base remainder=0;
    r.divbase(d,remainder);

}



big_number big_number::operator /(const big_number &right ) const
{

    if(m_len < right.m_len)
    {
        big_number q (1);
        q.m_data[0] = 0;
        return q;
    }
    big_number q(m_len-right.m_len+2);
    big_number r(right.m_len);
    div(right,q,r);
    q.delete_leading_zeroes();
    return q;
}



/*
big_number big_number::operator/ ( const big_number & right ) const
{

    if(m_len < right.m_len)
    {
        big_number q (1);
        q.m_data[0] = 0;
        return q;
    }
    big_number q(m_len-right.m_len+2);
    big_number r(right.m_len);
    division(right,q,r);
    q.delete_leading_zeroes();
    return q;
}*/

/*
big_number big_number::operator % ( const big_number & right )
{
    if(m_len < right.m_len)
        return *this;
    big_number q(m_len-right.m_len+2);
    big_number r(right.m_len);
    div(right,q,r);
    q.delete_leading_zeroes();
    return r;
}*/


big_number big_number::operator% ( const big_number & right )
{
    if(m_len < right.m_len)
        return *this;
    big_number q(m_len-right.m_len+2);
    big_number r(right.m_len);
    division(right,q,r);
    //div(right,q,r);
    q.delete_leading_zeroes();
    return r;
}

base& big_number::operator[] ( int i )
{
    if(i >= m_len || i < 0)
    {
        printf("\ni = %d len = %d", i, m_len );
        assert(!"WTF");
    }

    return m_data[i];
}

bool big_number::operator>(const big_number& right) const
{
    if (m_len > right.m_len)
        return true;
    if (m_len < right.m_len)
        return false;
    for (int i = m_len - 1; i >= 0; i--)
    {
        if(m_data[i] > right.m_data[i])
            return true;
        if (m_data[i] < right.m_data[i])
            return false;
    }

    return false;

}

bool  big_number::operator>=( const big_number& right) const
{
    if (m_len > right.m_len)
        return true;
    if (m_len < right.m_len)
        return false;
    for (int i = m_len - 1; i >= 0; i--)
    {
        if(m_data[i] > right.m_data[i])
            return true;
        if (m_data[i] < right.m_data[i])
            return false;
    }

    return true;
}

bool  big_number::operator<( const big_number& right) const
{
    if (m_len > right.m_len)
        return false;
    if (m_len < right.m_len)
        return true;
    for (int i = m_len - 1; i >= 0; i--)
    {
        if(m_data[i] > right.m_data[i])
            return false;
        if (m_data[i] < right.m_data[i])
            return true;
    }

    return false;
}

bool  big_number::operator<=(const big_number& right) const
{
    if (m_len > right.m_len)
        return false;
    if (m_len < right.m_len)
        return true;
    for (int i = m_len - 1; i >= 0; i--)
    {
        if(m_data[i] > right.m_data[i])
            return false;
        if (m_data[i] < right.m_data[i])
            return true;
    }

    return true;
}

bool  big_number::operator==( const big_number& right) const
{
    if (m_len > right.m_len)
        return false;
    if (m_len < right.m_len)
        return false;
    for (int i = m_len - 1; i >= 0; i--)
    {
        if(m_data[i] > right.m_data[i])
            return false;
        if (m_data[i] < right.m_data[i])
            return false;
    }

    return true;
}



bool  big_number::operator!=( const big_number& right) const
{
    if (m_len > right.m_len)
        return true;
    if (m_len < right.m_len)
        return true;
    for (int i = m_len - 1; i >= 0; i--)
    {
        if(m_data[i] > right.m_data[i])
            return true;
        if (m_data[i] < right.m_data[i])
            return true;
    }

    return false;
}

void big_number::dec_input(const char* text)
{

    int text_len = strlen(text);

    m_len = text_len * log2(10)/(sizeof(base)*8) + 1;

    m_data = new base [m_len];

    m_capacity = m_len;

    memset(m_data,0,sizeof(base)*m_len); // fill by nulls

    big_number ten (1);

    big_number milliard (1);

    milliard.m_data[0] = 1000*1000*1000;

    ten.m_data[0] = 10;

    base current_base = 0;

    for(int i = 0; i < text_len / 9 ; i++)
    {
        *this = *this * milliard;

        current_base = 0;

        for(int j = 0; j < 9; j++)
        {
            current_base *= 10;

            current_base += text[i*9+j] - '0';
        }

        add_base(current_base);
    }

    int i = text_len%9;

    big_number ne_milliard (1);

    ne_milliard.m_data[0] = ceil(pow(10,i));


    *this = *this * ne_milliard;

    current_base = 0;

    for(int j = 0; j < i; j++)
    {
        current_base *= 10;

        current_base += text[strlen(text)/9*9+j] - '0';
    }

    add_base(current_base);
}

big_number big_number::poww(const big_number &y, const big_number &mod) const
{
    // declaration
    big_number z(mod.m_len, ZERO), q = *this;

    // step 1
    if ((y.m_data[0] & 1) == 0) z.m_data[0] = 1;

    else z = *this;

    // step 2
    for (int j = 1; j < 8*sizeof(base); j++){

        q = (q * q);
        q = q % mod;
        if ((y.m_data[0] >> j) & 1 == 1) z = (z * q) % mod;
    }

    for (int i = 1; i < m_len; i++)
        for (int j = 0; j < 8*sizeof(base); j++){

            q = q * q % mod;
            if ((y.m_data[i] >> j) & 1 == 1) z = (z * q) % mod;
        }
    return (z % mod);
}

int big_number::get_count_of_bits( const big_number number  )
{
    int count_of_bits = 0;

    int i;

    for( i = 0; i < number.m_len - 1; i ++ )
        count_of_bits = count_of_bits + ( sizeof(base) * 8 );

    int j = 0;

    while( ( number.m_data[ i ] >> j ) != 0 )
    {
        count_of_bits ++;

        j ++;
    }

    return count_of_bits;
}


big_number big_number::left_right_dichotomus_rising( const big_number number, const big_number degree, const big_number mod )
{
    int n = get_count_of_bits( degree );

    // STEP 1

    big_number answer = number;

    // STEP 2

    int j = n % ( sizeof(base) * 8 ) - 2;

    int mask = 1;

    int current_base = degree.m_len - 1;

    for( int i = n - 2; i >= 0; i -- )
    {
        answer = answer * answer % mod;

        if( ( degree.m_data[ current_base ] & ( mask << j ) ) != 0 )
            answer = answer * number % mod;

        j --;

        if( j < 0 )
        {
            current_base --;

            j = sizeof(base) * 8 - 1;
        }
    }

    return answer;

}


big_number big_number::right_left_dichotomous_rising( big_number & number, big_number & degree, big_number & mod )
{
    int z_len = get_len( mod );

    big_number w;

    w.create_z( z_len );

    //w.hex_print();

    w = w / mod;

    //if( number > mod )
        //number = number % mod;

    big_number q = number; // find q value

    big_number one(1, "int");

    base mask = 1;

    int base_index = 0;

    big_number z;

    if( ( degree.m_data[ 0 ] & mask ) == 0 ) // find z value
        z = one;
    else
        z = number;


    int n = get_count_of_bits( degree );

    //cout << n;

    int j = 1;

    for( int i = 1; i < n ; i ++ ) // STEP 2
    {
        //cout <<i<<endl;

        q = q * q;

        //q.hex_print();

        //mod.hex_print();

        //w.hex_print();

        //q = q % mod;

        q = barrett_mod( q, mod, w );

        if( ( ( degree.m_data[ base_index ] >> j ) & mask ) == 1 )
        {
            z = z * q;

            //z = z % mod;

            z = barrett_mod( z, mod, w );

        }

        j ++;

        if( j == sizeof(base)*8 )
        {
            j = 0;

            base_index ++ ;

        }

    }
    return z ;
}

int big_number::get_len( big_number number )
{
    int len = number.m_len;

    return len;
}

big_number big_number::create_z( int len )
{
    big_number z( 2 * len + 1 );

    z.m_data[ 2 * len ] = 1;

    return *this = z;
}

void big_number::delete_zeroes(big_number &A)
{
	while (!A.m_data[A.m_len - 1] && A.m_len > 1)A.m_len--;
	for (int i = 0; i < A.m_len; ++i)
	{
		while (!A.m_data[i] && A.m_len > 1)
		{
			for (int j = 0; j < A.m_len - 1; ++j)
				A.m_data[j] = A.m_data[j + 1];
			A.m_len--;
		}

	}

}


big_number big_number::barrett_mod( big_number number, big_number mod, big_number z )
{
   //FIRST STEP: calculate assumed q
    if( number < mod )
        return number;

    big_number temp_q( number.m_len - mod.m_len + 1 ); // x / b^(k-1)

    for( int i = number.m_len - 1, j = temp_q.m_len - 1; j >= 0; j--, i-- )
        temp_q.m_data[ j ] = number.m_data[ i ];

    //temp_q.hex_print();

    temp_q = temp_q * z; // (x / b^(k-1)) * z

    //temp_q.hex_print(); // i didn't check operator *, but i don't think. that here can be a problem

    big_number assumed_q( temp_q.m_len - mod.m_len - 1); //  ( (x / b^(k-1)) * z ) / b^(k+1)

    for( int i = temp_q.m_len - 1, j = assumed_q.m_len - 1; j >= 0; j--, i-- )
        assumed_q.m_data[ j ] = temp_q.m_data[ i ];

    //assumed_q.hex_print();

    //-------------------------------------------

    //SECOND STEP: calculate r1, r2

    big_number r1( mod.m_len + 1 );     // r1 = x mod b^(k+1)

    for( int i = 0, j = 0; j < r1.m_len && i < number.m_len; j++, i++ )
        r1.m_data[ j ] = number.m_data[ i ];

    //r1.hex_print(); // r1 = 8 base of null

    //r1.m_len = r1.m_len - 1; // try

    big_number temp_r2 = assumed_q * mod;

    //temp_r2.hex_print();

    big_number r2( mod.m_len + 1 );     // r2 = (q' * m) mod b^(k+1)

    for( int i = 0, j = 0; j < r2.m_len && i < temp_r2.m_len; j++, i++ )
        r2.m_data[ j ] = temp_r2.m_data[ i ];

    //r2.hex_print();

    //cout<<"r2.m_len = "<< r2.m_len <<endl;

    //r2.m_len = r2.m_len - 1;  // try

    //-------------------------------------------

    //THIRD STEP: compare r1 and r2

    big_number r;

    big_number b( mod.m_len + 2 );

    b.m_data[ mod.m_len + 1 ] = 1;

   // b.hex_print();

    //cout<<"b.m_len = "<< b.m_len <<endl;

    if( r1 >= r2 )
    {
        //cout<<"1 bla bla bla"<<endl;

        r = r1 - r2;
    }

    else
    {
       // cout<<"2 bla bla bla"<<endl;


        r = b + r1 - r2;
    }


    //r.hex_print();

    //cout<<"r.m_len = "<< r.m_len <<endl;

    //-------------------------------------------

    //FOURTH STEP: decrease r

    while( r >= mod )
    {
        //r.hex_print();

        r = r - mod;

    }


    //-------------------------------------------


    return r;


}

big_number big_number::left_base_shift( int base_count )
{

    big_number result( m_len + base_count );

    for( int i = m_len - 1, j = result.m_len - 1; i >= 0; i --, j -- )
        result.m_data[ j ] = m_data[ i ];

    /*cout<<"Shifted number: "<<endl;

    result.hex_print();

    cout<<""<<endl;*/

    return result;

}

big_number big_number::right_base_shift( int base_count )
{
    if( base_count >= m_len )
    {
        big_number result( 1, ZERO );

        return result;
    }

    big_number result( m_len - base_count );

    for( int i = base_count, j = 0; i < m_len && j < result.m_len; i++, j++ )
        result.m_data[ j ] = m_data[ i ];

    return result;
}

big_number big_number::karatsuba_mul( big_number u, big_number v )
{
    if( ( u.m_len < KARATSUBA_BORDER ) && ( v.m_len < KARATSUBA_BORDER ) )
        return u * v;

    big_number result( u.m_len + v.m_len );

    int max_len;

    if( u.m_len > v.m_len )
        max_len = u.m_len;
    else
        max_len = v.m_len;

    int n = max_len >> 1;

    n = max_len - n;

    int u_size, v_size;

    u_size = n;

    v_size = n;

    if( n > u.m_len )
        u_size = u.m_len;

    if( n > v.m_len )
        v_size = v.m_len;

    big_number u0( u_size ), v0( v_size );

    int j = 0, i = 0;

    for( i = 0; i < u_size; i++, j++ )
        u0.m_data[ i ] = u.m_data[ i ];

    big_number u1( u.m_len - j );

    for( i = 0; i < u1.m_len; i ++, j ++ )
        u1.m_data[ i ] = u.m_data[ j ];

    j = 0;

    for( int i = 0; i < v_size; i++, j++ )
        v0.m_data[ i ] = v.m_data[ i ];

    big_number v1( v.m_len - j );

    for( int i = 0; i < v1.m_len; i ++, j ++ )
        v1.m_data[ i ] = v.m_data[ j ];

    big_number a, b, c;

    a = karatsuba_mul( u1, v1 );

    b = karatsuba_mul( u0, v0 );

    c = karatsuba_mul( (u0 + u1), (v0 + v1) );

    result = ( a.left_base_shift( 2 * n ) ) + ( ( c - a - b).left_base_shift( n ) ) + b;

    while (!result.m_data[result.m_len - 1] && result.m_len > 1)
        result.m_len--;

    return result;

}

int big_number::get_power_of_two( big_number number, big_number &r )
{
    int s = 0;

    int i = 0;

    int j = 0;

    while( number.m_data[ i ] == 0 )
    {
        s = s + sizeof( base ) * 8;

        i ++;
    }

    while( ( (number.m_data[ i ] >> j) & 1 ) == 0 )
    {
        s ++;

        j++;
    }

    r = number;

    r = r >> s;

    return s;
}


bool big_number::miller_rabin_test( big_number number, int t)
{
    int z_len = big_number::get_len( number );

    big_number z;

    z.create_z( z_len );

    z = z / number;

    big_number one("0x1");

    big_number two("0x2");

    if( number == one )
    {
        cout <<"1 is not prime or composite!" << endl;

        return 0;
    }

    if( (number.m_data[ 0 ] & 1) == 0 )
        if( number == big_number("2") )
            return 1;
        else
        {
            cout <<"Wrong number!" << endl;

            return 0;
        }

    big_number r;

    int s = big_number::get_power_of_two( number - one, r );

    cout << "s: " << s << endl;

    cout << "r: ";

    r.hex_print();

    big_number neg_one = number - one;

    big_number b("2");

    for( int i = 0; i < t; i ++ )
    {
        cout<<"b: ";

        b.dec_print();

        if( b > (number - two) )
            return 1;

        big_number y = big_number::right_left_dichotomous_rising( b, r, number );

        cout <<"y: ";

        y.hex_print();

        if( ( y != one ) && ( y != neg_one ) )
        {
            int j = 1;

            cout <<"WTF s: " << s << endl;

            while( ( y != neg_one ) && ( j < s ) )
            {
                //y = y * y % number;

                y = y * y;

                y = big_number::barrett_mod( y, number, z );

                //y.dec_print();

                if( y == one )
                {

                    return 0;
                }


                j ++;
            }

            y.hex_print();

            if( y != neg_one )
            {
                cout << "I finish here" <<endl;

                return 0;
            }

        }

        if( i == 0)
            b = b + one;
        else
            b = b + two;

    }

    return 1;
}

big_number big_number::prime_number_generator( int bits_count )
{
    bool out_flag = 0;

    big_number answer;

    while( out_flag == 0 )
    {
        big_number number( bits_count, 1 );

        out_flag = miller_rabin_test( number, 10 );

        answer = number;
    }

    return answer;
}

void big_number::dec_print()
{
    std::string s = "";

    big_number tmp = *this;

    base print_char;

    big_number ten (1);
    ten.m_data[0] = 10;

    big_number zero (1);
    zero.m_data[0] = 0;

    while(zero != tmp)
    {
        print_char = (tmp % ten).m_data[0];

        s = std::string(1,print_char + '0') + s;

        tmp = tmp/ten;
    }

    std::cout << s << std::endl;
}



void big_number::hex_print()
{

    for(int i = m_len - 1; i >= 0 ; i --)

    {
        printf("%08x ", m_data[i]);


    }

    cout<<""<<endl;

    //printf("\n len = %d\n capacity = %d\n", m_len, m_capacity);
}

void big_number::setLen( int len )
{
    int n = m_len;

    m_len = len;

    if( len > n )
    {
        base *mask = new base[ len ];

        if( n )
            memmove( mask, m_data, n * sizeof( base ) );

        for( int i = n; i < len; i++ )
            mask[ i ] = 0;

        delete [] m_data;

        m_capacity = len;

        m_data = mask;
    }
}

big_number big_number::operator <<= ( const base &x )
{
    base shift_bits_number = x;

    int shift_bases_number = shift_bits_number / bitsCount;

    if( shift_bases_number )
    {
        setLen( m_len + shift_bases_number );

        for( int i = m_len - shift_bases_number; i < m_len; i++ )
            m_data[ i ] = 0;

        for( int i = m_len - 1; i >= 0; i-- )
            if( i < shift_bases_number )
                m_data[ i ] = 0;
            else
                m_data[ i ] = m_data[ i - shift_bases_number ];

        shift_bits_number %= bitsCount;
    }

    if( shift_bits_number )
    {
        int no_shift_bits_number = bitsCount - shift_bits_number;

        base mask = ( 1 << shift_bits_number ) - 1;

        setLen( m_len + 1 );

        m_data[ m_len - 1 ] = 0;

        for( int i = m_len - 1; i >= 0; i-- )
        {
            m_data[ i ] <<= shift_bits_number;

            if( i > 0 )
                m_data[ i ] |= ( m_data[ i - 1 ] >> no_shift_bits_number ) & mask;
        }
    }

    for( int i = m_len - 1; !m_data[ i ] && i > 0; m_len--, i-- );

    return *this;
}

big_number big_number::operator >>( int cout_bits_to_shift )
{
    base j;

    base i = cout_bits_to_shift / (sizeof( base ) * 8);

    base k = cout_bits_to_shift % ( sizeof(base) * 8 );

    for( j = 0; j < i; j++ )
        m_data[ j ] = m_data[ j + i ];

    m_len = m_len - j;

    int index;

    if( m_len == 1 )
    {
        m_data[ 0 ] >>= cout_bits_to_shift;

        return *this;
    }

    int mask1 = 0;

    int mask2 = 0;

    for( index = m_len - 1; index > 0; index -= 2 )
    {
        mask1 <<= ( 32 - k );

        mask2 = m_data[ index ];

        m_data[ index ] >>= k;

        m_data[ index ] = m_data[ index ] | mask1;

        mask2 <<= ( 32 - k );

        mask1 = m_data[ index - 1 ];

        m_data[ index - 1] >>= k;

        m_data[ index - 1 ] = m_data[ index - 1 ] | mask2;
    }

    if( index == 0 )
    {
        mask1 <<= ( 32 - k );

        m_data[ index ] >>= k;

        m_data[ index ] = m_data[ index ] | mask1;
    }

    return *this;
}

big_number big_number::operator >>= ( const base &x )
{
    base shift_bits_num = x;

    int shift_bases_num = shift_bits_num / bitsCount;

    if( shift_bases_num >= m_len )
    {
        setLen( 1 );
        m_data[ 0 ] = 0;

        return *this;
    }

    if( shift_bases_num )
    {
        for( int i = shift_bases_num; i < m_len; i++ )
            m_data[ i - shift_bases_num ] = m_data[ i ];

        setLen( m_len - shift_bases_num);
        shift_bits_num %= bitsCount;

        if( !shift_bits_num )
        {
            for( int i = m_len - 1; !m_data[ i ] && i > 0; m_len--, i-- );

            return *this;
        }
    }

    int no_shift_bits_number = bitsCount - shift_bits_num;

    base mask = ( 1 << shift_bits_num ) - 1;

    for( int i = 0; i <= m_len - 1; i++ )
    {
        m_data[ i ] >>= shift_bits_num;

        if( i < m_len - 1 )
            m_data[ i ] |= ( ( m_data[ i + 1 ] & mask ) << no_shift_bits_number );
    }

    for( int i = m_len - 1; !m_data[ i ] && i > 0; m_len--, i-- );

    return *this;
}


void big_number::count_and_delete_first_zeroes( big_number &a, int &count )
{
    count = 0;

    while( !a.m_data[ count ] )
        count++;

    int i = count;

    count <<= 5;

    base mask = 1;

    while( !( mask & a.m_data[ i ] ) )
    {
        count++;
        mask <<= 1;
    }

    a >>= count;
}

void big_number::swap( big_number &a, big_number &b )
{
    big_number c( a );

    a = b;
    b = c;
}

big_number big_number::gcd( const big_number a, const big_number b )
{
    big_number null("0");

    if( a == null )
        return b;

    if( b == null )
        return a;

    big_number x( a ), y( b );

    int s, k, j;

    count_and_delete_first_zeroes( x, s );

    count_and_delete_first_zeroes( y, k );

    j = min( s, k );

    while( x != y )
    {
        if( x < y )
            swap( x, y );

        x = x - y;

        count_and_delete_first_zeroes( x, s );
    }

    x <<= j;

    return x;
}

big_number big_number::rootsquaring()
{
    big_number x = *this;

    big_number answer = x;

    big_number two( "2" );

    do
    {
        answer = x;

        x = ( ( *this / x ) + x ) / two;

    }
    while( answer > x );

    return answer;
}

big_number big_number::ferma_method()
{
    int iteration_count = 0;

    big_number n = *this;

    big_number x = n.rootsquaring(); // sqrt(x)

    big_number one("1"); big_number two("2"); big_number six("6"); big_number nine("9");

    if( x*x == n )  // if number is a FULL SQUARE
    {
        big_number first_answer = x;

        big_number second_answer = x;

        cout <<"Full square: "<< iteration_count << endl;

        first_answer.dec_print();

        return first_answer;
    }

    x = x + one;

    big_number z = x * x - n;

    big_number y = z.rootsquaring();

    big_number border = (n + nine) / six ;

    while( y * y != z )
    {
        x = x + one;

        //if( x == ( n + big_number("1") ) / big_number("2") )
        if( x > border )
        {
            big_number first_answer = n;

            big_number second_answer("1");

            cout <<"Count of iterations: "<< iteration_count << endl;

            return first_answer;
        }

        z = z + ( (x + x) - one ); // z = z + 2x -1

        y = z.rootsquaring();

        iteration_count ++;

    }

    big_number first_answer = x + y;

    big_number second_answer = x - y;

    big_number diff = first_answer - second_answer;

    cout <<"Count of iterations: "<< iteration_count << endl;

    first_answer.dec_print();

    second_answer.dec_print();

    cout << "Difference: ";

    diff.dec_print();

    cout<<""<<endl;

    return first_answer;
}

void big_number::ferma_method_ext( int k )
{
    int iteration_count = 0;

    big_number n = *this;

    if( !(k & 1) && (( k >> 1 ) & 1)  )
    {
        printf("Wrong k\n");

        return;
    }


    big_number big_k( k, "int");

    //printf("k:");
    //big_k.dec_print();

   // big_n = big_n * big_k;

    big_number x = (big_k * n).rootsquaring();

    if( x*x == big_k * n )  // if number is a FULL SQUARE
    {
        big_number first_answer = x;

        big_number second_answer = x;

        big_number diff = first_answer - second_answer;

        first_answer.dec_print();

        second_answer.dec_print();

        cout <<"Count of iterations: "<< iteration_count << endl;

        second_answer = gcd( first_answer, n );

        printf("Del: ");

        second_answer.dec_print();

        return;
    }

    x = x + big_number("1");

    big_number z = x * x - big_k * n;

    big_number y = z.rootsquaring();

    while( y * y != z )
    {
        x = x + big_number("1");

        if( x == ( /*big_k * */n + big_number("1") ) / big_number("2") )  // обосновать границу, разобраться с n нечетным (n+9)/6
        {
            big_number first_answer = n;

            big_number second_answer("1");

            big_number diff = first_answer - second_answer;

            cout <<"Count of iterations: "<< iteration_count << endl;

            first_answer.dec_print();

            second_answer.dec_print();

            return;
        }

        z = z + ( big_number("2") * x - big_number("1") );

        y = z.rootsquaring();

        iteration_count ++;

    }

    cout <<"Count of iterations: "<< iteration_count << endl;

    big_number first_answer = gcd( x + y, n );
    printf("Del: ");

    first_answer.dec_print();

}




