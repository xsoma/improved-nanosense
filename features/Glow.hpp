#pragma once
#include "../options.hpp"
#include "../Structs.hpp"
#include "../helpers/math.hpp"
#include "../helpers/utils.hpp"
#include "../singleton.hpp"

class Glow
	: public Singleton<Glow>
{
	friend class Singleton<Glow>;

	Glow();
	~Glow();

public:
	void Run();
	void Shutdown();
};