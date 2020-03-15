#define TJE_IMPLEMENTATION
#include "tiny_jpeg.h"
#include "jpeg_decoder.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

using std::vector;
using std::cout;
using std::endl;

bool load_jpg_data(const std::string fpath, vector<uint8_t>& data, int& width, int& height) {
  FILE *f = fopen(fpath.c_str(), "rb");
  if (!f) { return false; }
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  unsigned char *buf = (unsigned char*)malloc(size);
  fseek(f, 0, SEEK_SET);
  size_t read = fread(buf, 1, size, f);
  fclose(f);
  Jpeg::Decoder decoder(buf, size);
  if (decoder.GetResult() != Jpeg::Decoder::OK) {
    static const vector<std::string> error_msgs = { "OK", "NotAJpeg", "Unsupported", "OutOfMemory", "InternalError", "SyntaxError", "Internal_Finished" };
    cout << "Error decoding the input file " << error_msgs[decoder.GetResult()] << endl;
    return false;
  }
  if (!decoder.IsColor()) {
    cout << "Need a color image for this demo" << endl;
    return false;
  }
  width = decoder.GetWidth();
  height = decoder.GetHeight();
  data.resize(width*height*3);
  std::memcpy(data.data(), decoder.GetImage(), data.size());
  return true;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    cout << "Usage:- ./rotate input.jpg output.jpg" << endl;
    return -1;
  }
  vector<uint8_t> image;
  int width, height;
  if (load_jpg_data(argv[1], image, width, height)) {
    cout << "size " << width << " x " << height << endl;

    vector<uint8_t> rotated_image(height*width*3);
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        rotated_image[x*height*3+y*3+0] = image[y*width*3+x*3+0];
        rotated_image[x*height*3+y*3+1] = image[y*width*3+x*3+1];
        rotated_image[x*height*3+y*3+2] = image[y*width*3+x*3+2];
      }
    }
    tje_encode_to_file(argv[2], height, width, 3, rotated_image.data());
  } else {
    cout << "Failed to open file " << argv[1] << endl;
  }
}
