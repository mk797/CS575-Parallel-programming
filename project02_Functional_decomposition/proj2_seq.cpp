#
#include <stdlib.h>
#include<stdio.h>
#include <omp.h>
#include <cmath>




int NowYear;        // 2023 - 2028
int NowMonth;       // 0 - 11
float   NowPrecip;      // inches of rain per month
float   NowTemp;        // temperature this month
float   NowHeight;      // rye grass height in inches
int NowNumRabbits;      // number of rabbits in the current population
int nextNumRabbits;
float nextHeight;
float tempFactor;
float precipFactor;

int NextNumWolves;
int NowNumWolves;


const float RYEGRASS_GROWS_PER_MONTH =      20.0;
const float ONE_RABBITS_EATS_PER_MONTH =     1.0;

const float AVG_PRECIP_PER_MONTH =         12.0;    // average
const float AMP_PRECIP_PER_MONTH =      4.0;    // plus or minus
const float RANDOM_PRECIP =         2.0;    // plus or minus noise

const float AVG_TEMP =              60.0;   // average
const float AMP_TEMP =              20.0;   // plus or minus
const float RANDOM_TEMP =           10.0;   // plus or minus noise

const float MIDTEMP =               60.0;
const float MIDPRECIP =             14.0;

unsigned int seed = 0;

float Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

  

float Sqr( float x )
{
        return x*x;
}







void seq()
{
    while(NowYear < 2029)
    {

    float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
    float temp = AVG_TEMP - AMP_TEMP * cos( ang );
    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
   
    if( NowPrecip < 0. )
        NowPrecip = 0.;

    tempFactor = exp(   -Sqr(  ( NowTemp - MIDTEMP ) / 10.  )   );
    precipFactor = exp(   -Sqr(  ( NowPrecip - MIDPRECIP ) / 10.  )   );


    NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );
    NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );

    //grass
    NowHeight += tempFactor * precipFactor * RYEGRASS_GROWS_PER_MONTH;
    NowHeight -= (float)NowNumRabbits * ONE_RABBITS_EATS_PER_MONTH;
    if( NowHeight < 0. ) NowHeight = 0.;


    //rabbits
    int carryingCapacity = (int)( NowHeight );
    if( NowNumRabbits < carryingCapacity )
        NowNumRabbits++;
    else
        if( NowNumRabbits > carryingCapacity )
                NowNumRabbits--;

    if( NowNumRabbits < 0 )
        NowNumRabbits = 0;

   
    //wolves 
     if(NowNumRabbits ==0)
        {
            NowNumWolves = 0;   
        }
        else if(NowNumRabbits > NowNumWolves)
        {
            NowNumWolves++;
	        NowNumRabbits--;
        }
        else
        {
            NowNumWolves--;

        }




  
 fprintf(stderr, "%d,%d,%lf,%lf,%lf\n", NowNumRabbits,NowNumWolves, 2.54*NowHeight,NowPrecip,((5./9.)*(NowTemp-32))  );
 // fprintf(stderr, "%d,%lf,%lf,%lf \n",NowNumRabbits, NowHeight, NowPrecip, NowTemp );
    // fprintf(stderr, "\n\n");
    //  fprintf(stderr, "%d,%d\n", NowMonth, NowYear);

      NowMonth++;
    if(NowMonth==12)
    {
        NowMonth=0;
        NowYear++;
    }
    }

}






int main()
{

NowMonth =    0;
NowYear  = 2023;
NowNumRabbits = 1;
NowHeight =  5.;
NowNumWolves = 1;

    
    
    // compute a temporary next-value for this quantity
    // based on the current state of the simulation:
    fprintf(stderr, "rabbits,wolves,height, prec, temp\n");
    seq();
    

}
