#ifndef KP2_REPRESENTATION_H_
#define KP2_REPRESENTATION_H_

#include <vector>
#include <ostream>

using RepKP2 = std::vector <bool>;

inline std::ostream& operator<<(std::ostream& s, const RepKP2& rep){
	for(const bool& x : rep)
		s << x;
	return s;
}

#endif /*KP2_REPRESENTATION_H_*/

