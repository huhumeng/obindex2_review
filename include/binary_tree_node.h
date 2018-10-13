#pragma once

#include <vector>
#include <unordered_set>

#include "binary_descriptor.h"

namespace obindex2{

class BinaryTreeNode;
typedef std::shared_ptr<BinaryTreeNode> BinaryTreeNodePtr;

class BinaryTreeNode{
public:
    
    // Constructors
    BinaryTreeNode();
    explicit BinaryTreeNode(const bool leaf,
                            BinaryDescriptorPtr desc = nullptr,
                            BinaryTreeNodePtr root = nullptr);

    // Methods
    inline bool isLeaf() const {
        return is_leaf_;
    }

    inline void setLeaf(const bool leaf){
        is_leaf_ = leaf;
    }

    inline bool isBad() const {
        return is_bad_;
    }

    inline void setBad(const bool bad){
        is_bad_ = bad;
    }

    inline BinaryDescriptorPtr getDescriptor() const {
        return desc_;
    }

    inline void setDescriptor(BinaryDescriptorPtr desc){
        desc_ = desc;
    }

    inline BinaryTreeNodePtr getRoot() const {
        return root_;
    }

    inline void setRoot(BinaryTreeNodePtr root){
        root_ = root;
    }

    inline double distance(BinaryDescriptorPtr desc) const {
        return obindex2::BinaryDescriptor::distHamming(*desc_, *desc);
    }

    inline void addChildNode(BinaryTreeNodePtr child){
        ch_nodes_.insert(child);
    }

    inline void deleteChildNode(BinaryTreeNodePtr child){
        ch_nodes_.erase(child);
    }

    inline std::unordered_set<BinaryTreeNodePtr>* getChildrenNodes(){
        return &ch_nodes_;
    }

    inline unsigned childNodeSize() const {
        return ch_nodes_.size();
    }

    inline void addChildDescriptor(BinaryDescriptorPtr child){
        ch_descs_.insert(child);
    }

    inline void deleteChildDescriptor(BinaryDescriptorPtr child){
        ch_descs_.erase(child);
    }

    inline std::unordered_set<BinaryDescriptorPtr>* getChildrenDescriptors(){
        return &ch_descs_;
    }

    inline unsigned childDescriptorSize() const {
        return ch_descs_.size();
    }

    inline void selectNewCenter(){
        desc_ = *std::next(ch_descs_.begin(), rand() % ch_descs_.size());
    }

private:

    bool is_leaf_;
    bool is_bad_;
    BinaryDescriptorPtr desc_;
    BinaryTreeNodePtr root_;
    std::unordered_set<BinaryTreeNodePtr> ch_nodes_;
    std::unordered_set<BinaryDescriptorPtr> ch_descs_;
};

typedef std::unordered_set<BinaryTreeNodePtr> NodeSet;

}  // namespace obindex2
