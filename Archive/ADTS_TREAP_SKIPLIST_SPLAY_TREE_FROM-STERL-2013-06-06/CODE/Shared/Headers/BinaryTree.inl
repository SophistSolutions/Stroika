

namespace ADT {
    namespace BinaryTree {

inline  bool    FlipCoin ()
{
	static	size_t	sCounter = 0;
	return (++sCounter & 1);
}



    }   // namespace BinaryTree
}   // namespace ADT
