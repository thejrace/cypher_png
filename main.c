// Obarey Inc. 2018

#define _CRT_SECURE_NO_DEPRECATE

#include "main.h";

int main() {
	obarey_enc_dec_init();
	return 0;
}

void read_png_file(char *filename) {
	FILE *fp;
	if ( fp = fopen(filename, "rb")) {
		png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png) abort();

		png_infop info = png_create_info_struct(png);
		if (!info) abort();

		if (setjmp(png_jmpbuf(png))) abort();

		png_init_io(png, fp);

		png_read_info(png, info);

		width = png_get_image_width(png, info);
		height = png_get_image_height(png, info);
		color_type = png_get_color_type(png, info);
		bit_depth = png_get_bit_depth(png, info);

		// Read any color_type into 8bit depth, RGBA format.
		// See http://www.libpng.org/pub/png/libpng-manual.txt

		if (bit_depth == 16)
			png_set_strip_16(png);

		if (color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_palette_to_rgb(png);

		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
			png_set_expand_gray_1_2_4_to_8(png);

		if (png_get_valid(png, info, PNG_INFO_tRNS))
			png_set_tRNS_to_alpha(png);

		if (color_type == PNG_COLOR_TYPE_RGB ||
			color_type == PNG_COLOR_TYPE_GRAY ||
			color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

		if (color_type == PNG_COLOR_TYPE_GRAY ||
			color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png);

		png_read_update_info(png, info);

		row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
		for (int y = 0; y < height; y++) {
			row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
		}

		png_read_image(png, row_pointers);

		fclose(fp);
	} else {
		printf("Supplied image not found!. Please try again. \n");
		read_img_name();
	}

	
}

void print_row_pointers( char *filename ) {
	read_png_file(filename);
	for (int y = 1; y < height; y++) {
		png_bytep row = row_pointers[y];
		for (int x = 1; x < width; x++) {
			png_bytep px = &(row[x * 4]);

			printf("%4d, %4d = RGB(%3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
		}
	}
}

void write_png_file(char *filename) {
	int y;

	FILE *fp = fopen(filename, "wb");
	if (!fp) abort();

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) abort();

	png_infop info = png_create_info_struct(png);
	if (!info) abort();

	if (setjmp(png_jmpbuf(png))) abort();

	png_init_io(png, fp);

	png_set_IHDR(
		png,
		info,
		width, height,
		8,
		PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png, info);
	png_write_image(png, row_pointers);
	png_write_end(png, NULL);

	for ( y = 0; y < height; y++) {
		free(row_pointers[y]);
	}
	free(row_pointers);

	fclose(fp);
}

void encrypt_message_to_img() {

	// generate random pixel coords
	int random_pixel_x_array[255];
	int random_pixel_y_array[255];
	generate_random_ints(1, width, message_length, &random_pixel_x_array);
	generate_random_ints(1, height, message_length, &random_pixel_y_array);

	printf("\nRandom X cords = ");
	for (int k = 0; k < message_length; k++) printf(" %d ", random_pixel_x_array[k]);
	printf("\nRandom Y cords = ");
	for (int k = 0; k < message_length; k++) printf(" %d ", random_pixel_y_array[k]);
	printf("\n");

	int random_pixel_index = 0;
	int pixel_found_x_index = 0;
	int pixel_found_y_index = 0;
	for (int y = 1; y < height; y++) {
		png_bytep row = row_pointers[y];
		for (int x = 1; x < width; x++) {
			png_bytep px = &(row[x * 4]);
			// when we get to the our random pixel, hide the message
			if ( in_array(x, random_pixel_x_array, message_length, &pixel_found_x_index) && in_array(y, random_pixel_y_array, message_length, &pixel_found_y_index )) {
				px[0] = target_red;
				// use pixel_found_x_index to shuffle the message and random array
				px[1] = message[pixel_found_x_index];
				px[2] = random_array[pixel_found_x_index];
				// reset used random pixel indexes
				random_pixel_x_array[pixel_found_x_index] = 0;
				random_pixel_y_array[pixel_found_y_index] = 0;
				random_pixel_index++;
				printf("%4d, %4d = RGB(%3d, %3d, %3d) X:%d  Y:%d\n", x, y, px[0], px[1], px[2], pixel_found_x_index, pixel_found_y_index );
			}
		}
	}
}

void process_img_before_encryption() {
	for (int y = 0; y < height; y++) {
		png_bytep row = row_pointers[y];
		for (int x = 0; x < width; x++) {
			png_bytep px = &(row[x * 4]);
			// replace target red values
			if (px[0] == target_red) {
				//printf("Target Red match (%d, %d)\n", x, y);
				px[0] = target_red + 1;
			}
			// add encryption info to first pixell
			if (x == 0 && y == 0) {
				px[0] = message_length;
				px[1] = target_red;
			}
		}
	}
}

void read_img_name() {
	printf("Please enter the name of PNG image you want to process.( Write with the .png extension. )\n");
	img_file_name = scan_line(message);
}

void read_encryption_info() {
	for (int y = 0; y < 1; y++) {
		png_bytep row = row_pointers[y];
		for (int x = 0; x < 1; x++) {
			png_bytep px = &(row[x * 4]);
			message_length = px[0];
			target_red = px[1];
		}
	}
}

void decrypt_image() {
	int index = 0;
	for (int y = 1; y < height; y++) {
		png_bytep row = row_pointers[y];
		for (int x = 1; x < width; x++) {
			png_bytep px = &(row[x * 4]);
			if (px[0] == target_red) {
				encrypted_message[index] = px[1];
				random_array[index] = px[2];
				index++;
			}
		}
	}
	int message_sorted[255];
	int random_array_sorted[255];
	int k, j;
	for (k = 0; k < message_length; k++) random_array_sorted[k] = random_array[k];
	qsort(random_array_sorted, message_length, sizeof(int), compare);

	printf("Random Array = ");
	for (k = 0; k < message_length; k++) printf("  %d ", random_array[k]);
	printf("\nMessage Array = ");
	for (k = 0; k < message_length; k++) printf("  %d ", (char)encrypted_message[k]);

	for (k = 0; k < message_length; k++) {
		for (j = 0; j < message_length; j++) {
			if (random_array_sorted[k] == random_array[j]) {
				message_sorted[k] = encrypted_message[j];
				break;
			}
		}
	}
	printf("\nRandom Array Sorted = ");
	for (k = 0; k < message_length; k++) printf("  %d ", random_array_sorted[k]);
	printf("\n#### Decrypted Message  = ");
	for (k = 0; k < message_length; k++) printf("%c", (char)message_sorted[k]);
}

void obarey_enc_dec_init() {
	printf("\n\n");
	printf("**************** ----------- ******************\n");
	printf("**************** OBAREY INC. ******************\n");
	printf("**************** ----------- ******************\n");
	printf("\n\n");
	printf("For encryption type 'e', for decryption type 'd', hit Enter!\n");
	action_mode = scan_line(action_mode);

	if ( *action_mode == 'e' ) {
		printf("ENCRYPTION MODE INITIALIZED!\n");
		// get image name and check it, then read it
		read_img_name();
		read_png_file(img_file_name);

		read_message();

		// generate random array and sort it ASC
		generate_random_ints(0, 255, message_length, &random_array);
		qsort(random_array, message_length, sizeof(int), compare);
		printf("Random Array = ");
		for (int k = 0; k < message_length; k++) printf("  %d ", random_array[k]);

		// enc action
		process_img_before_encryption();
		encrypt_message_to_img();
		// save img
		write_png_file(img_file_name);
		printf("Encryption completed!");
	} else if ( *action_mode == 'd') {
		printf("DECRYPTION MODE INITIALIZED!\n");

		// get image name and check it, then read it
		read_img_name();
		read_png_file(img_file_name);

		read_encryption_info();

		decrypt_image();

	} else {
		printf("Invalid mode! Try again!\n");
		obarey_enc_dec_init();
	}
	printf("\n");
	obarey_enc_dec_init();

}

void read_message() {
	printf("Please enter message.( Message must be 1-255 characters long. )\n");
	message = scan_line(message);
	message_length = strlen(message);
	printf("Message = %s\n", message);
	printf("Message Length = %d\n", strlen(message));
	if (message_length == 0 || message_length > 255) {
		printf("Invalid message, please try again!\n");
		read_message();
	}
}