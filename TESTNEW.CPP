#include <windows.h>
#include <stdio.h>

char File[10000];

void main( ) {



 FILE *f; 


 f = fopen( "CHRGEN.BIN" , "rb" );



 fread( File , 10000 , 1, f );


 //printf("signature = %s\n" , File );



 WORD  *m_long = ( WORD * ) ( File  );




 for ( int i = 0; i<256; ++ i )
 {
    printf("Offset of %d is  %ld\n" , i , m_long[i] );
 }

exit(0);
}



