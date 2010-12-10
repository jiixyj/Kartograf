/* See LICENSE file for copyright and license details. */
#include <png.h>

png_bytep* read_png_file(const char* file_name, uint32_t& width, uint32_t& height) {
  char header[8];

  FILE *fp = fopen(file_name, "rb");
  if (!fp)
    throw std::runtime_error("[read_png_file] File could not be opened for reading");
  size_t bytes_read = fread(header, 1, 8, fp);
  if (bytes_read != 8) {
    throw std::runtime_error("Could not read png header!");
  }
  if (png_sig_cmp(reinterpret_cast<png_byte*>(header), 0, 8))
    throw std::runtime_error("[read_png_file] File is not recognized as a PNG file");

  /* initialize stuff */
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
    throw std::runtime_error("[read_png_file] png_create_read_struct failed");
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    throw std::runtime_error("[read_png_file] png_create_info_struct failed");
  if (setjmp(png_jmpbuf(png_ptr)))
    throw std::runtime_error("[read_png_file] Error during init_io");

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);

  png_read_info(png_ptr, info_ptr);

  width = static_cast<uint32_t>(png_get_image_width(png_ptr, info_ptr));
  height = static_cast<uint32_t>(png_get_image_height(png_ptr, info_ptr));

  png_read_update_info(png_ptr, info_ptr);

  /* read file */
  if (setjmp(png_jmpbuf(png_ptr)))
    throw std::runtime_error("[read_png_file] Error during read_image");

  png_bytep* row_pointers = new png_bytep[height];
  for (size_t y = 0; y < height; y++)
    row_pointers[y] = new png_byte[png_get_rowbytes(png_ptr,info_ptr)];

  png_read_image(png_ptr, row_pointers);

  fclose(fp);
  return row_pointers;
}
