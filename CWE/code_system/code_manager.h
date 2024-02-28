#pragma once
#include "cwe_code.h"
#include <vector>
#include <memory>
#include <IniFile.h>
#include <iostream>

class CodeManager {
private:
	const IniFile* m_config;
	std::vector <std::unique_ptr<Code>> m_vector;

	static CodeManager& GetInstance(const IniFile* ini = nullptr)
	{
		static CodeManager _instance(ini);
		return _instance;
	}

public:
	CodeManager(const IniFile* ini) : m_config(ini) {
		std::cout << "CodeManager constructed" << std::endl;
	}

	auto begin() {
		return m_vector.begin();
	}

	auto end() {
		return m_vector.end();
	}

	template <typename T>
	void AddCode(int param) {
		
		std::unique_ptr<Code> _code = std::make_unique<T>(param);
		_code->Init();
		m_vector.push_back(std::move(_code));
	}

	bool GetBool(const char* name, bool def) {
		return m_config->getBool(name, def);
	}

	int GetInt(const char* name, bool def) {
		int retval = 0;
		try {
			retval = m_config->getInt(name, def);
		}
		catch ( ... ) {
			//TODO: fix this to use typeid.name or something
			MessageBoxA(0, "Please reconfigure Monster Evolution in the Mod Manager. Until then, Monster Evolution will remain disabled.", "Chao World Extended", 0);
		}
		return retval;
	}

	//i didnt make the config type a template argument because it only supports bool and int
	template <typename T>
	void Add(const char* name, bool def) {
		bool b = m_config->getBool(name, def);
		if (!b) return; //exit/invalid value for bool is always false

		AddCode<T>(0);
	}

	template <typename T>
	void Add(const char* section, const char* name, bool def) {
		bool b = m_config->getBool(section, name, def);
		if (!b) return; //exit/invalid value for bool is always false

		AddCode<T>(0);
	}

	template <typename T>
	void Add(const char* name, int def, int invalid) {
		int i = m_config->getInt(name, def);
		if (i == invalid) return;

		AddCode<T>(i);
	}

	static CodeManager& Instance()
	{
		return GetInstance();
	}

	static CodeManager& Init(const IniFile* _ini) {
		return GetInstance(_ini);
	}
};