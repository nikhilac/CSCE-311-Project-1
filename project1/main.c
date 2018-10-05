#include "projectheader.h"

int main(){
  char filename[] = "Example text\n";

  pipefun(filename);
  socketfun(filename);
  sharedmemfun(filename);
  return(0);
 
}
