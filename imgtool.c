#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define HEADER_SIZE 14

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

typedef struct {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
}PIXEL;

#pragma pack(pop) 

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

char get_type(HEADER header){
	if(header.semnatura[0] != 'B' || header.semnatura[1] != 'M'){
		return 0;
	}
	if(header.header_size <40){
		return 0;
	}
	return 1;
}

void procesare(FILE *f){
	BITMAPINFOHEADER info;
	fread(&info,sizeof(BITMAPINFOHEADER),1,f);
	printf("Information about image:\n Height:%d\n Width:%d\n Number of bytes per pixel:%d\n",info.height,info.width,info.bit_depth);
}

void grayscale(HEADER header,FILE *f,char *out){
	BITMAPINFOHEADER info;
	fread(&info,sizeof(BITMAPINFOHEADER),1,f);
	FILE *f2;
	PIXEL pixel;
	unsigned char gray[3];
	int padding=(4-(info.width*3)%4)%4;	
	f2=fopen(out,"wb");
	header.header_size=40;
	fwrite(&header,sizeof(HEADER),1,f2);
	fwrite(&info,sizeof(BITMAPINFOHEADER),1,f2);
	fseek(f,header.offset,SEEK_SET);
	fseek(f2,header.offset,SEEK_SET);
	for(unsigned int i=0;i<info.height;i++){
		for(unsigned int j=0;j<info.width;j++){
			fread(&pixel,sizeof(pixel),1,f);
			gray[0]=(0.299 * pixel.red)+(0.587 * pixel.green) + (0.114 * pixel.blue);
			gray[1]=gray[0];
			gray[2]=gray[1];
			fwrite(&gray,sizeof(unsigned char),3,f2);
		}
		gray[0]=0;
		for(int k=0;k<padding;k++){
			fwrite(&gray,sizeof(unsigned char),1,f2);
		}
	}
	fclose(f2);
}

void inverse(HEADER header,FILE *f,char *out){ //este efectiv functia pt grayscale dar se modifica formula
	BITMAPINFOHEADER info;
	fread(&info,sizeof(BITMAPINFOHEADER),1,f);
	FILE *f2;
	PIXEL pixel;
	unsigned char inverse[3];
	int padding=(4-(info.width*3)%4)%4;	
	f2=fopen(out,"wb");
	header.header_size=40;
	fwrite(&header,sizeof(HEADER),1,f2);
	fwrite(&info,sizeof(BITMAPINFOHEADER),1,f2);
	fseek(f,header.offset,SEEK_SET);
	fseek(f2,header.offset,SEEK_SET);
	for(unsigned int i=0;i<info.height;i++){
		for(unsigned int j=0;j<info.width;j++){
			fread(&pixel,sizeof(pixel),1,f);
			inverse[0]=255-pixel.blue;
			inverse[1]=255-pixel.green;
			inverse[2]=255-pixel.red;
			fwrite(&inverse,sizeof(unsigned char),3,f2);
		}
		inverse[0]=0;
		for(int k=0;k<padding;k++){
			fwrite(&inverse,sizeof(unsigned char),1,f2);
		}
	}
	fclose(f2);
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
	char type=get_type(header);
	if(!type){
		printf("Invalid format\n");
		fclose(f);
		exit(1);
	}
	switch(comanda){
		case INFO:{
			procesare(f); 
			break;
		}
		case GRAYSCALE:{
			if(argc != 4){
				printf("Invalid amount of arguments\n");
				fclose(f);
				exit(1);
			}
			grayscale(header,f,argv[3]);
			break;
		}
		case INVERSARE:{
			if(argc != 4){
				printf("Invalid amount of arguments\n");
				fclose(f);
				exit(1);
			}
			inverse(header,f,argv[3]);
			break;
		}
		case DEFAULT:{
			printf("Invalid command, write -help for a list of the commands\n");
		}
	}
	fclose(f);
}
