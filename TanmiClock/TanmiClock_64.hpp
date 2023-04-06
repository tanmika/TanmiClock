//
//	Tanmika --2023.4.2
//
#pragma once
#include <iostream>
#include <Windows.h>
#include <time.h>
#include <string>
#include <unordered_map>
#include <mutex>
#include <algorithm>
#include <exception>

// 使用单例模式与线程安全 支持拓展
namespace TanmiClock
{
	using ull = unsigned long long;
	using lint = LARGE_INTEGER;

	// exception
	class ClockException : public std::exception
	{
	public:
		virtual const char* what () const throw ()
		{
			return "::Expection Clock Exception basic";
		}
	};
	class ClockOutOfRangeException : public ClockException
	{
	public:
		virtual const char* what () const throw ()
		{
			return "::Expection out_of_range";
		}
	};
	class ClockNoFoundException : public ClockException
	{
	public:
		virtual const char* what () const throw ()
		{
			return "::Expection clock_no_found";
		}
	};
	class ClockNameExist :public ClockException
	{
	public:
		virtual const char* what () const throw ()
		{
			return "::Expection name_exist";
		}
	};

	// element
	class ClockElem
	{
	public:
		~ClockElem() = default;
		ClockElem() = delete;
		ClockElem(std::string _name, ull _cycle, ull _update, float _scale, bool _pause) :
			name(_name), cycle(_cycle), last_cycle(_cycle), update_tick(_update), ins_cycle(_cycle),
			pause_cycle(0), relative_tick(0), scale(_scale), pause(_pause), temp_ull(0), temp_lint({})
		{}
		// cycle -> 绝对计数 ,tick -> 相对计数
		std::string name;		// 名字
		ull cycle;				// 上次唤起更新计数点
		ull last_cycle;			// 上次有效更新帧计数点
		ull update_tick;		// 更新周期
		ull ins_cycle;			// 初始时计数点
		ull pause_cycle;		// 暂停时刻计数点
		ull relative_tick;		// 相对经过的总计数
		float scale;			// 放缩率
		bool pause;				// 是否暂停
		ull temp_ull;			// 缓冲
		lint temp_lint;			// 缓冲
		std::mutex lock;		// 数据锁
		std::mutex templock;	// 缓冲锁
		//----------setting----------
		inline void setNameStr(const std::string _str)
		{
			std::lock_guard<std::mutex> elemguard(lock);
			name = _str;
		}
		inline void setFrameRateUll(const ull _rate)
		{
			std::lock_guard<std::mutex> elemguard(lock);
			update_tick = _rate;
		}
		inline void setScaleFloat(const float _scale)
		{
			std::lock_guard<std::mutex> elemguard(lock);
			scale = _scale;
		}
	};

	// singleton class
	class Clock
	{
	private:
		std::unordered_map<std::string, std::shared_ptr<ClockElem>> clockMap;
		std::mutex lock_clk;
		ull temp_ull_clk;
		lint temp_lint_clk;
	private:
		const int MAX_FRAME_RATE_PER_SECOND = 1000;
		const int MIN_FRAME_RATE_PER_SECOND = 0.001;
		const int MAX_SCALE = 1000;
		const int MIN_SCALE = 0.001;
	private:
		Clock();
		//----------tool----------
		// 获取时钟对象指针，失败时返回nullptr
		inline std::shared_ptr<ClockElem> getIterator(std::string);
		// 获取ull时钟计数与频率，使用类内缓冲
		inline std::pair<ull, ull> getCycleAndFreqIns();
		// 获取ull时钟计数，使用元素缓冲
		inline ull& getCycleNow(std::string);
		// 获取ull时钟计数，使用元素缓冲
		inline ull& getCycleNow(std::shared_ptr<ClockElem>);
		// 获取ull时钟频率，使用元素缓冲
		inline ull& getFreqNow(std::string);
		// 获取ull时钟频率，使用元素缓冲
		inline ull& getFreqNow(std::shared_ptr<ClockElem>);
		//----------function----------
		// 获取时钟是否超过更新点，是则更新时钟
		inline bool isUpdate(std::string);
		// 获取时钟是否超过更新点，是则更新时钟
		inline bool isUpdate(std::shared_ptr<ClockElem>);

	public:
		// 获取Clock实例引用
		static Clock& Instance();
		//----------elemFunction----------
		// 新建时钟
		bool NewClock(std::string, double);
		// 移除时钟
		bool EraseClock(std::string);
		//----------getFunction----------
		// 获取时钟是否超过更新点，是则更新时钟
		bool GetUpdate(std::string);
		// 获取时钟刷新率
		int GetFramePerSecond(std::string);
		// 获取时钟自创建以来经过的绝对时长
		int GetElapsed(std::string);
		// 获取时钟自上一刷新节点经过的绝对时长
		double GetTick(std::string);
		// 获取时钟自创建以来经过的相对时长
		int GetElapsedRelative(std::string);
		// 获取时钟自上一刷新节点经过的相对时长
		double GetTickRelative(std::string);
		//----------setFunction----------
		// 设置时钟状态暂停或继续
		void SetPause(std::string, bool);
		// 设置刷新率
		void SetFramePerSecond(std::string, double);
		// 设置时间缩放
		void SetFrameScale(std::string, double);
		// 重置时钟计时器
		void ResetClockIns(std::string);
		//-----------tool-----------
		// usingned long long计数器与ms计数器转换（Debug）
		inline int ull2ms_freq(ull time_ull)
		{
			auto freq = getCycleAndFreqIns().second;
			return (time_ull * 1000 / freq);
		}
#ifdef CLOCK_EVENT

#endif // CLOCK_EVENT
		void DEBUG(std::string);
		~Clock() = default;
	};

	Clock& Clock::Instance()
	{
		static Clock clk;
		return clk;
	}

	inline bool Clock::NewClock(std::string str, double FPS = 60.0f)
	{
		try
		{
			if (clockMap.contains(str))
				throw ClockNameExist();
			if (FPS<0 || FPS>MAX_FRAME_RATE_PER_SECOND)
				throw ClockOutOfRangeException();
			auto pair = getCycleAndFreqIns();
			auto clkelem = std::make_shared<ClockElem>
				(str, pair.first, 1.0f / FPS * pair.second, 1.0f, false);
			clockMap[str] = clkelem;
		}
		catch (ClockException& e)
		{
			std::cout << "\n::Clock::NewClock()" << e.what() << std::endl;
			return false;
		}
		return true;
	}

	inline bool Clock::EraseClock(std::string str)
	{
		try
		{
			auto c = clockMap.begin();
			for (c; c != clockMap.end(); c++)
			{
				if (c->first == str)
				{
					clockMap.erase(c);
					return true;
				}
			}
			if (c == clockMap.end())
				throw ClockNoFoundException();
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::EraseClock()" << exp.what() << std::endl;
		}
		return false;
	}

	inline bool Clock::GetUpdate(std::string str = "Golbal")
	{
		return this->isUpdate(str);
	}

	inline int Clock::GetFramePerSecond(std::string str = "Golbal")
	{
		auto e = this->getIterator(str);
		try
		{
			if (e.get() == nullptr)
				throw ClockNoFoundException();
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::GetFramePerSecond()" << exp.what() << std::endl;
		}
		return static_cast<int>(this->getFreqNow(e) / e->update_tick);
	}

	inline int Clock::GetElapsed(std::string str = "Golbal")
	{
		auto e = this->getIterator(str);
		try
		{
			if (e.get() == nullptr)
				throw ClockNoFoundException();
			unsigned long long elapsed_cycles = this->getCycleNow(e) - e->ins_cycle;
			unsigned long long freq = getFreqNow(e);
			return (elapsed_cycles * 1000) / freq;
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::GetElapsed()" << exp.what() << std::endl;
		}
		return 0;
	}

	inline int Clock::GetElapsedRelative(std::string str = "Golbal")
	{
		auto e = this->getIterator(str);
		try
		{
			if (e.get() == nullptr)
				throw ClockNoFoundException();
			unsigned long long elapsed_cycles =
				e->relative_tick + (this->getCycleNow(e) - e->cycle) * e->scale;
			unsigned long long freq = getFreqNow(e);
			return (elapsed_cycles * 1000) / freq;
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::GetElapsedRelative()" << exp.what() << std::endl;
		}
		return 0;
	}

	inline double Clock::GetTick(std::string str = "Golbal")
	{
		auto e = this->getIterator(str);
		try
		{
			if (e.get() == nullptr)
				throw ClockNoFoundException();
			unsigned long long elapsed_cycles = e->cycle - e->last_cycle;
			unsigned long long freq = getFreqNow(e);
			return (elapsed_cycles * 1000) / (double)freq;
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::GetTick()" << exp.what() << std::endl;
		}
		return 0.0f;
	}

	inline double Clock::GetTickRelative(std::string str = "Golbal")
	{
		auto e = this->getIterator(str);
		try
		{
			if (e.get() == nullptr)
				throw ClockNoFoundException();
			unsigned long long elapsed_cycles = e->cycle - e->last_cycle;
			unsigned long long freq = getFreqNow(e);
			return (elapsed_cycles * 1000) / (double)freq * e->scale;
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::GetTick()" << exp.what() << std::endl;
		}
		return 0.0f;
	}

	inline void Clock::SetPause(std::string str, bool is_pause)
	{
		auto e = this->getIterator(str);
		try
		{
			if (e.get() == nullptr)
				throw ClockNoFoundException();
			if (is_pause == e->pause)
			{
				return;
			}
			else if (e->pause == false)
			{
				std::lock_guard<std::mutex> lock(e->lock);
				e->pause_cycle = this->getCycleNow(e);
				e->pause = true;
				return;
			}
			else
			{
				std::lock_guard<std::mutex> lock(e->lock);
				e->pause = false;
				e->cycle += this->getCycleNow(e) - e->pause_cycle;
				return;
			}
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::SetPause()" << exp.what() << std::endl;
		}
	}

	inline void Clock::SetFramePerSecond(std::string str, double i)
	{
		auto e = this->getIterator(str);
		try
		{
			if (e.get() == nullptr)
				throw ClockNoFoundException();
			else if (i<MIN_FRAME_RATE_PER_SECOND || i>MAX_FRAME_RATE_PER_SECOND)
				throw ClockOutOfRangeException();
			std::lock_guard<std::mutex> lock(e->lock);
			e->update_tick = 1.0f / i * this->getFreqNow(e);
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::SetFramePerSecond()" << exp.what() << std::endl;
		}
	}

	inline void Clock::SetFrameScale(std::string str, double s)
	{
		auto e = this->getIterator(str);
		try
		{
			if (e.get() == nullptr)
				throw ClockNoFoundException();
			else if (s < MIN_SCALE || s > MAX_SCALE)
				throw ClockOutOfRangeException();
			std::lock_guard<std::mutex> lock(e->lock);
			e->scale = s;
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::SetFrameScale()" << exp.what() << std::endl;
		}
	}

	inline void Clock::ResetClockIns(std::string str)
	{
		auto e = this->getIterator(str);
		try
		{
			if (e.get() == nullptr)
				throw ClockNoFoundException();
			std::lock_guard<std::mutex> lock(e->lock);
			e->ins_cycle = this->getCycleNow(e);
			e->relative_tick = 0;
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::SetFrameScale()" << exp.what() << std::endl;
		}
	}

	inline void Clock::DEBUG(std::string str = "Golbal")
	{
		auto e = this->getIterator(str);
		//auto pair = getCycleAndFreqIns();
		//std::cout << pair.first << " " << pair.second << std::endl;
		std::cout << "cycle:" << (this->getCycleNow(e) - e->ins_cycle) * 1000 << std::endl;
		std::cout << "freq:" << getFreqNow(e) << std::endl;

		unsigned long long elapsed_cycles = this->getCycleNow(e) - e->last_cycle;
		unsigned long long freq = getFreqNow(e);
		std::cout << "tick" << (elapsed_cycles * 1000) / (double)freq << "\n\n";
	}

	Clock::Clock()
	{
		std::string str("Golbal");
		auto pair = getCycleAndFreqIns();
		std::shared_ptr<ClockElem> clk = std::make_shared<ClockElem>
			(str, pair.first, 1.0f / 60.0f * pair.second, 1.0f, false);
		clockMap[str] = clk;
	}

	inline std::shared_ptr<ClockElem> Clock::getIterator(std::string str)
	{
		auto search = clockMap.find(str);
		if (search != clockMap.end())
		{
			return search->second;
		}
		return nullptr;
	}

	inline std::pair<ull, ull> Clock::getCycleAndFreqIns()
	{
		std::lock_guard<std::mutex> lock(lock_clk);
		QueryPerformanceCounter(&temp_lint_clk);
		temp_ull_clk = temp_lint_clk.QuadPart;
		QueryPerformanceFrequency(&temp_lint_clk);
		return std::pair<ull, ull>(temp_ull_clk, temp_lint_clk.QuadPart);
	}

	inline ull& Clock::getCycleNow(std::string str)
	{
		std::shared_ptr<ClockElem> i(nullptr);
		try
		{
			i = getIterator(str);
			if (i.get() == nullptr)
				throw ClockNoFoundException();
			std::lock_guard<std::mutex> lock(i->templock);
			QueryPerformanceCounter(&i->temp_lint);
			i->temp_ull = i->temp_lint.QuadPart;
			return i->temp_ull;
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::getCycleNow()" << exp.what() << std::endl;
		}
		ull void_ull = 0;
		return void_ull;
	}

	inline ull& Clock::getCycleNow(std::shared_ptr<ClockElem> i)
	{
		try
		{
			if (i.get() == nullptr)
				throw ClockNoFoundException();
			std::lock_guard<std::mutex> lock(i->templock);
			QueryPerformanceCounter(&i->temp_lint);
			i->temp_ull = i->temp_lint.QuadPart;
			return i->temp_ull;
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::getCycleNow()" << exp.what() << std::endl;
		}
		ull void_ull = 0;
		return void_ull;
	}

	inline ull& Clock::getFreqNow(std::string str)
	{
		std::shared_ptr<ClockElem> i(nullptr);
		try
		{
			i = getIterator(str);
			if (i.get() == nullptr)
				throw ClockNoFoundException();

			std::lock_guard<std::mutex> lock(i->templock);
			QueryPerformanceFrequency(&i->temp_lint);
			i->temp_ull = i->temp_lint.QuadPart;

			return i->temp_ull;
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::getFreqNow()" << exp.what() << std::endl;
		}
		ull void_ull = 0;
		return void_ull;
	}

	inline ull& Clock::getFreqNow(std::shared_ptr<ClockElem> i)
	{
		try
		{
			if (i.get() == nullptr)
				throw ClockNoFoundException();

			std::lock_guard<std::mutex> lock(i->templock);
			QueryPerformanceFrequency(&i->temp_lint);
			i->temp_ull = i->temp_lint.QuadPart;

			return i->temp_ull;
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::getFreqNow()" << exp.what() << std::endl;
		}
		ull void_ull = 0;
		return void_ull;
	}

	inline bool Clock::isUpdate(std::string str)
	{
		std::shared_ptr<ClockElem> i(nullptr);
		try
		{
			i = getIterator(str);
			if (i.get() == nullptr)
				throw ClockNoFoundException();

			if (i->pause == true)
			{
				return false;
			}
			std::lock_guard<std::mutex> lock(i->templock);
			QueryPerformanceCounter(&i->temp_lint);
			if (i->temp_lint.QuadPart - i->cycle > i->update_tick)
			{
				i->last_cycle = i->cycle;
				i->cycle = i->temp_lint.QuadPart;
				i->relative_tick += (i->cycle - i->last_cycle) * i->scale;
				return true;
			}
			else
			{
				return false;
			}
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::isUpdate()" << exp.what() << std::endl;
		}
		return false;
	}

	inline bool Clock::isUpdate(std::shared_ptr<ClockElem> i)
	{
		try
		{
			if (i.get() == nullptr)
				throw ClockNoFoundException();

			if (i->pause == true)
			{
				return false;
			}
			std::lock_guard<std::mutex> lock(i->templock);
			QueryPerformanceCounter(&i->temp_lint);
			if (i->temp_lint.QuadPart - i->cycle > i->update_tick)
			{
				i->last_cycle = i->cycle;
				i->cycle = i->temp_lint.QuadPart;
				i->relative_tick += (i->cycle - i->last_cycle) * i->scale;
				return true;
			}
			else
			{
				return false;
			}
		}
		catch (ClockException& exp)
		{
			std::cout << "\n::Clock::isUpdate()" << exp.what() << std::endl;
		}
		return false;
	}
}