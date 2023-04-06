#include "TanmiClock_64.hpp"
#include <vector>

using namespace TanmiClock;

auto calFPS(std::string&) -> int;
auto calStability(std::vector<double>&) -> double;

auto main() -> int
{
	Clock& clk = Clock::Instance();
	clk.SetFramePerSecond("Golbal", 5);
	std::cout << clk.GetFramePerSecond() << std::endl;
	std::string str("demo");
	clk.NewClock(str, 10);
	clk.SetFrameScale(str, 0.1);
	//calFPS(str);
	clk.EraseClock(str);
	return 0;
}
auto calFPS(std::string& str) -> int
{
	Clock& clk = Clock::Instance();
	const float time = 10.0f;
	int count = 0;
	int begin = clk.GetElapsed(str);
	int last = clk.GetElapsed(str);
	bool con = true;
	std::vector<double> vec;
	while (con)
	{
		if (clk.GetUpdate(str))
		{
			std::cout << clk.GetElapsed(str) << " " << clk.GetTick(str) << std::endl;
			last = clk.GetElapsed(str);
			vec.push_back(1.0f / clk.GetTick(str));
			std::cout << clk.GetElapsedRelative(str) << " " << clk.GetTickRelative(str) << std::endl;
			//clk.DEBUG(str);
			count++;
		}
		if (clk.GetUpdate())
		{
			std::cout << "**********\n";
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
