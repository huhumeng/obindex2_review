#include "binary_tree.h"

namespace obindex2 {

BinaryTree::BinaryTree(BinaryDescriptorSetPtr dset,
                       const unsigned tree_id,
                       const unsigned k,
                       const unsigned s) :
    dset_(dset),
    tree_id_(tree_id),
    root_(nullptr),
    k_(k),
    s_(s),
    k_2_(k_ / 2)
{
    srand(time(NULL));
    buildTree();
}

BinaryTree::~BinaryTree(){
    deleteTree();
}

void BinaryTree::buildTree(){
    
    // Deleting the previous tree, if exists any
    deleteTree();

    degraded_nodes_ = 0;
    nvisited_nodes_ = 0;

    // Creating the root node
    root_ = std::make_shared<BinaryTreeNode>();
    nset_.insert(root_);

    // Generating a new copy set with the descriptor's ids 
    BinaryDescriptorSet descs = *dset_;

    buildNode(descs, root_);
}

void BinaryTree::buildNode(BinaryDescriptorSet dset, BinaryTreeNodePtr root){
    
    // Validate if this should be a leaf node
    // 如果描述子数量小于s, 全部分配当前的叶节点中
    if(dset.size() < s_){
        
        // We set the previous node as a leaf
        root->setLeaf(true);

        // Adding descriptors as leaf nodes
        for(auto it = dset.begin(); it != dset.end(); it++){
            
            BinaryDescriptorPtr d = *it;
            root->addChildDescriptor(d);

            // Storing the reference of the node where the descriptor hangs
            desc_to_node_[d] = root;
        }
    }

    // 否则当前节点应该再被划分为K个子节点
    else{
        
        // This node should be split
        // Randomly selecting the new centers
        std::vector<BinaryDescriptorPtr> new_centers;
        std::vector<BinaryDescriptorSet> assoc_descs(k_);

        // 随机分成k个子节点
        for (unsigned i = 0; i < k_; i++){
            
            // Selecting a new center
            // 随机选取一个描述子
            BinaryDescriptorPtr desc = *std::next(dset.begin(), rand() % dset.size());
            
            // 将描述子插入到中心中
            new_centers.push_back(desc);

            // 将此描述子放入到子节点组中
            assoc_descs[i].insert(desc);
            
            // 从总的节点数中删除掉此节点
            dset.erase(desc);
        }
        
        // 将每一个描述子放入到不同的节点中
        // Associating the remaining descriptors to the new centers
        for (auto it = dset.begin(); it != dset.end(); it++){
            
            BinaryDescriptorPtr d = *it;
            int best_center = -1;
            double min_dist = DBL_MAX;
            for (unsigned i = 0; i < k_; i++){
                double dist = obindex2::BinaryDescriptor::distHamming(*d, *(new_centers[i]));

                if(dist < min_dist){
                    min_dist = dist;
                    best_center = i;
                }
            }

            assert(best_center != -1);
            assoc_descs[best_center].insert(d);
        }

        // 去除掉所有的描述子
        dset.clear();

        // Creating a new tree node for each new cluster
        for (unsigned i = 0; i < k_; i++){
            
            // 生成一个新的节点
            BinaryTreeNodePtr node = std::make_shared<BinaryTreeNode>(false, new_centers[i], root);

            // Linking this node with its root
            // 将其附于父节点上
            root->addChildNode(node);

            // Storing the reference to this node
            nset_.insert(node);

            // Recursively apply the algorithm
            // 迭代进行此操作
            buildNode(assoc_descs[i], node);
        }
    }
}

void BinaryTree::deleteTree(){
    
    if(nset_.size() > 0){

        nset_.clear();
        desc_to_node_.clear();

        // Invalidating last reference to root
        root_ = nullptr;
    }
}

unsigned BinaryTree::traverseFromRoot(BinaryDescriptorPtr q,
                                      NodeQueuePtr pq,
                                      DescriptorQueuePtr r){

    nvisited_nodes_ = 0;

    // 生成搜索的队列
    traverseFromNode(q, root_, pq, r);

    return nvisited_nodes_;
}

void BinaryTree::traverseFromNode(BinaryDescriptorPtr q,
                                  BinaryTreeNodePtr n,
                                  NodeQueuePtr pq,
                                  DescriptorQueuePtr r){
    nvisited_nodes_++;
    
    // If its a leaf node, the search ends
    if(n->isLeaf()){
      
        // Adding points to R
        BinaryDescriptorSet* descs = n->getChildrenDescriptors();
        
        for(auto it = descs->begin(); it != descs->end(); it++){
            
            BinaryDescriptorPtr d = *it;
            double dist = obindex2::BinaryDescriptor::distHamming(*q, *d);
            
            DescriptorQueueItem item(dist, d);
            r->push(item);
        }
    }
    else{

        // Search continues
        NodeSet* nodes = n->getChildrenNodes();

        int best_node = -1;
        double min_dist = DBL_MAX;

        // Computing distances to nodes
        std::vector<NodeQueueItem> items;
        unsigned node_id = 0;
        
        // Computing distances to nodes
        
        for(auto it = nodes->begin(); it != nodes->end(); it++){
            
            BinaryTreeNodePtr bn = *it;
            
            double dist = bn->distance(q);
            
            NodeQueueItem item(dist, tree_id_, bn);
            
            items.push_back(item);

            if(dist < min_dist){
                min_dist = dist;
                best_node = node_id;
            }

            node_id++;
        }

        assert(best_node != -1);

        // Adding remaining nodes to pq
        for(unsigned i = 0; i < items.size(); i++){
            
            // Is it the best node?
            if(i == static_cast<unsigned>(best_node)){
                continue;
            }

            pq->push(items[i]);
        }

        // Traversing the best node
        traverseFromNode(q, items[best_node].node, pq, r);
    }
}

BinaryTreeNodePtr BinaryTree::searchFromRoot(BinaryDescriptorPtr q){
    return searchFromNode(q, root_);
}

BinaryTreeNodePtr BinaryTree::searchFromNode(BinaryDescriptorPtr q,
                                             BinaryTreeNodePtr n){
    
    // If it's a leaf node, the search ends
    if(n->isLeaf()){
        // This is the node where this descriptor should be included
        return n;
    }
    else{
      
        // Search continues
        std::unordered_set<BinaryTreeNodePtr>* nodes = n->getChildrenNodes();
        int best_node = -1;
        double min_dist = DBL_MAX;

        // Computing distances to nodes
        std::vector<BinaryTreeNodePtr> items;
        // Computing distances to nodes
      
        for(auto it = (*nodes).begin(); it != (*nodes).end(); it++){
            
            BinaryTreeNodePtr bn = *it;
            items.push_back(bn);
            double dist = bn->distance(q);

            if(dist < min_dist){
                min_dist = dist;
                best_node = static_cast<int>(items.size()) - 1;
            }
        }

        assert(best_node != -1);

        // Searching in the best node
        return searchFromNode(q, items[best_node]);
    }
}

void BinaryTree::addDescriptor(BinaryDescriptorPtr q){
        
    BinaryTreeNodePtr n = searchFromRoot(q);
    assert(n->isLeaf());
    if(n->childDescriptorSize() + 1 < s_){

        // There is enough space at this node for this descriptor, so we add it
        n->addChildDescriptor(q);
        // Storing the reference of the node where the descriptor hangs
        desc_to_node_[q] = n;
    }
    else{
      
        // This node should be split
        n->setLeaf(false);

        // Gathering the current descriptors
        std::unordered_set<BinaryDescriptorPtr>* descs =
                                                        n->getChildrenDescriptors();
        BinaryDescriptorSet set;
      
        for (auto it = (*descs).begin(); it != (*descs).end(); it++){
            BinaryDescriptorPtr d = *it;
            set.insert(d);
        }
        set.insert(q);  // Adding the new descritor to the set

        // Rebuilding this node
        buildNode(set, n);
    }
}

void BinaryTree::deleteDescriptor(BinaryDescriptorPtr q){
    
    // We get the node where the descriptor is stored
    BinaryTreeNodePtr node = desc_to_node_[q];
    assert(node->isLeaf());

    // We remove q from the node
    node->deleteChildDescriptor(q);

    if(node->childDescriptorSize() > 0){
        // We select a new center, if required
        if(node->getDescriptor() == q){
            // Selecting a new center
            node->selectNewCenter();
        }
    }
    else{

        // Otherwise, we need to remove the node
        BinaryTreeNodePtr parent = node->getRoot();
        parent->deleteChildNode(node);
        nset_.erase(node);

        deleteNodeRecursive(parent);
    }

    desc_to_node_.erase(q);
}

void BinaryTree::deleteNodeRecursive(BinaryTreeNodePtr n){
    
    assert(!n->isLeaf());
    
    // Validating if this node is degraded
    if(n->childNodeSize() < k_2_ && !n->isBad()){
        degraded_nodes_++;
        n->setBad(true);
    }

    if(n->childNodeSize() == 0 && n != root_){
        // We remove this node
        BinaryTreeNodePtr parent = n->getRoot();
        parent->deleteChildNode(n);
        nset_.erase(n);

        deleteNodeRecursive(parent);
    }
}

void BinaryTree::printTree(){
    printNode(root_);
}

void BinaryTree::printNode(BinaryTreeNodePtr n){
    
    std::cout << "---" << std::endl;
    std::cout << "Node: " << n << std::endl;
    std::cout << (n->isLeaf() ? "Leaf" : "Node") << std::endl;
    std::cout << "Descriptor: " << n->getDescriptor() << std::endl;
    
    if(n->isLeaf()){
        std::cout << "Children descriptors: " << n->childDescriptorSize() << std::endl;
    }
    else{
        std::cout << "Children nodes: " << n->childNodeSize() << std::endl;
        std::unordered_set<BinaryTreeNodePtr>* nodes = n->getChildrenNodes();
        for (auto it = (*nodes).begin(); it != (*nodes).end(); it++){
            printNode(*it);
        }
    }
}

}  // namespace obindex2
