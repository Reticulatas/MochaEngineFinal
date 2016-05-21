#ifndef FMODBANKEXPORTER_H
#define FMODBANKEXPORTER_H

#include <string>

class FModBankExporter
{
public:
    FModBankExporter();
    static void ExportEventFiles(const std::string& filepath);
};

#endif