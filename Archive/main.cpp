#include <stdio.h>	    
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include "random.h"


// Car the basic structure of the program. It uses minimum specification : position q, speed v, and distance ahead d. vmax is also
// specified, altough it is not used by the car itself. The speed of the car is always checked against the vmax speed of the highway
// the car is driving on. Finally, to every highway corresponds a list of cars; this list is defined through dynamic pointers.
struct Car {
  unsigned q;
  unsigned v;
  unsigned d;
  unsigned vmax;
  Car *next;
  void *highway;
};


// The Highway class is the building block of the program. It defines many methods, most of them are self-explanatory. Note also that
// this class makes good use of polymorphism, since Junctions are a special case of highways with more features.
class Highway {
public:
  unsigned L;
  unsigned vmax;
  unsigned nb_of_cars;
  double p;
  
  Highway* exit; 
  Car* cars;
  Car* buffer;

  Highway ();
  Highway (int L, double p, int vmax);
  ~Highway();
  virtual int insert_car (Car*);
  int populate (double density);
  Car* remove_car (Car** p);
  virtual void update();
  void draw();
  virtual int distance_ahead();

  void step1();
  void step2();
  void step3();
  void step4();
};

Highway::Highway() {
  cars = NULL;
  exit = NULL;
  buffer = NULL;
}

Highway::Highway(int L, double p, int vmax) {
  cars = NULL;
  exit = NULL;
  buffer = NULL;
  Highway::L = L;
  Highway::p = p;
  Highway::vmax = vmax;
}

Highway::~Highway() {
  while (cars != NULL) {
    Car *c = cars;
    cars = c->next;
    delete c;
  }
}

void Highway::draw () {
  Car* c = cars;
  for (int i=0; i<L; i++) {
    if ( c != NULL && c->q == i) {
      printf("1 ");
      c = c->next;
      continue;
    }
    printf("0 ");
  }
}

int Highway::distance_ahead () {
  if (cars != NULL)
    return cars->q;
  else if (exit != NULL)
    return L;
  else
    return exit->distance_ahead();
}

int Highway::insert_car (Car* newcar) {
  newcar->vmax = vmax;
  newcar->highway = (void*) this;
  newcar->next = buffer;
  buffer = newcar;
}

Car* Highway::remove_car(Car** p) {
  Car *c = cars, *prev = c;
  while ( c->next != NULL ) {
    prev = c;
    c = c->next;
  }
  prev->next = NULL;
  *p = prev;
  nb_of_cars--;
  return c;
}

int Highway::populate (double density) {
  double i = 0;
  while (i/L < density) {
    Car *c = cars;
    Car *prev = NULL;
    int x = kps_int() % L;
    try {
      do {
	if ( c == NULL )
	  break;
	if ( x == c->q )
	  throw 0;
	if ( x < c->q )
	  break;
	if ( x > c->q ) {
	  prev = c;
	  c = c->next;
	}
      } while (1);
    }
    catch (int e) {continue;}
    Car *newcar = new Car;
    newcar->q = x;
    newcar->v = vmax;
    newcar->vmax = Highway::vmax;
    newcar->highway = (void*) this;
    newcar->next = c;
    c = newcar;
    if (prev == NULL)
      cars = c;
    else
      prev->next = c;
    i++;
  }
  return i;
}

void Highway::step1() {
  Car *c = cars;
  while (c != NULL) {
    if (c->v < c->vmax)
      (c->v)++;
    if (c->v > c->vmax)
      c->v = c->vmax;
    c = c->next;
  }
}

void Highway::step2() {
  Car *c = cars;
  if (c == NULL)
    return;
  while (c != NULL) {
    if (c->next == NULL)
      c->d = (exit->distance_ahead()+L) - c->q;
    else
      c->d = c->next->q - c->q;
    if (c->d <= c->v)
      c->v = c->d-1;
    c = c->next;
  }
}
    

void Highway::step3() {
  Car *c = cars;
  while (c != NULL) {
    double prob = kps_double();
    if (prob <= p && c->v > 0)
      (c->v)--;
    c = c->next;
  }
}

void Highway::step4() {
  Car *c = cars, *prev = NULL;
  while (c != NULL) {
    c->q += c->v;
    if (c->q >= L) {
      c->q %= L;
      if (prev == NULL) {
	cars = NULL;
	prev = c;
      }
      prev->next = c->next;
      exit->insert_car(c);
      c = prev->next;
      continue;
    }
    prev = c;
    c = c->next;
  }
}

void Highway::update() {
  Car* c = NULL;
  while (buffer != NULL) {
    c = buffer;
    buffer = buffer->next;
    c->next = cars;
    cars = c;
  }
}

// Priority to the first highway!
class Junction : public Highway {
public:
  double p;
  Highway *entrance1, *entrance2;
  Highway *exit1, *exit2;
  Car *c1, *c2;
  Car *buffer1, *buffer2;

  Junction();
  ~Junction();
  int distance_ahead();
  int insert_car (Car* newcar);
  void update();
};

Junction::Junction() : Highway() {
  L = 1;
  p = 0.5;
  entrance1 = NULL;
  entrance2 = NULL;
  exit1 = NULL;
  exit2 = NULL;
  c1 = NULL;
  c2 = NULL;
  buffer1 = NULL;
  buffer2 = NULL;
}

Junction::~Junction() {
  if (c1 != NULL)
    delete c1;
  if (c2 != NULL)
    delete c2;
  return;
}

int Junction::distance_ahead() {
  if (cars != NULL)
    return 0;
  else
    return 1;
}

int Junction::insert_car (Car* newcar) {
  newcar->next = NULL;
  if ( (Highway*) newcar->highway == entrance1) {
    vmax = entrance1->vmax;
    c1 = newcar;
  }
  else {
    c2 = newcar;
    vmax = entrance2->vmax;
  }
}

void Junction::update () {
  if ( c1 != NULL) {
    cars = c1;
    c1 = NULL;
  }
  else if (c2 != NULL) {
    cars = c2;
    c2 = NULL;
  }
  double r = kps_double();
  if (r < p)
    exit = exit1;
  else
    exit = exit2;
}
  
  
// The System class is the class thorugh every interaction with the user should take place
class System {
public:
  Highway** H;
  Junction* J;
  int focus;
  int t;
  System();
  ~System();
  Highway* add_highway(int L, double p, int vmax, double density);
  Junction* add_junction (double p);
  int connect_highway (Highway* h1, Highway* h2);
  int connect_highway (Highway* h1, Junction *j);
  void evolve(int time);
  void draw();
};

System::System() {
  t = 0;
  J = new Junction;
  H = new Highway*[10];
  for (int i=0; i<10; i++)
    H[i] = NULL;
}

System::~System() {
  delete J;
  delete[] H;
}

Junction* System::add_junction (double p=0.5) {
  J->p = p;
  return J;
}

Highway* System::add_highway (int L, double p, int vmax, double density) {
  int i = 0;
  int max = 10;
  while (H[i] != NULL && i <= max-1)
    i++;
  if (H[i] != NULL)
    return NULL;
  H[i] = new Highway(L,p,vmax);
  H[i]->populate(density);
  return H[i];
}

int System::connect_highway (Highway* h1, Highway* h2) {
  if (h1->exit != NULL)
    return 0;
  h1->exit = h2;
}

int System::connect_highway(Highway *h, Junction *j) {
  h->exit = J;
  if (J->entrance1 == NULL) {
    J->entrance1 = h;
    J->exit1 = h;
  }
  else if (J->entrance1 != NULL && J->entrance2 == NULL) {
    J->entrance2 = h;
    J->exit2 = h;
  }
}
  
void System::draw() {
  int j = 0;
  while (H[j] != NULL) {
    H[j]->draw();
    fprintf(stdout," ");
    j++;
  }
}


void System::evolve (int time=1) {
  int j;
  for (int i=0; i<time; i++) {
    if (time != 1) {
      draw();
      printf("\n");
      fprintf(stderr,"%d\n",i);
    }
    j = 0;
    while (H[j] != NULL) {
      H[j]->step1();
      j++;
    }
    if (J)
      J->step1();
    
    j = 0;
    while (H[j] != NULL) { 
      H[j]->step2();
      j++;
    }
    if (J)
      J->step2();
	
    j = 0;
    while (H[j] != NULL) {
      H[j]->step3();
      j++;
    }
    if (J)
      J->step3();

    
    j = 0;
    while (H[j] != NULL) {
      H[j]->step4();
      j++;
    }
    if (J)
      J->step4();
    
    j = 0;
    while (H[j] != NULL) {
      H[j]->update();
      j++;
    }
    if (J)
      J->update();
  }
}


double FundamentalDiagram(double c, int L=50, int vmax=1, double p=0) {
  int runs = 100;
  double subruns = 3000;
  double J = 0;
  for (int i=0; i<runs; i++) {
    double j = 0;
    Car* pc = NULL;
    System s;
    Highway* h1 = s.add_highway(L,p,vmax,c);
    s.connect_highway(h1,h1);
    for (int k=0; k<subruns; k++) {
      s.evolve();
      if (h1->cars != pc) {
	pc = h1->cars;
	j++;
      }
    }
    j = j/subruns;
    delete h1;
    J += j;
  }
  return J/runs;
}


// Returns the spatial correlation of a highway with given specifications
double SpatialCorrelation(double r, double c, double p=0, int vmax=1) {
  double G = 0;
  double T = 1000;
  double L = 1000;
  System s;
  Highway *h = s.add_highway(2*L,p,vmax,c);
  s.connect_highway(h,h);

  for (int i=0; i<T; i++) {
    for (int j=0; j<L; j++) {
      int nj=0, njr=0;
      Car* c = h->cars;
      while (c != NULL) {
	if (c->q == j && nj == 0)
	  nj = 1;
	if (c->q == j+r) {
	  njr = 1;
	  break;
	}
	if (c->q > j+r)
	  break;
	c = c->next;
      }
      G += nj*njr;
    }
    s.evolve();
  }

  G = G/(T*L) - c*c;
  return G;
}

//Plotting main function
// These lines of code should be clear as to which line does what.
// Commented out code is perfectly functional
int main(int argc, char* argv[]) {
  //int seed = (int)(time(0));
  int seed = 2;
  kps_init(seed);
  int t= 10;
  
  System s;
//   Highway *h1 = s.add_highway(100,0,2,0.7);
//   Highway *h2 = s.add_highway(100,0.10,2,0);
//   Highway *h3 = s.add_highway(150,0.15,3,0.2);
//   Highway *h4 = s.add_highway(150,0.25,4,0.3);
//   Highway *h5 = s.add_highway(250,0.33,2,0);
//   Highway *h6 = s.add_highway(250,0.6,1,0.3);
//   s.connect_highway(h1,h2);
//   s.connect_highway(h2,h3);
//   s.connect_highway(h3,h4);
//   s.connect_highway(h4,h5);
//   s.connect_highway(h5,h6);
//   s.connect_highway(h6,h1);
//   s.evolve(2000);
  
  Highway *h1 = s.add_highway(500,0,5,0.6);
  Highway *h2 = s.add_highway(500,.92,10,0);
  Junction *J = s.add_junction(.5);
  s.connect_highway(h1,J);
  s.connect_highway(h2,J);
  s.evolve(2000);
  
  
 //  s.connect_highway(h1,h2);
//   s.connect_highway(h2,h1);
//   s.evolve(t);
  
  return 0;
}



// // Correlation main function
// int main() {
//   int seed = (int)(time(0));
//   kps_init(seed);

//   FILE* fp = fopen("data/SpatCorrp2838.dat","w");
//   int r=0;
//   double g;
//   while (r<31) {
//     g = SpatialCorrelation(r,.38,0.0078125,2);
//     fprintf(stdout,"%d \t %f\n", r,g);
//     fprintf(fp,"%d \t %f\n",r,g);
//     r++;
//   }
//   fclose(fp);
// }
   
   
  
// Fundamental diagram main function
// int main() {
//   int seed = (int)(time(0));
//   //int seed = 2;
//   kps_init(seed);
//   double f;
  
//   FILE* fp = fopen("data/Limitp4.dat","w");
//   double p = 0.0;
//   while (p <= .99) {
//     double f = FundamentalDiagram(.3,400,4,p);
//     fprintf(fp,"%f \t %f\n",p,f);
//     fprintf(stdout,"%f \t %f\n",p,f);
//     p += 0.01;
//   }
//   fclose(fp);
// }

