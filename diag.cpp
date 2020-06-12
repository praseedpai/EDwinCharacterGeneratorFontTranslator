#include <windows.h>
#include <stdio.h>

char File[10000];

void main( ) {



 FILE *f; 


 f = fopen( "NEWCHR.BIN" , "rb" );



 fread( File , 10000 , 1, f );


 printf("signature = %s\n" , File );


 printf("No of Chars = %ld \n" , * (DWORD *) ( File + 26 ) );

 printf("Start of Data Offset  = %ld \n" , * (DWORD *) ( File + 30 ) );


 LONG *m_long = ( LONG * ) ( File + 64 );




 for ( int i = 0; i<256; ++ i )
 {
    printf("Offset of %d is  %ld\n" , i , m_long[i] );
 }

exit(0);
}



