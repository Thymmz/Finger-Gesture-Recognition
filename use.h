#include<stdio.h>
#include<stdlib.h>
#include "bmp.h"
#include "switch.h"

 int filesize(FILE *ifp)
 {
     int position=ftell(ifp);
     if(position==-1)
         return -1;
     if(fseek(ifp,0,SEEK_END)!=0)
         return -2;
     int fsize=ftell(ifp);
     if(fsize==-1)
         return -3;
     if (fseek(ifp,position, SEEK_SET) != 0)
         return -4;
     return fsize;
 }

 int imagesize(BMPHeader *header)
 {
     int isize;
     isize=abs(((header->width*3 + (4- (header->width*3)%4 )%4) * header->height));
     return isize;
 }



 int check_header(BMPHeader *header,FILE *ifp)
 {
     int n=0;
 if(header->type[0]!= 'B' && header->type[0]!= 'M')
     n=1;
 else if(header->offset!=54)
     n=1;
 else if(header->header_size!=40)
     n=1;
 else if(header->planes!=1)
     n=1;
 else if(header->compression!=0)
     n=1;
 else if(header->num_colors!=0)
    n=1;
 else if(header->important_colors!=0)
     n=1;
 else if(header->bits_per_pixel!=24)
     n=1;
 else if(header->file_size!=filesize(ifp))
    {
        printf("1");
      n=1;
 }
 else if(header->image_size!=imagesize(header))
     {
        printf("Invalid imagesize");
        n=1;}

 return n;
 }
int visited(int x,int y,int ht,int wt,line *l)
 {
     int c=0,i=0;
    while(l[i].id!=0)
    {
        if(l[i].x==x && l[i].y==y)
            c=1;
        i++;
    }
    if(c==1)
        return 1;
    else
        return 0;
}

int point(line *l)
{
    int i=0;
    while(l[i].id!=0)
    {
        i++;
    }
    return i;
}

int recur(int x,int y,int** gray,int index,int id, line *l, int ht, int wt)
{
    int c=0;
    if(x>=0 && y>=0 && x<ht && x<wt)
    {
    if(gray[x][y]==255)
    {
        c=visited(x,y,ht,wt,l);
        if(c==0)
        {
            l[index].x=x;
            l[index].y=y;
            l[index].id=id;
            index++;    
                index=point(l);
                recur(x-1,y,gray,index,id,l,ht,wt);
                index=point(l);
                recur(x+1,y,gray,index,id,l,ht,wt);
                index=point(l);
                recur(x,y-1,gray,index,id,l,ht,wt);
                index=point(l);
                recur(x,y+1,gray,index,id,l,ht,wt);
                index=point(l);
                recur(x-1,y-1,gray,index,id,l,ht,wt);
                index=point(l);
                recur(x-1,y+1,gray,index,id,l,ht,wt);
                index=point(l);
                recur(x+1,y-1,gray,index,id,l,ht,wt);
                index=point(l);
                recur(x+1,y+1,gray,index,id,l,ht,wt);

            }
        }
    return index;   
    }
    return 0;

}



IoCameraImage_t* imageinput(IoCameraImage_t* image)
{
FILE *ifp,*ofp;
char * data;
BMPHeader *header;
int num,n;
ifp= fopen("image.bmp" , "rb");
header=(BMPHeader*)malloc(sizeof(BMPHeader));
if(header==NULL)
{
    printf("Not enough memory.");
    exit(0);
}
if(ifp==NULL)
{
    printf("Cannot open image file.");
    exit(0);
}
 num= fread(header, sizeof(BMPHeader), 1,ifp);
if(num!=1)
{
    printf("Cannot read header.");
    exit(0);
}
n=check_header(header,ifp);
if(n==1)
{
    printf("Invalid header.");
    exit(0);
}
data= (char*)malloc(sizeof(char) * header->image_size);

if(data==NULL)
{
    printf("Not enough memory.");
    exit(0);
}
 num = fread(data, header->image_size, 1,ifp);
 if(num!=1)
{
    printf("Cannot read image.");
    exit(0);
}
int ht=abs(header->height),wt=abs(header->width),k;
    pixel_color  **pixel;
    int pad,i,j;
   // printf("ht=%d and wt=%d",ht,wt);

    pad=(header->image_size/ht)-wt *3;
    pixel= (pixel_color **)malloc(ht * sizeof(pixel_color*));

    for(i=0;i<ht;i++)
    {
        //printf("in i");
                
        pixel[i]=(pixel_color*)malloc(sizeof(pixel_color) * wt);
    }
    k=0;
    for(i=0;i<ht;i++)
    {
        for(j=0;j<wt;j++)
        {//printf("here");
            pixel[i][j].b=*(data+(k++));
            pixel[i][j].g=*(data+(k++));
            pixel[i][j].r=*(data+(k++));
        }
        k=k+pad;
    }

int red=0;
    for(i=0;i<ht;i++)
{
    for(j=0;j<wt;j++)
    {
        if((pixel[i][j].r>pixel[i][j].b && pixel[i][j].r>pixel[i][j].g) || (pixel[i][j].r>200))

        {
            red++;
        }
    }
}
printf("red=%d",red);


if(red>1000)
{
for(i=0;i<ht;i++)
{
    for(j=0;j<wt;j++)
    {
        if(!(pixel[i][j].r>pixel[i][j].b && pixel[i][j].r>pixel[i][j].g) || (pixel[i][j].r<200) )
        {
            pixel[i][j].b=0;
            pixel[i][j].g=0;
            pixel[i][j].r=0;
        }
        else
        {
            pixel[i][j].b=255;
            pixel[i][j].g=255;
            pixel[i][j].r=255;
        }
    }
}
  
int **gray,c,count=0;
line *l;
gray= (int **)malloc(ht * sizeof(int*));
    for(i=0;i<ht;i++)
    {
        gray[i]=(int*)malloc(sizeof(int) * wt);
    }
    
    for(i=0;i<ht;i++)
    {
        for(j=0;j<wt;j++)
        {
            gray[i][j]=(pixel[i][j].b*0.3)+ (pixel[i][j].g* 0.59) + (pixel[i][j].r*0.11);
        }
    }
    
    l=(line*)malloc(ht*wt*sizeof(line));
    for(i=0;i<ht*wt;i++)
    {
        l[i].x=0;
        l[i].y=0;
        l[i].id=0;

    } 


static int index=0;
    int id=1,first;
    for(i=0;i<ht;i++)
    {
        for(j=0;j<wt;j++)
        {
            if(gray[i][j]==255)
            {
                c=visited(i,j,ht,wt,l);
                if(c==0)
                {
                    index=recur(i,j,gray,index,id,l,ht,wt);
                    id++;
                }
            }
        }
    }   
    c=0,j=0;
    int len=point(l)-1;
    int linecount=0;
    while(j<len)
    {
        count=0;
        first=l[j].id;
        while(l[j].id==first)
        {
            count++;
            j++;
        }
        c=j-count;
        if(count<=20)
        {
    
            while(c<j)
            {
                gray[l[c].x][l[c].y]=0;
                c++;
            }
        }
        else
        {
            linecount++;
        }
    }
    int finger =linecount-1;
    printf("GESTURE=%d",finger);
    if(finger<=0)
{
    printf("No Gesture");
}
else
    {
        gesture(finger);
    }
k=0;
for(i=0;i<ht;i++)
{
    for(j=0;j<wt;j++) 
    {
        *(data+(k++)) =  gray[i][j];
        *(data+(k++)) =  gray[i][j];
        *(data+(k++)) =  gray[i][j];
    }
    k=k+pad;
}

index=0;

ofp= fopen("output.bmp" , "wb");
if(ofp==NULL)
{
    printf("Cannot open image file.");
    exit(0);
}


num= fwrite(header, sizeof(BMPHeader), 1, ofp);
if(num!=1)
{
    printf("Cannot write header.");
    exit(0);
}
num=fwrite(data,header->image_size, 1,ofp);
if(num!=1)
{
    printf("Cannot write image.");
    exit(0);
}


fclose(ifp);
fclose(ofp);
free(header);
free(data);
}
else
{
    printf("LASER LINE NOT DETECTED");
}

return 0;

}