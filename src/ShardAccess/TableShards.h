
#ifndef TABLE_SHARDS_H
#define TABLE_SHARDS_H

#include <iostream>
#include <string>
#include <memory>
#include <set>
#include <map>
#include <fstream>
#include <shared_mutex>
#include <mutex>


class TableShards
{
public:
	TableShards(std::string info_file_path_) : 
		info_file_path(info_file_path_),  dropped(false)
	{}
	~TableShards()
	{
		std::cout << "~TableShards for " << this->table_name << ". dropped is " << this->dropped << std::endl;
	}

	std::string GetTableName() const 
	{
		std::shared_lock<std::shared_mutex> guard(mutex);
		return this->table_name;
	}
	void SetTableName(const std::string & tablename) 
	{
		std::unique_lock<std::shared_mutex> guard(mutex);
		this->table_name = tablename; 
	}
	bool GetDropped() const { return this->dropped; }
	void SetDropped(bool drop) { this->dropped = drop; }

	void AddShardId(std::string shardid) 
	{ 
		std::unique_lock<std::shared_mutex> guard(mutex);
		this->shard_ids.insert(shardid); 
	}
	template <typename InputIt>
	void AddShardIds(InputIt first, InputIt last)
	{
		std::unique_lock<std::shared_mutex> guard(mutex);
		while (first != last)
		{
			this->AddShardId(*first);
			first++;
		}
	}
	bool RemoveShardId(std::string shardid) 
	{ 
		std::unique_lock<std::shared_mutex> guard(mutex);
		return this->shard_ids.erase(shardid) == 1; 
	}
	bool HasShardId(std::string shardid) const 
	{
		std::shared_lock<std::shared_mutex> guard(mutex);
		return this->shard_ids.count(shardid) > 0; 
	}
	size_t ShardCount() const { return this->shard_ids.size(); }

	using iterator = std::set<std::string>::iterator;
	using const_iterator = std::set<std::string>::const_iterator;
	iterator begin() { return this->shard_ids.begin(); }
	const_iterator begin() const { return this->shard_ids.begin(); }
	iterator end() { return this->shard_ids.end(); }
	const_iterator end() const { return this->shard_ids.end(); }

public:
	bool loadFromFile()
	{
		std::unique_lock<std::shared_mutex> guard(mutex);

		std::fstream in(info_file_path, std::fstream::in);
		if(!in.open())
			return false;

		return parseFromString(in.getline());
	}

	bool saveToFile()
	{
		std::unique_lock<std::shared_mutex> guard(mutex);

		std::fstream out(info_file_path, std::fstream::out | std::fstream::trunc);
		if(!in.open())
			return false;

		std::string info = toString();
		out.write(info.c_str(), info.size());

		return true;
	}

	bool removeFile()
	{
		std::unique_lock<std::shared_mutex> guard(mutex);

		Poco::File file(info_file_path);
		file.remove();
	}

protected:
	std::string toString()
	{
		if(0 == shard_ids.size())
			return "";

		std::ostringstream os;
		os << table_name << ":";

		for(size_t pos = 0; shard_ids.size() > pos; ++pos)
		{
			if(shard_ids.size() - 1 != pos)
				os << shard_id << "|";
			else
				os << shard_id;
		}

		return os.str();
	}

	bool parseFromString(const std::string& info)
	{
		if(0 == info.size())
			return false;

		size_t table_pos = info.find(":");
		if(std::string::npos == table_pos)
			return false;

		table_name = info.sub_str(0, table_pos);
		std::string shardsStr = info.sub_str(table_pos + 1);

		while(true)
		{
			std::string shardIDPos =  = shardsStr.find("|");
			if(std::string::npos == table_pos)
				break;

			shard_ids.insert(shardsStr.sub_str(0, shardIDPos));			
			shardsStr = shardsStr.sub_str(shardIDPos + 1);
		}

		if(0 == shard_ids.size())
			return false;

		return true;
	}

private:
	std::string table_name;
	std::set<std::string> shard_ids;
	bool dropped;

	//信息对应的存储文件
	std::string info_file_path;

	std::shared_mutex mutex;
};


class TableShardsManager
{
public:
	TableShardsManager(const std::string& shards_dir_):
		shards_dir(shards_dir_)
	{
		
	}

	virtual ~TableShardsManager()
	{
		for(auto item : tables)
			item->second.saveToFile();
	}

	std::shared_ptr<TableShards> getTableShards(const std::string& table_name_)
	{
		std::shared_lock<std::shared_mutex> guard(mutex);

		auto iter = tables.find(table_name_);
		if(tables.end() == iter)
			return std::shared_ptr<TableShards>(NULL);

		return iter->second;
	}

	std::shared_ptr<TableShards> getTableShardsOrCreate(const std::string& table_name_)
	{
		std::unique_lock<std::shared_mutex> guard(mutex);

		auto iter = tables.find(table_name_);
		if(tables.end() != iter)
		{
			std::shared_ptr<TableShards> tableShards = 
				std::make_shared<TableShards>(shards_dir + "/" + table_name_ + ".shards");
			tables[table_name_] = tableShards;
			return tableShards;
		}

		return iter->second;
	}

	bool remove(const std::string& table_name_)
	{
		std::unique_lock<std::shared_mutex> guard(mutex);
		this->tables.erase(table_name_);
		return true;
	}

public:
	bool loadAllTableInfo()
	{
		Poco::File dir(shards_dir);
		if(!dir.exists() || !dir.isDirectory())
			return false;
		
		std::vector<std::string> files;
		dir.list(files);

		for(auto& file : files)
		{
			if(std::string::npos == file.find(".shards"))
				continue;

			std::shared_ptr<TableShards> tableShards = 
				std::make_shared<TableShards>(shards_dir + "/" + file);

			tables[file] = tableShards;
		}

		return true;
	}


private:
	std::string shards_dir;
	// key为table name
	std::map<std::string, std::shared_ptr<TableShards>> tables;

	std::shared_mutex mutex;
};


#endif // TABLE_SHARDS_H
