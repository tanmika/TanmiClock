#include "../src/TanmiClock_64.hpp"
#include <vector>

using namespace TanmiEngine;

auto calFPS(ClockID) -> int;
auto calStability(std::vector<double>&) -> double;
auto showDemo(ClockID) -> int;

auto main() -> int
{
	Clock& clk = Clock::Instance();
	auto clk0 = clk.NewClock();
	clk.SetFramePerSecond(clk0, 5);
	std::cout << clk.GetFramePerSecond(clk0) << std::endl;

	auto clk1 = clk.NewClock(clk.GetFramePerSecond(clk0)*2);
	clk.SetFrameScale(clk1, 0.1);
	calFPS(clk1);
	clk.EraseClock(clk1);

	auto clk2 = clk.NewClock(15);
	showDemo(clk2);
	clk.EraseClock(clk2);
	return 0;
}
auto calFPS(ClockID id) -> int
{
	Clock& clk = Clock::Instance();
	const float time = 3.0f;
	int count = 0;
	int begin = clk.GetElapsed(id);
	int last = clk.GetElapsed(id);
	bool con = true;
	std::vector<double> vec;
	while (con)
	{
		if (clk.GetUpdate(id))
		{
			std::cout << "clock elapsed:"<<clk.GetElapsed(id) << "ms, tick cost:" << clk.GetTick(id)<<"ms";
			last = clk.GetElapsed(id);
			vec.push_back(1.0f / clk.GetTick(id));
			std::cout << "  clock elapsed:"<<clk.GetElapsedRelative(id) << "ms relative,  tick cost:" << clk.GetTickRelative(id) << "ms relative.\n";
			count++;
		}
		if (clk.GetElapsed(id) - begin >= time * 1000)
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
auto showDemo(ClockID id) -> int
{
	Clock& clk = Clock::Instance();
	auto clk3 = clk.CopyClock(id);
	clk.SetFramePerSecond(clk3, 5);
	const float time = 10.0f;
	int count = 0;
	int begin = clk.GetElapsed(id);
	bool con = true;
	int set_pause = 0;
	while (con)
	{
		if (clk.GetUpdate(id))
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
		if (clk.GetUpdate(clk3))
		{
			std::cout << "-----\n";
		}
		if (clk.GetElapsed(id) - begin >= time * 400 && set_pause == 0)
		{
			std::cout << "PAUSE!\n";
			clk.SetPause(clk3, true);
			++set_pause;
		}
		if (clk.GetElapsed(id) - begin >= time * 600 && set_pause == 1)
		{
			std::cout << "CONTINUE!\n";
			clk.SetPause(clk3, false);
			++set_pause;
		}
		if (clk.GetElapsed(id) - begin >= time * 1000)
			con = false;
	}
	return 0;
}
