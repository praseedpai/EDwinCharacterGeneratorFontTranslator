//////////////////////////////////////////////////////
// Program To Perform Consistency Check in 
// Edwin Font Format Files
// Written By :=      Praseed Pai KT
//                    VISIONICS INDIA PVT LTD
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>


//================== OPCODE STRUCTURE
typedef struct 
{
  short op_code:4,
        x_width:6,
        y_width:6;
}NEW_CELL;



//================== Global Variables 

char *GlobalBuffer;
///////////////////////////////////////////////////////////////////
//  Computes the FileSize 
//
//
//

DWORD ComputeFileSize( LPSTR pFileName  )
 {
    DWORD tellptr;
    HANDLE hf;

     if((hf = CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == NULL) {
             return (DWORD) 0;
     }

    tellptr = GetFileSize( hf , NULL );

    CloseHandle( hf );
    return tellptr; 

 }


void main( int argc , char **argv )
 {

    FILE *ip;
    FILE *op; 

    //=================== Compute The File Size of NewChrGen.bin 
 
          DWORD  SrcFileSize = ComputeFileSize("NEWCHR.BIN");

          fprintf( stdout,"Source File Size = %ld\n" , SrcFileSize );  
   
             if ( SrcFileSize == 0 ) {
                    fprintf( stdout,"Error Computing File Size " );
                    exit(0);
               }

    if ((ip = fopen("NEWCHR.BIN" , "rb" )) == NULL ) 
    {
       fprintf( stdout,"Error Opening File chrgen.bin\n" ); 
       exit(0);
    }


        
    if ( ( op = fopen("32bit.txt" , "w" ) ) == NULL )
      {
         fprintf( stdout,"Error Opening File 16 bit.txt" );
         exit(0);
      }

  
      //================= Allocate Memory To Read Data To Buffer 
         
      if ((  GlobalBuffer = ( char *)malloc(  SrcFileSize )) == NULL ) {
               fclose(ip);
               fclose(op);
               fprintf( stdout , "malloc Failure" );
      }

     if (  fread( GlobalBuffer , 1 , SrcFileSize,ip ) != SrcFileSize ) {
             fprintf(stdout,"Error Reading Objects From file\n" );
             exit(0);
      }




      NEW_CELL *DataPtr = ( NEW_CELL  *) ( GlobalBuffer + (1024+64) );

      unsigned char c;

      fprintf( stdout, " size of NEW_CELL = %d\n" , sizeof(NEW_CELL) );
 



      for( int i=0; i<256; ++i ) 
      {
         
            do  
             {

                if ( DataPtr->op_code == 2 ) 
                    {
                       
                       #if 0     
                       fprintf( op,"LINETO\n" );
                       #else
                          fprintf( op,"LINETO %d %d\n" ,DataPtr->x_width ,DataPtr->y_width);

                       #endif
                       DataPtr++;
                    } 
                        
                else if ( DataPtr->op_code == 1 ) {
          
                       #if 0
                         fprintf( op,"MOVETO\n"); 
                       #else
                         fprintf( op,"MOVETO %d %d\n" , DataPtr->x_width , DataPtr->y_width);
                       #endif  
                        DataPtr++;

                    }
                else if ( DataPtr->op_code == 0 ) {
                        DataPtr++;

                        fprintf( op,"EOFCHAR\n" ); 
                        break;
                  }
                else {
                     printf("%d\n" , DataPtr->op_code );
                     fprintf(stdout,"Value of DataPtr = %p\n" , DataPtr );
                     fprintf(stdout, "Value of End Ptr = %p\n" ,  (  GlobalBuffer + SrcFileSize )  );

                     fprintf( stdout , "difference = %d\n" , ((long)(GlobalBuffer + SrcFileSize )) - (long) ((void *)DataPtr) );

             
                     fprintf(stdout,"Invalid Op Code\n" ); 

                     exit(0); 
                 }         


               if ( (long) (DataPtr)  >  (long)(GlobalBuffer + SrcFileSize ) ) {
                       fprintf(stdout,"\n Was Processing  Char #%d\n", i );
                       fprintf( stdout,"Over Run of Space\n" );
                       exit(0);
                }
 

                          
                                         
               }
              while ( 1 );

           }

 
         fprintf(stdout,"Value of DataPtr = %p\n" , DataPtr );
         fprintf(stdout, "Value of End Ptr = %p\n" ,  (  GlobalBuffer + SrcFileSize )  );

 
        

    fclose(ip);
    fclose(op);

   return;   
 }

