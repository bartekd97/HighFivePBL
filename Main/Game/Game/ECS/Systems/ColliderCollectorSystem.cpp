#include "ColliderCollectorSystem.h"
#include "../../Physics/Physics.h"

void ColliderCollectorSystem::Init()
{
	EventManager::AddListener(METHOD_LISTENER(Events::GameObject::DESTROYED, ColliderCollectorSystem::OnGameObjectDestroyed));
}

void ColliderCollectorSystem::OnGameObjectDestroyed(Event& e)
{
	GameObject gameObject = e.GetParam<GameObject>(Events::GameObject::GameObject);
	Physics::RemoveNode(gameObject);
}
