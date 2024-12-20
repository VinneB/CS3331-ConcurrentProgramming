#include "mandel.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <sys/wait.h>
#include <stdlib.h>
//-----------------------------------------------------------------
// Computes members of mandelbrot for specified region
// of the real/imaginary plane. Creates PPM image file
// according to one of several color schemes.
//-----------------------------------------------------------------
//
// Compilation: g++ -c mandel.cc
//              g++ -c mandelPlot.cc
//              g++ -o mandelPlot mandelPlot.o mandel.o -lm
//
// There is a makefile in the project directory
//-----------------------------------------------------------------
//
// Invocation: mandelPlot RealCoord ImagCoord RealSideLength ComplexSideLength EscapeIterations HorizontalPixels VerticalPixels ImageFile
//
// See the project handout for a detailed description of the parameters and
// operation of the program.
//------------------------------------------------------------------

#define PRINTBUFSIZE 241
#define COLORS 15
unsigned char palette[COLORS][3] = {{255,255,255},{255,0,0},{255,128,0},{255,255,0},{128,255,0},{0,255,0},{0,255,128},{0,255,255},{0,128,255},{0,0,255},{128,0,225},{255,0,255},{255,0,128},{128,128,128},{0,0,0}};

//------------------------------------------------------------------
// Associate a color with one point and add point values
// to the PPM file.  See the project handout for a more complete
// description.
//------------------------------------------------------------------
void addPoint(int scheme, int iterations, int maxiterations, FILE *fp){

  unsigned char color[3];
  int index;

  //
  //--- Three possible schemes for coloring each point.
  //
  if (scheme==1){
      // Scheme 1 - Greyscale uniform 
      color[0]=255 - ((double)iterations/(double)maxiterations)*255;
      color[1]=255 - ((double)iterations/(double)maxiterations)*255;
      color[2]=255 - ((double)iterations/(double)maxiterations)*255;
  }
  if ((scheme==2)||(scheme==3)){
       if (scheme==2){
            iterations=sqrt( (double)iterations/(double)maxiterations)*(float)(maxiterations);
       }
       
       if (iterations==maxiterations)
	        index=COLORS-1; //-- Points in the set are always black, assumed last in palette
       else
            index=iterations/(maxiterations/(COLORS-1));

       color[0]=palette[index][0];
       color[1]=palette[index][1];
       color[2]=palette[index][2];
  }            

  //-- Add the point the file
  fputc(color[0],fp);
  fputc(color[1],fp);
  fputc(color[2],fp);
}
//
//------------ Main 
//
int main(int argc, char *argv[]){
  
  dblcpoint start;       //-- Top left corner
  double hrange;         //-- Length of a side in real plane
  double vrange;         //-- Length of a side in complex plane
  int hpixels;           //-- Pixels on the real side
  int vpixels;           //-- Pixels on the imaginary side
  double hincrement;     //-- Length between pixels in complex plane
  double vincrement;     //-- Length between pixels in complex plane
  int maxiterations;     //-- Number of iterations use to identify points in the set
  int i;                 //-- Loop index for recurrence relation
  FILE *fp;              //-- Descriptor for file to hold image
  int colorscheme;       //-- Color scheme for the plot
  char printBuf[PRINTBUFSIZE];     //-- Output buffer  
  MandelPointRegion *mandelRegion; //-- Point region -- contains points and methods to generate them
  MandelPoint point;               //-- One point

  int nprocs;            //-- Number of processes to run in parallel

  // Create a image file name template
  int t_len = strlen("pic0.ppm")+1;
  int id_index = 3;
  char temp_filename[t_len];
  strcpy(temp_filename, "pic0.ppm");

  //
  // -- Process the input arguments
  //
  if (argc!=10){
    printf("mandel RealCoord ImagCoord RealSideLength ImagSideLength EscapeIterations HorizontalPixels VerticalPixels ImageFile nprocs\n");
    exit(1);
  }
  start={strtod(argv[1],NULL),strtod(argv[2],NULL)};
  printf("Top left coordinate is: %f + %fi\n", start.real(), start.imag());
  hrange=strtod(argv[3],NULL);
  if (hrange<=0){printf("Real side must be greater than zero.\n");exit(1);}
  printf("Length of real side:  %f\n",hrange);
  vrange=strtod(argv[4],NULL);
  if (vrange<=0){printf("Imaginary side must be greater than zero.\n");exit(1);}
  printf("Length of vertical side:  %f\n",vrange);
  maxiterations=atoi(argv[5]);
  if (maxiterations<2){printf("Max iterations must be at least 2.\n");exit(1);}
  hpixels=atoi(argv[6]);
  printf("Pixels on horizontal side:  %d\n",hpixels);
  if (hpixels<10){printf("Pixels on horizontal side must be at least 10.\n");exit(1);}
  vpixels=atoi(argv[7]);
  printf("Pixels on vertical side:  %d\n",vpixels);
  if (vpixels<10){printf("Pixels on horizontal side must be at least 10.\n");exit(1);}
  nprocs = atoi(argv[9]);

  // The number of processes casted to a double for calculations
  double nprocs_d = (double)nprocs;

  // Process-specific madelbrot plot specific parameters
  dblcpoint start_p = start;
  double hrange_p = hrange;
  double vrange_p = vrange / nprocs_d;
  int hpixels_p = hpixels;
  int vpixels_p = vpixels / nprocs;

  // Fork processes to compute a section of the set
  int f_ret;
  int j;
  for (j = 1; j < nprocs+1; j++) {
    if ((f_ret = fork()) == 0) {
      break;
    }
    // Update start coord
    double imag_start_p = imag(start_p) - vrange_p;
    start_p = {real(start_p), imag_start_p};
  }

  if (f_ret == 0) { // Child
    temp_filename[id_index] = (char)(j + 48);

    fp=fopen(temp_filename, "wb"); /* b - binary mode */
    if (fp==NULL){printf("%s cannot be opened for write\n",temp_filename);}
    (void) fprintf(fp, "P6\n%d %d\n255\n", hpixels_p, vpixels_p);


      //
      //-- Output the calculation parameters
      //
      snprintf(printBuf,PRINTBUFSIZE,"Process %d testing rectangle at %.8f + %.8f \n\twidth %.8f and height %.8f \n\tplot area width %d by height %d pixels.\n",getpid(),start_p.real(),start_p.imag(),hrange_p,vrange_p,hpixels_p,vpixels_p);
      write(1,printBuf,strlen(printBuf));

      //
      //--- Create a new point region and calculate the point values.  The "value" is
      //    the number of iterations before the recurrence value exceeds 2. If maxiterations
      //    is reached without exceeding 2, the point is not in the set.
      //
      mandelRegion=new MandelPointRegion(start_p,hrange_p,vrange_p,hpixels_p,vpixels_p,maxiterations);
      mandelRegion->ComputePoints();

    //
    //--- Now create the image file
    //

    #ifndef COLORSCHEME
      colorscheme=1;
    #else
      colorscheme=COLORSCHEME;
    #endif
      
    for (i=0;i<hpixels_p*vpixels_p;i++)
    {
      point=mandelRegion->getPoint(i);
      addPoint(colorscheme,point.iterationsCompleted,maxiterations,fp);
    }

    //
    //-- Done
    //
    snprintf(printBuf,PRINTBUFSIZE,"Process %d done.\n",getpid());
    write(1,printBuf,strlen(printBuf));
    fclose(fp);
    exit(0);
  } else { // Parent
    // Wait on all the child processes
    for (int k = 0; k < nprocs; k++) {
      wait(NULL);
    }

    size_t dummysize = 0;
    char *line = NULL;

    FILE *img_fp;
    long dataseg_st;
    long dataseg_end;
    long dataseg_size;
    void *filedata = NULL;
    fp=fopen(argv[8], "wb"); /* b - binary mode */
    if (fp==NULL){printf("%s cannot be opened for write\n",argv[8]);}
    (void) fprintf(fp, "P6\n%d %d\n255\n", hpixels, vpixels);

    for (int k = 1; k < nprocs+1; k++) {
      temp_filename[id_index] = (char)(k + 48);
      img_fp = fopen(temp_filename, "r");
      if (img_fp==NULL){printf("%s cannot be opened for write\n",temp_filename);}

      // Get to the beginning of image data
      getline(&line, &dummysize, img_fp);
      free(line);
      line = NULL;
      getline(&line, &dummysize, img_fp);
      free(line);
      line = NULL;
      getline(&line, &dummysize, img_fp);
      free(line);
      line = NULL;

      // Get beginning and start of ppm image data
      dataseg_st = ftell(img_fp);
      fseek(img_fp, 0L, SEEK_END);
      dataseg_end = ftell(img_fp);
      dataseg_size = dataseg_end - dataseg_st;
      
      // Concatenate ppm files
      filedata = malloc(dataseg_size);
      fseek(img_fp, dataseg_st, SEEK_SET);
      fread(filedata, 1, dataseg_size, img_fp);
      fwrite(filedata, 1, dataseg_size, fp);
      free(filedata);
      fclose(img_fp);
    }

    fclose(fp);
    exit(0);
  }
}
