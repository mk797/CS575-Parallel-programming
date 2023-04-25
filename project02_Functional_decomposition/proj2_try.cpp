#include <stdlib.h>
#include<stdio.h>
#include <omp.h>
#include <cmath>


omp_lock_t  Lock;
volatile int    NumInThreadTeam;
volatile int    NumAtBarrier;
volatile int    NumGone;


void    InitBarrier( int );
void    WaitBarrier( );

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

void WaitBarrier( )
{
        omp_set_lock( &Lock );
        {
                NumAtBarrier++;
                if( NumAtBarrier == NumInThreadTeam )
                {
                        NumGone = 0;
                        NumAtBarrier = 0;
                        // let all other threads get back to what they were doing
            // before this one unlocks, knowing that they might immediately
            // call WaitBarrier( ) again:
                        while( NumGone != NumInThreadTeam-1 );
                        omp_unset_lock( &Lock );
                        return;
                }
        }
        omp_unset_lock( &Lock );

        while( NumAtBarrier != 0 ); // this waits for the nth thread to arrive

        #pragma omp atomic
        NumGone++;          // this flags how many threads have returned
}


void InitBarrier( int n )
{
        NumInThreadTeam = n;
        NumAtBarrier = 0;
    omp_init_lock( &Lock );
}


float Sqr( float x )
{
        return x*x;
}




void Rabbits()
{

    while( NowYear < 2029 )
    {
    // compute a temporary next-value for this quantity
    // based on the current state of the simulation:
    

    // DoneComputing barrier:
    nextNumRabbits = NowNumRabbits;
    int carryingCapacity = (int)( NowHeight );
    if( nextNumRabbits < carryingCapacity )
        nextNumRabbits++;
    else
        if( nextNumRabbits > carryingCapacity )
                nextNumRabbits--;

    if( nextNumRabbits < 0 )
        nextNumRabbits = 0;

    WaitBarrier( );
     NowNumRabbits = nextNumRabbits;

    // DoneAssigning barrier:
    WaitBarrier( ); 
    
//    fprintf(stderr, "%d,", NowNumRabbits );

    // DonePrinting barrier:
    // NowMonth++;
    // if(NowMonth==11)
    // {
    //     NowMonth=0;
    //     NowYear++;
	WaitBarrier();
    }
    

    
    
}



void RyeGrass()
{
    while( NowYear < 2029 )
    {
    // compute a temporary next-value for this quantity
    // based on the current state of the simulation:
    

    // DoneComputing barrier:
    nextHeight = NowHeight;
    nextHeight += tempFactor * precipFactor * RYEGRASS_GROWS_PER_MONTH;
    nextHeight -= (float)NowNumRabbits * ONE_RABBITS_EATS_PER_MONTH;
 
    if( nextHeight < 0. ) nextHeight = 0.;
    WaitBarrier( );
    NowHeight = nextHeight;

    // DoneAssigning barrier:
    WaitBarrier( ); 
  //  fprintf(stderr, "%lf,", NowHeight );

    // DonePrinting barrier:
    WaitBarrier( ); 
    }

}


void Watcher()
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

    WaitBarrier();
    NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );
    NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );

    WaitBarrier();
  
 fprintf(stderr, "%d,%d,%lf,%lf,%lf\n", NowNumRabbits,NowNumWolves, 2.54*NowHeight,NowPrecip,((5./9.)*(NowTemp-32))  );
 // fprintf(stderr, "%d,%lf,%lf,%lf \n",NowNumRabbits, NowHeight, NowPrecip, NowTemp );
    // fprintf(stderr, "\n\n");
    //  fprintf(stderr, "%d,%d\n", NowMonth, NowYear);

      NowMonth++;
    if(NowMonth==11)
    {
        NowMonth=0;
        NowYear++;
    }
    WaitBarrier();
    }

}


void Wolf()
{
    while( NowYear < 2029 )
    {
   
        NextNumWolves = NowNumWolves;

        if(NowNumRabbits ==0)
        {
            NextNumWolves = 0;   
        }
        else if(NowNumRabbits > NowNumWolves)
        {
            NextNumWolves++;
        }
        else
        {
            NextNumWolves--;

        }
    
//printf("Now rabbits: %d Now wolves: %d next wolves:%d \n", NowNumRabbits,NowNumWolves, NextNumWolves);
    WaitBarrier( );
     NowNumWolves = NextNumWolves;

    // DoneAssigning barrier:
    WaitBarrier( ); 
 
	WaitBarrier();

    }
}



int main()
{

NowMonth =    0;
NowYear  = 2023;
NowNumRabbits = 1;
NowHeight =  5.;
NowNumWolves = 1;

    omp_set_num_threads( 4 );   // or 4
    InitBarrier( 4 );       // or 4
    
    // compute a temporary next-value for this quantity
    // based on the current state of the simulation:
    fprintf(stderr, "rabbits,wolves,height, prec, temp\n");
     #pragma omp parallel sections
    {
        #pragma omp section
        {
            Rabbits( );
        }

        #pragma omp section
        {
            RyeGrass( );
        }

         #pragma omp section
        {
            Watcher( );
        }

        #pragma omp section
        {
            Wolf( );
        }


        
   
    }
    

}
