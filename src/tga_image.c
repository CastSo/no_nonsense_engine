#include "tga_image.h"

int fget_little_short(FILE *f) {
    uint8_t b1, b2;

    b1 = fgetc(f);
    b2 = fgetc(f);

    return (short)(b1 + b2*256);
}

struct color4ub *load_tga(char *file_name, TGAHeader *tga_header) {
    //Assumes bottom left origin
    FILE *fptr;
    fptr = fopen(file_name, "r");
    if (fptr == NULL) {
        perror("Unable to open file");
        exit(1);
    }
    
    
    int width, height, number_pixels;
    int row, column;
    uint8_t *tga_rgba;

    uint8_t bpp = 0;

    tga_header->id_length = fgetc(fptr);
    tga_header->colormap_type = fgetc(fptr);
    tga_header->data_type_code = fgetc(fptr);

    tga_header->colormap_origin = fget_little_short(fptr);
    tga_header->colormap_length = fget_little_short(fptr);
    tga_header->colormap_depth = fgetc(fptr);
    tga_header->x_origin = fget_little_short(fptr);
    tga_header->y_origin = fget_little_short(fptr);
    tga_header->width = fget_little_short(fptr);
    tga_header->height = fget_little_short(fptr);
    tga_header->bits_per_pixel = fgetc(fptr);
    tga_header->image_descriptor = fgetc(fptr);


    //Image data
    bool tga_is_RLE = false; //Checks is lossless (RLE) or lossy
    int req_comp; //number of components N
    int comp; //checks to see if opaque (checks number of channels)

    uint8_t *tga_palette = NULL;
    uint8_t raw_data[4];
	uint8_t trans_data[4] = {0,0,0,0};
    int RLE_count = 0;
	int RLE_repeating = 0;
	int read_next_pixel = 1;

    width = tga_header->width; //columns
    height = tga_header->height; //rows
    number_pixels = width * height;


    if (tga_header->data_type_code >= 8) {
        tga_header->data_type_code -= 8;
        tga_is_RLE = true;
    }
    //Error checkers
    if (tga_header->data_type_code < 1 || tga_header->data_type_code > 3) {
        perror("LoadTGA: Invalid image type.\n");
        return NULL;
    }

    if ((tga_header->bits_per_pixel !=32 && tga_header->bits_per_pixel!=24 && tga_header->bits_per_pixel!=16 && tga_header->bits_per_pixel!=8))
	{	
        perror("Texture_LoadTGA: Invalid images supported (no colormaps)\n");
        return NULL;
    }




    tga_header->image_descriptor = 1 - ((tga_header->image_descriptor >> 5) & 1);
    
    //Set to RGBA format
    req_comp = 4;
    comp = tga_header->bits_per_pixel / 8;

    color4ub *pixbuf = malloc(sizeof(color4ub) * width * height);

    if (tga_header->id_length != 0)
        fseek(fptr, tga_header->id_length, SEEK_CUR); //Skip TARGA image comment


    for (int i = 0; i < width * height; i++) {
        if(tga_is_RLE) {
            if (RLE_count == 0) {
                //run length encoding
                int RLE_cmd = getc(fptr);
                
                RLE_count = 1 + (RLE_cmd & 127);
                RLE_repeating = RLE_cmd >> 7;
                //printf("(u8int: %d, [%d, %d]), ", RLE_cmd, RLE_count, RLE_repeating);
                read_next_pixel = 1;
            } else if (!RLE_repeating) {
                read_next_pixel = 1;
            }
        } else {
            read_next_pixel = 1;
        }

        if (read_next_pixel) {
            if(tga_header->colormap_type) {
                int pal_idx = getc(fptr);
                if(pal_idx >= tga_header->colormap_length) {
                    //invalid index
                    pal_idx = 0;
                }
                pal_idx *= tga_header->bits_per_pixel / 8;
                for(int j = 0; j*8 < tga_header->bits_per_pixel; j++)
				{
					raw_data[j] = tga_palette[pal_idx+j];
                    
				}
            }else {
                for(int j = 0; j*8 < tga_header->bits_per_pixel; j++)
				{
					raw_data[j] = getc(fptr);
                    //printf("%d, ", raw_data[j]);
				}
            }
            //Convert raw to RGBA
        switch(tga_header->bits_per_pixel) {
            case 8:
                trans_data[0] = raw_data[0];
                trans_data[1] = raw_data[0];
                trans_data[2] = raw_data[0];
                trans_data[3] = 255;
                break;
            case 16:
                trans_data[0] = raw_data[0];
                trans_data[1] = raw_data[0];
                trans_data[2] = raw_data[0];
                trans_data[3] = raw_data[1];
                break;
            case 24:
            //RGB
                trans_data[0] = raw_data[2];
                trans_data[1] = raw_data[1];
                trans_data[2] = raw_data[0];
                trans_data[3] = 255;
                break;
            case 32:
            //RGBA
                trans_data[0] = raw_data[2];
                trans_data[1] = raw_data[1];
                trans_data[2] = raw_data[0];
                trans_data[3] = raw_data[3];
                break;
            }
            read_next_pixel = 0;
        }

        uint8_t y =  (uint8_t)(((trans_data[0]*77) + (trans_data[1]*150) + (trans_data[2]*29)) >> 8);
        //Finishes conversion into pixel buffer
        switch( req_comp )
		{
		case 1:
            pixbuf[i].r = y;
            break;
		case 2:
            pixbuf[i].r = y;
            pixbuf[i].a = trans_data[3];
			break;
		case 3:
            pixbuf[i].r = trans_data[0];
            pixbuf[i].g = trans_data[1];
            pixbuf[i].b = trans_data[2];
			break;
		case 4:
            pixbuf[i].r = trans_data[0];
            pixbuf[i].g = trans_data[1];
            pixbuf[i].b = trans_data[2];
            pixbuf[i].a = trans_data[3];
			break;
		}
        
        RLE_count--;
    }
    
    //Determines if invert image
    if (tga_header->image_descriptor) {

        for (int j = 0; j < height/2; j++) {
            int a = j * width;
            int b = (height - 1 - j) * width;
            for (int i = 0; i < width; i++) {
                color4ub tmp = pixbuf[a];
                pixbuf[a] = pixbuf[b];
                pixbuf[b] = tmp;
                a++;
                b++;
            }
        }

    }

    if(tga_palette != NULL)
        free(tga_palette);
    

    fclose(fptr);
    
    return pixbuf;
}

