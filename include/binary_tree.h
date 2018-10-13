#pragma once

#include <stdlib.h>
#include <time.h>

#include <limits>
#include <unordered_set>

#include "binary_descriptor.h"
#include "priority_queues.h"

namespace obindex2 {

class BinaryTree {
public:

    // Constructors
    explicit BinaryTree(BinaryDescriptorSetPtr dset,
                        const unsigned tree_id = 0,
                        const unsigned k = 16,
                        const unsigned s = 150);
    virtual ~BinaryTree();

    // Methods
    void buildTree();
    void deleteTree();
    unsigned traverseFromRoot(BinaryDescriptorPtr q,
                                NodeQueuePtr pq,
                                DescriptorQueuePtr r);
    void traverseFromNode(BinaryDescriptorPtr q,
                            BinaryTreeNodePtr n,
                            NodeQueuePtr pq,
                            DescriptorQueuePtr r);
    BinaryTreeNodePtr searchFromRoot(BinaryDescriptorPtr q);
    BinaryTreeNodePtr searchFromNode(BinaryDescriptorPtr q,
                                    BinaryTreeNodePtr n);
    void addDescriptor(BinaryDescriptorPtr q);
    void deleteDescriptor(BinaryDescriptorPtr q);
    void printTree();
    inline unsigned numDegradedNodes() {
        return degraded_nodes_;
    }

    inline unsigned numNodes() {
        return nset_.size();
    }

private:

    BinaryDescriptorSetPtr dset_;
    unsigned tree_id_;
    BinaryTreeNodePtr root_;
    unsigned k_;
    unsigned s_;
    unsigned k_2_;
    NodeSet nset_;
    std::unordered_map<BinaryDescriptorPtr, BinaryTreeNodePtr> desc_to_node_;

    // Tree statistics
    unsigned degraded_nodes_;
    unsigned nvisited_nodes_;

    void buildNode(BinaryDescriptorSet d, BinaryTreeNodePtr root);
    void printNode(BinaryTreeNodePtr n);
    void deleteNodeRecursive(BinaryTreeNodePtr n);
};

typedef std::shared_ptr<BinaryTree> BinaryTreePtr;

}  // namespace obindex2

