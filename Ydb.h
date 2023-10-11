#pragma once
#include <vector>
#include <fstream>
#include <stdexcept>
#include <string.h>
#include "json.hpp"
namespace ydb
{
	static struct DataTags
	{
		static const std::string& key;
		static const std::string& data;
		static const std::string& inner;
		static const std::string& count;
	};
	class Document
	{
	public:
		enum Flags : std::uint8_t
		{
			KEY,
			DATA,
			INNER,
			FLAG_COUNT
		};
		Document() = default;
		Document(const std::string& key, const std::string& jsonStr);
		~Document();
		void Create(const std::string& key, const std::string& jsonStr);
		ydb::Document& Get(const std::string& key);
		ydb::Document& Get(const std::size_t& index);
		std::vector<ydb::Document>& Docs();
		bool Update(const std::string& jsonStr);
		bool Update(const std::string& key, const std::string& jsonStr);
		void Delete(const std::string& key);
		std::string& Read(std::uint8_t flag);
	protected:
		std::string _key;
		std::string _data;
		std::vector<ydb::Document> _inner;
		std::size_t _count{ _inner.size() };
		void RecursiveJsonizer(const std::vector<ydb::Document>& docs, nlohmann::json& json, std::size_t& count);
		void RecursiveGetDbFileData(const std::vector<ydb::Document>& docs, nlohmann::json& json, std::size_t& count);
	};
	class Database : public ydb::Document
	{
	public:
		Database(const std::string& path, uint8_t indentation = 4);
		~Database();
		const std::string& Path();
		const std::string& Extension();
		const std::string& Name();
		const std::size_t& Count();
		bool Clear();
		void Save();
	private:
		std::uint8_t _indentation;
		std::string _path;
		std::string _extension;
		nlohmann::json _json;
		std::ifstream _ifstream;
		std::ofstream _ofstream;
	};
}