#ifndef CUSTOMGRAPH_H
#define CUSTOMGRAPH_H

#include "qcustomplot.h"
#include "graph_types.h"

#include <cstdint>
#include <vector>
#include <fstream>

#include <sys/stat.h>

class CustomGraph
{
public:
    enum CustomGraph_Type_ : uint16_t {
        CustomGraph_Type_int16_IQ,
        CustomGraph_Type_int32_IQ,
        CustomGraph_Type_float_IQ,
        CustomGraph_Type_double_IQ,

        CustomGraph_Type_Breakpoint,

        CustomGraph_Type_int16_plain,
        CustomGraph_Type_int32_plain,
        CustomGraph_Type_float_plain,
        CustomGraph_Type_double_plain
    };

protected:
    static bool exists(std::string path) {
        struct stat stBuffer;
        return (stat(path.c_str(), &stBuffer) == 0);
    }

    static uint64_t getSize(std::string path) {
        struct stat stBuffer;
        if (stat(path.c_str(), &stBuffer) == 0) {
            return stBuffer.st_size;
        }
        return 0;
    }

    static bool checkSize(uint64_t fileSize, uint16_t type) {
        switch (type) {
        case CustomGraph_Type_int16_IQ:
            return (fileSize % sizeof (datatypes::int16_iq_t) == 0);
        case CustomGraph_Type_int32_IQ:
            return (fileSize % sizeof (datatypes::int32_iq_t) == 0);
        case CustomGraph_Type_float_IQ:
            return (fileSize % sizeof (datatypes::float_iq_t) == 0);
        case CustomGraph_Type_double_IQ:
            return (fileSize % sizeof (datatypes::double_iq_t) == 0);
        case CustomGraph_Type_int16_plain:
            return (fileSize % sizeof (int16_t) == 0);
        case CustomGraph_Type_int32_plain:
            return (fileSize % sizeof (int32_t) == 0);
        case CustomGraph_Type_float_plain:
            return (fileSize % sizeof (float) == 0);
        case CustomGraph_Type_double_plain:
            return (fileSize % sizeof (double) == 0);
        default:
            return false;
        }
    }

    uint16_t graphType = 0;
    std::string filePath = "";
    uint64_t fileSize = 0;
    uint32_t pagesCount = 0;
    std::vector<char> byteBuffer;
    std::ifstream inputFile;
    uint64_t offset = 0;

//    std::vector<QCPGraph> graphs;

    static constexpr uint32_t pageSize = 32 * 1024 * 1024;

public:
    CustomGraph(std::string path, uint16_t type) {
        if (!exists(path))
            throw std::runtime_error("Target file not exists");

        fileSize = getSize(path);
        if (fileSize == 0)
            throw std::runtime_error("Target file is empty");

        if (!checkSize(fileSize, type))
            throw std::runtime_error("File data unalign for selected data type");

        if (fileSize > pageSize) {
            pagesCount = (fileSize - (fileSize % pageSize)) / pageSize;
            pagesCount = pageSize + ((fileSize % pageSize > 0) ? (fileSize % pageSize) : 0);

            this->byteBuffer.resize(pageSize);
        } else {
            this->byteBuffer.resize(fileSize);
        }

        inputFile.open(path, std::ios::binary);
        if (!inputFile.is_open()) {
            throw std::runtime_error("Can`t open target file \"" + path + "\"");
        }
    }

//    void update(uint64_t offset = 0) {
//        this->graphs.clear();

//        if (pagesCount == 0) {
//            if (graphType < CustomGraph_Type_Breakpoint) {
//                this->graphs.emplace_back();
//                this->graphs.emplace_back();
//            } else {
//                this->graphs.emplace_back();
//            }
//        } else {

//        }
//    }

//    std::vector<QCPGraph>::iterator getGraphsIt() {
//        return std::begin(this->graphs);
//    }
};

#endif // CUSTOMGRAPH_H
