#pragma once

class Function_Prototype_Base {
public:
	//Required for dynamic_casting
	virtual ~Function_Prototype_Base(void) = default;
};

template <typename T, typename U>
class Function_Prototype_With_Object;

template <typename Type, typename ReturnType, typename... Params>
class Function_Prototype_With_Object<Type, ReturnType(Params...)> : public Function_Prototype_Base
{
public:
	Function_Prototype_With_Object(const std::function<ReturnType(Type*, Params...)>& f_prototype)
		: m_fct(f_prototype) {}

	ReturnType operator()(Type* obj, Params... params)
	{
		return m_fct(obj, params...);
	}

private:
	std::function<ReturnType(Type*, Params...)> m_fct;
};

template <typename T>
class Function_Prototype_Without_Object;

template <typename ReturnType, typename... Params>
class Function_Prototype_Without_Object<ReturnType(Params...)> : public Function_Prototype_Base
{
public:
	Function_Prototype_Without_Object(const std::function<ReturnType(Params...)>& f_prototype)
		: m_fct(f_prototype) {}

	ReturnType operator()(Params... params)
	{
		return m_fct(params...);
	}

private:
	std::function<ReturnType(Params...)> m_fct;
};
