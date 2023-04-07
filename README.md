# TanmiClock
## 2023.4.7

一个自建引擎的简单时钟库，基于C++20 & VS2022

### 用法

```c++
	// 获得时钟实例引用，默认自带一个时钟对象"Golbal"
	Clock& clk = Clock::Instance();

	// 新建时钟test
	clk.NewClock("test", 60);

	// 获取时钟test的刷新率
	int fps = clk.GetFramePerSecond("test");
	// 设置时钟test的刷新率
	clk.SetFramePerSecond("test", 30);

	// 获得时钟test是否暂停
	bool isPause = clk.GetPause("test");
	// 设置时钟test暂停
	clk.SetPause("test", true);
	// 取消时钟test暂停
	clk.SetPause("test", false);

	// 获取时钟test是否更新
	bool isUpdate = clk.GetUpdate("test");
	// 获取时钟test是否更新
	bool isUpdate = clk.GetUpdate("test");

	// 获取时钟test自创建以来经过的绝对时长
	int elapsed = clk.GetElapsed("test");
	// 获取时钟test自上一刷新节点经过的绝对时长
	double tick = clk.GetTick("test");

	// 获取时钟test的缩放率
	double scale = clk.GetFrameScale("test");
	// 设置时钟test的缩放率
	clk.SetFrameScale("test", 0.5);

	// 获取时钟test自创建以来经过的相对时长
	int elapsedRelative = clk.GetElapsedRelative("test");
	// 获取时钟test自上一刷新节点经过的相对时长
	double tickRelative = clk.GetTickRelative("test");

	// 重置时钟test计时器
	clk.ResetClockIns("test");

	// 移除时钟test
	clk.EraseClock("test");
```


### 结构

```c++
class Clock
	{
	...
	private:
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
		void DEBUG(std::string);
		...
	};
```