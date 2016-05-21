#include "FModBankExporter.h"
#include "RoasterUtility.h"

#include <windows.h>
#include <fstream>
#include <sstream>
#include <iostream>

void FModBankExporter::ExportEventFiles(const std::string& filepath)
{
    std::ifstream iFile(filepath + "\\" + FModGUIDFile);

    if(!iFile.is_open())
    {
        std::cout << "Failed to open " << FModGUIDFile << std::endl;
        return;
    }

    std::string line;
    std::getline(iFile,line);

    while(!iFile.eof())
    {
        unsigned int guidLeft = line.find_first_of('{') + 1;
        unsigned int guidRight = line.find_first_of('}',guidLeft);
        unsigned int pathLeft = line.find_first_not_of(' ',guidRight + 1);
        unsigned int pathRight = line.length();

        if((line.substr(pathLeft,line.find_first_of(':') - pathLeft)) == "event")
        {
            // Get event name
            std::string eventName(line.substr(line.find_last_of('/') + 1,pathRight));

            // If file already generated, skip
            std::ifstream tempIFile(filepath + "\\" + eventName + VenteExt,std::ios::binary);

            if(!tempIFile.is_open())
            {
                std::ofstream oFile(filepath + "\\" + eventName + VenteExt,std::ios::binary);

                // get GUID
                GUID guid;
                std::stringstream ss;

                // 8 hexadecimal digits (4 bytes)
                ss << line.substr(guidLeft,8) << '\0';
                ss >> std::hex >> guid.Data1;
                ss.str(std::string());
                ss.clear();

                // 4 hexadecimal digits (2 bytes)
                guidLeft += 9;
                ss << line.substr(guidLeft,4) << '\0';
                ss >> std::hex >> guid.Data2;
                ss.str(std::string());
                ss.clear();

                // 4 hexadecimal digits (2 bytes)
                guidLeft += 5;
                ss << line.substr(guidLeft,4) << '\0';
                ss >> std::hex >> guid.Data3;
                ss.str(std::string());
                ss.clear();

                // 4 hexadecimal digits (2 x 1 bytes)
                guidLeft += 5;
                ss << line.substr(guidLeft,4) << '\0';
                ss >> std::hex >> *reinterpret_cast<WORD*>(&guid.Data4[0]);
                std::swap(guid.Data4[0],guid.Data4[1]);
                ss.str(std::string());
                ss.clear();

                // 2 x 6 hexadecimal digits (3 x 2 byte)
                guidLeft += 5;
                ss << line.substr(guidLeft,4) << '\0';
                ss >> std::hex >> *reinterpret_cast<WORD*>(&guid.Data4[2]);
                std::swap(guid.Data4[2],guid.Data4[3]);
                ss.str(std::string());
                ss.clear();
                guidLeft += 4;
                ss << line.substr(guidLeft,4) << '\0';
                ss >> std::hex >> *reinterpret_cast<WORD*>(&guid.Data4[4]);
                std::swap(guid.Data4[4],guid.Data4[5]);
                ss.str(std::string());
                ss.clear();
                guidLeft += 4;
                ss << line.substr(guidLeft,4) << '\0';
                ss >> std::hex >> *reinterpret_cast<WORD*>(&guid.Data4[6]);
                std::swap(guid.Data4[6],guid.Data4[7]);
                ss.str(std::string());
                ss.clear();

                oFile.write(reinterpret_cast<const char*>(&guid),sizeof(guid)+1);
                oFile << '\0';
                oFile.close();

                std::cout << "Exported " << eventName + VenteExt << std::endl;
            }
            tempIFile.close();
        }
        std::getline(iFile,line);
    }
    iFile.close();
}