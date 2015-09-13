#pragma once

#include <string>

#include "../../Shared/Headers/Config.h"


class   Exception {
    public:
        Exception () :
            fReason ("")
        {
        }

        Exception (const std::string& s) :
            fReason (s)
        {
        }

        Exception (const char* s) :
            fReason (s)
        {
        }

        ~Exception ()
        {
        }

        std::string GetReason () const
        {
            return fReason;
        }

        void    SetReason (const std::string& s)
        {
            fReason = s;
        }

    private:
        std::string  fReason;
};
