#pragma once

u32 GenerateStringHash(const string_char*);

struct Name
{
	Name();
	Name(const Name& name);
	Name(const string_char* name);
	Name(const std_string& name);
	Name& operator= (const Name& in_other);

	bool operator< (const Name& in_other) const;
	bool operator== (const Name& in_other) const;

	operator const string_char*() const { return text.c_str(); }
	operator const std_string&() const { return text; }

private:
	std_string text;
	unsigned int hash = 0;
};
