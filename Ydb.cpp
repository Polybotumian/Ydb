#include "Ydb.h"
const std::string& ydb::DataTags::key = "key";
const std::string& ydb::DataTags::data = "data";
const std::string& ydb::DataTags::inner = "documents";
const std::string& ydb::DataTags::count = "count";

ydb::Document::Document(const std::string& key, const std::string& jsonStr)
{
	_key = key;
	_data = jsonStr;
};
ydb::Document::~Document()
{

};
void ydb::Document::Create(const std::string& key, const std::string& jsonStr)
{
	if (_inner.size() > 0)
	{
		for (ydb::Document& doc : _inner)
		{
			if (doc._key == key)
			{
				throw std::runtime_error("ydb::Document::Create > " + key + " : Duplication!");
			}
		}
		_inner.push_back(Document(key, jsonStr));
	}
	else
	{
		_inner.push_back(Document(key, jsonStr));
	}
};
ydb::Document& ydb::Document::Get(const std::string& key)
{
	for (ydb::Document& doc : _inner)
	{
		if (doc._key == key)
		{
			return doc;
		}
	}
	throw std::runtime_error("ydb::Document::Get > " + key + " : Not found!");
};
ydb::Document& ydb::Document::Get(const std::size_t& index)
{
	if (_inner.size() > index && index >= 0 )
	{
		return _inner[index];
	}
	else
	{
		throw std::runtime_error("ydb::Document::Get > " + std::to_string(index) + " : Invalid index!");
	}
};
std::vector<ydb::Document>& ydb::Document::Docs() { return _inner; };
bool ydb::Document::Update(const std::string& jsonStr)
{
	_data = jsonStr;
	return true;
};
bool ydb::Document::Update(const std::string& key, const std::string& jsonStr)
{
	for (ydb::Document& doc : _inner)
	{
		if (doc._key == key)
		{
			doc._data = jsonStr;
			return true;
		}
	}
	return false;
};
void ydb::Document::Delete(const std::string& key)
{
	auto it = std::remove_if(_inner.begin(), _inner.end(), [&key](const Document& doc) { return doc._key == key; });

	if (it != _inner.end()) {
		_inner.erase(it, _inner.end());
	}
	else
	{
		throw std::runtime_error("ydb::Document::Delete > " + key + " : Not found!");
	}
};
std::string& ydb::Document::Read(std::uint8_t flag)
{
	switch (flag)
	{
	case ydb::Document::Flags::KEY:
		return _key;
	case ydb::Document::Flags::DATA:
		return _data;
	default:
		throw std::runtime_error("ydb::Document::Read > "  + _key + " : No property found!");
		break;
	}
};
void ydb::Document::RecursiveJsonizer(const std::vector<ydb::Document>& docs, nlohmann::json& json, std::size_t& count)
{
	for (const ydb::Document& doc : docs) 
	{
		nlohmann::json docJson;
		docJson[ydb::DataTags::data] = doc._data;
		++count;
		if (!doc._inner.empty()) {
			RecursiveJsonizer(doc._inner, docJson[ydb::DataTags::inner], count);
		}
		json[doc._key] = docJson;
	}
};
void ydb::Document::RecursiveGetDbFileData(const std::vector<ydb::Document>& docs, nlohmann::json& json, std::size_t& count)
{
	for (auto it = json.begin(); it != json.end(); ++it)
	{
		if (it.key() != ydb::DataTags::count)
		{
			const std::string& key = it.key();
			const std::string& data = (*it)[ydb::DataTags::data];
			_inner.push_back(Document(key, data));
			++count;
			if (it->find(ydb::DataTags::inner) != it->end()) {
				_inner.back().RecursiveGetDbFileData(_inner.back()._inner, (*it)[ydb::DataTags::inner], count);
			}
		}
	}
};
ydb::Database::Database(const std::string& path, uint8_t indentation)
{
	_indentation = indentation;
	_path = path;
	_data = "This is the main document which is database.";
	std::size_t seperatorIndex = _path.find_last_of("/\\");
	_key = path.substr(seperatorIndex + 1);
	seperatorIndex = _key.find_last_of('.');
	_extension = _key.substr(seperatorIndex);
	_key = _key.substr(0, seperatorIndex);
	_ifstream.open(_path);
	_ifstream.seekg(0, std::ios::end);
	if (_ifstream.good() && _ifstream.is_open() && _ifstream.tellg() > 0)
	{
		_ifstream.seekg(0, std::ios::beg);
		_ifstream >> _json;
		_ifstream.close();
		RecursiveGetDbFileData(_inner, _json, _count);
	}
	else
	{
		_json = {};
		_ofstream.open(_path, std::ios_base::out | std::ios_base::app);
		_ofstream << _json.dump(4);
		_ofstream.close();
	}
	_json.clear();
	_ifstream.close();
};
ydb::Database::~Database()
{
	_json.clear();
	_ifstream.close();
	_ofstream.close();
};
const std::string& ydb::Database::Path() { return _path; };
const std::string& ydb::Database::Extension() { return _extension; };
const std::string& ydb::Database::Name() { return _key; };
const std::size_t& ydb::Database::Count() { return _count; };
bool ydb::Database::Clear()
{
	_inner.clear();
	if (_inner.empty())
	{
		return true;
	}
	return false;
};
void ydb::Database::Save()
{
	_json.clear();
	_count = 0;
	RecursiveJsonizer(_inner, _json, _count);
	_json[ydb::DataTags::count] = _count;
	_ofstream.open(_path, std::ios_base::out | std::ios_base::trunc);
	_ofstream << _json.dump(_indentation);
	_ofstream.close();
};