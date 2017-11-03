#include <stdint.h>

class ComputePlacesBounds
{
public:
	ComputePlacesBounds();
	int64_t *placesBounds;
	int64_t* computePlacesBounds();
	void solve_invariant_lp(int *invariant, int place_to_find);
	int solve_place_bound_lp(int *invariant, int place);
};