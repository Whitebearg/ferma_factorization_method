#include "big_number.h"
#include <ctime>

using namespace std;

void ferma_test( int count_of_bit )
{
    int loop_count = 10;

    //int* time_array_main = new int [ loop_count ];

    //int* time_array_ext = new int [loop_count];

    //cout << "Enter the count of bit: ";

    /*int count_of_bit;

    cin >> count_of_bit;

    cout << "Enter k: ";

    int k;

    cin >> k;*/

    for( int i = 0; i < loop_count; i ++ )
    {
        big_number a( count_of_bit, 1 );

        big_number b( count_of_bit, 1 );

       /* printf("a:\n");

        a.dec_print();

        printf("b:\n");

        b.dec_print();*/

        big_number c = a * b;

        cout << "Number " << i + 1 << ": ";

        c.dec_print();

        //cout << "Result: " << endl;

        //int t_main = time(NULL);

        c.ferma_method();

        //t_main = time(NULL) - t_main;

        //time_array_main[ i ] = t_main;

        //int t_ext = time( NULL );

        c.ferma_method_ext( 1 );

        //t_ext = time(NULL) - t_ext;

        //time_array_ext[ i ] = t_ext;
    }

    //cout << "Time array main: " << endl;

    //for( int i = 0; i < loop_count; i ++ )
        //printf("%d ", time_array_main[ i ] );

    //cout << "\n";

    //cout << "Time array extension: " << endl;

    //for( int i = 0; i < loop_count; i ++ )
        //printf("%d ", time_array_ext[ i ] );


}

void dichotomous_rising_test( int loop_count, int base_count_border )
{
    int base_count = 1 + rand()%base_count_border;

    big_number deg("11111111111111111111111111111111411111111111111111111111111111111");

    big_number mod = deg;

    big_number one("1");

    deg = deg - one;

    for( int i = 0; i < loop_count; i++ )
    {
        big_number number( base_count, big_number::RANDOM );

        if( one == big_number::right_left_dichotomous_rising( number, deg, mod ))
        //if( number == big_number::left_right_dichotomus_rising( number, deg, mod ))
            cout <<"Test "<< i + 1 <<" complete! "<< endl;
        else
        {
            cout <<" ERROR! "<< endl;

            number.hex_print();

            deg.hex_print();

            mod.hex_print();

            break;
        }
    }

}

void ferma_iteration_counter()
{
int k;

cout <<"Enter k: ";

cin >> k;

for( int i = 1; i <= 10; i++ )
{
    switch( i )
    {
        cout << i + 1 <<" number :"<< endl;

        case 1:
            {
                big_number number("664448802949411");

                //number.ferma_method();

                number.ferma_method_ext( k );

                break;
            }

        case 2:
            {
                big_number number("611136864568079");

                //number.ferma_method();

                number.ferma_method_ext( k );

                break;
            }

        case 3:
            {
                big_number number("608925499767435");

                //number.ferma_method();

                number.ferma_method_ext( k );

                break;
            }

        case 4:
            {
                big_number number("1124691247186353");

                //number.ferma_method();

                number.ferma_method_ext( k );

                break;
            }

        case 5:
            {
                big_number number("547438706699487");

                //number.ferma_method();

                number.ferma_method_ext( k );

                break;
            }

        case 6:
            {
                big_number number("800056599770259");

                //number.ferma_method();

                number.ferma_method_ext( k );

                break;
            }

        case 7:
            {
                big_number number("587647339601565");

                //number.ferma_method();

                number.ferma_method_ext( k );

                break;
            }

        case 8:
            {
                big_number number("1125519095357685");

               // number.ferma_method();

                number.ferma_method_ext( k );

                break;
            }

        case 9:
            {
                big_number number("1055683458416553");

                //number.ferma_method();

                number.ferma_method_ext( k );

                break;
            }

        case 10:
            {
                big_number number("588704899403841");

                //number.ferma_method();

                number.ferma_method_ext( k );

                break;
            }
    }

    cout <<" ------------ " << endl;

}
}

void barret_single_test()
{
    //big_number number(100, big_number::RANDOM );

    //big_number number("0xFFFFC9AA0C55450AFFFFB486");

    big_number number("0xFFFFBAE1");

    cout <<"Number: "<<endl;

    number.hex_print();

    cout<<""<<endl;

    //big_number mod(50, big_number::RANDOM );

    //big_number mod("0x3C914FE7FA7B1247FFFFFEF9");

    big_number mod("0x9E083EAB");

    cout <<"Mod: "<<endl;

    mod.hex_print();

    cout<<""<<endl;

    //big_number z(1, big_number::RANDOM );

    int z_len = big_number::get_len( mod );

    big_number z;

    z.create_z( z_len );

    z = z / mod;

    cout <<"Z: "<<endl;

    z.hex_print();

    cout<<""<<endl;

    big_number answer = big_number::barrett_mod( number, mod, z );

    cout<<"x mod m: "<<endl;

    answer.hex_print();

    cout << "-------------------" <<endl;

    big_number a = number % mod;

    a.hex_print();


}

void barret_test( int loop_count, int mod_size )
{
    int number_size = 2 * mod_size;

    for( int i = 0; i < loop_count; i++ )
    {
        int m = 1 + rand()%mod_size;

        big_number mod( m, big_number::RANDOM );

        int z_len = big_number::get_len( mod );

        int n = 1 + rand()%(2 * z_len );

        big_number number( n, big_number::RANDOM );

        big_number z;

        z.create_z( z_len );

        z = z / mod;

        big_number answer = big_number::barrett_mod( number, mod, z );

        if( answer == number % mod )
            cout<<"Test "<< i + 1 <<" complete! "<< endl;
        else
            cout<<"ERROR"<<endl;
    }

}

void karatsuba_single_test()
{
    big_number u( 7, big_number::RANDOM );

    cout<<"u: "<<endl;

    u.hex_print();

    cout<<""<<endl;

    big_number v( 2, big_number::RANDOM );

    cout<<"v: "<<endl;

    v.hex_print();

    cout<<""<<endl;

    big_number answer;

    answer = big_number::karatsuba_mul( u, v );

    answer.hex_print();

    cout<<""<<endl;

    big_number a = u * v;

    a.hex_print();


}

void karatsuba_test( int loop_count, int first_number_size, int second_number_size )
{
    int size1 = 1 + rand() % first_number_size;

    int size2 = 1 + rand() % second_number_size;

    for( int i = 0; i < loop_count; i++ )
    {
        big_number first_number( size1, big_number::RANDOM );

        big_number second_number( size2, big_number::RANDOM );

        if( big_number::karatsuba_mul( first_number, second_number ) == ( first_number * second_number ) )
            cout<<"Test "<< i + 1 <<" complete!" <<endl;
    }


}

void miller_rabin()
{
    //big_number a("11111111111111111111111111111111411111111111111111111111111111111");

    //big_number a("211");

    //big_number a("11111111111111111111111111111111411111111111111111111111111111111" );
    //big_number a("8388607");
    big_number a("151217133020331712151");

    a.hex_print();

    if( big_number::miller_rabin_test( a, 20 ) )
    {
        cout << "Number ";

        a.hex_print();

        cout <<"is prime!"<<endl;
    }
    else
    {
        if( a != big_number("1") )
        {
            cout << "Number ";

            a.hex_print();

            cout <<"is composite!"<<endl;
        }
    }
}

void prime_number_generator_test()
{
    big_number a;

    for( int i = 3; i < 41; i++ )
    {
        cout << i << " bit: ";

        a = big_number::prime_number_generator( i );

        a.hex_print();
    }



}

void big_prime_number_generaor( int bits_count )
{
    big_number a;

    a = big_number::prime_number_generator( bits_count );

    a.hex_print();
}

void division_test( int loop_count )
{
    for( int i = 0; i < loop_count; i++ )
    {
        int first_size = 1 + rand()%1000;

        int second_size = 1 + rand()%1000;

        big_number a( first_size, big_number::RANDOM );

        big_number b( second_size, big_number::RANDOM );

        if( a > b)
        {
            big_number c = a/b;

            big_number r = a%b;

            if( ( c * b + r == a ) && ( a - c*b == r) )
                cout << i+1 << "'s is good"<<endl;
            else
            {
                cout<<"bad"<<endl;

                cout << "first_size: " << first_size << endl;

                cout << "second_size: " << second_size << endl;

                a.hex_print();

                b.hex_print();

                break;

            }
        }
        else
        {
            big_number c = b/a;

            big_number r = b%a;

            if( ( c * a + r == b ) && ( b - c*a == r) )
                cout << i+1 << "'s is good"<<endl;
            else
            {
                cout<<"bad"<<endl;

                cout << "first_size: " << first_size << endl;

                cout << "second_size: " << second_size << endl;

                a.hex_print();

                b.hex_print();

                break;

            }
        }



    }

}

void add_sub_test( int loop_count )
{
    for( int i = 0; i < loop_count; i++ )
    {
        int first_size = 1 + rand()%10000;

        int second_size = 1 + rand()%10000;

        big_number a( first_size, big_number::RANDOM );

        big_number b( second_size, big_number::RANDOM );

        if( a > b)
        {
            big_number c = a - b;

            big_number d = c + b;

            if( d == a )
                cout<<"good"<<endl;
            else
            {
                cout<<"bad"<<endl;

                a.hex_print();

                b.hex_print();

                c.hex_print();

                d.hex_print();

                break;
            }

        }
        else
        {
            big_number c = b - a;

            big_number d = c + a;

            if( d == b )
                cout<<"good"<<endl;
            else
            {
                cout<<"bad"<<endl;

                b.hex_print();

                a.hex_print();

                c.hex_print();

                d.hex_print();

                break;
            }
        }


    }
}

int main()
{
    srand( time( 0 ) );

    //add_sub_test( 100000 );

    dichotomous_rising_test( 1000, 5 );

    //barret_single_test();

    //ferma_iteration_counter();

    //barret_test( 1000, 100);

    //karatsuba_single_test();

    //karatsuba_test( 100, 1200, 70 );

    //miller_rabin();

    //division_test( 10000 );

    //prime_number_generator_test();

    //big_prime_number_generaor( 3 );

    return 0;
}
