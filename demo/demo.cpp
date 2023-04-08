#include "../src/TanmiClock_64.hpp"
#include <vector>

using namespace TanmiEngine;

auto calFPS(std::string&) -> int;
auto calStability(std::vector<double>&) -> double;
auto showDemo(std::string&) -> int;

auto main() -> int
{
	Clock& clk = Clock::Instance();
	clk.SetFramePerSecond("Global", 5);
	std::cout << clk.GetFramePerSecond() << std::endl;

	std::string str1("democlk1");
	clk.NewClock(str1, 10);
	clk.SetFrameScale(str1, 0.1);
	calFPS(str1);
	clk.EraseClock(str1);

	std::string str2("democlk2");
	clk.NewClock(str2);
	clk.SetFramePerSecond(str2, 15);
	showDemo(str2);
	clk.EraseClock(str2);
	return 0;
}
auto calFPS(std::string& str) -> int
{
	Clock& clk = Clock::Instance();
	const float time = 3.0f;
	int count = 0;
	int begin = clk.GetElapsed(str);
	int last = clk.GetElapsed(str);
	bool con = true;
	std::vector<double> vec;
	while (con)
	{
		if (clk.GetUpdate(str))
		{
			std::cout << "clock elapsed:"<<clk.GetElapsed(str) << "ms, tick cost:" << clk.GetTick(str)<<"ms";
			last = clk.GetElapsed(str);
			vec.push_back(1.0f / clk.GetTick(str));
			std::cout << "  clock elapsed:"<<clk.GetElapsedRelative(str) << "ms relative,  tick cost:" << clk.GetTickRelative(str) << "ms relative.\n";
			count++;
		}
		if (clk.GetElapsed(str) - begin >= time * 1000)
			con = false;
	}
	std::cout << "FPS:" << count / time << std::endl;
	std::cout << "Stability:" << calStability(vec) * 100 << "%" << std::endl;
	return 0;
}

auto calStability(std::vector<double>& v) -> double
{
	double mean = 0;
	double sigma = 0;
	for (const auto i : v)
	{
		mean += i;
	}
	mean /= v.size();
	for (const auto i : v)
	{
		sigma += (i - mean) * (i - mean);
	}
	sigma /= v.size();
	return 1 - sigma;
}
auto showDemo(std::string& str) -> int
{
	Clock& clk = Clock::Instance();
	std::string str3("clk3");
	clk.CopyClock(str, str3);
	clk.SetFramePerSecond(str3, 5);
	const float time = 10.0f;
	int count = 0;
	int begin = clk.GetElapsed(str);
	bool con = true;
	int set_pause = 0;
	while (con)
	{
		if (clk.GetUpdate(str))
		{
			if (count == 1)
			{
				std::cout << " *-*\n";
			}
			else
			{
				std::cout << "( * )\n";
			}
			if (count > 3)
			{
				count = 0;
			}
			++count;
		}
		if (clk.GetUpdate(str3))
		{
			std::cout << "-----\n";
		}
		if (clk.GetElapsed(str) - begin >= time * 400 && set_pause == 0)
		{
			std::cout << "PAUSE!\n";
			clk.SetPause(str3, true);
			++set_pause;
		}
		if (clk.GetElapsed(str) - begin >= time * 600 && set_pause == 1)
		{
			std::cout << "CONTINUE!\n";
			clk.SetPause(str3, false);
			++set_pause;
		}
		if (clk.GetElapsed(str) - begin >= time * 1000)
			con = false;
	}
	return 0;
}
