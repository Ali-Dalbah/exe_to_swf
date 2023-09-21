#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exe_to_swf(const char *, const char *);

int main(int argc, char** argv) {
  DIR *d;
  struct dirent *dir;
  d = opendir("files/");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      char outname[100];
      if (dir->d_type == DT_REG) {
        char name[100];
        strcpy(name, dir->d_name);
        strtok(name, ".");
        printf("%s\n", dir->d_name);
        char output[100] = "outputs/";
        strcat(output, name);
        strcat(output, ".swf");
        printf("%s\n", output);
        char fullname[100] = "files/";
        strcat(fullname, name);
        strcat(fullname, ".exe");
        exe_to_swf(fullname, output);
      }
    }
    closedir(d);
  }
  return 0;
}

void exe_to_swf(const char *filename, const char *output) {
  int szFile;
  char dataEnd[8];
  uint32_t lpNumberOfBytesRW;
  void *hfInput, *hfOutput, *hMemHeap;
  hfInput = fopen(filename, "rb");
  if (hfInput == NULL) {
    printf("Cannot open %s for reading!\r\n", filename);
  } else {
    fseek(hfInput, -8L, SEEK_END);
    szFile = ftell(hfInput);
    fread(dataEnd, 8, 1, hfInput);
    if (*(uint32_t *)&dataEnd == 0xFA123456) { // Check the signature
      hMemHeap = calloc(1, *(uint32_t *)&dataEnd[4]);
      if (hMemHeap != NULL) {
        fseek(hfInput, szFile - *(uint32_t *)&dataEnd[4], SEEK_SET);
        lpNumberOfBytesRW =
            fread(hMemHeap, 1, *(uint32_t *)&dataEnd[4], hfInput);
        hfOutput = fopen(output, "wb");
        if (hfOutput == NULL) {
          printf("Cannot create output file for writing!\r\n");
        } else {
          fwrite(hMemHeap, 1, *(uint32_t *)&dataEnd[4], hfOutput);
          fclose(hfOutput);
        }
        free(hMemHeap);
      } else {
        printf("Can't fetch the process heap!\r\n");
      }
    } else {
      printf("This doesn't seems to be a projector bundle.\r\n");
    }
    fclose(hfInput);
  }
}
