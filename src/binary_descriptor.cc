#include "binary_descriptor.h"

namespace obindex2 {

BinaryDescriptor::BinaryDescriptor(const unsigned nbits) {
    
    assert(nbits % 8 == 0);
    size_in_bits_ = nbits;
    size_in_bytes_ = static_cast<unsigned>(nbits / 8);
    
    bits_ = new unsigned char[size_in_bytes_];
    
    // Initializing the bits
    memset(bits_, 0, sizeof(unsigned char)*size_in_bytes_);
}

BinaryDescriptor::BinaryDescriptor(const unsigned char* bits, unsigned nbytes) {
    
    assert(nbytes % 8 == 0);
    
    size_in_bits_ = nbytes * 8;
    size_in_bytes_ = nbytes;
    
    bits_ = new unsigned char[size_in_bytes_];
    
    memcpy(bits_, bits, sizeof(unsigned char) * nbytes);
}

BinaryDescriptor::BinaryDescriptor(const cv::Mat& desc) {
    
    assert(desc.type() == CV_8U);

    size_in_bytes_ = static_cast<unsigned>(desc.cols);
    size_in_bits_ = size_in_bytes_ * 8;
    bits_ = new unsigned char[size_in_bytes_];

    // Creating the descriptor
    const unsigned char* chars = desc.ptr<unsigned char>(0);
    memcpy(bits_, chars, sizeof(unsigned char) * size_in_bytes_);
}

BinaryDescriptor::BinaryDescriptor(const BinaryDescriptor& bd) :
    size_in_bytes_(bd.size_in_bytes_),
    size_in_bits_(bd.size_in_bits_)
{    
    bits_ = new unsigned char[size_in_bytes_];
    memcpy(bits_, bd.bits_, sizeof(unsigned char) * size_in_bytes_);
}

BinaryDescriptor::~BinaryDescriptor(){
    delete [] bits_;
}

cv::Mat BinaryDescriptor::toCvMat(){
    
    cv::Mat m = cv::Mat::zeros(1, size_in_bytes_, CV_8U);
    unsigned char* d = m.ptr<unsigned char>(0);
    
    memcpy(d, bits_, sizeof(unsigned char) * size_in_bytes_);
    
    return m.clone();
}

std::string BinaryDescriptor::toString(){
    
    boost::dynamic_bitset<> b(size_in_bits_);
    for(unsigned i = 0; i < size_in_bytes_; i++) {
        
        unsigned char cur = bits_[i];
        int offset = i * 8;
        
        for(int bit = 0; bit < 8; bit++){
            b[offset] = cur & 1;
            offset++;   // Move to next bit in b
            cur >>= 1;  // Move to next bit in array
        }
    }

    std::string st;
    
    to_string(b, st);
    return st;
}

}  // namespace obindex2
