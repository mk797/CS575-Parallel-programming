#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int	NowYear;		// 2023 - 2028
int	NowMonth;		// 0 - 11
int	month;
float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// rye grass height in inches
int	NowNumRabbits;		// number of rabbits in the current population
int	ForestPoachers;		// number of poachers for rabbits and rye grass

const float RYEGRASS_GROWS_PER_MONTH =		20.0;
const float ONE_RABBITS_EATS_PER_MONTH =	 1.0;

const float AVG_PRECIP_PER_MONTH =	       12.0;	// average
const float AMP_PRECIP_PER_MONTH =		4.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				60.0;
const float MIDPRECIP =				14.0;

omp_lock_t	Lock;
volatile int	NumInThreadTeam;
volatile int	NumAtBarrier;
volatile int	NumGone;

unsigned int seed = 0;

void	InitBarrier( int );
void	WaitBarrier( );

float
Sqr( float x )
{
        return x*x;
}

float
Ranf( unsigned int *seedp,  float low, float high )
{
       float r = (float) rand_r( seedp );              // 0 - RAND_MAX

      return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

//float
//Ranf( float low, float high )
//{
//        float r = (float) rand( );              // 0 - RAND_MAX
//
//       return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
//}

void
InitBarrier( int n )
{
        NumInThreadTeam = n;
        NumAtBarrier = 0;
        omp_init_lock( &Lock );
}

// have the calling thread wait here until all the other threads catch up:

void
WaitBarrier( )
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

        while( NumAtBarrier != 0 );     // this waits for the nth thread to arrive

        #pragma omp atomic
        NumGone++;                      // this flags how many threads have returned
}

void
RyeGrass()
{
while( NowYear < 2029 )
	{
	// compute a temporary next-value for this quantity
	// based on the current state of the simulation

	float tempFactor = exp(   -Sqr(  ( NowTemp - MIDTEMP ) / 10.  )   );
	float precipFactor = exp(   -Sqr(  ( NowPrecip - MIDPRECIP ) / 10.  )   );	

	float nextHeight = NowHeight;
        nextHeight += tempFactor * precipFactor * RYEGRASS_GROWS_PER_MONTH;
        nextHeight -= (float)NowNumRabbits * ONE_RABBITS_EATS_PER_MONTH;

        if( nextHeight < 0. )
		 nextHeight = 0.;


	// DoneComputing barrier:
	WaitBarrier( );

	NowHeight =  nextHeight;
	
	// DoneAssigning barrier:
	WaitBarrier( );

	// DonePrinting barrier:
	WaitBarrier( );	
	}
}

void
Rabbits()
{
while( NowYear < 2029 )
	{
	// compute a temporary next-value for this quantity
	// based on the current state of the simulation:
	int nextNumRabbits = NowNumRabbits;
	int carryingCapacity = (int)( NowHeight );
	if( nextNumRabbits < carryingCapacity )
        	nextNumRabbits++;
	else
        	if( nextNumRabbits > carryingCapacity )
                	nextNumRabbits--;

	if( nextNumRabbits < 0 )
        	nextNumRabbits = 0;

	// DoneComputing barrier:
	WaitBarrier( );
	
	NowNumRabbits =  nextNumRabbits;
	// DoneAssigning barrier:
	WaitBarrier( );

	// DonePrinting barrier:
	WaitBarrier( );

	}
}

void
Poaching()
{
while ( NowYear < 2029)
	{
	int poachers = ForestPoachers;
	int grass = (int)( NowHeight*2.54 );

	if ( NowNumRabbits > poachers )
		//if ( grass > 30)
			poachers++;

	 if ( NowNumRabbits <  poachers )
		//if ( grass < 30)
                        poachers--;

	if ( poachers < 0)
		poachers = 0;

	// DoneComputing barrier:
        WaitBarrier( );
	
	ForestPoachers = poachers ;

	 // DoneAssigning barrier:
        WaitBarrier( );

        // DonePrinting barrier:
        WaitBarrier( );
	}
}

void
Watcher()
{
while( NowYear < 2029 )
	{
	// compute a temporary next-value for this quantity
	// based on the current state of the simulation:
	
	// DoneComputing barrier:
	WaitBarrier( );

	// DoneAssigning barrier:
	WaitBarrier( );

	int month = NowMonth;

	fprintf(stderr,"%d,%d,%6.3f,%6.3f,%6.3f,%d,%d\n",NowYear,month,(5./9.)*(NowTemp-32),(NowPrecip*2.54),(NowHeight*2.54), NowNumRabbits, ForestPoachers);
	NowMonth++;
	

	if(NowMonth % 12 == 0)
		{
			//NowMonth = 0;
			NowYear++; 
		}
	

	float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

	float temp = AVG_TEMP - AMP_TEMP * cos( ang );
	NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP );

	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
	NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP );
	
	if( NowPrecip < 0. )
        	NowPrecip = 0.;

	// DonePrinting barrier:
	WaitBarrier( );
	}
}

// specify how many threads will be in the barrier:
//	(also init's the Lock)

int
main( int argc, char *argv[ ] )
{

// starting date and time:

NowMonth =    0;
NowYear  = 2023;

// starting state (feel free to change this if you want):

NowNumRabbits = 1;
NowHeight =  5.;

ForestPoachers = 0;

omp_set_num_threads( 4 );	// same as # of sections
InitBarrier( 4 );

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
		Poaching( );	// your own
	}
}       // implied barrier -- all functions must return in order
	// to allow any of them to get past here
}
