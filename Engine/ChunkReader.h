#ifndef CHUNKREADER_H_
#define CHUNKREADER_H_

#include <fstream>

struct GChunk
{
  unsigned Type;
  unsigned Size;
  unsigned StartPos;
  unsigned EndPos;
};

class ChunkReader
{
public:
  ChunkReader(){};

  bool Open(std::string filename)
  {
    file.open(filename.c_str() , std::ios::binary  | std::ios::in);

    if(file.is_open())
      return true;
    else
      return false;
  }

  void Close()
  {
    file.close();
  }

  GChunk PeekChunk()
  {
    GChunk c = ReadChunkHeader();
    file.seekg(-int( sizeof(unsigned)*2 ) , std::ios::cur);
    return c;
  }

  GChunk ReadChunkHeader()
  {
    GChunk c;
    c.Type = 0;
    file.read((char*)&c.Type , sizeof(unsigned)*2);
    c.StartPos = (unsigned)file.tellg();
    c.EndPos = c.StartPos + c.Size;
    return c;
  }

  GChunk ReadUntilChunk(unsigned chunktype)
  {
    GChunk Chunk = ReadChunkHeader();		
    while(Chunk.Type != chunktype && Chunk.Type!=0)
    {
      SkipChunk(Chunk);
      Chunk = ReadChunkHeader();		
    }
    return Chunk;
  }

  void SkipChunk(GChunk& c)
  {
    file.seekg(c.EndPos);
  }

  void MoveToChunk(GChunk& c)
  {
    file.clear();
    file.seekg(c.StartPos);
  }

  template< typename type>
  void Read(type& data)
  {
    file.read((char*)&data , sizeof(type));	
  }

  void Read(std::string& str)
  {
    unsigned char strSize = 0;
    Read(strSize);
    str.resize(strSize);
    ReadArray( &str[0] , strSize );
  }

  template< typename type>
  void ReadArray(type * data , int Number)
  {
    file.read((char*)data , sizeof(type) * Number);
  }

  template< typename type>
  void ReadArraySize( type * data , int Size)
  {
    file.read((char*)data , Size);
  }

private:
  std::ifstream file;
};

#endif