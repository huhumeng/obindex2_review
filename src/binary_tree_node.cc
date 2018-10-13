#include "binary_tree_node.h"

namespace obindex2 {

BinaryTreeNode::BinaryTreeNode() :
    is_leaf_(false),
    is_bad_(false),
    desc_(nullptr),
    root_(nullptr)
{}

BinaryTreeNode::BinaryTreeNode(const bool leaf,
                               BinaryDescriptorPtr desc,
                               BinaryTreeNodePtr root) :
    is_leaf_(leaf),
    is_bad_(false),
    desc_(desc),
    root_(root)
{}

}  // namespace obindex2
