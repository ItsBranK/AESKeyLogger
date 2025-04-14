#pragma once
#include "../../pch.h"

namespace AESLogger
{
	class FunctionHook
	{
	private:
		uintptr_t* m_targetFunction; // Address of the actual function we want to detour.
		uintptr_t* m_hookedFunction; // The callback for the original function created AFTER hooking, to call from the override function.
		void* m_overrideFunction;

	public:
		FunctionHook();
		FunctionHook(const FunctionHook& functionHook);
		~FunctionHook();

	public:
		bool IsAttached() const;
		uintptr_t* GetTarget() const;
		uintptr_t* GetHooked() const;
		void* GetOverride() const;

	public:
		bool Attach(uintptr_t* targetFunction, void* overrideFunction);
		template <typename T> T Call()
		{
			return reinterpret_cast<T>(GetHooked());
		}
		void Detatch();
		void Enable();
		void Disable();

	public:
		FunctionHook& operator=(const FunctionHook& functionHook);
	};
}