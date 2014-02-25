/*---------------------------------------------------------------------------

  random.h   header file for random.c 

    C version of feedback shift register random number generator -
    This file contains the initialization function ini_kps (iseed) 
    that requires an integer seed and the the actual rng functions
    *_random_kps().
   
   USAGE:

   int   init_kps ( int seed ) ** returns the number of tries for 
				  successful seeding of the generator 
   int   kps_seed ()           ** returns the seed of the last successful 
                                  seeding - 0 if not initialized.
   int   int_random_kps ()     ** returns positive integers, bit patterns
				  from 0b to 0111...b 
   unsigned unsigned_random_kps () ** unsigned integers with bit patterns
				  from 0b to 111...b 
   double double_random_kps () ** returns 'double' values in the closed 
				  interval [0,1]
   double gauss_random_kps  () ** returns 'double' values from a Gaussian
                                  distribution with zero mean and unit
				  variance
				  
     
   - S. Kirkpatrick, E. Stoll, J. Comput. Phys. 40, 517(1981).
   - A.M. Ferrenberg, D.P. Landau, Y.J. Wong, PRL 69, 3382(1992) 
           for problems when this generator is combined with the 
	   Wolff cluster-flipping MC method.    

   - G. Marsaglia, B. Narasimhan, A. Zaman, Comput. Phys. Commun. 60, 345(1990)
           original reference for a subtract with carry generator which 
	   also seems to have good statistical properties (as alternative).

   - see also Vattulainen93, 93-1

  --------------------------------------------------------------------------*/

#ifndef   KPS_H
#define   KPS_H

#include <limits.h>


int       kps_init    (int);   
int       kps_seed       ();            

unsigned  kps_unsigned   ();
double    kps_double     ();
int       kps_int        ();         

double    kps_gauss      ();

#define   gauss_random_kps    kps_gauss
#define   int_random_kps      kps_int
#define   double_random_kps   kps_double
#define   unsigned_random_kps kps_unsigned
#define   init_kps            kps_init



#endif    /* KPS_H */

