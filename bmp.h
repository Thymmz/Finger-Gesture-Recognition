
#include<stdio.h>
#include <stdint.h>
#pragma pack(push,1)

typedef struct {             
  char  type[2];             
  unsigned int  file_size;          // File size 
  short reserved1;                   
  short reserved2;
  unsigned int  offset;             // Offset 
  
  unsigned int  header_size;         //Header size(40 bytes)
  int  width;                 // Width of the image
  int  height;                // Height of image
  short  planes;              // Number of color planes
  short  bits_per_pixel;        // Bits per pixel
  unsigned int  compression;        // Compression type
  unsigned int  image_size;     // Image size in bytes
  int x_resolution;         // Pixels per meter
  int y_resolution;           // Pixels per meter
  unsigned int  num_colors;         // Number of colors  
  unsigned int  important_colors;   // Important colors 
} BMPHeader;
typedef struct{
    uint8_t b;
    uint8_t g;
    uint8_t r;
  //uint8_t gray;
}pixel_color;
/*typedef struct{
    int b;
    int g;
    int r;
}pixel_color;*/
typedef struct{
    int x;
    int y;
    int id;
}line;

#pragma pack(pop)