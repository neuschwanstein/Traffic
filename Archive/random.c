/*--------------------------------------------------------------------------
  random.c
  ------------------------------------------------------------------------- */


#define   LENGTH      /* 1279 */  250   /* magic number */
#define   LAG         /* 1063 */  103   /* magic number */

#include "random.h"

#include <math.h>
/*#include <values.h> */


static unsigned   irnd[LENGTH];    /* array for Kirkpatrick Stoll generator */
static int        k=0, kq = LAG;
static double     max_unsigned = UINT_MAX;   /* from limits.h */ 
static int        seed;            /* for successful seeding */


/* ------------------------------------------------------------------------
   Initialize the random number generator
   ------------------------------------------------------------------------ */

int init_kps ( int iseed )
{
  /* Initialization */

  int       i, j, is, i_half, i_try=0;
  unsigned  mask;
  
  /* constants for a congruential random number generator */
  /* numerical recipes in c, page 209, 211                */

  int im = 243000;
  int ia = 4561;
  int ic = 51349;

  is     = (iseed + ic) % im;
  i_half = im / 2;


  do { 

    /* seed the KS generator bitwise */

    for ( i=0; i< LENGTH; i++ )
      {
	irnd[i] = 0;
	for ( j=0; j < (sizeof(unsigned)*8); j++ )
	  {
	    if ( is >= i_half ) /* set the bit to one */
	      mask = 1;
	    else 
	      mask = 0;
	    
	    mask   <<= j; 
	    irnd[i] |= mask;
	    
	    /* new random number */
	    is = (is * ia + ic) % im;
	  }
      }
	
    /* check the most obvious problems */
    mask = 0;
    for (i = 0; i < LENGTH; i++) 
      mask |= irnd[i];
    if (mask != ~0u ) 
      i_try++;
    
  } while ( mask != ~0u );  
  
  k  =       0; 
  kq =     LAG;
  seed = iseed;
  return i_try;
}


/*--------------------------------------------------------------------------
  Random number generator
  --------------------------------------------------------------------------*/

unsigned kps_unsigned ()
{	

  k++;  if ( k  >= LENGTH ) k  = 0;
  kq++; if ( kq >= LENGTH ) kq = 0;

  return ( irnd[k] ^= irnd[kq] );
}


double kps_double ()
{

  k++;  if ( k  >= LENGTH ) k  = 0;
  kq++; if ( kq >= LENGTH ) kq = 0;

  /* divide by the largest unsigned integer, the  returned 
     numbers will be from the CLOSED interval [0,1] */

  return ( (irnd[k] ^= irnd[kq]) / max_unsigned );
}  


int kps_int ()
{
  k++;  if ( k  >= LENGTH ) k  = 0;
  kq++; if ( kq >= LENGTH ) kq = 0;


  /* this kills the sign bit, before it returns the random number,
     all returned numbers will be positive */

  return ( (irnd[k] ^= irnd[kq]) & INT_MAX );

}


/* ------------------------------------------------------------------- 
   random number generator for Gaussian random numbers (allen tildesley)
   ------------------------------------------------------------------- */

double kps_gauss ()
{
  /* the generator makes calls to the initialized Kirkpatrick-Stoll 
     generator coded above, thus the KPS generator MUST BE initialized, in 
     order for the Gussian generator to work 
   */

  static int    last_returned;  /* pair index of the last r.n. returned */
  static double u1, u2, v1, v2;

  if ( last_returned == 1 ) 
    {  /* then we have already sucessfully thrown a r.n. pair
	  and returned its first value - now return the second value */
      last_returned = 0; 
      return ( v2 );
    }

  /* determine our random number */
  do {
    u1  =  double_random_kps ();
    u2  =  double_random_kps ();
  } while ( u1 == 0. || u2 == 0. );

  /* v1, v2 are two independent random numbers from a Gaussian distribution
     with unit variance and zero mean */

  v1 = sqrt ( -2. * log ( u1 ) ) * cos ( 2. * M_PI * u2 );
  v2 = sqrt ( -2. * log ( u2 ) ) * sin ( 2. * M_PI * u1 );

  /* return the first of these numbers and keep that in mind */
  last_returned = 1;
  return ( v1 );

}


int kps_seed ( void )
{
  return seed;
}












