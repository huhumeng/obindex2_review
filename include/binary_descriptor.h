#pragma once

#include <bitset>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_set>

#include <boost/dynamic_bitset.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/hal/hal.hpp>

namespace obindex2 {

class BinaryDescriptor {
public:

    // Constructors
    explicit BinaryDescriptor(const unsigned nbits = 256);                 // 初始化数据为0
    explicit BinaryDescriptor(const unsigned char* bits, unsigned nbytes); // 从bits拷贝数据
    explicit BinaryDescriptor(const cv::Mat& desc);                        // 从cv::Mat拷贝数据
    explicit BinaryDescriptor(const BinaryDescriptor& bd);                 // 拷贝构造函数
    
    // Destructor
    // 析构存储的数据
    virtual ~BinaryDescriptor();

    // Methods
    
    // set bit to 1
    inline void set(int nbit) {
        
        // Detecting the correct byte
        int nbyte = nbit / 8;
        int nb = 7 - (nbit % 8);

        // Setting the bit
        bits_[nbyte] |= 1 << nb;
    }

    // set bit to 0
    inline void reset(int nbit) {
        
        // Detecting the correct byte
        int nbyte = nbit / 8;
        int nb = 7 - (nbit % 8);

        // Setting the bit
        bits_[nbyte] &= ~(1 << nb);
    }

    inline int size() const {
        return static_cast<int>(size_in_bits_);
    }

    // 计算汉明距离
    inline static double distHamming(const BinaryDescriptor& a,
                                     const BinaryDescriptor& b)
    {
        int hamming = cv::hal::normHamming(a.bits_, b.bits_, a.size_in_bytes_);
        return static_cast<double>(hamming);
    }

    // Operator overloading
    inline bool operator==(const BinaryDescriptor& d) {
        int hamming = cv::hal::normHamming(bits_, d.bits_, size_in_bytes_);
        
        return hamming == 0;
    }

    inline bool operator!=(const BinaryDescriptor& d) {
        int hamming = cv::hal::normHamming(bits_, d.bits_, size_in_bytes_);
        
        return hamming != 0;
    }

    // 复制构造函数
    inline BinaryDescriptor& operator=(const BinaryDescriptor& other) {
        
        // Clearing previous memory
        if (bits_ != nullptr) {
            delete [] bits_;
        }

        // Allocating new memory
        size_in_bits_ = other.size_in_bits_;
        size_in_bytes_ = other.size_in_bytes_;
        
        bits_ = new unsigned char[size_in_bytes_];
        
        memcpy(bits_, other.bits_, sizeof(unsigned char) * size_in_bytes_);

        return *this;
    }

    // 重载与
    inline BinaryDescriptor& operator&=(const BinaryDescriptor& other) {
        
        unsigned size = other.size_in_bytes_;
        for(unsigned i = 0; i < size; i++) {
            bits_[i] = bits_[i] & other.bits_[i];
        }

        return *this;
    }

    // 重载或
    inline BinaryDescriptor& operator|=(const BinaryDescriptor& other) {
        
        unsigned size = other.size_in_bytes_;
        
        for (unsigned i = 0; i < size; i++) {
            bits_[i] = bits_[i] | other.bits_[i];
        }

        return *this;
    }

    cv::Mat toCvMat();
    std::string toString();

    // For simplicity, we made it public, but you should use the public methods
    unsigned char* bits_;
    unsigned size_in_bytes_;
    unsigned size_in_bits_;
};

typedef std::shared_ptr<BinaryDescriptor> BinaryDescriptorPtr;          // 智能指针包装
typedef std::unordered_set<BinaryDescriptorPtr> BinaryDescriptorSet;    
typedef std::shared_ptr<BinaryDescriptorSet> BinaryDescriptorSetPtr;

}  // namespace obindex2

