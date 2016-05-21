#ifndef CHUNKWRITER_H_
#define CHUNKWRITER_H_

#include <string>
#include <fstream>

class ChunkFileWriter
{
public:
  ChunkFileWriter(){};

  bool Open(std::string filename)
  {
    file.open(filename.c_str() , std::ios::binary);

    if(file.is_open())
      return true;
    else
      return false;
  }

  void Close()
  {
    file.close();
  }

  unsigned StartChunk(unsigned Mark)
  {		
    unsigned chunkStartPos = (unsigned)file.tellp();
    Write(Mark);
    Write(Mark);
    return chunkStartPos;
  }

  unsigned GetPosition()
  {
    return (unsigned)file.tellp();
  }

  void EndChunk(unsigned Pos)
  {
    unsigned curpos = (unsigned)file.tellp();
    unsigned size = curpos - Pos;
    file.seekp(Pos + 4); //Move pass the header
    file.write((char*)&size , sizeof(unsigned));
    file.seekp(curpos);
  }

  void Write(std::string& str)
  {
    Write(unsigned char(str.size()));
    Write(&str[0] , str.size());
  }

  template< typename type>
  void Write( const type& data)
  {
    file.write((const char*)&data , sizeof(type));	
  }

  template< typename type>
  void Write( type * data , unsigned Number )
  {
    file.write((char*)data , sizeof(type) * Number);
  }

  void WriteChunk(unsigned Mark , unsigned Size)
  {
    Write(Mark);
    Write(Size);
  };

private:
  std::ofstream file;
};

#endif