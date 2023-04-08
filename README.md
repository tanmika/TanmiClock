# TanmiClock
##
### 2023.4.9 update

TanmiClock是一个基于单例模式的简单时钟系统，使用mutex保证线程安全。

通过本系统，用户可以创建时钟实例并管理其设置，例如复制、获取和设置其帧率、暂停和取消暂停时钟、获取其经过的时间以及重置时钟。

在包含事件系统库[TanmiEventSystem](https://github.com/tanmika/TanmiEventSystem)后，还可以实现通过时钟唤起事件更新功能。

## 依赖

src文件夹下包含：
- TanmiClock_64.hpp
时钟系统

demo文件夹下包含：
- TanmiClock.sln
演示项目

将TanmiClock_64.hpp包含至项目中即可使用，调用时需使用命名空间TanmiEngine

注意，在使用该项目时需要支持 C++20 特性的编译器。

## 用法

```c++
	// 获得时钟实例引用，默认自带一个时钟对象"Global"
	Clock& clk = Clock::Instance();

	// 新建时钟test
	clk.NewClock("test", 60);

	// 以test时钟为蓝本复制时钟clk
	clk.CopyClock("test", "clk");

	// 获取时钟test的刷新率
	double fps = clk.GetFramePerSecond("test");
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
	double elapsed = clk.GetElapsed("test");
	// 获取时钟test自上一刷新节点经过的绝对时长
	double tick = clk.GetTick("test");

	// 获取时钟test的缩放率
	double scale = clk.GetFrameScale("test");
	// 设置时钟test的缩放率
	clk.SetFrameScale("test", 0.5);

	// 获取时钟test自创建以来经过的相对时长
	double elapsedRelative = clk.GetElapsedRelative("test");
	// 获取时钟test自上一刷新节点经过的相对时长
	double tickRelative = clk.GetTickRelative("test");

	// 重置时钟test
	clk.ResetClockIns("test");

	// 移除时钟test
	clk.EraseClock("test");

	// 以下内容需包含事件系统以启用
		// 添加事件test_event至时钟clk
		clk.AddEvent("clk", "test_event");
		// 移除clk中事件test_event
		clk.RemoveEvent("clk", "test_event");
		// 获取clk中的所有事件
		std::vector<std::string> eventList = clk.GetEventList("clk");
		// 移除clk中所有事件
		clk.ClearEventList("clk");
```


## 结构

```c++
class Clock
	{
	...
	private:
		//----------tool----------
		// 获取时钟对象指针，失败时返回nullptr
		inline std::shared_ptr<ClockElem> getIterator(const std::string&);
		// 获取ull时钟计数与频率，使用类内缓冲
		inline std::pair<ull, ull> getCycleAndFreqIns();
		// 获取ull时钟计数，使用元素缓冲
		inline ull& getCycleNow(const std::string&);
		// 获取ull时钟计数，使用元素缓冲
		inline ull& getCycleNow(std::shared_ptr<ClockElem>);
		// 获取ull时钟频率，使用元素缓冲
		inline ull& getFreqNow(const std::string&);
		// 获取ull时钟频率，使用元素缓冲
		inline ull& getFreqNow(std::shared_ptr<ClockElem>);
		//----------function----------
		// 获取时钟是否超过更新点，是则更新时钟
		inline bool isUpdate(const std::string&);
		// 获取时钟是否超过更新点，是则更新时钟
		inline bool isUpdate(std::shared_ptr<ClockElem>);

	public:
		// 获取Clock实例引用
		static Clock& Instance();
		//----------elemFunction----------
		// 新建时钟，参数为时钟名称和刷新率
		bool NewClock(const std::string&, double);
		// 移除时钟，参数为时钟名称
		bool EraseClock(const std::string&);
		// 复制时钟，参数为时钟名称和新时钟名称
		bool CopyClock(const std::string&, const std::string&);
		//----------getFunction----------
		// 获取时钟是否超过更新点，是则更新时钟，参数为时钟名称
		bool GetUpdate(const std::string&);
		// 获取时钟是否暂停，参数为时钟名称
		bool GetPause(const std::string&);
		// 获取时钟刷新率，参数为时钟名称
		double GetFramePerSecond(const std::string&);
		// 获取时钟自创建以来经过的绝对时长，参数为时钟名称
		double GetElapsed(const std::string&);
		// 获取时钟自上一刷新节点经过的绝对时长，参数为时钟名称
		double GetTick(const std::string&);
		// 获取时钟自创建以来经过的相对时长，参数为时钟名称
		double GetElapsedRelative(const std::string&);
		// 获取时钟自上一刷新节点经过的相对时长，参数为时钟名称
		double GetTickRelative(const std::string&);
		//----------setFunction----------
		// 设置时钟状态暂停或继续，参数为时钟名称和布尔值，true表示暂停，false表示继续
		void SetPause(const std::string&, bool);
		// 设置刷新率，参数为时钟名称和刷新率
		void SetFramePerSecond(const std::string&, double);
		// 设置时间缩放，参数为时钟名称和缩放值
		void SetFrameScale(const std::string&, double);
		// 重置时钟计时器，参数为时钟名称
		void ResetClockIns(const std::string&);
		//-----------tool-----------
		// usingned long long计数器与ms计数器转换（Debug）
		inline int ull2ms_freq(ull time_ull)；

		// 以下内容需包含事件系统以启用
			// 添加事件至时钟
			void AddEvent(const std::string&, const std::string&);
			// 移除事件
			void RemoveEvent(const std::string&, const std::string&);
			// 获取事件列表
			std::vector<std::string>& GetEventList(const std::string&);
			// 清空事件列表
			void ClearEventList(const std::string&);

		void DEBUG(const std::string&);
		...
	};
```