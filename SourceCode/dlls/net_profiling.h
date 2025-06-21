// net_profiling.h
#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <cstdio>
#include <enginecallback.h>

namespace NetProfiling
{
	struct MsgInfo
	{
		int count = 0;
		std::string file;
		int line = 0;
	};

	class Profiler
	{
	private:
		std::unordered_map<std::string, MsgInfo> msgMap;
		std::mutex mtx;
	public:
		void Submit(const char* file, int line, const char* msgType)
		{
			std::lock_guard<std::mutex> lock(mtx);
			auto& info = msgMap[msgType];
			info.count++;
			if (info.file.empty())
			{
				info.file = file;
				info.line = line;
			}
		}

		void DumpStats()
		{
			printf("Net Profiling Stats:\n");
			for (const auto& pair : msgMap)
			{
				ALERT(at_console, "MsgType: %d, Count: %d\n", pair.first, pair.second.count);
			}
			ALERT(at_console, "[NetProfiling] End of stats.\n");
		}

		void Clear()
					{
			std::lock_guard<std::mutex> lock(mtx);
			msgMap.clear();
		}
	};

	static Profiler& GetProfiler()
	{
		static Profiler instance;
		return instance;
	}

	inline void Submit(const char* file, int line, const char* msgType)
	{
		GetProfiler().Submit(file, line, msgType);
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