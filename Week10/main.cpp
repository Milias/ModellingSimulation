#include "elevator.hpp"

int main(int argc, char ** argv)
{
  double x = 0.0;
  integrate_adaptive( make_controlled( 1E-12 , 1E-12 , stepper_type() ) ,
                      rhs , x , 1.0 , 10.0 , 0.1 , write_cout );
  return 0;
}
