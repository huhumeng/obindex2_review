#pragma once

#include <list>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "binary_tree.h"

namespace obindex2{

enum MergePolicy{
    MERGE_POLICY_NONE,
    MERGE_POLICY_AND,
    MERGE_POLICY_OR
};

struct InvIndexItem{
    InvIndexItem() :
        image_id(0),
        pt(0.0f, 0.0f),
        dist(DBL_MAX),
        kp_ind(-1){}

    InvIndexItem(const int id,
                 const cv::Point2f& kp,
                 const double d,
                 const int kp_i = -1) :
    image_id(id),
    pt(kp),
    dist(d),
    kp_ind(kp_i)
    {}

    unsigned image_id;
    cv::Point2f pt;
    double dist;
    int kp_ind;
};

struct ImageMatch{
    ImageMatch() :
    image_id(-1),
    score(0.0)
    {}

    explicit ImageMatch(const int id, const double sc = 0.0) :
        image_id(id),
        score(sc) 
    {}

    int image_id;
    double score;

    bool operator<(const ImageMatch &lcr) const { return score > lcr.score; }
};

struct PointMatches{
    std::vector<cv::Point2f> query;
    std::vector<cv::Point2f> train;
};

class ImageIndex{
public:

    // Constructors

    // @para K: 将树分为多少叉
    // @para S: 叶节点的最大数目
    // @para t: Tree amount
    // @para merge_policy: 更新描述子的方式
    // @para purge_descriptors: 删除不稳定的描述子
    // @para min_feat_apps
    explicit ImageIndex(const unsigned k = 16,
                        const unsigned s = 150,
                        const unsigned t = 4,
                        const MergePolicy merge_policy = MERGE_POLICY_NONE,
                        const bool purge_descriptors = true,
                        const unsigned min_feat_apps = 3);

    // Methods
    void addImage(const unsigned image_id,
                  const std::vector<cv::KeyPoint>& kps,
                  const cv::Mat& descs);

    void addImage(const unsigned image_id,
                  const std::vector<cv::KeyPoint>& kps,
                  const cv::Mat& descs,
                  const std::vector<cv::DMatch>& matches);

    void searchImages(const cv::Mat& descs,
                      const std::vector<cv::DMatch>& gmatches,
                      std::vector<ImageMatch>* img_matches,
                      bool sort = true);

    void searchDescriptors(const cv::Mat& descs,
                           std::vector<std::vector<cv::DMatch> >* matches,
                           const unsigned knn = 2,
                           const unsigned checks = 32);

    void deleteDescriptor(const unsigned desc_id);

    void getMatchings(const std::vector<cv::KeyPoint>& query_kps,
                      const std::vector<cv::DMatch>& matches,
                      std::unordered_map<unsigned, PointMatches>* point_matches);

    inline unsigned numImages(){
        return nimages_;
    }

    inline unsigned numDescriptors(){
        return dset_.size();
    }

    inline void rebuild(){
        if(init_){
            trees_.clear();
            initTrees();
        }
    }

private:

    // 二进制描述子集合
    BinaryDescriptorSet dset_;

    // param:
    unsigned k_;                // K
    unsigned s_;                // S
    unsigned t_;                // Tree amount
    unsigned init_;             // 是否初始化
    unsigned nimages_;          // 图像数目
    unsigned ndesc_;            // 描述子数目
    MergePolicy merge_policy_;  // 融合策略
    bool purge_descriptors_;    // 删除不稳定描述子
    unsigned min_feat_apps_;    // 

    // t颗树
    std::vector<BinaryTreePtr> trees_;
    
    // 描述子与InvIndexItem的索引
    std::unordered_map<BinaryDescriptorPtr, std::vector<InvIndexItem>> inv_index_;

    // 描述子的反索引
    std::unordered_map<BinaryDescriptorPtr, unsigned> desc_to_id_;
    
    // 描述子的索引
    std::unordered_map<unsigned, BinaryDescriptorPtr> id_to_desc_;
    
    // 最近添加的描述子
    std::list<BinaryDescriptorPtr> recently_added_;

    void initTrees();
    
    // 返回最近的knn个描述子, 和它们的距离
    void searchDescriptor(BinaryDescriptorPtr q,
                          std::vector<BinaryDescriptorPtr>* neigh,
                          std::vector<double>* distances,
                          unsigned knn = 2,
                          unsigned checks = 32);

    void insertDescriptor(BinaryDescriptorPtr q);

    void deleteDescriptor(BinaryDescriptorPtr q);

    void purgeDescriptors(const unsigned curr_img);

};

}  // namespace obindex2
