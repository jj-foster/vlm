#include <vector>
#include <string>

namespace utils
{
	/// <summary>
	/// Find indices where a value should be inserted to maintain order.
	/// </summary>
	/// <param name="a">: 1D array</param>
	/// <param name="v">: Value to insert into a</param>
	/// <param name="side"></param>
	/// <returns></returns>
	template <class T>
	int search(const std::vector<T>& a, const T& v, std::string side = "left")
	{
		if (a.empty()) {
			throw std::invalid_argument("Input vector cannot be empty.");
		}

		if (side != "left" && side != "right") {
			throw std::invalid_argument("Invalid value for 'side' parameter. "
				"Must be either 'left' or 'right'");
		}

		if (v <= a[0]) { return 0; }
		if (v >= a.back()) { return a.size() - 1; }

		for (int i{ 1 }; i != a.size(); i++)
		{
			if (side == "left")
			{
				if (a[i - 1] < v && v <= a[i])
				{
					return i;
				}
			}
			else if (side == "right")
			{
				if (a[i - 1] <= v && v < a[i])
				{
					return i;
				}
			}
		}

		throw std::logic_error("The input vector is not sorted.");

	}


	template <class T>
	T lerp2D(const T& x, const T& x0, const T& x1, const T& y0, const T& y1)
	{
		if (x0 == x1) {
			return 0;
		}
		else {
			return ((x - x1) / (x1 - x0)) * (y1 - y0) + y1;
		}
	}

}