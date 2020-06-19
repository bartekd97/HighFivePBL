#include "TriggerColliderCollectorSystem.h"
#include "../../Physics/Physics.h"

void TriggerColliderCollectorSystem::Init()
{
	EventManager::AddListener(METHOD_LISTENER(Events::GameObject::DESTROYED, TriggerColliderCollectorSystem::OnGameObjectDestroyed));
}

void TriggerColliderCollectorSystem::OnGameObjectDestroyed(Event& e)
{
	GameObject gameObject = e.GetParam<GameObject>(Events::GameObject::GameObject);
	Physics::RemoveNode(gameObject);
}
