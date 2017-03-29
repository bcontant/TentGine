#pragma once

struct IContainer
{
	virtual size_t GetCount(void* container) = 0;
	virtual void* GetValue(void* container, int index) = 0;
};

template <typename TYPE>
struct VectorContainer : public IContainer
{
	size_t GetCount(void* container);
	void* GetValue(void* container, int index);
};


//-----------------------------------------
//VectorContainer
//-----------------------------------------

template<typename TYPE>
size_t VectorContainer<TYPE>::GetCount(void* container)
{
	return ((std::vector<TYPE>*)container)->size();
}

template<typename TYPE>
void* VectorContainer<TYPE>::GetValue(void* container, int index)
{
	return &((std::vector<TYPE>*)container)->at(index);
}


