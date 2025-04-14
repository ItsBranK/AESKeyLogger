#include "FunctionHook.hpp"
#include "../MinHook/MinHook.hpp"

namespace AESLogger
{
	FunctionHook::FunctionHook() : m_targetFunction(nullptr), m_hookedFunction(nullptr), m_overrideFunction(nullptr) {}

	FunctionHook::FunctionHook(const FunctionHook& functionHook) : m_targetFunction(functionHook.m_targetFunction), m_hookedFunction(functionHook.m_hookedFunction), m_overrideFunction(functionHook.m_overrideFunction) {}

	FunctionHook::~FunctionHook() { Detatch(); }

	bool FunctionHook::IsAttached() const
	{
		return (GetTarget() && GetHooked());
	}

	uintptr_t* FunctionHook::GetTarget() const
	{
		return m_targetFunction;
	}

	uintptr_t* FunctionHook::GetHooked() const
	{
		return m_hookedFunction;
	}

	void* FunctionHook::GetOverride() const
	{
		return m_overrideFunction;
	}

	bool FunctionHook::Attach(uintptr_t* targetFunction, void* overrideFunction)
	{
		if (!IsAttached() && targetFunction && overrideFunction)
		{
			m_targetFunction = targetFunction;
			m_overrideFunction = overrideFunction;
			m_hookedFunction = nullptr;

			MinHook::MH_STATUS detourResult = MinHook::MH_CreateHook(GetTarget(), GetOverride(), reinterpret_cast<LPVOID*>(&m_hookedFunction));
			std::cout << MinHook::MH_StatusToString(detourResult);
			return (detourResult == MinHook::MH_STATUS::MH_OK);
		}

		return false;
	}

	void FunctionHook::Detatch()
	{
		if (IsAttached() && GetOverride())
		{
			Disable();
			MinHook::MH_RemoveHook(GetTarget());
			m_hookedFunction = nullptr;
		}
	}

	void FunctionHook::Enable()
	{
		if (IsAttached())
		{
			MinHook::MH_EnableHook(GetTarget());
		}
	}

	void FunctionHook::Disable()
	{
		if (IsAttached())
		{
			MinHook::MH_DisableHook(GetTarget());
		}
	}
}