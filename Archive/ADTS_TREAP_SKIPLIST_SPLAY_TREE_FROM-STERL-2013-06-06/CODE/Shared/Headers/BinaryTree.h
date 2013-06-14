#pragma once

#include "Config.h"

#include "ADT.h"

namespace   ADT {
    namespace   BinaryTree {
        typedef enum Direction {
            kLeft = 0,
            kRight = 1,
            kBadDir = 2,

            kFirstChild = kLeft,
            kLastChild = kRight,
        } Direction;

        static  bool    FlipCoin ();
    }
}


#include "BinaryTree.inl"


