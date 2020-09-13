#ifndef VISIONTCPPACKAGE_H
#define VISIONTCPPACKAGE_H

#include <stdint.h>
#include <string>
#include <vector>

class VisionTcpPackage
{
public:
    int64_t requestId;
    uint64_t headerSize = 0;
    uint64_t bufferSize = 0;
    std::string header;
    std::vector<uint8_t> buffer;
    static const uint64_t U_LONG = sizeof (uint64_t);
    static const uint64_t S_LONG = sizeof (int64_t);
    VisionTcpPackage(int64_t timestamp);
    void clear();
};

#endif // VISIONTCPPACKAGE_H
