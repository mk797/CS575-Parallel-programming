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
    nextHeight += tempFactor * precipFactor * RYEGRASS_GROWS_PER_MONTH;
    nextHeight -= (float)NowNumRabbits * ONE_RABBITS_EATS_PER_MONTH;
    if( nextHeight < 0. ) nextHeight = 0.;


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


