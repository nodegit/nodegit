#include "../include/tracker_wrap.h"

#include <cassert>
#include <unordered_set>
#include <unordered_map>

namespace {
  /**
   * \class TrackerWrapTreeNode
   * 
   * Parents of a TrackerWrapTreeNode will be the nodes holding TrackerWrap objects that
   * are owners of the TrackerWrap object that this node holds. The same way for its children.
   */
  class TrackerWrapTreeNode
  {
  public:
    TrackerWrapTreeNode(nodegit::TrackerWrap *trackerWrap) : m_trackerWrap(trackerWrap) {}
    TrackerWrapTreeNode() = delete;
    ~TrackerWrapTreeNode();
    TrackerWrapTreeNode(const TrackerWrapTreeNode &other) = delete;
    TrackerWrapTreeNode(TrackerWrapTreeNode &&other) = delete;
    TrackerWrapTreeNode& operator=(const TrackerWrapTreeNode &other) = delete;
    TrackerWrapTreeNode& operator=(TrackerWrapTreeNode &&other) = delete;

    inline const std::unordered_set<nodegit::TrackerWrap *>& Children() const;
    inline nodegit::TrackerWrap* TrackerWrap();
    inline void AddChild(nodegit::TrackerWrap *child);

  private:
    // Children are stored by their TrackerWrap* KEY (not by node pointer).
    // A child may have several parents, so it can be freed by another parent's
    // subtree walk before this parent is visited; keying by TrackerWrap* lets
    // the walk re-resolve (and skip) already-freed children via the map without
    // dereferencing freed memory.
    std::unordered_set<nodegit::TrackerWrap *> m_children {};
    nodegit::TrackerWrap *m_trackerWrap {};
  };

  /**
   * TrackerWrapTreeNode::~TrackerWrapTreeNode()
   * Frees the memory of the TrackerWrap pointer it holds.
   */
  TrackerWrapTreeNode::~TrackerWrapTreeNode() {
    if (m_trackerWrap != nullptr) {
      m_trackerWrap->Unlink();
      m_trackerWrap->DestroyNative();
    }
  }

  /**
   * TrackerWrapTreeNode::Children()
   * 
   * Returns a reference to the children nodes of this.
   */
  const std::unordered_set<nodegit::TrackerWrap *>& TrackerWrapTreeNode::Children() const {
    return m_children;
  }

  /**
   * TrackerWrapTreeNode::TrackerWrap()
   * 
   * Returns a pointer to the node's TrackerWrap object.
   */
  nodegit::TrackerWrap* TrackerWrapTreeNode::TrackerWrap() {
    return m_trackerWrap;
  }

  /**
   * TrackerWrapTreeNode::AddChild()
   */
  void TrackerWrapTreeNode::AddChild(nodegit::TrackerWrap *child) {
    m_children.insert(child);
  }

  /**
   * \class TrackerWrapTrees
   * 
   * Class containing a list of trees with nodes holding TrackerWrap objects.
   * For a TrackerWrap object 'P' which owns another TrackerWrap object 'C',
   * 'P' will be held in a node which will be the parent of the child node
   * that holds 'C'.
   * On destruction, nodes will be freed in a children-first way.
   * 
   * NOTE: nodegit code previous to this change is prepared to manage an array of
   * owners, so class TrackerWrapTrees considers multiple owners (parent nodes) too.
   */
  class TrackerWrapTrees
  {
  public:
    TrackerWrapTrees(nodegit::TrackerWrap::TrackerList *trackerList);
    TrackerWrapTrees() = delete;
    ~TrackerWrapTrees();
    TrackerWrapTrees(const TrackerWrapTrees &other) = delete;
    TrackerWrapTrees(TrackerWrapTrees &&other) = delete;
    TrackerWrapTrees& operator=(const TrackerWrapTrees &other) = delete;
    TrackerWrapTrees& operator=(TrackerWrapTrees &&other) = delete;

  private:
    void addNode(nodegit::TrackerWrap *trackerWrap);
    void addParentNode(nodegit::TrackerWrap *owner, TrackerWrapTreeNode *child);
    void deleteTree(nodegit::TrackerWrap *trackerWrap);
    void freeAllTreesChildrenFirst();

    using TrackerWrapTreeNodeMap = std::unordered_map<nodegit::TrackerWrap*, std::unique_ptr<TrackerWrapTreeNode>>;

    TrackerWrapTreeNodeMap m_mapTrackerWrapNode {};
    // Roots are stored by TrackerWrap* key for the same reason children are.
    std::vector<nodegit::TrackerWrap *> m_roots {};
  };

  /**
   * TrackerWrapTrees::TrackerWrapTrees(nodegit::TrackerWrap::TrackerList *trackerList)
   * 
   * Unlinks items from trackerList and adds them to a tree.
   * For each root (TrackerWrap item without owners), it adds a new tree root.
   * 
   * \param trackerList TrackerList pointer from which the TrackerWrapTrees object will be created.
   */
  TrackerWrapTrees::TrackerWrapTrees(nodegit::TrackerWrap::TrackerList *trackerList)
  {
    nodegit::TrackerWrap *trackerWrap {};
    while ((trackerWrap = nodegit::TrackerWrap::UnlinkFirst(trackerList)) != nullptr) {
      addNode(trackerWrap);
    }
  }

  /*
  * TrackerWrapTrees::~TrackerWrapTrees
  */
  TrackerWrapTrees::~TrackerWrapTrees() {
    freeAllTreesChildrenFirst();
  }

  /**
   * TrackerWrapTrees::addNode
   * 
   * \param trackerWrap pointer to the TrackerWrap object to add as a node in a tree.
   */
  void TrackerWrapTrees::addNode(nodegit::TrackerWrap *trackerWrap) {
    // add trackerWrap as a node
    // NOTE: 'emplace' will create a temporal TrackerWrapTreeNode and will
    // free it if trackerWrap already exists as a key. To prevent freeing
    // the node at this moment we have to find it first.
    auto addedNodeIter = m_mapTrackerWrapNode.find(trackerWrap);
    if (addedNodeIter == m_mapTrackerWrapNode.end()) {
      addedNodeIter = m_mapTrackerWrapNode.emplace(std::make_pair(
        trackerWrap, std::make_unique<TrackerWrapTreeNode>(trackerWrap))).first;
    }
    TrackerWrapTreeNode *addedNode = addedNodeIter->second.get();

    // if trackerWrap has no owners, add it as a root node
    const std::vector<nodegit::TrackerWrap*> *owners = trackerWrap->GetTrackerWrapOwners();
    if (owners == nullptr) {
      m_roots.push_back(trackerWrap);
    }
    else {
      // add addedNode's parents and link them with this child
      for (nodegit::TrackerWrap *owner : *owners) {
        addParentNode(owner, addedNode);
      }
    }
  }

  /**
   * TrackerWrapTrees::addParentNode
   * 
   * \param owner TrackerWrap pointer for the new parent node to add.
   * \param child TrackerWrapTreeNode pointer to be the child node of the new parent node to add.
   */
  void TrackerWrapTrees::addParentNode(nodegit::TrackerWrap *owner, TrackerWrapTreeNode *child)
  {
    // adds a new parent node (holding the owner)
    // NOTE: 'emplace' will create a temporal TrackerWrapTreeNode and will
    // free it if trackerWrap already exists as a key. To prevent freeing
    // the node at this moment we have to find it first.
    auto addedParentNodeIter = m_mapTrackerWrapNode.find(owner);
    if (addedParentNodeIter == m_mapTrackerWrapNode.end()) {
      addedParentNodeIter = m_mapTrackerWrapNode.emplace(std::make_pair(
        owner, std::make_unique<TrackerWrapTreeNode>(owner))).first;
    }
    TrackerWrapTreeNode *addedParentNode = addedParentNodeIter->second.get();

    // links parent to child
    addedParentNode->AddChild(child->TrackerWrap());
  }

  /**
   * TrackerWrapTrees::deleteTree
   * 
   * Deletes the tree from the node passed as a parameter
   * in a children-first way and recursively.
   * 
   * \param trackerWrap TrackerWrap key from where to delete all its children and itself.
   */
  void TrackerWrapTrees::deleteTree(nodegit::TrackerWrap *trackerWrap)
  {
    auto nodeIter = m_mapTrackerWrapNode.find(trackerWrap);
    if (nodeIter == m_mapTrackerWrapNode.end()) {
      // Already freed while walking another parent's subtree.
      return;
    }

    TrackerWrapTreeNode *node = nodeIter->second.get();

    // Copy the child keys before recursing: the map is mutated by the
    // recursive calls, and a reference into the node's set could not be held
    // across erases.
    const std::unordered_set<nodegit::TrackerWrap *> &children = node->Children();
    std::vector<nodegit::TrackerWrap *> childKeys(children.begin(), children.end());
    for (nodegit::TrackerWrap *childKey : childKeys) {
      // Re-resolve each child through the map: a sibling subtree may already
      // have freed it (a TrackerWrap can have multiple owners). Never
      // dereference a possibly-stale node pointer here.
      deleteTree(childKey);
    }

    // then deletes itself from the container, which will
    // actually free 'node' and the TrackerWrap object it holds
    m_mapTrackerWrapNode.erase(nodeIter);
  }

  /**
   * TrackerWrapTrees::freeAllTreesChildrenFirst
   * 
   * Deletes all the trees held, in a children-first way.
   */
  void TrackerWrapTrees::freeAllTreesChildrenFirst() {
    // A root may be freed while walking an earlier root's subtree, so copy the
    // keys and rely on deleteTree to skip anything already gone.
    std::vector<nodegit::TrackerWrap *> roots(m_roots);
    for (nodegit::TrackerWrap *root : roots) {
      deleteTree(root);
    }
    m_roots.clear();
  }
} // end anonymous namespace


namespace nodegit {
  TrackerWrap::TrackerWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<TrackerWrap>(info) {
  }

  TrackerWrap::~TrackerWrap() {
    this->SuppressDestruct();
    if (this->_ref != nullptr) {
      napi_delete_reference(this->_env, this->_ref);
      this->_ref = nullptr;
    }
  }

  TrackerWrap* TrackerWrap::UnlinkFirst(TrackerList *listStart) {
    assert(listStart != nullptr);
    if (listStart->head == nullptr) {
      return nullptr;
    }

    TrackerWrap *first = listStart->head;
    listStart->head = first->m_next;
    if (listStart->head == nullptr) {
      listStart->tail = nullptr;
    }

    return first->Unlink();
  }

  int TrackerWrap::SizeFromList(TrackerList *listStart) {
    assert(listStart != nullptr);
    int count {0};
    for (TrackerWrap *t = listStart->head; t != nullptr; t = t->m_next) {
      ++count;
    }
    return count;
  }

  void TrackerWrap::DeleteFromList(TrackerList *listStart) {
    assert(listStart != nullptr);
    // creates an object TrackerWrapTrees, which will free
    // the nodes of its trees in a children-first way
    TrackerWrapTrees trackerWrapTrees(listStart);
  }
}