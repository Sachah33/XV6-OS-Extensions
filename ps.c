// MAMAN 01
// User space program that call the cps() syscall and prints the process listing.
// T.Z: 329604441
// Name: Sacha HADJADJ

#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[] )
{
  cps141();     // syscall name + 1 + T.Z
  exit();
}
