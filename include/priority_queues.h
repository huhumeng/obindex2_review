#pragma once

#include <algorithm>
#include <queue>
#include <string>
#include <sstream>
#include <vector>

#include "binary_descriptor.h"
#include "binary_tree_node.h"

namespace obindex2{

struct NodeQueueItem{

public:

    inline explicit NodeQueueItem(const double d,
                                const unsigned id,
                                BinaryTreeNodePtr n) :
        dist(d),
        tree_id(id),
        node(n)
    {}

    double dist;
    unsigned tree_id;
    BinaryTreeNodePtr node;

    inline bool operator<(const NodeQueueItem& item) const {
        return dist < item.dist;
    }
};

class NodeQueue{
public:
    inline void push(const NodeQueueItem& item){
        items.push_back(item);
    }

    inline NodeQueueItem get(unsigned index){
        return items[index];
    }

    inline void sort(){
        std::sort(items.begin(), items.end());
    }

    inline unsigned size() const {
        return items.size();
    }

private:
    std::vector<NodeQueueItem> items;
};

typedef std::shared_ptr<NodeQueue> NodeQueuePtr;

class CompareNodeQueueItem{
public:
    inline bool operator()(const NodeQueueItem& a, const NodeQueueItem& b){
        return a.dist > b.dist;
    }
};

typedef std::priority_queue<NodeQueueItem,
                            std::vector<NodeQueueItem>,
                            CompareNodeQueueItem> NodePriorityQueue;

typedef std::shared_ptr<NodePriorityQueue> NodePriorityQueuePtr;

struct DescriptorQueueItem{
public:

    inline explicit DescriptorQueueItem(const double d, BinaryDescriptorPtr bd) :
        dist(d),
        desc(bd)
    {}

    double dist;
    BinaryDescriptorPtr desc;

    inline bool operator<(const DescriptorQueueItem& item) const {
        return dist < item.dist;
    }
};

class DescriptorQueue{
public:

    inline void push(const DescriptorQueueItem& item){
        items.push_back(item);
    }

    inline DescriptorQueueItem get(unsigned index){
        return items[index];
    }

    inline void sort(){
        std::sort(items.begin(), items.end());
    }

    inline unsigned size() const {
        return items.size();
    }

private:
    std::vector<DescriptorQueueItem> items;
};

typedef std::shared_ptr<DescriptorQueue> DescriptorQueuePtr;

}  // namespace obindex2
