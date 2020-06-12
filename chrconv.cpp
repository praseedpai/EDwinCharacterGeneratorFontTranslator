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
           //=================== Compute The File Size of Chrgen.bin 
 
           DWORD  SrcFileSize = ComputeFileSize(argv[1]);

           fprintf( stdout,"Source File Size = %ld\n" , SrcFileSize );  
   
                  if ( SrcFileSize == 0 ) {
                       fprintf( stdout,"Error Computing File Size " );
                       exit(0);
                  }
  
           
      //==================== Open ChrGen.Bin File 

           if ( ( SrcFile = fopen( argv[1] , "rb" )) == NULL )
           {
              fprintf(stdout , "Error Opening Src File " );
              exit(0);
           }


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

      size_t unp = fread( DataBuffer , 1 , SrcFileSize  , SrcFile );


      fprintf(stdout,"===============================\n" );
      fprintf(stdout,"%d\n" , (int) unp );  



       //============== Write The Header To Destination  File  

          memset( Header , 0 , sizeof(Header) );
          strcpy( Header , NEW_SIGNATURE );    // copy the Signature
         
          *(DWORD * )( Header + 26 ) =  (DWORD) 256;

          *(DWORD * )( Header + 30 ) =  sizeof(Header)+ sizeof(OffsetArray);

          if ( fwrite(  Header , 1 , 64 , DestFile ) != 64 ) {
              fprintf( stdout,"Was not Able To Write Full Data\n" );
              exit(0);
           }
             

           
       //================ Write The Blank Offset                

         memset( (void *) OffsetArray ,  0 , sizeof( OffsetArray ) );
         if (fwrite( (void *) OffsetArray ,  sizeof(DWORD),256 , DestFile ) != 256 ) {
             fprintf( stdout,"Was not Able To Write Full Data\n" );
              exit(0);
           }
  

        
     
       //=============== Skip PAST Offset Table in the old Format  
        char *DataPtr =  ( DataBuffer  + 514 );

        
        
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

                     c = *DataPtr++;   // Get The Next Char From Stream
      
                     memset( &x_op , 0 , 2 ); // Set To Null OpCode

                        
                     if ( (c&0x80 ) )
                      {
                           x_op.x_width = 6;
                           x_op.y_width = 0;
                      }
                     else
                      {
                           x_op.x_width =  *DataPtr++;   
                           x_op.y_width =  *DataPtr++;

                           fprintf( stdout, "Value of x = %hd\n" , x_op.x_width );
                           fprintf(stdout,"Value of y = %hd\n" , x_op.y_width );
                      }  
                      //============================ 
                      if ( c&1 ) {
                         x_op.op_code = 2;
                       }
                      else 
                       {
                          if ( c&0x80 ) break;
                              x_op.op_code = 1; 
                       }

                           

                          
                      if ( fwrite( (void *) &x_op , 2 , 1 , DestFile ) != 1 ) {
                            fprintf( stdout,"Was not Able To Write Full Data\n" );
                            exit(0);
                      }

                       current_sum +=2; 
                                         
               }
              while ( !(c&0x80) );

                    
                memset( &x_op , 0 , 2 ); // Set To Null OpCode
                
                          
                if ( fwrite( (void *) &x_op , 2 , 1 , DestFile ) != 1 ) {
                      fprintf( stdout,"Was not Able To Write Full Data\n" );
                      exit(0);
                 }


                current_sum +=2; 
                                   
                running_sum += current_sum;  

                if ( DataPtr > (  DataBuffer + SrcFileSize )  )
                 {
                     fprintf( stdout,"Error Pointer Over Run \n" );
                     exit(0);
                 }  

           }

         fprintf(stdout,"Value of DataPtr = %p\n" , DataPtr );
         fprintf(stdout, "Value of End Ptr = %p\n" ,  (  DataBuffer + SrcFileSize )  );

 
         char TempBuffer[100];
  
         memset(TempBuffer , 0 , 100 ); 
         strncpy( TempBuffer, DataPtr , ( DataBuffer + SrcFileSize ) - DataPtr );
  
          fprintf( stdout,"%s\n", TempBuffer );
          if ( strncmp(DataPtr,"MAREK",5 ) != 0 ) {
              fprintf(stdout,"Error Reading a Bad Loop\n" );
              exit(0);
         }
  
         
         fseek( DestFile , 64 , SEEK_SET );
         if (  fwrite( (void *) OffsetArray ,  sizeof(DWORD) ,256, DestFile ) != 256 ) 
          {
                  fprintf( stdout,"Was not Able To Write Full Data\n" );
                  exit(0);
          }


         fclose( DestFile );
         fclose(SrcFile);
         printf("Conversion SuccessFull \n" );  

   return;   
 }




         
