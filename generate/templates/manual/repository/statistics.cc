/**
 * \struct CommitsGraphNode
 */
struct CommitsGraphNode
{
  CommitsGraphNode(uint32_t aParentsLeft) : parentsLeft(aParentsLeft) {}
  CommitsGraphNode() = default;
  ~CommitsGraphNode() = default;
  CommitsGraphNode(const CommitsGraphNode &other) = delete;
  CommitsGraphNode(CommitsGraphNode &&other) = delete;
  CommitsGraphNode& operator=(const CommitsGraphNode &other) = delete;
  CommitsGraphNode& operator=(CommitsGraphNode &&other) = delete;

  std::vector<CommitsGraphNode *> children {};
  uint32_t parentsLeft {0}; // used when calculating the maximum history depth
};

/**
 * \class CommitsGraph
 */
class CommitsGraph
{
public:
  CommitsGraph() = default;
  ~CommitsGraph() = default;
  CommitsGraph(const CommitsGraph &other) = delete;
  CommitsGraph(CommitsGraph &&other) = delete;
  CommitsGraph& operator=(const CommitsGraph &other) = delete;
  CommitsGraph& operator=(CommitsGraph &&other) = delete;

  using CommitsGraphMap = std::unordered_map<std::string, std::unique_ptr<CommitsGraphNode>>;

  void AddNode(const std::string &oidStr, const std::vector<std::string> &parents);
  uint32_t CalculateMaxDepth();

private:
  void addParentNode(const std::string &oidParentStr, CommitsGraphNode *child);

  CommitsGraphMap m_mapOidNode {};
  std::vector<CommitsGraphNode *> m_roots {};
};

/**
 * CommitsGraph::AddNode
 * 
 * \param oidStr oid of the commit object to add.
 * \param parents oids of the commit's parents.
 */
void CommitsGraph::AddNode(const std::string &oidStr, const std::vector<std::string> &parents)
{
  const uint32_t numParents = static_cast<uint32_t>(parents.size());

  auto emplacePair = m_mapOidNode.emplace(std::make_pair(
    oidStr, std::make_unique<CommitsGraphNode>(numParents)));

  CommitsGraphMap::iterator itNode = emplacePair.first;

  // if this node already added by a child, update its parentsLeft
  if (emplacePair.second == false) {
    itNode->second.get()->parentsLeft = numParents;
  }

  // set roots
  if (numParents == 0) {
    m_roots.emplace_back(itNode->second.get());
  }

  // add parents
  for (unsigned int i = 0; i < numParents; ++i) {
    addParentNode(parents.at(i), itNode->second.get());
  }
}

/**
 * CommitsGraph::CalculateMaxDepth
 * \return Calculated maximum depth of the tree.
 * 
 * Uses iterative algorithm to count levels.
 * Considers multiple initial commits.
 * Considers that children of one level can have multiple parents, hence we insert unique children
 * at each level.
 * Considers that same child can be in different levels. Here to prevent counting the same child
 * multiple times, we only add a child when the last parent (parentsLeft) inserts it. This is
 * actually what makes the algorithm fast.
 * Recursive algorithm avoided to prevent stack overflow in case of excessive levels in the tree.
 * 
 * Explanation of the algorithm:
 * once the graph is built with the commit history, `CalculateMaxDepth()` counts the maximum number
 * of levels from any of the roots to any of the leaves, which gives us the maximum depth
 * (`historyStructure.maxDepth` in the final result).
 * Inside `CalculateMaxDepth()`, to count levels, we add in an iterative way for each level and
 * starting at the roots level, all the children from that level, but only if each child is the last
 * time we'll consider it in the algorithm (for example if a child node 'C' has 2 parents 'P1' and
 * 'P2', and 'P1' has already been considered before in the algorithm as parent of 'C', and now we are
 * processing 'C' as a child from 'P2', which will be the last time, as 'C' has no more parents left).
 * This way we prevent counting 'C' multiple times.
 */
uint32_t CommitsGraph::CalculateMaxDepth()
{
  uint32_t maxDepth {0};
  std::unordered_set<CommitsGraphNode *> parents {};
  std::unordered_set<CommitsGraphNode *> children {};

  // start from the root commmits
  for (CommitsGraphNode *root : m_roots) {
    children.insert(root);
  }

  while (!children.empty()) {
    ++maxDepth;
    parents = std::move(children);

    // add unique children of next level, and only if from the last parent
    for (CommitsGraphNode *parent : parents) {
      for (CommitsGraphNode *child : parent->children) {
        if (--child->parentsLeft == 0) {
          children.insert(child);
        }
      }
    }
  }

  return maxDepth;
}

/**
 * CommitsGraph::addParentNode
 * 
 * \param oidParentStr oid of the parent commit to add.
 * \param child Child of the parent commit being added.
 */
void CommitsGraph::addParentNode(const std::string &oidParentStr, CommitsGraphNode *child)
{
  CommitsGraphMap::iterator itParentNode = m_mapOidNode.emplace(std::make_pair(
    oidParentStr, std::make_unique<CommitsGraphNode>())).first;

  // add child to parent
  itParentNode->second->children.emplace_back(child);
}

/**
 * \struct TreeStatistics
 *  Structure to store statistics for a git tree object.
 */
struct TreeStatistics
{
  TreeStatistics() = default;
  ~TreeStatistics() = default;
  TreeStatistics(const TreeStatistics &other) = delete;
  TreeStatistics(TreeStatistics &&other) = default;
  TreeStatistics& operator=(const TreeStatistics &other) = delete;
  TreeStatistics& operator=(TreeStatistics &&other) = default;

  size_t numDirectories{0};
  size_t maxPathDepth {0};
  size_t maxPathLength {0};
  size_t numFiles {0};
  size_t totalFileSize {0};
  size_t numSymlinks {0};
  size_t numSubmodules {0};
};

/**
 * \struct Statistics
 * Stores statistics of the analyzed repository.
 */
struct Statistics
{
  Statistics() = default;
  ~Statistics() = default;
  Statistics(const Statistics &other) = delete;
  Statistics(Statistics &&other) = delete;
  Statistics& operator=(const Statistics &other) = delete;
  Statistics& operator=(Statistics &&other) = delete;

  struct {
    struct { size_t count {0}; size_t size {0}; } commits;
    struct { size_t count {0}; size_t size {0}; size_t entries {0}; } trees;
    struct { size_t count {0}; size_t size {0}; } blobs;
    struct { size_t count {0}; } annotatedTags;
    struct { size_t count {0}; } references;
  } repositorySize {};

  struct {
    struct { size_t maxSize {0}; size_t maxParents {0}; } commits;
    struct { size_t maxEntries {0}; } trees;
    struct { size_t maxSize {0}; } blobs;
  } biggestObjects {};

  struct {
    uint32_t maxDepth {0};
    uint32_t maxTagDepth {0};
  } historyStructure {};

  TreeStatistics biggestCheckouts {};
};

/**
 * \struct OdbObjectsData
 * Structure to store, for each object read from the repository:
 * - its information (size, parents for a commit, etc.)
 * - different data needed to obtain the resulting statistics
 */
struct OdbObjectsData
{
  static constexpr uint32_t kUnreachable = 0;

  struct CommitInfo {
    CommitInfo() = default;
    ~CommitInfo() = default;
    CommitInfo(const CommitInfo &other) = delete;
    CommitInfo(CommitInfo &&other) = default;
    CommitInfo& operator=(const CommitInfo &other) = delete;
    CommitInfo& operator=(CommitInfo &&other) = default;

    std::string oidTree {};
    size_t size {0};
    std::vector<std::string> parents {};
    // number of sources from which a commit can be reached:
    // a child commit, a tag, or a direct git reference
    uint32_t reachability {kUnreachable};
  };

  struct TreeInfoAndStats {
    TreeInfoAndStats() = default;
    ~TreeInfoAndStats() = default;
    TreeInfoAndStats(const TreeInfoAndStats &other) = delete;
    TreeInfoAndStats(TreeInfoAndStats &&other) = default;
    TreeInfoAndStats& operator=(const TreeInfoAndStats &other) = delete;
    TreeInfoAndStats& operator=(TreeInfoAndStats &&other) = default;

    size_t size {0};
    size_t numEntries {0};
    std::vector<std::string> entryBlobs {};
    std::vector< std::pair<std::string, size_t> > entryTreesNameLen {};
    // number of sources from which a tree can be reached:
    // a commit, another tree's entry, or a tag
    uint32_t reachability {kUnreachable};
    TreeStatistics stats {};
    bool statsDone {false};
  };

  struct BlobInfo {
    BlobInfo() = default;
    ~BlobInfo() = default;
    BlobInfo(const BlobInfo &other) = delete;
    BlobInfo(BlobInfo &&other) = default;
    BlobInfo& operator=(const BlobInfo &other) = delete;
    BlobInfo& operator=(BlobInfo &&other) = default;

    size_t size {0};
    // number of sources from which a blob can be reached:
    // a tree's entry, or a tag
    uint32_t reachability {kUnreachable};
  };

  struct TagInfo {
    static constexpr uint32_t kUnsetDepth = 0;

    TagInfo() = default;
    ~TagInfo() = default;
    TagInfo(const TagInfo &other) = delete;
    TagInfo(TagInfo &&other) = default;
    TagInfo& operator=(const TagInfo &other) = delete;
    TagInfo& operator=(TagInfo &&other) = default;

    std::string oidTarget {};
    git_object_t typeTarget {GIT_OBJECT_INVALID};
    uint32_t depth {kUnsetDepth};
    // number of sources from which a tag can be reached:
    // a reference, or another tag
    uint32_t reachability {kUnreachable};
  };

  OdbObjectsData() = default;
  ~OdbObjectsData() = default;
  OdbObjectsData(const OdbObjectsData &other) = delete;
  OdbObjectsData(OdbObjectsData &&other) = delete;
  OdbObjectsData& operator=(const OdbObjectsData &other) = delete;
  OdbObjectsData& operator=(OdbObjectsData &&other) = delete;

  struct {
    std::unordered_map<std::string, CommitInfo> info {};
    std::unordered_set<std::string> unreachables {};
    // Tree of commits (graph) to be built after having read the object
    // database, and pruned unreachable objects.
    // Used to calculate the maximum history depth.
    CommitsGraph graph {};
    size_t totalSize {0};
    size_t maxSize {0};
    size_t maxParents {0};
  } commits {};

  struct {
    std::unordered_map<std::string, TreeInfoAndStats> info;
    std::unordered_set<std::string> unreachables {};
    size_t totalSize {0};
    size_t totalEntries {0};
    size_t maxEntries {0};
  } trees {};

  struct {
    std::unordered_map<std::string, BlobInfo> info {};
    std::unordered_set<std::string> unreachables {};
    size_t totalSize {0};
    size_t maxSize {0};
  } blobs {};

  struct {
    std::unordered_map<std::string, TagInfo> info;
    std::unordered_set<std::string> unreachables {};
  } tags {};

  struct {
    std::mutex commits {};
    std::mutex trees {};
    std::mutex blobs {};
    std::mutex tags {};
  } infoMutex;

  using iterCommitInfo = std::unordered_map<std::string, CommitInfo>::iterator;
  using iterUnreachable = std::unordered_set<std::string>::iterator;
  using iterTreeInfo = std::unordered_map<std::string, TreeInfoAndStats>::iterator;
  using iterBlobInfo = std::unordered_map<std::string, BlobInfo>::iterator;
  using iterTagInfo = std::unordered_map<std::string, TagInfo>::iterator;
};

/**
 * \class WorkItemOid
 * WorkItem storing odb oids for the WorkPool.
 */
class WorkItemOid : public WorkItem {
public:
  WorkItemOid(const git_oid &oid)
    : m_oid(oid) {}
  ~WorkItemOid() = default;
  WorkItemOid(const WorkItemOid &other) = delete;
  WorkItemOid(WorkItemOid &&other) = delete;
  WorkItemOid& operator=(const WorkItemOid &other) = delete;
  WorkItemOid& operator=(WorkItemOid &&other) = delete;
  
  const git_oid& GetOid() const { return m_oid; }

private:
  git_oid m_oid {};
};

/**
 * \class WorkerStoreOdbData
 * Worker for the WorkPool to store odb object data.
 */
class WorkerStoreOdbData : public IWorker
{
public:
  WorkerStoreOdbData(const std::string &repoPath, OdbObjectsData *odbObjectsData)
    : m_repoPath(repoPath), m_odbObjectsData(odbObjectsData) {}
  ~WorkerStoreOdbData();
  WorkerStoreOdbData(const WorkerStoreOdbData &other) = delete;
  WorkerStoreOdbData(WorkerStoreOdbData &&other) = delete;
  WorkerStoreOdbData& operator=(const WorkerStoreOdbData &other) = delete;
  WorkerStoreOdbData& operator=(WorkerStoreOdbData &&other) = delete;

  bool Initialize();
  bool Execute(std::unique_ptr<WorkItem> &&work);

private:
  OdbObjectsData::TreeInfoAndStats thisTreeInfoAndStats(const git_tree *tree, size_t size, size_t numEntries);

  std::string m_repoPath {};
  git_repository *m_repo {nullptr};
  git_odb *m_odb {nullptr};
  OdbObjectsData *m_odbObjectsData {nullptr};
};

/**
 * WorkerStoreOdbData::~WorkerStoreOdbData
 */
WorkerStoreOdbData::~WorkerStoreOdbData() {
  if (m_odb) {
    git_odb_free(m_odb);
  }
  if (m_repo) {
    git_repository_free(m_repo);
  }
}

/**
 * WorkerStoreOdbData::Initialize
 */
bool WorkerStoreOdbData::Initialize() {
  if (m_repo != nullptr) { // if already initialized
    return true;
  }

  return !m_repoPath.empty() &&
    git_repository_open(&m_repo, m_repoPath.c_str()) == GIT_OK &&
    git_repository_odb(&m_odb, m_repo) == GIT_OK;
}

/**
 * WorkerStoreOdbData::Execute
 */
bool WorkerStoreOdbData::Execute(std::unique_ptr<WorkItem> &&work)
{
  std::unique_ptr<WorkItemOid> wi {static_cast<WorkItemOid*>(work.release())};
  const git_oid &oid = wi->GetOid();

  // NOTE about PERFORMANCE (May 2021):
  // git_object_lookup() is as expensive as git_odb_read().
  // They give access to different information from the libgit2 API.
  // Try to call only one of them if possible.

  git_object *target {nullptr};
  if (git_object_lookup(&target, m_repo, &oid, GIT_OBJECT_ANY) != GIT_OK) {
    return false;
  }

  switch (git_object_type(target))
  {
    case GIT_OBJECT_COMMIT:
    {
      git_commit *commit = (git_commit*)target;
      // NOTE about PERFORMANCE (May 2021):
      // calling git_odb_object_size() was slightly faster than calculating header size + message size + 1 with GK repo

      // obtain size
      git_odb_object *obj {nullptr};
      if (git_odb_read(&obj, m_odb, &oid) != GIT_OK) {
        git_object_free(target);
        return false;
      }
      const size_t size = git_odb_object_size(obj);
      git_odb_object_free(obj);

      // obtain CommitInfo
      const unsigned int numParents = git_commit_parentcount(commit);
      std::vector<std::string> parents {};
      for (unsigned int i = 0; i < numParents; ++i) {
        parents.emplace_back(reinterpret_cast<const char *>(git_commit_parent_id(commit, i)->id),
          GIT_OID_RAWSZ);
      }

      OdbObjectsData::CommitInfo commitInfo {
        std::string(reinterpret_cast<const char *>(git_commit_tree_id(commit)->id), GIT_OID_RAWSZ),
        size,
        std::move(parents),
        OdbObjectsData::kUnreachable};

      { // lock
        std::lock_guard<std::mutex> lock(m_odbObjectsData->infoMutex.commits);

        m_odbObjectsData->commits.info.emplace(std::make_pair(
          std::string(reinterpret_cast<const char *>(oid.id), GIT_OID_RAWSZ),
          std::move(commitInfo)));
      }
    }
      break;

    case GIT_OBJECT_TREE:
    {
      git_tree *tree = (git_tree*)target;

      // do not count empty trees, like git's empty tree "4b825dc642cb6eb9a060e54bf8d69288fbee4904"
      const size_t numEntries = git_tree_entrycount(tree);
      if (numEntries == 0) {
        git_object_free(target);
        return true;
      }

      // obtain size
      git_odb_object *obj {nullptr};
      if (git_odb_read(&obj, m_odb, &oid) != GIT_OK) {
        git_object_free(target);
        return false;
      }
      const size_t size = git_odb_object_size(obj);
      git_odb_object_free(obj);

      // obtain tree data and calculate statistics for only this tree (not recursively)
      OdbObjectsData::TreeInfoAndStats treeInfoAndStats = thisTreeInfoAndStats(tree, size, numEntries);
     
      { // lock
        std::lock_guard<std::mutex> lock(m_odbObjectsData->infoMutex.trees);

        m_odbObjectsData->trees.info.emplace(std::make_pair(
          std::string(reinterpret_cast<const char *>(oid.id), GIT_OID_RAWSZ),
          std::move(treeInfoAndStats)));
      }
    }
      break;

    case GIT_OBJECT_BLOB:
    {
      git_blob *blob = (git_blob*)target;
      const size_t size = git_blob_rawsize(blob);
      OdbObjectsData::BlobInfo blobInfo {size, OdbObjectsData::kUnreachable};

      { // lock
        std::lock_guard<std::mutex> lock(m_odbObjectsData->infoMutex.blobs);

        m_odbObjectsData->blobs.info.emplace(std::make_pair(
          std::string(reinterpret_cast<const char *>(oid.id), GIT_OID_RAWSZ),
          std::move(blobInfo)));
      }
    }
      break;

    case GIT_OBJECT_TAG:
    {
      // obtain TagInfo
      git_tag *tag = (git_tag*)target;
      const git_oid *oid_target = git_tag_target_id(tag);
      OdbObjectsData::TagInfo tagInfo {
        std::string(reinterpret_cast<const char *>(oid_target->id), GIT_OID_RAWSZ),
        git_tag_target_type(tag),
        OdbObjectsData::TagInfo::kUnsetDepth,
        OdbObjectsData::kUnreachable};

      { // lock
        std::lock_guard<std::mutex> lock(m_odbObjectsData->infoMutex.tags);
        m_odbObjectsData->tags.info.emplace(std::make_pair(
            std::string(reinterpret_cast<const char *>(oid.id), GIT_OID_RAWSZ),
            std::move(tagInfo)));
      }
    }
      break;

    default:
      break;
  }

  git_object_free(target);

  return true;
}

/**
 * WorkerStoreOdbData::thisTreeInfoAndStats
 * 
 * Obtain tree data and calculate the part of this tree's statistics that each thread can do.
 * 
 * \param tree tree to get data from and calculate partial statistics of.
 * \param size tree size, to be added to the final result.
 * \param numEntries number of entries of this tree.
 * \return this tree's data and partial statistics.
  */
OdbObjectsData::TreeInfoAndStats WorkerStoreOdbData::thisTreeInfoAndStats(const git_tree *tree, size_t size,
  size_t numEntries)
{
  OdbObjectsData::TreeInfoAndStats treeInfoAndStats {};
  treeInfoAndStats.size = size;
  treeInfoAndStats.numEntries = numEntries;

  for (size_t i = 0; i < numEntries; ++i)
  {
    const git_tree_entry *te = git_tree_entry_byindex(tree, i);
    if (te == nullptr) {
      continue;
    }
    const git_object_t te_type = git_tree_entry_type(te);
    const char *teName {nullptr};
    size_t teNameLen {0};
    const git_oid *te_oid {nullptr};

    switch (te_type)
    {
      // count submodules
      case GIT_OBJECT_COMMIT:
        if (git_tree_entry_filemode(te) == GIT_FILEMODE_COMMIT) {
          ++treeInfoAndStats.stats.numSubmodules;
        }
        break;

      case GIT_OBJECT_BLOB:
      {
        // count symbolic links, but don't add them as blob entries
        if (git_tree_entry_filemode(te) == GIT_FILEMODE_LINK) {
          ++treeInfoAndStats.stats.numSymlinks;
        }
        else {
          ++treeInfoAndStats.stats.numFiles;
          teName = git_tree_entry_name(te);
          teNameLen = std::char_traits<char>::length(teName);
          treeInfoAndStats.stats.maxPathLength =
            std::max<size_t>(treeInfoAndStats.stats.maxPathLength, teNameLen);
        }
        // store both types of files (symbolic links and non symbolic links) as entryBlob
        te_oid = git_tree_entry_id(te);
        treeInfoAndStats.entryBlobs.emplace_back(
          reinterpret_cast<const char *>(te_oid->id), GIT_OID_RAWSZ);
      }
        break;
        
      case GIT_OBJECT_TREE:
      {
        // We store tree's name length to compare in posterior stage, after threads work
        teName = git_tree_entry_name(te);
        teNameLen = std::char_traits<char>::length(teName);

        te_oid = git_tree_entry_id(te);
        treeInfoAndStats.entryTreesNameLen.emplace_back(std::make_pair(
          std::string(reinterpret_cast<const char *>(te_oid->id), GIT_OID_RAWSZ),
          teNameLen));
      }
        break;
        
      default:
        break;
    }
  }

  return treeInfoAndStats;
}

/**
 * \class WorkItemOidStrType
 * WorkItem storing pointers to object info structs for the WorkPool.
 */
class WorkItemOidStrType : public WorkItem {
public:
  WorkItemOidStrType(void *objectInfo, git_object_t type)
    : m_objectInfo(objectInfo), m_oid_type(type) {}
  ~WorkItemOidStrType() = default;
  WorkItemOidStrType(const WorkItemOidStrType &other) = delete;
  WorkItemOidStrType(WorkItemOidStrType &&other) = delete;
  WorkItemOidStrType& operator=(const WorkItemOidStrType &other) = delete;
  WorkItemOidStrType& operator=(WorkItemOidStrType &&other) = delete;
  
  void* GetObjectInfo() const { return m_objectInfo; }
  const git_object_t& GetOidType() const { return m_oid_type; }

private:
  void *m_objectInfo {nullptr};
  git_object_t m_oid_type {};
};

/**
 * \class WorkerReachCounter
 * Worker for the WorkPool to count reachability of each object.
 */
class WorkerReachCounter : public IWorker
{
public:
  WorkerReachCounter(OdbObjectsData *odbObjectsData)
    : m_odbObjectsData(odbObjectsData) {}
  ~WorkerReachCounter() = default;
  WorkerReachCounter(const WorkerReachCounter &other) = delete;
  WorkerReachCounter(WorkerReachCounter &&other) = delete;
  WorkerReachCounter& operator=(const WorkerReachCounter &other) = delete;
  WorkerReachCounter& operator=(WorkerReachCounter &&other) = delete;

  bool Initialize() { return true; }
  bool Execute(std::unique_ptr<WorkItem> &&work);

private:
  void setReachabilityFromTags(void *objectInfo);
  void setReachabilityFromCommits(void *objectInfo);
  void setReachabilityFromTrees(void *objectInfo);

  OdbObjectsData *m_odbObjectsData {nullptr};
};

/**
 * WorkerReachCounter::Execute
 */
bool WorkerReachCounter::Execute(std::unique_ptr<WorkItem> &&work)
{
  std::unique_ptr<WorkItemOidStrType> wi {static_cast<WorkItemOidStrType*>(work.release())};
  void *objectInfo = wi->GetObjectInfo();
  const git_object_t &oid_type = wi->GetOidType();

  switch (oid_type) {
    case GIT_OBJECT_TAG:
      setReachabilityFromTags(objectInfo);
      break;
    case GIT_OBJECT_COMMIT:
      setReachabilityFromCommits(objectInfo);
      break;
    case GIT_OBJECT_TREE:
      setReachabilityFromTrees(objectInfo);
      break;
    case GIT_OBJECT_BLOB:
      // do not process blobs in this stage
      break;
    default:
      break;
  }

  return true;
}

/**
 * WorkerReachCounter::setReachabilityFromTags
 * Adds reachability counter where tags point (any type of object).
 */
void WorkerReachCounter::setReachabilityFromTags(void *objectInfo)
{
  const OdbObjectsData::TagInfo *tagInfo = static_cast<const OdbObjectsData::TagInfo *>(objectInfo);

  switch (tagInfo->typeTarget) {
    case GIT_OBJECT_COMMIT:
    {
      OdbObjectsData::iterCommitInfo itCommitInfo =
      m_odbObjectsData->commits.info.find(tagInfo->oidTarget);
    
      if (itCommitInfo != m_odbObjectsData->commits.info.end()) {
        { // lock
          std::lock_guard<std::mutex> lock(m_odbObjectsData->infoMutex.commits);
          ++itCommitInfo->second.reachability;
        }
      }
    }
      break;

    case GIT_OBJECT_TREE:
    {
      OdbObjectsData::iterTreeInfo itTreeInfo =
      m_odbObjectsData->trees.info.find(tagInfo->oidTarget);
    
      if (itTreeInfo != m_odbObjectsData->trees.info.end()) {
        { // lock
          std::lock_guard<std::mutex> lock(m_odbObjectsData->infoMutex.trees);
          ++itTreeInfo->second.reachability;
        }
      }
    }

    case GIT_OBJECT_BLOB:
    {
      OdbObjectsData::iterBlobInfo itBlobInfo =
      m_odbObjectsData->blobs.info.find(tagInfo->oidTarget);
    
      if (itBlobInfo != m_odbObjectsData->blobs.info.end()) {
        { // lock
          std::lock_guard<std::mutex> lock(m_odbObjectsData->infoMutex.blobs);
          ++itBlobInfo->second.reachability;
        }
      }
    }

    case GIT_OBJECT_TAG:
    {
      OdbObjectsData::iterTagInfo itTargetTagInfo =
      m_odbObjectsData->tags.info.find(tagInfo->oidTarget);
    
      if (itTargetTagInfo != m_odbObjectsData->tags.info.end()) {
        { // lock
          std::lock_guard<std::mutex> lock(m_odbObjectsData->infoMutex.tags);
          ++itTargetTagInfo->second.reachability;
        }
      }
    }
    default:
      break;
  }
}

/**
 * WorkerReachCounter::setReachabilityFromCommits
 * Adds reachability counter where commits point (parents and tree).
 */
void WorkerReachCounter::setReachabilityFromCommits(void *objectInfo)
{
  const OdbObjectsData::CommitInfo *commitInfo =
    static_cast<const OdbObjectsData::CommitInfo *>(objectInfo);
  const size_t numParents = commitInfo->parents.size();

  // set parents' reachability
  for (size_t i = 0; i < numParents; ++i) {
    OdbObjectsData::iterCommitInfo itParentCommitInfo =
      m_odbObjectsData->commits.info.find(commitInfo->parents.at(i));
    
    if (itParentCommitInfo != m_odbObjectsData->commits.info.end()) {
      { // lock
        std::lock_guard<std::mutex> lock(m_odbObjectsData->infoMutex.commits);
        ++itParentCommitInfo->second.reachability;
      }
    }
  }

  // add 1 to its tree's reachability
  OdbObjectsData::iterTreeInfo itCommitTreeInfo =
    m_odbObjectsData->trees.info.find(commitInfo->oidTree);
  
  if (itCommitTreeInfo != m_odbObjectsData->trees.info.end()) {
    { // lock
      std::lock_guard<std::mutex> lock(m_odbObjectsData->infoMutex.trees);
      ++itCommitTreeInfo->second.reachability;
    }
  }
}

/**
 * WorkerReachCounter::setReachabilityFromTrees
 * Adds reachability counter where tree entries point (blobs and other trees).
 */
void WorkerReachCounter::setReachabilityFromTrees(void *objectInfo)
{
  const OdbObjectsData::TreeInfoAndStats *treeInfo =
    static_cast<const OdbObjectsData::TreeInfoAndStats *>(objectInfo);

  // set entry blobs' reachability
  for (auto &blob : treeInfo->entryBlobs) {
    OdbObjectsData::iterBlobInfo itBlobInfo = m_odbObjectsData->blobs.info.find(blob);
    
    if (itBlobInfo != m_odbObjectsData->blobs.info.end()) {
      { // lock
        std::lock_guard<std::mutex> lock(m_odbObjectsData->infoMutex.blobs);
        ++itBlobInfo->second.reachability;
      }
    }
  }

  // set entry trees' reachability
  for (auto &treeNameLen : treeInfo->entryTreesNameLen) {
    OdbObjectsData::iterTreeInfo itTreeInfo = m_odbObjectsData->trees.info.find(treeNameLen.first);
    
    if (itTreeInfo != m_odbObjectsData->trees.info.end()) {
      { // lock
        std::lock_guard<std::mutex> lock(m_odbObjectsData->infoMutex.trees);
        ++itTreeInfo->second.reachability;
      }
    }
  }
}

/**
 * forEachOdbCb. Callback for git_odb_foreach.
 * Returns GIT_OK on success; GIT_EUSER otherwise
 */
static int forEachOdbCb(const git_oid *oid, void *payloadToCast)
{
  WorkerPool<WorkerStoreOdbData,WorkItemOid> *workerPool =
    static_cast<WorkerPool<WorkerStoreOdbData,WorkItemOid>*>(payloadToCast);

  // Must insert copies of oid, since the pointers might not survive until worker thread picks it up
  workerPool->InsertWork(std::make_unique<WorkItemOid>(*oid));

  // check there were no problems inserting work
  if (workerPool->Status() != WPStatus::kOk) {
    return GIT_EUSER;
  }

  return GIT_OK;
}

/**
 * \class RepoAnalysis
 * Class to analyse and hold repository statistics
 */
class RepoAnalysis
{
public:
  static constexpr unsigned int kMinThreads = 4;

  explicit RepoAnalysis(git_repository *repo)
    : m_repo(repo) {}
  ~RepoAnalysis() = default;
  RepoAnalysis(const RepoAnalysis &other) = delete;
  RepoAnalysis(RepoAnalysis &&other) = delete;
  RepoAnalysis& operator=(const RepoAnalysis &other) = delete;
  RepoAnalysis& operator=(RepoAnalysis &&other) = delete;

  int Analyze();
  v8::Local<v8::Object> StatisticsToJS() const;

private:
  // stage 1 methods: store data from repository (with threads)
  int storeObjectsInfo();
  int storeAndCountRefs();
  // stage 2 methods: count reachability of each object (with threads)
  // NOTE: we need this stage, since so far libgit2 doesn't provide unreachable objects
  bool setObjectsReachability();
  void setReachabilityFromRefs();
  void setUnreachables();
  // stage 3 methods: prune unreachable oids
  void pruneUnreachables();
  void pruneUnreachableTags();
  void pruneUnreachableCommits();
  void pruneUnreachableTrees();
  void pruneUnreachableBlobs();
  // stage 4 methods: repositorySize and biggestObjects
  void statsCountAndMax();
  // stage 5 methods: historyStructure and biggestCheckouts
  bool statsHistoryAndBiggestCheckouts();
  bool calculateBiggestCheckouts();
  OdbObjectsData::iterTreeInfo calculateTreeStatistics(const std::string &oidTree);
  bool calculateMaxTagDepth();
  OdbObjectsData::iterTagInfo calculateTagDepth(const std::string &oidTag);
  // methods to return the statistics calculated
  void fillOutStatistics();
  v8::Local<v8::Object> repositorySizeToJS() const;
  v8::Local<v8::Object> biggestObjectsToJS() const;
  v8::Local<v8::Object> historyStructureToJS() const;
  v8::Local<v8::Object> biggestCheckoutsToJS() const;

  git_repository *m_repo {nullptr};
  Statistics m_statistics {};
  // odb objects info to build while reading the object database by each thread
  OdbObjectsData m_odbObjectsData {};
  // oid and type of peeled references
  std::unordered_map<std::string, git_object_t> m_peeledRefs {};
};

/**
 * RepoAnalysis::Analyze
 * To obtain the final result, the whole process is run in different stages.
 * If a stage leverages threads via a worker pool, the worker pool is created
 * and we wait until all the threads are done to continue with the next stage.
 */
int RepoAnalysis::Analyze()
{
  int errorCode {GIT_OK};

  // stage 1
  if ((errorCode = storeObjectsInfo() != GIT_OK)) {
    return errorCode;
  }

  // stage 2
  if (!setObjectsReachability()) {
    return GIT_EUSER;
  }

  // stage 3
  pruneUnreachables();

  // stage 4
  statsCountAndMax();

  // stage 5
  if (!statsHistoryAndBiggestCheckouts()) {
    return GIT_EUSER;
  }

  fillOutStatistics();

  return errorCode;
}

/**
 * RepoAnalysis::StatisticsToJS
 */
v8::Local<v8::Object> RepoAnalysis::StatisticsToJS() const
{
  v8::Local<v8::Object> result = Nan::New<Object>();

  v8::Local<v8::Object> repositorySize = repositorySizeToJS();
  Nan::Set(result, Nan::New("repositorySize").ToLocalChecked(), repositorySize);

  v8::Local<v8::Object> biggestObjects = biggestObjectsToJS();
  Nan::Set(result, Nan::New("biggestObjects").ToLocalChecked(), biggestObjects);

  v8::Local<v8::Object> historyStructure = historyStructureToJS();
  Nan::Set(result, Nan::New("historyStructure").ToLocalChecked(), historyStructure);

  v8::Local<v8::Object> biggestCheckouts = biggestCheckoutsToJS();
  Nan::Set(result, Nan::New("biggestCheckouts").ToLocalChecked(), biggestCheckouts);

  return result;
}

/**
 * RepoAnalysis::storeObjectsInfo
 * Store information from read odb objects.
 * Starts building a container which eventually will hold only reachable objects.
 * Leverages threads via a worker pool <WorkerStoreOdbData,WorkItemOid>.
 */
int RepoAnalysis::storeObjectsInfo()
{
  int errorCode {GIT_OK};

  // get the objects database
  git_odb *odb {nullptr};
  if ((errorCode = git_repository_odb(&odb, m_repo)) != GIT_OK) {
    return errorCode;
  }

  // initialize workers for the worker pool
  const std::string repoPath = git_repository_path(m_repo);
  const unsigned int numThreads =
    std::max<unsigned int>(std::thread::hardware_concurrency(), static_cast<unsigned int>(kMinThreads));

  std::vector< std::shared_ptr<WorkerStoreOdbData> > workers {};
  for (unsigned int i = 0; i < numThreads; ++i) {
    workers.emplace_back(std::make_shared<WorkerStoreOdbData>(repoPath, &m_odbObjectsData));
  }

  // initialize worker pool
  WorkerPool<WorkerStoreOdbData,WorkItemOid> workerPool {};  
  workerPool.Init(workers);

  if ((errorCode = git_odb_foreach(odb, forEachOdbCb, &workerPool)) != GIT_OK) {
    workerPool.Shutdown();
    git_odb_free(odb);
    return errorCode;
  }

  // main thread will work on the refs while waiting for the threads to finish
  if ((errorCode = storeAndCountRefs() != GIT_OK)) {
    workerPool.Shutdown();
    git_odb_free(odb);
    return errorCode;
  }

  // wait for the threads to finish and shutdown the work pool
  workerPool.Shutdown();

  // check there were no problems during execution
  if (workerPool.Status() != WPStatus::kOk) {
    git_odb_free(odb);
    return GIT_EUSER;
  }

  git_odb_free(odb);

  return errorCode;
}

/**
 * RepoAnalysis::storeAndCountRefs
 * Stores the oid and type of peeled references.
 * Also counts total references.
 */
int RepoAnalysis::storeAndCountRefs()
{
  int errorCode {GIT_OK};
  git_strarray ref_list;

  // count refs
  if ((errorCode = git_reference_list(&ref_list, m_repo)) != GIT_OK) {
    return errorCode;
  }
  m_statistics.repositorySize.references.count = ref_list.count;

  // store refs info
  for (size_t i = 0; i < ref_list.count; ++i)
  {
    // lookup ref
    git_reference *ref {nullptr};
    const int refLookupError = git_reference_lookup(&ref, m_repo, ref_list.strings[i]);
    if (refLookupError == GIT_ENOTFOUND || refLookupError == GIT_EINVALIDSPEC) {
      continue;
    }
    else if (refLookupError != GIT_OK) {
      git_strarray_dispose(&ref_list);
      return refLookupError;
    }

    // obtain peeled oid of the reference
    const git_oid *oid_ref {nullptr};
    switch (git_reference_type(ref))
    {
      case GIT_REFERENCE_DIRECT:
        oid_ref = git_reference_target(ref);
        break;

      case GIT_REFERENCE_SYMBOLIC:
      {
        git_reference *ref_resolved {nullptr};
        if ((errorCode = git_reference_resolve(&ref_resolved, ref)) != GIT_OK) {
          git_reference_free(ref);
          git_strarray_dispose(&ref_list);
          return errorCode;
        }
        oid_ref = git_reference_target(ref_resolved);
        git_reference_free(ref_resolved);
      }
        break;

      default:
        break;
    }

    // store object's oid and type
    if (oid_ref != nullptr)
    {
      git_object *target {nullptr};
      if ((errorCode = git_object_lookup(&target, m_repo, oid_ref, GIT_OBJECT_ANY)) != GIT_OK) {
        git_reference_free(ref);
        git_strarray_dispose(&ref_list);
        return errorCode;
      }

      m_peeledRefs.emplace(std::make_pair(
        std::string(reinterpret_cast<const char *>(oid_ref->id), GIT_OID_RAWSZ),
        git_object_type(target)));

      git_object_free(target);
    }
    git_reference_free(ref);
  }
  git_strarray_dispose(&ref_list);

  return errorCode;
}

/**
 * RepoAnalysis::setObjectsReachability
 * Leverages threads via a worker pool <WorkerReachCounter,WorkItemOidStrType> to
 * set reachability from tags, commits, and trees.
 * NOTE: the worker pool leveraged in this method runs at a different stage than the
 * worker pool leveraged in previous stages, meaning they do not run at the same time, hence
 * access to 'm_odbObjectsData->....info' won't suffer from a data race.
 * NOTE: performance didn't improve leveraging threads for adding objects to unreachables container.
 * \return false if the workerPool finished with errors; true otherwise
 */
bool RepoAnalysis::setObjectsReachability()
{
  // references are not objects, hence they won't be sent to the worker threads
  setReachabilityFromRefs();

  const unsigned int numThreads =
    std::max<unsigned int>(std::thread::hardware_concurrency(), static_cast<unsigned int>(kMinThreads));
  std::vector< std::shared_ptr<WorkerReachCounter> > workers {};
  for (unsigned int i = 0; i < numThreads; ++i) {
    workers.emplace_back(std::make_shared<WorkerReachCounter>(&m_odbObjectsData));
  }

  // initialize worker pool
  WorkerPool<WorkerReachCounter,WorkItemOidStrType> workerPool {};  
  workerPool.Init(workers);

  // NOTE: avoid queueing same type of objects in a row, so that different mutex can be used concurrently
  uint8_t workInserted {0};
  OdbObjectsData::iterTagInfo itTagInfo = m_odbObjectsData.tags.info.begin();
  OdbObjectsData::iterCommitInfo itCommitInfo = m_odbObjectsData.commits.info.begin();
  OdbObjectsData::iterTreeInfo itTreeInfo = m_odbObjectsData.trees.info.begin();
  do {
    workInserted = 0;
    // insert tag
    if (itTagInfo != m_odbObjectsData.tags.info.end()) {
      workerPool.InsertWork(std::make_unique<WorkItemOidStrType>(&itTagInfo->second, GIT_OBJECT_TAG));
      ++itTagInfo;
      ++workInserted;
    }
    // insert commmit
    if (itCommitInfo != m_odbObjectsData.commits.info.end()) {
      workerPool.InsertWork(std::make_unique<WorkItemOidStrType>(&itCommitInfo->second, GIT_OBJECT_COMMIT));
      ++itCommitInfo;
      ++workInserted;
    }
    // insert tree
    if (itTreeInfo != m_odbObjectsData.trees.info.end()) {
      workerPool.InsertWork(std::make_unique<WorkItemOidStrType>(&itTreeInfo->second, GIT_OBJECT_TREE));
      ++itTreeInfo;
      ++workInserted;
    }
    // blobs do not reach to any other object, hence no need to process them
  } while (workInserted);

  // wait for the threads to finish and shutdown the work pool
  workerPool.Shutdown();

  // check there were no problems during execution
  if (workerPool.Status() != WPStatus::kOk) {
    return false;
  }

  setUnreachables();

  return true;
}

/**
 * RepoAnalysis::setReachabilityFromRefs
 * Adds reachability counter where peeled refs point (normally a commit or a tag).
  */
void RepoAnalysis::setReachabilityFromRefs()
{
  for (const auto &ref : m_peeledRefs) {
    switch (ref.second) {
      case GIT_OBJECT_COMMIT:
      {
        OdbObjectsData::iterCommitInfo itCommitInfo =
        m_odbObjectsData.commits.info.find(ref.first);
      
        if (itCommitInfo != m_odbObjectsData.commits.info.end()) {
          ++itCommitInfo->second.reachability;
        }
      }
        break;
      case GIT_OBJECT_TREE:
      {
        OdbObjectsData::iterTreeInfo itTreeInfo =
        m_odbObjectsData.trees.info.find(ref.first);
      
        if (itTreeInfo != m_odbObjectsData.trees.info.end()) {
          ++itTreeInfo->second.reachability;
        }
      }
        break;
      case GIT_OBJECT_BLOB:
      {
        OdbObjectsData::iterBlobInfo itBlobInfo =
        m_odbObjectsData.blobs.info.find(ref.first);
      
        if (itBlobInfo != m_odbObjectsData.blobs.info.end()) {
          ++itBlobInfo->second.reachability;
        }
      }
        break;
      case GIT_OBJECT_TAG:
      {
        OdbObjectsData::iterTagInfo itTagInfo =
        m_odbObjectsData.tags.info.find(ref.first);
      
        if (itTagInfo != m_odbObjectsData.tags.info.end()) {
          ++itTagInfo->second.reachability;
        }
      }
        break;
      default:
        break;
    }
  }
}

/**
 * RepoAnalysis::setUnreachables
 * After setting reachability, we add the unreached objects to their unreachables container.
 */
void RepoAnalysis::setUnreachables()
{
  for (const auto &tag : m_odbObjectsData.tags.info) {
    if (!tag.second.reachability) {
      m_odbObjectsData.tags.unreachables.emplace(tag.first);
    }
  }
  for (const auto &commit : m_odbObjectsData.commits.info) {
    if (!commit.second.reachability) {
      m_odbObjectsData.commits.unreachables.emplace(commit.first);
    }
  }
  for (const auto &tree : m_odbObjectsData.trees.info) {
    if (!tree.second.reachability) {
      m_odbObjectsData.trees.unreachables.emplace(tree.first);
    }
  }
  for (const auto &blob : m_odbObjectsData.blobs.info) {
    if (!blob.second.reachability) {
      m_odbObjectsData.blobs.unreachables.emplace(blob.first);
    }
  }
}

/**
 * RepoAnalysis::pruneUnreachables
 * Removes from their containers the unreachable objects.
 * Decreases reachability of the objects they can reach.
  */
void RepoAnalysis::pruneUnreachables()
{
  // NOTE: order is important here, since each method prunes its own objects, but
  // only decreases reachability of the objects connected to it; and those
  // connected objects will be checked and pruned afterwards.
  pruneUnreachableTags();
  pruneUnreachableCommits();
  pruneUnreachableTrees();
  pruneUnreachableBlobs();
}

/**
 * RepoAnalysis::pruneUnreachableTags
 * Prune tags and their chained tags if they become unreachable.
 * Also decreases reachability of targets.
  */
void RepoAnalysis::pruneUnreachableTags()
{
  while (!m_odbObjectsData.tags.unreachables.empty()) {
    std::unordered_set<std::string> newUnreachables {};

    // erase unreachable tags
    for (OdbObjectsData::iterUnreachable itTagUnrch = m_odbObjectsData.tags.unreachables.begin();
      itTagUnrch != m_odbObjectsData.tags.unreachables.end(); ++itTagUnrch)
    {
      OdbObjectsData::iterTagInfo itTagInfo = m_odbObjectsData.tags.info.find(*itTagUnrch);
    
      if (itTagInfo != m_odbObjectsData.tags.info.end()) {
        const std::string &oidTarget = itTagInfo->second.oidTarget;
        switch (itTagInfo->second.typeTarget) {
          case GIT_OBJECT_TAG:
          {
            // if target is another tag, add it to newUnreachables
            OdbObjectsData::iterTagInfo itTargetTagInfo = m_odbObjectsData.tags.info.find(oidTarget);
            if (itTargetTagInfo != m_odbObjectsData.tags.info.end()) {
              if (--itTargetTagInfo->second.reachability == OdbObjectsData::kUnreachable) {
                newUnreachables.emplace(itTargetTagInfo->first);
              }
            }
          }
            break;
          case GIT_OBJECT_COMMIT:
          {
            OdbObjectsData::iterCommitInfo itCommitInfo = m_odbObjectsData.commits.info.find(oidTarget);
            if (itCommitInfo != m_odbObjectsData.commits.info.end()) {
              if (--itCommitInfo->second.reachability == OdbObjectsData::kUnreachable) {
                m_odbObjectsData.commits.unreachables.emplace(itCommitInfo->first);
              }
            }
          }
            break;
          case GIT_OBJECT_TREE:
          {
            OdbObjectsData::iterTreeInfo itTreeInfo = m_odbObjectsData.trees.info.find(oidTarget);
            if (itTreeInfo != m_odbObjectsData.trees.info.end()) {
              if (--itTreeInfo->second.reachability == OdbObjectsData::kUnreachable) {
                m_odbObjectsData.trees.unreachables.emplace(itTreeInfo->first);
              }
            }
          }
            break;
          case GIT_OBJECT_BLOB:
          {
            OdbObjectsData::iterBlobInfo itBlobInfo = m_odbObjectsData.blobs.info.find(oidTarget);
            if (itBlobInfo != m_odbObjectsData.blobs.info.end()) {
              if (--itBlobInfo->second.reachability == OdbObjectsData::kUnreachable) {
                m_odbObjectsData.blobs.unreachables.emplace(itBlobInfo->first);
              }
            }
          }
            break;
          default:
            break;
        }
        // erase tag from the tag's container
        m_odbObjectsData.tags.info.erase(itTagInfo);
      }
    }
    // set new unreachable tags
    m_odbObjectsData.tags.unreachables = std::move(newUnreachables);
  }
}

/**
 * RepoAnalysis::pruneUnreachableCommits
 * Prune commits and decrease reachability of their associated trees.
  */
void RepoAnalysis::pruneUnreachableCommits()
{
  while (!m_odbObjectsData.commits.unreachables.empty()) {
    std::unordered_set<std::string> newUnreachables {};

    // erase unreachable commits
    for (OdbObjectsData::iterUnreachable itCommitUnrch = m_odbObjectsData.commits.unreachables.begin();
      itCommitUnrch != m_odbObjectsData.commits.unreachables.end(); ++itCommitUnrch)
    {
      OdbObjectsData::iterCommitInfo itCommitInfo = m_odbObjectsData.commits.info.find(*itCommitUnrch);
    
      if (itCommitInfo != m_odbObjectsData.commits.info.end())
      {
        // decrease commit's parents reachability and add them as newUnreachable
        const size_t numParents = itCommitInfo->second.parents.size();
        for (size_t i = 0; i < numParents; ++i) {
          OdbObjectsData::iterCommitInfo itParentCommitInfo =
            m_odbObjectsData.commits.info.find(itCommitInfo->second.parents.at(i));
          
          if (itParentCommitInfo != m_odbObjectsData.commits.info.end()) {
            if (--itParentCommitInfo->second.reachability == OdbObjectsData::kUnreachable) {
              newUnreachables.emplace(itParentCommitInfo->first);
            }
          }
        }
        // decrease reachability of the commit's tree
        OdbObjectsData::iterTreeInfo itTreeInfo =
          m_odbObjectsData.trees.info.find(itCommitInfo->second.oidTree);
        if (itTreeInfo != m_odbObjectsData.trees.info.end()) {
          if (--itTreeInfo->second.reachability == OdbObjectsData::kUnreachable) {
            m_odbObjectsData.trees.unreachables.emplace(itTreeInfo->first);
          }
        }
        // erase commit from the commit's container
        m_odbObjectsData.commits.info.erase(itCommitInfo);
      }
    }
    // set new unreachable commits
    m_odbObjectsData.commits.unreachables = std::move(newUnreachables);
  }
}

/**
 * RepoAnalysis::pruneUnreachableTrees
 * Prune unreachable trees and decrement reachability of their entries.
  */
void RepoAnalysis::pruneUnreachableTrees()
{
  while (!m_odbObjectsData.trees.unreachables.empty()) {
    std::unordered_set<std::string> newUnreachables {};

    // erase unreachable trees
    for (OdbObjectsData::iterUnreachable itTreeUnrch = m_odbObjectsData.trees.unreachables.begin();
      itTreeUnrch != m_odbObjectsData.trees.unreachables.end(); ++itTreeUnrch)
    {
      OdbObjectsData::iterTreeInfo itTreeInfo = m_odbObjectsData.trees.info.find(*itTreeUnrch);
    
      if (itTreeInfo != m_odbObjectsData.trees.info.end()) {
        // decrease reachability of the entry blobs
        for (auto &blob : itTreeInfo->second.entryBlobs) {
          OdbObjectsData::iterBlobInfo itEntryBlobInfo = m_odbObjectsData.blobs.info.find(blob);
          if (itEntryBlobInfo != m_odbObjectsData.blobs.info.end()) {
            if (--itEntryBlobInfo->second.reachability == OdbObjectsData::kUnreachable) {
              m_odbObjectsData.blobs.unreachables.emplace(blob);
            }
          }
        }
        // decrease reachability of the entry trees and add them as newUnreachables
        for (auto &treeNameLen : itTreeInfo->second.entryTreesNameLen) {
          OdbObjectsData::iterTreeInfo itEntryTreeInfo =
            m_odbObjectsData.trees.info.find(treeNameLen.first);
          if (itEntryTreeInfo != m_odbObjectsData.trees.info.end()) {
            if (--itEntryTreeInfo->second.reachability == OdbObjectsData::kUnreachable) {
              newUnreachables.emplace(treeNameLen.first);
            }
          }
        }
        // erase tree from the tree's container
        m_odbObjectsData.trees.info.erase(itTreeInfo);
      }
    }
    // set new unreachable trees
    m_odbObjectsData.trees.unreachables = std::move(newUnreachables);
  }
}

/**
 * RepoAnalysis::pruneUnreachableBlobs
 * Rremoves unreachable blobs from their container.
  */
void RepoAnalysis::pruneUnreachableBlobs()
{
  for (OdbObjectsData::iterUnreachable itBlobUnrch = m_odbObjectsData.blobs.unreachables.begin();
    itBlobUnrch != m_odbObjectsData.blobs.unreachables.end(); ++itBlobUnrch)
  {
    m_odbObjectsData.blobs.info.erase(*itBlobUnrch);
  }
}

/**
 * RepoAnalysis::statsCountAndMax
 * Statistics for repositorySize (count objects) and biggestObjects (get maximum of them).
 * Also builds the commits graph.
 * NOTE: better results achieved not leveraging threads.
 */
void RepoAnalysis::statsCountAndMax()
{
  // commits
  for (auto &info : m_odbObjectsData.commits.info) {
    OdbObjectsData::CommitInfo &commitInfo = info.second;
    const size_t objectSize = commitInfo.size;

    m_odbObjectsData.commits.totalSize += objectSize;
    m_odbObjectsData.commits.maxSize = std::max<size_t>(m_odbObjectsData.commits.maxSize, objectSize);
    m_odbObjectsData.commits.maxParents = std::max<size_t>(
      m_odbObjectsData.commits.maxParents, commitInfo.parents.size());

    // build commit's graph
    m_odbObjectsData.commits.graph.AddNode(info.first, commitInfo.parents);
  }
  // trees
  for (auto &info : m_odbObjectsData.trees.info) {
    OdbObjectsData::TreeInfoAndStats &treeInfo = info.second;
    const size_t numEntries = treeInfo.numEntries;
    const size_t objectSize = treeInfo.size;

    m_odbObjectsData.trees.totalSize += objectSize;
    m_odbObjectsData.trees.totalEntries += numEntries;
    m_odbObjectsData.trees.maxEntries = std::max<size_t>(m_odbObjectsData.trees.maxEntries, numEntries);
  }
  // blobs
  for (auto &info : m_odbObjectsData.blobs.info) {
    OdbObjectsData::BlobInfo &blobInfo = info.second;
    const size_t objectSize = blobInfo.size;

    m_odbObjectsData.blobs.totalSize += objectSize;
    m_odbObjectsData.blobs.maxSize = std::max<size_t>(m_odbObjectsData.blobs.maxSize, objectSize);
  }
  // no need to process tags here (we already have the count)
}

/**
 * RepoAnalysis::statsHistoryAndBiggestCheckouts
 * Statistics for historyStructure and biggestCheckouts.
 * \return true if success; false if something went wrong.
 */
bool RepoAnalysis::statsHistoryAndBiggestCheckouts()
{
  if (!calculateBiggestCheckouts()) {
    return false;
  }

  if (!calculateMaxTagDepth()) {
    return false;
  }

  // calculate max commit history depth
  m_statistics.historyStructure.maxDepth = m_odbObjectsData.commits.graph.CalculateMaxDepth();

  return true;
}

/**
 * RepoAnalysis::calculateBiggestCheckouts
 * 
 * Once threads have collected data from objects and unreachable objects
 * have been pruned, biggest checkouts can be calculated.
 * Threads have already collected partial non-recursive tree statistics.
 * \return true if success; false if something went wrong.
 */
bool RepoAnalysis::calculateBiggestCheckouts()
{
  for (auto &commitInfo : m_odbObjectsData.commits.info)
  {
    // calculate this commit's data
    const std::string &commitOidTree = commitInfo.second.oidTree;

    OdbObjectsData::iterTreeInfo itTreeInfo {};
    if ((itTreeInfo = calculateTreeStatistics(commitOidTree)) == m_odbObjectsData.trees.info.end()) {
      return false;
    }

    // update biggestCheckouts data
    OdbObjectsData::TreeInfoAndStats &treeInfoAndStats = itTreeInfo->second;
    m_statistics.biggestCheckouts.numDirectories = std::max<size_t>(
      m_statistics.biggestCheckouts.numDirectories, treeInfoAndStats.stats.numDirectories);
    m_statistics.biggestCheckouts.totalFileSize = std::max<size_t>(
      m_statistics.biggestCheckouts.totalFileSize, treeInfoAndStats.stats.totalFileSize);
    m_statistics.biggestCheckouts.maxPathDepth = std::max<size_t>(
      m_statistics.biggestCheckouts.maxPathDepth, treeInfoAndStats.stats.maxPathDepth);
    m_statistics.biggestCheckouts.numFiles = std::max<size_t>(
      m_statistics.biggestCheckouts.numFiles, treeInfoAndStats.stats.numFiles);
    m_statistics.biggestCheckouts.maxPathLength = std::max<size_t>(
      m_statistics.biggestCheckouts.maxPathLength, treeInfoAndStats.stats.maxPathLength);
    m_statistics.biggestCheckouts.numSymlinks = std::max<size_t>(
      m_statistics.biggestCheckouts.numSymlinks, treeInfoAndStats.stats.numSymlinks);
    m_statistics.biggestCheckouts.numSubmodules = std::max<size_t>(
      m_statistics.biggestCheckouts.numSubmodules, treeInfoAndStats.stats.numSubmodules);
  }

  return true;
}

/**
 * RepoAnalysis::calculateTreeStatistics
 * 
 * Calculates tree statistics recursively, considering individual tree's statistics
 * have already been calculated.
 * The maximum number of recursive calls depend directly on the maximum path depth of
 * the repository. For instance, the linux repository have a maximum path depth of 13,
 * so it should be safe against stack overflow.
 * Returns an iterator to the tree info container, or to end if something went wrong.
 */
OdbObjectsData::iterTreeInfo RepoAnalysis::calculateTreeStatistics(const std::string &oidTree)
{
  OdbObjectsData::iterTreeInfo itTreeInfo = m_odbObjectsData.trees.info.find(oidTree);
  if (itTreeInfo == m_odbObjectsData.trees.info.end()) {
    return itTreeInfo;
  }

  OdbObjectsData::TreeInfoAndStats &treeInfoAndStats = itTreeInfo->second;

  // prune recursivity
  if (treeInfoAndStats.statsDone) {
    return itTreeInfo;
  }

  ++treeInfoAndStats.stats.numDirectories;
  ++treeInfoAndStats.stats.maxPathDepth;
  // the following partial statistics have also been calculated in previous stage with threads:
  // - treeInfoAndStats.stats.numFiles
  // - treeInfoAndStats.stats.maxPathLength
  // - treeInfoAndStats.stats.numSymLinks
  // - treeInfoAndStats.stats.numSubmodules

  // totalFileSize
  OdbObjectsData::iterBlobInfo itBlobInfo {};
  for (auto &oidBlob : treeInfoAndStats.entryBlobs)
  {
    if ((itBlobInfo = m_odbObjectsData.blobs.info.find(oidBlob)) == m_odbObjectsData.blobs.info.end()) {
      return m_odbObjectsData.trees.info.end(); // to let the caller know that something went wrong
    }

    treeInfoAndStats.stats.totalFileSize += itBlobInfo->second.size;
  }

  // recursively into subtrees
  for (const auto &subTreeNameLen : treeInfoAndStats.entryTreesNameLen)
  {
    OdbObjectsData::iterTreeInfo itSubTreeInfo {};
    if ((itSubTreeInfo = calculateTreeStatistics(subTreeNameLen.first)) ==
      m_odbObjectsData.trees.info.end()) {
      return itSubTreeInfo;
    }

    OdbObjectsData::TreeInfoAndStats &subTreeInfoAndStats = itSubTreeInfo->second;
    treeInfoAndStats.stats.numDirectories += subTreeInfoAndStats.stats.numDirectories;
    treeInfoAndStats.stats.maxPathDepth = std::max<size_t>(treeInfoAndStats.stats.maxPathDepth,
      subTreeInfoAndStats.stats.maxPathDepth + 1);
    treeInfoAndStats.stats.maxPathLength = std::max<size_t>(treeInfoAndStats.stats.maxPathLength,
      subTreeNameLen.second + 1 + subTreeInfoAndStats.stats.maxPathLength);
    treeInfoAndStats.stats.numFiles += subTreeInfoAndStats.stats.numFiles;
    treeInfoAndStats.stats.totalFileSize += subTreeInfoAndStats.stats.totalFileSize;
    treeInfoAndStats.stats.numSymlinks += subTreeInfoAndStats.stats.numSymlinks;
    treeInfoAndStats.stats.numSubmodules += subTreeInfoAndStats.stats.numSubmodules;
  }

  treeInfoAndStats.statsDone = true;

  return itTreeInfo;
}

/**
 * RepoAnalysis::calculateMaxTagDepth
 * \return true if success; false if something went wrong.
 */
bool RepoAnalysis::calculateMaxTagDepth()
{
  for (auto &tag : m_odbObjectsData.tags.info)
  {
    OdbObjectsData::iterTagInfo itTagInfo {};
    if ((itTagInfo = calculateTagDepth(tag.first)) == m_odbObjectsData.tags.info.end()) {
      return false;
    }

    // update maxTagDepth
    OdbObjectsData::TagInfo &tagInfo = itTagInfo->second;
    m_statistics.historyStructure.maxTagDepth = std::max<uint32_t>(
      m_statistics.historyStructure.maxTagDepth, tagInfo.depth);
  }

  return true;
}

/**
 * RepoAnalysis::calculateTagDepth
 * 
 * Calculates recursively the tag depth of the oidTag passed as a parameter.
 * Returns an iterator to the tag info container, or to end if something went wrong.
 */
OdbObjectsData::iterTagInfo RepoAnalysis::calculateTagDepth(const std::string &oidTag)
{
  OdbObjectsData::iterTagInfo itTagInfo = m_odbObjectsData.tags.info.find(oidTag);
  if (itTagInfo == m_odbObjectsData.tags.info.end()) {
    return itTagInfo;
  }

  OdbObjectsData::TagInfo &tagInfo = itTagInfo->second;

  // prune recursivity
  if (tagInfo.depth != OdbObjectsData::TagInfo::kUnsetDepth) {
    return itTagInfo;
  }

  ++tagInfo.depth;

  if (tagInfo.typeTarget == GIT_OBJECT_TAG)
  {
    OdbObjectsData::iterTagInfo itChainedTagInfo {};
    if ((itChainedTagInfo = calculateTagDepth(tagInfo.oidTarget)) == m_odbObjectsData.tags.info.end()) {
      return itChainedTagInfo;
    }

    OdbObjectsData::TagInfo &chainedTagInfo = itChainedTagInfo->second;
    tagInfo.depth += chainedTagInfo.depth;
  }

  return itTagInfo;
}

/**
 * RepoAnalysis::fillOutStatistics
 */
void RepoAnalysis::fillOutStatistics()
{
  m_statistics.repositorySize.commits.count = m_odbObjectsData.commits.info.size();
  m_statistics.repositorySize.commits.size = m_odbObjectsData.commits.totalSize;
  m_statistics.repositorySize.trees.count = m_odbObjectsData.trees.info.size();
  m_statistics.repositorySize.trees.size = m_odbObjectsData.trees.totalSize;
  m_statistics.repositorySize.trees.entries = m_odbObjectsData.trees.totalEntries;
  m_statistics.repositorySize.blobs.count = m_odbObjectsData.blobs.info.size();
  m_statistics.repositorySize.blobs.size = m_odbObjectsData.blobs.totalSize;
  m_statistics.repositorySize.annotatedTags.count = m_odbObjectsData.tags.info.size();

  m_statistics.biggestObjects.commits.maxSize = m_odbObjectsData.commits.maxSize;
  m_statistics.biggestObjects.commits.maxParents = m_odbObjectsData.commits.maxParents;
  m_statistics.biggestObjects.trees.maxEntries = m_odbObjectsData.trees.maxEntries;
  m_statistics.biggestObjects.blobs.maxSize = m_odbObjectsData.blobs.maxSize;

  // m_statistics.biggestCheckouts have already been filled out while running
}

/**
 * RepoAnalysis::repositorySizeToJS
 */
v8::Local<v8::Object> RepoAnalysis::repositorySizeToJS() const
{
  v8::Local<v8::Object> commits = Nan::New<Object>();
  Nan::Set(commits, Nan::New("count").ToLocalChecked(),
    Nan::New<Number>(m_statistics.repositorySize.commits.count));
  Nan::Set(commits, Nan::New("size").ToLocalChecked(),
    Nan::New<Number>(m_statistics.repositorySize.commits.size));

  v8::Local<v8::Object> trees = Nan::New<Object>();
  Nan::Set(trees, Nan::New("count").ToLocalChecked(),
    Nan::New<Number>(m_statistics.repositorySize.trees.count));
  Nan::Set(trees, Nan::New("size").ToLocalChecked(),
    Nan::New<Number>(m_statistics.repositorySize.trees.size));
  Nan::Set(trees, Nan::New("entries").ToLocalChecked(),
    Nan::New<Number>(m_statistics.repositorySize.trees.entries));

  v8::Local<v8::Object> blobs = Nan::New<Object>();
  Nan::Set(blobs, Nan::New("count").ToLocalChecked(),
    Nan::New<Number>(m_statistics.repositorySize.blobs.count));
  Nan::Set(blobs, Nan::New("size").ToLocalChecked(),
    Nan::New<Number>(m_statistics.repositorySize.blobs.size));

  v8::Local<v8::Object> annotatedTags = Nan::New<Object>();
  Nan::Set(annotatedTags, Nan::New("count").ToLocalChecked(),
    Nan::New<Number>(m_statistics.repositorySize.annotatedTags.count));

  v8::Local<v8::Object> references = Nan::New<Object>();
  Nan::Set(references, Nan::New("count").ToLocalChecked(),
    Nan::New<Number>(m_statistics.repositorySize.references.count));

  v8::Local<v8::Object> result = Nan::New<Object>();
  Nan::Set(result, Nan::New("commits").ToLocalChecked(), commits);
  Nan::Set(result, Nan::New("trees").ToLocalChecked(), trees);
  Nan::Set(result, Nan::New("blobs").ToLocalChecked(), blobs);
  Nan::Set(result, Nan::New("annotatedTags").ToLocalChecked(), annotatedTags);
  Nan::Set(result, Nan::New("references").ToLocalChecked(), references);

  return result;
}

/**
 * RepoAnalysis::biggestObjectsToJS
 */
v8::Local<v8::Object> RepoAnalysis::biggestObjectsToJS() const
{
  v8::Local<v8::Object> commits = Nan::New<Object>();
  Nan::Set(commits, Nan::New("maxSize").ToLocalChecked(),
    Nan::New<Number>(m_statistics.biggestObjects.commits.maxSize));
  Nan::Set(commits, Nan::New("maxParents").ToLocalChecked(),
    Nan::New<Number>(m_statistics.biggestObjects.commits.maxParents));

  v8::Local<v8::Object> trees = Nan::New<Object>();
  Nan::Set(trees, Nan::New("maxEntries").ToLocalChecked(),
    Nan::New<Number>(m_statistics.biggestObjects.trees.maxEntries));

  v8::Local<v8::Object> blobs = Nan::New<Object>();
  Nan::Set(blobs, Nan::New("maxSize").ToLocalChecked(),
    Nan::New<Number>(m_statistics.biggestObjects.blobs.maxSize));

  v8::Local<v8::Object> result = Nan::New<Object>();
  Nan::Set(result, Nan::New("commits").ToLocalChecked(), commits);
  Nan::Set(result, Nan::New("trees").ToLocalChecked(), trees);
  Nan::Set(result, Nan::New("blobs").ToLocalChecked(), blobs);

  return result;
}

/**
 * RepoAnalysis::historyStructureToJS
 */
v8::Local<v8::Object> RepoAnalysis::historyStructureToJS() const
{
  v8::Local<v8::Object> result = Nan::New<Object>();
  Nan::Set(result, Nan::New("maxDepth").ToLocalChecked(),
    Nan::New<Number>(m_statistics.historyStructure.maxDepth));
  Nan::Set(result, Nan::New("maxTagDepth").ToLocalChecked(),
    Nan::New<Number>(m_statistics.historyStructure.maxTagDepth));

  return result;
}

/**
 * RepoAnalysis::biggestCheckoutsToJS
 */
v8::Local<v8::Object> RepoAnalysis::biggestCheckoutsToJS() const
{
  v8::Local<v8::Object> result = Nan::New<Object>();
  Nan::Set(result, Nan::New("numDirectories").ToLocalChecked(),
    Nan::New<Number>(m_statistics.biggestCheckouts.numDirectories));
  Nan::Set(result, Nan::New("maxPathDepth").ToLocalChecked(),
    Nan::New<Number>(m_statistics.biggestCheckouts.maxPathDepth));
  Nan::Set(result, Nan::New("maxPathLength").ToLocalChecked(),
    Nan::New<Number>(m_statistics.biggestCheckouts.maxPathLength));
  Nan::Set(result, Nan::New("numFiles").ToLocalChecked(),
    Nan::New<Number>(m_statistics.biggestCheckouts.numFiles));
  Nan::Set(result, Nan::New("totalFileSize").ToLocalChecked(),
    Nan::New<Number>(m_statistics.biggestCheckouts.totalFileSize));
  Nan::Set(result, Nan::New("numSymlinks").ToLocalChecked(),
    Nan::New<Number>(m_statistics.biggestCheckouts.numSymlinks));
  Nan::Set(result, Nan::New("numSubmodules").ToLocalChecked(),
    Nan::New<Number>(m_statistics.biggestCheckouts.numSubmodules));

  return result;
}

NAN_METHOD(GitRepository::Statistics)
{
  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  StatisticsBaton* baton = new StatisticsBaton();

   baton->error_code = GIT_OK;
   baton->error = NULL;
   baton->repo = Nan::ObjectWrap::Unwrap<GitRepository>(info.This())->GetValue();
   baton->out = static_cast<void *>(new RepoAnalysis(baton->repo));
   
  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[info.Length() - 1]));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  StatisticsWorker *worker = new StatisticsWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRepository>("repo", info.This());
  nodegit::Context *nodegitContext =
    reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);
  return;
}

nodegit::LockMaster GitRepository::StatisticsWorker::AcquireLocks()
{
  nodegit::LockMaster lockMaster(true, baton->repo);

  return lockMaster;
}

void GitRepository::StatisticsWorker::Execute()
{
  git_error_clear();

  RepoAnalysis *repoAnalysis = static_cast<RepoAnalysis *>(baton->out);
  if ((baton->error_code = repoAnalysis->Analyze()) != GIT_OK)
  {
    if (git_error_last() != NULL) {
      baton->error = git_error_dup(git_error_last());
    }

    delete repoAnalysis;
    baton->out = nullptr;
  }
}

void GitRepository::StatisticsWorker::HandleErrorCallback()
{
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  RepoAnalysis *repoAnalysis = static_cast<RepoAnalysis *>(baton->out);
  if (repoAnalysis) {
    delete repoAnalysis;
  }

  delete baton;
}

void GitRepository::StatisticsWorker::HandleOKCallback()
{
  if (baton->out != NULL)
  {
    RepoAnalysis *repoAnalysis = static_cast<RepoAnalysis *>(baton->out);
    Local<v8::Object> result = repoAnalysis->StatisticsToJS();

    delete repoAnalysis;

    Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv, async_resource);
  }
  else if (baton->error)
  {
    Local<v8::Object> err;

    if (baton->error->message) {
      err = Nan::To<v8::Object>(Nan::Error(baton->error->message)).ToLocalChecked();
    } else {
      err = Nan::To<v8::Object>(Nan::Error("Method statistics has thrown an error.")).ToLocalChecked();
    }
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("GitRepository.statistics").ToLocalChecked());
    Local<v8::Value> argv[1] = {
      err
    };

    callback->Call(1, argv, async_resource);

    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }
  else if (baton->error_code < 0)
  {
    Local<v8::Object> err = Nan::To<v8::Object>(Nan::Error("Method statistics has thrown an error.")).ToLocalChecked();
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("GitRepository.statistics").ToLocalChecked());
    Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
  }
  else
  {
    callback->Call(0, NULL, async_resource);
  }

  delete baton;
}