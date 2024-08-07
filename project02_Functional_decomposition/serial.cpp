#include <stdlib.h>
#include <stdio.h>
#include <cmath>

int NowYear;        // 2023 - 2028
int NowMonth;       // 0 - 11
float NowPrecip;    // inches of rain per month
float NowTemp;      // temperature this month
float NowHeight;    // rye grass height in inches
int NowNumRabbits;  // number of rabbits in the current population
int NowNumWolves;

const float RYEGRASS_GROWS_PER_MONTH = 20.0;
const float ONE_RABBITS_EATS_PER_MONTH = 1.0;

const float AVG_PRECIP_PER_MONTH = 12.0;    // average
const float AMP_PRECIP_PER_MONTH = 4.0;     // plus or minus
const float RANDOM_PRECIP = 2.0;            // plus or minus noise

const float AVG_TEMP = 60.0;                // average
const float AMP_TEMP = 20.0;                // plus or minus
const float RANDOM_TEMP = 10.0;             // plus or minus noise

const float MIDTEMP = 60.0;
const float MIDPRECIP = 14.0;

unsigned int seed = 0;

float Ranf(unsigned int *seedp, float low, float high) {
    float r = (float)rand_r(seedp);          // 0 - RAND_MAX

    return (low + r * (high - low) / (float)RAND_MAX);
}

float Sqr(float x) {
    return x * x;
}

int main() {
    NowMonth = 0;
    NowYear = 2023;
    NowNumRabbits = 1;
    NowHeight = 5.;
    NowNumWolves = 1;

    //fprintf(stderr, "Rand1, Rand2, rabbits,wolves,height, prec, temp\n");

    while (NowYear < 2029) {
        //compute rabbit next value
        int nextNumRabbits = NowNumRabbits;
        int carryingCapacity = (int)(NowHeight);
        if (nextNumRabbits < carryingCapacity)
            nextNumRabbits++;
        else if (nextNumRabbits > carryingCapacity)
            nextNumRabbits--;

        if (nextNumRabbits < 0)
            nextNumRabbits = 0;
        // rabbit assignign next to now
        NowNumRabbits = nextNumRabbits;


        //compute height next value
        float nextHeight = NowHeight;
        float tempFactor = exp(-Sqr((NowTemp - MIDTEMP) / 10.));
        float precipFactor = exp(-Sqr((NowPrecip - MIDPRECIP) / 10.));
        nextHeight += tempFactor * precipFactor * RYEGRASS_GROWS_PER_MONTH;
        nextHeight -= (float)NowNumRabbits * ONE_RABBITS_EATS_PER_MONTH;
        if (nextHeight < 0.) nextHeight = 0.;

        //height assign next to now
        NowHeight = nextHeight;


        // compute next wolves value
        int NextNumWolves = NowNumWolves;
        if (NowNumRabbits == 0) {
            NextNumWolves = 0;
        } else if (NowNumRabbits > NowNumWolves) {
            NextNumWolves++;
            NowNumRabbits--;
        } else {
            NextNumWolves--;
        }

        // assign next wolves value to now
        NowNumWolves = NextNumWolves;

        float randValue1 = Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);
        float randValue2 = Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);

        fprintf(stderr, "%lf,%lf,%d,%d,%lf,%lf,%lf\n", randValue1, randValue2, NowNumRabbits, NowNumWolves, 2.54 * NowHeight, NowPrecip, ((5. / 9.) * (NowTemp - 32)));

        NowMonth++;
        if (NowMonth == 12) {
            NowMonth = 0;
            NowYear++;
        }

        float ang = (30. * (float)NowMonth + 15.) * (M_PI / 180.);
        float temp = AVG_TEMP - AMP_TEMP * cos(ang);
        float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);

        if (NowPrecip < 0.)
            NowPrecip = 0.;
        tempFactor = exp(-Sqr((NowTemp - MIDTEMP) / 10.));
        precipFactor = exp(-Sqr((NowPrecip - MIDPRECIP) / 10.));

        NowTemp = temp + randValue1;
        NowPrecip = precip + randValue2;
    }

    return 0;
}

