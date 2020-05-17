#pragma once

#include <memory>

class Model;
struct ModelHolder
{
	std::shared_ptr<Model> model;
};