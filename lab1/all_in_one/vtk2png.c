/* High performance computing - VTK legacy file format to png converter
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Constantin Heisler
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include "vtk.h"

/* Converts VTK-files into PNG-files by a given pattern.
 */
int main(int argc, char** argv) {
  VTK    vtk;
  glob_t globbuf;
  size_t frame;

  if (argc < 2) {
    fprintf(stderr, "ERROR: not enough arguments given.\n\n");
    fprintf(stderr, "usage: ./vtk2png \"<pattern>\"\nexample: ./vtk2png \"output-*.vtk\"\n");
    fprintf(stderr, "- reads all files according to the pattern \"output-*.vtk\" and converts them into png files.\n");
    return EXIT_FAILURE;
  }
  
  glob(argv[1], 0, NULL, &globbuf);
  
  if (globbuf.gl_pathc == 0) {
    fprintf(stderr, "No matching files found (%s)\n", argv[1]);
    return EXIT_FAILURE;
  }
  
  VTK_init(&vtk, argv[1]);
  
  for (frame = 0; frame < globbuf.gl_pathc; frame++) {
    printf("converting frame %ld/%ld (%s)... ", frame+1, globbuf.gl_pathc, globbuf.gl_pathv[frame]);
    VTK_openFileName(&vtk, globbuf.gl_pathv[frame], "r");
    VTK_checkFileType(&vtk);
    VTK_readDimensions(&vtk);
    VTK_finishHeader(&vtk);
    VTK_convertVTK2PNG(&vtk, globbuf.gl_pathv[frame]);
    VTK_close(&vtk);
    printf("done\n");
  }
  
  VTK_deinit(&vtk);
  globfree(&globbuf);
  
  return EXIT_SUCCESS;
}
