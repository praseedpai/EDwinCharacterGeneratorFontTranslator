//////////////////////////////////////////////////////
// Program To Perform Consistency Check in 
// Edwin Font Format Files
// Written By :=      Praseed Pai KT
//                    VISIONICS INDIA PVT LTD
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>


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
    

        //=================== Compute The File Size of Chrgen.bin 
 
          DWORD  SrcFileSize = ComputeFileSize("CHRGEN.BIN");

          fprintf( stdout,"Source File Size = %ld\n" , SrcFileSize );  
   
             if ( SrcFileSize == 0 ) {
                    fprintf( stdout,"Error Computing File Size " );
                    exit(0);
               }
  



      if ((ip = fopen("CHRGEN.BIN" , "rb" )) == NULL ) 
    {
       fprintf( stdout,"Error Opening File chrgen.bin\n" ); 
       exit(0);
    }


        
    if ( ( op = fopen("16bit.txt" , "w" ) ) == NULL )
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
   


      size_t unp = fread( GlobalBuffer , 1, 10000 ,ip );


      fprintf( stdout, "==================================\n" );
      fprintf( stdout, "%d\n" , (int)unp );
      char *DataPtr = ( GlobalBuffer + 514 );
      int c;
      char first_op;
      char second_op; 
      
       


      for( int i=0; i<256; ++i ) 
      {
         
            do  
             {
                 c = *DataPtr++;   // Get The Next Char From Stream

                 
                        
                     if ( (c&0x80 ) )
                      {
                        first_op   = 6;
                        second_op  = 0; 
                       
                      }
                     else
                      {
                          first_op   = *DataPtr++;   
                          second_op  = *DataPtr++;
                           
                      }  

                      if ( c&1 ) {
                         #if 1   
                         fprintf( op,"LINETO %d %d\n", (int)first_op ,(int)second_op);
                         #else
                         fprintf( op,"LINETO\n");
                         #endif 
                       }
                      else 
                       {
                          if ( c&0x80 ) break;
                          #if 1
                            fprintf( op,"MOVETO %d %d\n" , (int)first_op ,(int)second_op); 
                          #else
                            fprintf( op,"MOVETO\n" ); 

                          #endif 
                       }

                           

                          
                                         
               }
              while ( !(c&0x80) );

                  fprintf( op,"EOFCHAR\n" );

                    
           }


         fprintf(stdout,"Value of DataPtr = %p\n" , DataPtr );
         fprintf(stdout, "Value of End Ptr = %p\n" ,  (  GlobalBuffer + SrcFileSize )  );

 
         char TempBuffer[100];
  
         memset(TempBuffer , 0 , 100 ); 
         strncpy( TempBuffer, DataPtr , ( GlobalBuffer+ SrcFileSize ) - DataPtr );
  
          fprintf( stdout,"%s\n", TempBuffer );
          if ( strncmp(DataPtr,"MAREK",5 ) != 0 ) {
              fprintf(stdout,"Error Reading a Bad Loop\n" );
              exit(0);
         }



    fclose(ip);
    fclose(op);

   return;   
 }

