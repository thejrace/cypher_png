#pragma once
#include "obarey_common.h";
#include <png.h>

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers;
int target_red = 122;
int message_length;
char * message = NULL;
char * img_file_name;
char * action_mode;
int random_array[255];
int encrypted_message[255];

void read_png_file(char *filename);
void write_png_file(char *filename);
void print_row_pointers(char *filename); // for debugging
void process_img_before_encryption();
void encrypt_message_to_img();
void read_message();
void read_img_name();
void read_encryption_info();
void obarey_enc_dec_init();
void decrypt_image();