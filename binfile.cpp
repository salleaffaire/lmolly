#include "binfile.hpp"
#include <fstream>
#include <cstring>

namespace lmolly {

// binfile_in definition
// ------------------------------------------------------------------------------

binfile_in::binfile_in(std::string filename) : self(*this), 
                                               mFileName(filename),
                                               mData((uint8_t *)0), 
                                               mSize(0) {
   open();
}

binfile_in::~binfile_in() {
   if (mData) {
      delete [] mData;
   }
}

size_t binfile_in::get_size() {
   return mSize;
}

uint8_t *binfile_in::get_data_pointer() {
   return mData;
}

uint8_t &binfile_in::operator[](uint32_t index) {
   return *(mData+index);
}

void binfile_in::save(std::string filename) {
   std::ofstream os;
   os.open(filename.c_str(), std::ios::binary);

   os.write((char *)mData, mSize);

   os.close();
}

void binfile_in::open() {

   if (mData) {
      delete [] mData;
   }

   std::ifstream is;
   is.open(mFileName.c_str(), std::ios::binary|std::ios::ate);

   mSize = is.tellg();
   is.seekg(0, std::ios::beg);

   mData = new uint8_t [mSize];

   is.read((char *)mData, mSize);

   is.close();
}

// binfile_out definition
// ------------------------------------------------------------------------------

binfile_out::binfile_out(std::string filename) : mFileName(filename),
                                                 mData(0),
                                                 mSize(0),
                                                 mBufferSize(OUT_FILE_BUFFER_SIZE),
                                                 mPosition(0) {
   // Allocate interbal buffer
   mData = new uint8_t [OUT_FILE_BUFFER_SIZE];

   // Open the file
   mOS.open(mFileName.c_str(), std::ios::binary);
}

binfile_out::~binfile_out() {
   if (mData) delete [] mData;
}

bool binfile_out::write(uint8_t *data, size_t size) {
   bool rval = true;

   uint32_t bytes_left_in_buffer = OUT_FILE_BUFFER_SIZE - mPosition;

   if (size > bytes_left_in_buffer) {
      // First, writes the buffer to file
      flush_buffer();

      // Write direct to file
      mOS.write((char *)data, size);
   }
   else {
      std::memcpy(mData+mPosition, data, size);
      mPosition += size;

      if (mPosition == mBufferSize) {
         flush_buffer();
      }
   }

   return rval;
}

void binfile_out::close() {
   flush_buffer();
   mPosition = 0;
   mOS.close();
}

void binfile_out::flush_buffer() {
   mOS.write((char *)mData, mPosition);
   mPosition = 0;
}

} // end namespace molly