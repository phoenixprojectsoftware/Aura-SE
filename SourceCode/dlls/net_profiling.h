// net_profiling.h
#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <cstdio>
#include "enginecallback.h"

namespace NetProfiling
{
	struct MsgInfo
	{
		int count = 0;
	};

	class Profiler
	{
	private:
		std::unordered_map<int, MsgInfo> msgMap;
		std::unordered_map<int, std::string> msgNames;
		std::mutex mtx;

	public:
		// Register a human-readable name for a message type
		void RegisterName(int msgType, const char* name)
		{
			std::lock_guard<std::mutex> lock(mtx);
			msgNames[msgType] = name;
		}

		// Increment count for a message type
		void Submit(int msgType)
		{
			std::lock_guard<std::mutex> lock(mtx);
			msgMap[msgType].count++;
		}

		// Dump all stats to console
		void DumpStats()
		{
			ALERT(at_console, "----- Net Profiling Stats -----\n");
			for (const auto& pair : msgMap)
			{
				auto it = msgNames.find(pair.first);
				const char* name = it != msgNames.end() ? it->second.c_str() : "UNKNOWN";

				ALERT(at_console, "MsgType: %d (%s), Count: %d\n",
					pair.first, name, pair.second.count);
			}
			ALERT(at_console, "--------------------------------\n");
		}

		void Clear()
		{
			std::lock_guard<std::mutex> lock(mtx);
			msgMap.clear();
		}
	};

	// Global access functions
	inline Profiler& GetProfiler()
	{
		static Profiler instance;
		return instance;
	}

	inline void Submit(int msgType)
	{
		GetProfiler().Submit(msgType);
	}

	inline void RegisterName(int msgType, const char* name)
	{
		GetProfiler().RegisterName(msgType, name);
	}

	inline void DumpStats()
	{
		GetProfiler().DumpStats();
	}

	inline void Clear()
	{
		GetProfiler().Clear();
	}
}
