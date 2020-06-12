#include <windows.h>
#include <stdio.h>


typedef struct {
  int x:4;
  int x_c:6;
  int x_d:6;
}CELL;


void main( ) {

  CELL a;

  a.x = 1;
  a.x_c = -1;
  a.x_d = -5;

  printf("%d %d %d\n" , a.x , a.x_c , a.x_d );

 }

