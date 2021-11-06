#pragma once

#include <Ranae/Common.h>

#include <vector>
#include <deque>
#include <array>
#include <unordered_map>
#include <cstring>

namespace ranae {

  using EntityId = uint32_t;
  constexpr size_t MaxEntities = 1024;

  using ComponentType = uint32_t;
  constexpr ComponentType MaxComponents = 32;

  using ComponentSignature = uint32_t;

  class EntityManager {
  public:
    EntityManager() {
      for (EntityId i = 0; i < MaxEntities; i++)
        m_availableEntities.push_back(i);
    }

    inline EntityId createEntity() {
      rnAssert(!m_availableEntities.empty());

      EntityId id = m_availableEntities.front();
      m_availableEntities.pop_front();
      return id;
    }

    inline void destroyEntity(EntityId id) {
      rnAssert(id < MaxEntities);
      m_availableEntities.push_back(id);
      m_entitySignatures[id] = 0;
    }

    void setSignature(EntityId id, ComponentSignature signature) {
      rnAssert(id < MaxEntities);
      m_entitySignatures[id] = signature;
    }

    ComponentSignature getSignature(EntityId id) {
      rnAssert(id < MaxEntities);
      return m_entitySignatures[id];
    }
  private:
    std::deque<EntityId> m_availableEntities;
    std::array<ComponentSignature, MaxEntities> m_entitySignatures = {};
  };
  
  template <typename T>
  class ComponentArray;

  class GenericComponentArray {
  public:
    virtual ~GenericComponentArray() = default;

    virtual void removeData(EntityId entityId) = 0;

    void onEntityDestroyed(EntityId entityId) {
      if (m_entityToIndexMap.find(entityId) != m_entityToIndexMap.end())
        removeData(entityId);
    }
  protected:
    std::unordered_map<EntityId, size_t> m_entityToIndexMap;
    std::unordered_map<size_t, EntityId> m_indexToEntityMap;

    size_t m_size = 0;
  };
  
  template <typename T>
  class ComponentArray final : public GenericComponentArray {
  public:
    void insertData(EntityId entityId, T&& component) {
      rnAssert(m_entityToIndexMap.find(entityId) == m_entityToIndexMap.end());
      
      const size_t idx = m_size++;
      m_entityToIndexMap[entityId] = idx;
      m_indexToEntityMap[idx]      = entityId;
      m_componentArray  [idx]      = component;
    }
    
    void removeData(EntityId entityId) {
      rnAssert(m_entityToIndexMap.find(entityId) != m_entityToIndexMap.end());
      
      // Remove element by packing the last to keep contiguous.
      const size_t removedIdx     = m_entityToIndexMap[entityId];
      const size_t endElementIdx  = --m_size;
      const EntityId endEntityId  = m_indexToEntityMap[endElementIdx];
      if (removedIdx != endElementIdx) {
        // Explicitly call destructor on what we are removing.
        // and copy data of last element over without calling a constructor/destructor.
        // More efficient than std::move.
        m_componentArray[removedIdx].~T();
        std::memcpy(&m_componentArray[removedIdx], &m_componentArray[endElementIdx], sizeof(T));

        m_entityToIndexMap[endEntityId] = removedIdx;
        m_indexToEntityMap[removedIdx]  = endEntityId;
      }
      
      m_entityToIndexMap.erase(entityId);
      m_indexToEntityMap.erase(endElementIdx);
    }
    
    T* getData(EntityId entityId) {
      rnAssert(m_entityToIndexMap.find(entityId) != m_entityToIndexMap.end());
      
      return &m_componentArray[m_entityToIndexMap[entityId]];
    }
    
  private:
    std::array<T, MaxEntities> m_componentArray;
  };

  namespace Component {
    enum Components : uint32_t {
      Name,
      Count,
    };
  }

  struct NameComponent {
    static constexpr uint32_t ComponentIdx = Component::Name;
    static constexpr ComponentType Type = 1u << ComponentIdx;

    const char* name;
  };

  class ComponentManager {
  public:
    template <typename T>
    ComponentArray<T>& getComponentArray() {
      return *static_cast<ComponentArray<T>*>(m_componentArrays[T::ComponentIdx].get());
    }

    template <typename T>
    ComponentType getComponentType() {
      return T::Type;
    }

    template <typename T>
    void addComponent(EntityId entityId, T&& component) {
      getComponentArray<T>().insertData(entityId, std::move(component));
    }

    template <typename T>
    void removeComponent(EntityId entityId) {
      getComponentArray<T>().removeData(entityId);
    }

    template <typename T>
    T& getComponent(EntityId entityId) {
      return getComponentArray<T>().getData(entityId);
    }

    void onEntityDestroyed(EntityId entityId) {
      for (const auto& componentArray : m_componentArrays)
        componentArray->onEntityDestroyed(entityId);
    }

  private:
    std::array<std::unique_ptr<GenericComponentArray>, Component::Count> m_componentArrays;
  };

  /*class System {
  protected:
    std::set<EntityId> m_entities;
  }*/

}

