#include "visiontcppackage.h"

VisionTcpPackage::VisionTcpPackage(int64_t timestamp) : requestId(timestamp)
{

}

void VisionTcpPackage::clear()
{
    requestId = -1;
    headerSize = 0;
    bufferSize = 0;
    header.clear();
    buffer.clear();
}
