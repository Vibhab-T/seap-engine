#include "EntityManager.h"

EntityManager::EntityManager()
{
}

void EntityManager::update()
{
    // add entities from entitiesToAdd
    // add to vector of all entities
    // add to vector inside the map, with the tag as key

    // remove dead entites from vector of all entites
    removeDeadEntities(m_entities);

    // remove dead entities from each vector in the entity map
    for (auto &[tag, entityVec] : m_entityMap)
    {
        removeDeadEntities(entityVec);
    }
}

void EntityManager::removeDeadEntities(EntityVec &vec)
{
    // remove all dead entities from the input vecotr
    // called by the update function
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag)
{
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

    m_entitiesToAdd.push_back(entity);

    return entity;
}

const EntityVec &EntityManager::getEntites()
{
    return m_entities;
}

const EntityVec &EntityManager::getEntities(const std::string &tag)
{
    // return the correct vector from the map
    return m_entities;
}