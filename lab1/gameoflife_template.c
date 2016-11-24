#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <sys/time.h>

#include <omp.h>

#define calcIndex(width, x,y)  ((y)*(width) + (x))

long TimeSteps;

void writeVTK2(long timestep, double *data, char prefix[1024], long w, long h) {
  char filename[2048];
  int x,y;

  long offsetX=0;
  long offsetY=0;
  float deltax=1.0;
  float deltay=1.0;
  long  nxy = w * h * sizeof(float);

  snprintf(filename, sizeof(filename), "%s-%05ld%s", prefix, timestep, ".vti");
  FILE* fp = fopen(filename, "w");

  fprintf(fp, "<?xml version=\"1.0\"?>\n");
  fprintf(fp, "<VTKFile type=\"ImageData\" version=\"0.1\" byte_order=\"LittleEndian\" header_type=\"UInt64\">\n");
  fprintf(fp, "<ImageData WholeExtent=\"%d %d %d %d %d %d\" Origin=\"0 0 0\" Spacing=\"%le %le %le\">\n", offsetX, offsetX + w, offsetY, offsetY + h, 0, 0, deltax, deltax, 0.0);
  fprintf(fp, "<CellData Scalars=\"%s\">\n", prefix);
  fprintf(fp, "<DataArray type=\"Float32\" Name=\"%s\" format=\"appended\" offset=\"0\"/>\n", prefix);
  fprintf(fp, "</CellData>\n");
  fprintf(fp, "</ImageData>\n");
  fprintf(fp, "<AppendedData encoding=\"raw\">\n");
  fprintf(fp, "_");
  fwrite((unsigned char*)&nxy, sizeof(long), 1, fp);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      float value = data[calcIndex(w, x,y)];
      fwrite((unsigned char*)&value, sizeof(float), 1, fp);
    }
  }

  fprintf(fp, "\n</AppendedData>\n");
  fprintf(fp, "</VTKFile>\n");
  fclose(fp);
}


void show(double* currentfield, int w, int h) {
  printf("\033[H");
  int x,y;
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) printf(currentfield[calcIndex(w, x,y)] ? "\033[07m  \033[m" : "  ");
    //printf("\033[E");
    printf("\n");
  }
  fflush(stdout);
}


void evolve_sequential(double* currentfield, double* newfield, int w, int h) {
  int x,y,ii,jj,index_x,index_y;
  int counter;

  for (y = 0; y < h; y++) {

    #pragma omp parallel for private(ii,jj,index_x,index_y,counter)

    for (x = 0; x < w; x++) {
      counter = 0;
      // get neighboring cells
      for (ii = -1; ii <= 1; ii++){
        for (jj = -1; jj <= 1; jj++){
          if((ii) == 0 && (jj == 0)) continue; // dont check cell itself

          index_x = x + jj;
          index_y = y + ii;

          // apply periodic boundary
          if (index_x < 0){ // x direction
            index_x += w;
          } else if(index_x >= w){
            index_x -= w;
          }

          if (index_y < 0){ // y direction
            index_y += h;
          } else if(index_y >= h){
            index_y -= h;
          }

          if (currentfield[calcIndex(w,index_x,index_y)]){
            counter += 1;
          }
        }
      }

      if (currentfield[calcIndex(w,x,y)]  == 1){ // cell ALIVE
        if((counter == 2) || (counter == 3) ){
          newfield[calcIndex(w,x,y)] = 1;
        } else{
          newfield[calcIndex(w,x,y)] = 0;
        }

      } else if (currentfield[calcIndex(w,x,y)]  == 0){ // cell DEAD
        if(counter == 3) {
          newfield[calcIndex(w,x,y)] = 1;
        }else{
          newfield[calcIndex(w,x,y)] = 0;
        }
      }
      else{
        printf("something is wrong\n");
        exit(0);
      }

    }
  }
}


void evolve_omp_parallel(double* currentfield, double* newfield, int w, int h){

  // calculate start and end indices of domain of each thread
  // bsp 3 x 4 Zerlegung
  int nDomains_x = 2;
  int nDomains_y = 2;

  int n_omp_threads = nDomains_y * nDomains_x; // Use 3*4 = 12 threads

  int cells_per_domain_x = w / nDomains_x;
  int cells_per_domain_y = h / nDomains_y;

  // printf("cells_per_domain_x = %i\n",cells_per_domain_x);
  // printf("cells_per_domain_y = %i\n",cells_per_domain_y);

  int start_x,start_y,end_x,end_y;
  int thread_num;

  #pragma omp parallel private(start_x,start_y,end_x,end_y,thread_num)

  //for(thread_num = 0; thread_num < n_omp_threads; thread_num++){
	{
     int tid = omp_get_thread_num();

     start_x = cells_per_domain_x * (tid / nDomains_y) ;
     start_y = cells_per_domain_y * (tid % nDomains_y) ;

     end_x = start_x + cells_per_domain_x -1;
     end_y = start_y + cells_per_domain_y -1;


    //  printf("I am thread %i and my index range is (%i,%i) to (%i,%i)\n",tid,start_x,start_y,end_x,end_y);

    int x,y,ii,jj,index_x,index_y;
    int counter;

    for (y = start_y; y <= end_y; y++) {
      for (x = start_x; x <= end_x; x++) {

        counter = 0;
        // get neighboring cells
        for (ii = -1; ii <= 1; ii++){
          for (jj = -1; jj <= 1; jj++){
            if((ii) == 0 && (jj == 0)) continue; // dont check cell itself

            index_x = x + jj;
            index_y = y + ii;

            // printf("test\n");

            // apply periodic boundary
            if (index_x < 0){ // x direction
              index_x += w;
            } else if(index_x >= w){
              index_x -= w;
            }

            if (index_y < 0){ // y direction
              index_y += h;
            } else if(index_y >= h){
              index_y -= h;
            }

            if (currentfield[calcIndex(w,index_x,index_y)]){
              counter += 1;
            }
          }
        }

        if (currentfield[calcIndex(w,x,y)]  == 1){ // cell ALIVE
          if((counter == 2) || (counter == 3) ){
            newfield[calcIndex(w,x,y)] = 1;
          } else{
            newfield[calcIndex(w,x,y)] = 0;
          }

        } else if (currentfield[calcIndex(w,x,y)]  == 0){ // cell DEAD
          if(counter == 3) {
            newfield[calcIndex(w,x,y)] = 1;
          }else{
            newfield[calcIndex(w,x,y)] = 0;
          }
        }
        else{
          printf("something is wrong\n");
          exit(0);
        }
      }
    }
  }
}


void random_filling(double* currentfield, int w, int h) {
  int i;
  for (i = 0; i < h*w; i++) {
    currentfield[i] = (rand() < RAND_MAX / 2) ? 1 : 0; // initialize domain randomly
  }

}

static void glider_filling(double* currentfield, int w, int h) {

  int x = w / 2;
  int y = h / 2;

  currentfield[calcIndex(w, x+0, y+1)] = 1;
  currentfield[calcIndex(w, x+1, y+2)] = 1;
  currentfield[calcIndex(w, x+2, y+0)] = 1;
  currentfield[calcIndex(w, x+2, y+1)] = 1;
  currentfield[calcIndex(w, x+2, y+2)] = 1;

}

void game(int w, int h) {
  double *currentfield = calloc(w*h, sizeof(double));
  double *newfield     = calloc(w*h, sizeof(double));

  // random_filling(currentfield, w, h);
  glider_filling(currentfield, w, h);


  long t;
  for (t=0;t<TimeSteps;t++) {
    show(currentfield, w, h);
    // Field_printField(currentfield,w,h);

    //evolve_sequential(currentfield, newfield, w, h);
    evolve_omp_parallel(currentfield,newfield,w,h);

    printf("%ld timestep\n",t);
    writeVTK2(t,currentfield,"gol", w, h);

    usleep(150000);

    //SWAP
    double *temp = currentfield;
    currentfield = newfield;
    newfield = temp;
  }

  free(currentfield);
  free(newfield);

}

int main(int c, char **v) {
  int w = 0, h = 0;
  if (c > 1) w = atoi(v[1]); ///< read width
  if (c > 2) h = atoi(v[2]); ///< read height
  TimeSteps = atoi(v[3]);
  if (w <= 0) w = 30; ///< default width
  if (h <= 0) h = 30; ///< default height
  game(w, h);
}

void Field_printField(double* currentfield,int w,int h){

  for (int i = 0; i < w; i++){
    for (int j = 0; j < h; j++){
      if(currentfield[calcIndex(w,i,j)] == 1) printf("X");
      else{
        printf("O");
      }
    }
    printf("\n");
  }
}
