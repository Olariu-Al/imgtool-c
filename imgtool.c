#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#pragma pack(push,1)
typedef struct {
	unsigned char semnatura[2];
	uint32_t size;
	uint32_t reserved;
	uint32_t offset;
	uint32_t header_size;
}HEADER;

typedef struct{ //headersize
	uint32_t width;
	uint32_t height;
	uint16_t planes;
	uint16_t bit_depth;
	uint32_t compression;
	uint32_t image_size;
	uint32_t x_per_m;
	uint32_t y_per_m;
	uint32_t important_colors;
}BITMAPINFOHEADER;
#pragma pack(pop) 

typedef struct{
	int height;
	int width;
	int pixel;
}INFORMATION;

typedef enum {
	INFO,
	GRAYSCALE,
	INVERSARE,
	BRIGHTNESS,
	BLUR,
	ROTATE,
	CROP,
	DEFAULT
}COMANDA;

typedef enum {
	BITMAPINFOHEADER_CASE,
	BITMAPV5HEADER_CASE,
	BITMAPV3INFOHEADER_CASE,
	DEFAULT_CASE
}BITMAPTYPE;

COMANDA get_command(char *argv){
	if(strcmp(argv,"info") == 0)
		return INFO;
	else if(strcmp(argv,"grayscale") == 0)
		return GRAYSCALE;
	else if(strcmp(argv,"invert") == 0)
		return INVERSARE;
	else if(strcmp(argv,"crop") == 0)
		return CROP;
	else if(strcmp(argv,"blur") == 0)
		return BLUR;
	else if(strcmp(argv,"brightness") == 0)
		return BRIGHTNESS;
	else if(strcmp(argv,"rotate") == 0)
		return ROTATE;
	else 
		return DEFAULT;
}

BITMAPTYPE get_type(uint32_t header_size){
	if(header_size == 40)
		return BITMAPINFOHEADER_CASE;
	else{
		return DEFAULT_CASE;
	}
}

void procesare(INFORMATION info){
	printf("Information about image:\n Height:%d\n Width:%d\n Number of bytes per pixel:%d\n",info.height,info.width,info.pixel);
}

int main(int argc,char **argv){
	if(argc < 3){
		printf("Invalid amount of arguments\n");
		exit(1);
	}
	COMANDA comanda=get_command(argv[1]);
	FILE *f=NULL;
	HEADER header;
	if((f=fopen(argv[2],"rb")) == NULL){
		perror(argv[2]);
		exit(1);
	}
	fread(&header,sizeof(HEADER),1,f);
	if(header.semnatura[0] != 'B' || header.semnatura[1] != 'M'){
		printf("Invalid file format\n");
		fclose(f);
		exit(1);
	}
	BITMAPTYPE type=get_type(header.header_size);
	INFORMATION info;
	switch(type){
		case BITMAPINFOHEADER_CASE:{
			BITMAPINFOHEADER info_header;
			fread(&info_header,sizeof(BITMAPINFOHEADER),1,f);
			info.height=info_header.height;
			info.width=info_header.width;
			info.pixel=info_header.bit_depth;
			break;
		}
		case BITMAPV5HEADER_CASE:{
			//inca nu e facut
		}
		case BITMAPV3INFOHEADER_CASE:{
			//inca nu e facut
		}
		case DEFAULT_CASE:{
			printf("File format not supported\n");
			fclose(f);
			exit(1);
		}
	}
	switch(comanda){
		case INFO:{
			procesare(info); 
		}
		case GRAYSCALE:{
			
		}
		case DEFAULT:{
			
		}
	}
	fclose(f);
}
