#ifndef TRACKERWRAP_H
#define TRACKERWRAP_H

#include <napi.h>
#include <memory>
#include <vector>

namespace nodegit {
  /**
   * \class TrackerWrap
   *
   * Common ObjectWrap base for every nodegit wrapper object. We intentionally
   * use a SINGLE wrap type (`Napi::ObjectWrap<TrackerWrap>`) rather than
   * `Napi::ObjectWrap<T>` per generated class. This lets us unwrap *any*
   * wrapped object back to its `TrackerWrap*` generically, which the
   * ownership-graph cleanup (`TrackerWrapTrees`) depends on: a borrowed struct
   * records its owner as a `TrackerWrap*`, but the owner may be a completely
   * different generated class. With a per-class `ObjectWrap<T>` that generic
   * unwrap would be impossible.
   *
   * `cppClass` is always laid out so that `TrackerWrap` is its primary base, so
   * `static_cast<cppClass*>(TrackerWrap*)` is a no-op offset adjustment and is
   * safe to use when retrieving a strongly typed wrapper from a generic unwrap.
   */
  class TrackerWrap : public Napi::ObjectWrap<TrackerWrap> {
  public:
    // Required by Napi::ObjectWrap<T>; never invoked as a JS constructor
    // directly, only as the most-derived base ctor of a generated wrapper.
    TrackerWrap(const Napi::CallbackInfo& info);
    virtual ~TrackerWrap();

    virtual void DestroyNative() {}

    TrackerWrap(const TrackerWrap &other) = delete;
    TrackerWrap(TrackerWrap &&other) = delete;
    TrackerWrap& operator=(const TrackerWrap &other) = delete;
    TrackerWrap& operator=(TrackerWrap &&other) = delete;

    // Intrusive-list head. Deliberately NOT a TrackerWrap: that derives from
    // Napi::ObjectWrap (which has no default ctor), while this sentinel must be
    // default-constructible as a plain member of Context.
    struct TrackerList {
      TrackerWrap *head = nullptr;
      TrackerWrap *tail = nullptr;
    };

    // Links this tracker into the list starting at `listStart`.
    inline void Link(TrackerList* listStart) {
      if (listStart == nullptr) {
        return;
      }

      m_list = listStart;
      m_prev = nullptr;
      m_next = listStart->head;
      if (listStart->head != nullptr) {
        listStart->head->m_prev = this;
      }
      else {
        listStart->tail = this;
      }
      listStart->head = this;
    }

    // Unlinks this tracker from its list and returns it.
    inline TrackerWrap* Unlink() {
      if (m_list != nullptr) {
        if (m_list->head == this) {
          m_list->head = m_next;
        }
        if (m_list->tail == this) {
          m_list->tail = m_prev;
        }
      }

      if (m_prev != nullptr) {
        m_prev->m_next = m_next;
      }

      if (m_next != nullptr) {
        m_next->m_prev = m_prev;
      }

      m_next = m_prev = nullptr;
      m_list = nullptr;

      return this;
    }

    inline void SetTrackerWrapOwners(std::unique_ptr< std::vector<TrackerWrap*> > &&owners) {
      m_owners = std::move(owners);
    }

    inline const std::vector<TrackerWrap*>* GetTrackerWrapOwners() const {
      return m_owners.get();
    }

    static TrackerWrap* UnlinkFirst(TrackerList *listStart);
    static int SizeFromList(TrackerList *listStart);
    static void DeleteFromList(TrackerList *listStart);

  private:
    TrackerWrap* m_next {};
    TrackerWrap* m_prev {};
    TrackerList* m_list {};
    // m_owners will store pointers to native objects
    std::unique_ptr< std::vector<TrackerWrap*> > m_owners {};
  };
}

#endif
