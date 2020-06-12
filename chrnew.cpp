////////////////////////////////////////////////////////////////////////////////////////////
//  Quick and dirty utility to convert 
//  16 bit character generator File  to 32 bit character generator File 
//  generator 
//    
//  Written By :=  PRASEED PAI KT  
//                 VISIONICS INDIA PVT LTD 
//                   
////////////////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////////////// 
// New Structure For  32 bit Character Generator  Cell
//   op_code = 0   => END OF CHARACTER 
//   op_code = 1   => MoveTo
//   op_code = 2   => LineTo
//   

typedef struct 
{
  int op_code:4;
  int x_width:6;
  int y_width:6;
}NEW_CELL;


//================= SigNature  

#define  NEW_SIGNATURE    "EDWin CHARACTER GENERATOR"


//================= Global Variables

FILE *SrcFile;           // pointer to Source File
FILE *DestFile;          // pointer to Destination File
char  *DataBuffer;       // Buffer To Which Entire Contents of 16 bit Chrgen.bin will be read 
DWORD OffsetArray[256];    // Array of Reference
NEW_CELL x_op;           // Data structure for a chracter cell
char Header[64];         // Buffer For Header 
 
 
/////////////////////////////////////////////////////////////////
//  Extract FileName From Path 
//
//
//

void ExtractFileName( char *CmdLine , char *FileName )
 {
    char LocalBuffer[ MAX_PATH ];
    char *PtrToBuffer;
 
      if ( ( PtrToBuffer = strrchr( CmdLine , '\\' )) == 0 ) {
           strcpy( FileName , CmdLine );
       }else {
           strcpy( FileName , PtrToBuffer+1);
        }
     
      return;
       
 }

/////////////////////////////////////////////////////////////////
//  Computes the FileSize 
//
//
//

DWORD ComputeFileSize( FILE *fp )
 {
    DWORD tellptr;
    tellptr  = fseek( fp , 0 , SEEK_END );
    fseek( fp , 0 , SEEK_SET );
    return tellptr;
 }
  
/////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//  

  void main( int argc , char ** argv )
   {
      
      char FileName[MAX_PATH];
    
        if ( argc != 3 ) {
           fprintf( stdout , "Bad Command Line \n" );
           exit(0);
        }

  
      //==================== Open ChrGen.Bin File 

           if ( ( SrcFile = fopen( argv[1] , "rb" )) == NULL )
           {
              fprintf(stdout , "Error Opening Src File " );
              exit(0);
           }


      //=================== Compute The File Size of Chrgen.bin 

      DWORD  SrcFileSize = 10000;

      fprintf( stdout,"Source File Size = %ld\n" , SrcFileSize );  
   
      //==================  Create A File To Store New Format 
          
       if (( DestFile = fopen( argv[2] , "wb" )) == NULL ) {
               fclose(SrcFile);
               fprintf(stdout, "Error Opening Dest File");
               exit(0);
          }
 
        

      //================= Allocate Memory To Read Data To Buffer 
         
      if ((  DataBuffer = ( char *)malloc(  SrcFileSize )) == NULL ) {
               fclose(SrcFile);
               fclose(DestFile);
               fprintf( stdout , "malloc Failure" );
      }
   
       //===============  read entire data to memory;

       fread( DataBuffer , SrcFileSize , 1 , SrcFile );



       //============== Write The Header To Destination  File  

          memset( Header , 0 , sizeof(Header) );
          strcpy( Header , NEW_SIGNATURE );    // copy the Signature
         
          *(DWORD * )( Header + 26 ) =  (DWORD) 256;

          *(DWORD * )( Header + 30 ) =  sizeof(Header)+ sizeof(OffsetArray);

          fwrite(  Header , 64 , 1 , DestFile ); // write the Header

           
       //================ Write The Blank Offset                

         memset( (void *) OffsetArray ,  0 , sizeof( OffsetArray ) );
         fwrite( (void *) OffsetArray , 256 , sizeof(DWORD) , DestFile );

        
     
       //=============== Skip PAST Offset Table in the old Format  
        char *DataPtr =  ( DataBuffer  + 256*2 );

        
        
        int running_sum = 0;   // Track The Offset of Character Cells of a character
        int current_sum = 0;   // Track The Bytes Consumed By Current character for cell 
        unsigned char c; 
           

        //================ Iterate Every Character 

        for( int  curr_char =0; curr_char < 256 ; ++curr_char )
          {
              OffsetArray[ curr_char ] = running_sum;
              current_sum = 0;  
                
                fprintf(stdout,"Processing char %d\n" , curr_char );
                fprintf(stdout,"New Running Sum for %d %ld\n",curr_char , running_sum );

                
                do  
                  {

                     c = *DataPtr++;

                     if (c == 0 ) {
                        printf("Unknown Character %d \n" ,c); 
                        break;
                     }

                          
                    if  ( ( c & 0x80 ) )
                     {
                       
                        if ( c & 1 ) {      
                          x_op.op_code = 2;
                          x_op.x_width = 6;
                          x_op.y_width = 0;
                          fwrite( (void *) &x_op , 1 , 2 , DestFile );
                          current_sum +=2; 
                        }


                        memset( &x_op , 0 , 2 );
                        fwrite( (void *) &x_op , 1 , 2 , DestFile );
                        current_sum +=2;
                        fprintf(stdout,"Value Offset of %d = %d ",curr_char, running_sum + current_sum );                
                        break;  

                    }
                   else  if ( c & 1 ) {
                         x_op.op_code = 2;
                         x_op.x_width = *DataPtr++;
                         x_op.y_width = *DataPtr++;
                         fwrite( (void *) &x_op , 1 , 2 , DestFile );
                         current_sum +=2; 
                         }       
                   else  if ( !(c &1) ) {
                         x_op.op_code = 1;
                         x_op.x_width = *DataPtr++;
                         x_op.y_width = *DataPtr++;
                         fwrite( (void *) &x_op , 1 , 2 , DestFile );
                         current_sum +=2;
                    }  
                   
                        if ( curr_char == 255 ) {
                           fprintf(stdout,"char =%d\n" , c ); 
                           fprintf( stdout,"running Sum = %d\n" , running_sum );
                        }  
 
               }
              while ( !(c&0x80) );

              running_sum += current_sum;  

           }


         fseek( DestFile , 64 , SEEK_SET );
         fwrite( (void *) OffsetArray , 1024 , 1 , DestFile );
         fclose( DestFile );

         printf("Conversion SuccessFull \n" );  

   return;   
 }




         
