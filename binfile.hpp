#ifndef LMOLLY_BINFILE_HPP___
#define LMOLLY_BINFILE_HPP___

#include <fstream>

namespace lmolly {

#define OUT_FILE_BUFFER_SIZE (64*1024)

class binfile_in {
public:
   binfile_in(std::string filename);
   virtual ~binfile_in();

   size_t get_size();

   uint8_t *get_data_pointer();

   uint8_t &operator[](uint32_t index);

   void save(std::string filename);

protected:
   void open();

   binfile_in &self;

   std::string mFileName;
   uint8_t    *mData;
   size_t      mSize;
};

class binfile_out {
public:
   binfile_out(std::string filename);
   virtual ~binfile_out();

   bool write(uint8_t *data, size_t size);
   void close();

protected:

   std::string   mFileName;
   uint8_t      *mData;
   size_t        mSize;
   uint32_t      mBufferSize;
   uint32_t      mPosition;

   std::ofstream mOS;

   void flush_buffer();
};

} // end namespace molly

#endif
