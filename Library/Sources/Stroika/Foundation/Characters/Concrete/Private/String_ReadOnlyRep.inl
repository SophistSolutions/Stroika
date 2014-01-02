/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Concrete_Private_ReadOnlyRep_inl_
#define _Stroika_Foundation_Characters_String_Concrete_Private_ReadOnlyRep_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   Concrete {
                namespace   Private {


                    /*
                     ********************************************************************************
                     ***************************** ReadOnlyRep::_Rep ********************************
                     ********************************************************************************
                     */
                    inline ReadOnlyRep::_Rep::_Rep (const wchar_t* start, const wchar_t* end)
                        : _fStart (start)
                        , _fEnd (end)
                    {
                    }
                    inline  void    ReadOnlyRep::_Rep::_SetData (const wchar_t* start, const wchar_t* end)
                    {
                        Require (_fStart <= _fEnd);
                        _fStart = start;
                        _fEnd = end;
                    }
                    inline     size_t  ReadOnlyRep::_Rep::_GetLength () const
                    {
                        Assert (_fStart <= _fEnd);
                        return _fEnd - _fStart;
                    }
                    inline     Character   ReadOnlyRep::_Rep::_GetAt (size_t index) const
                    {
                        Assert (_fStart <= _fEnd);
                        Require (index < _GetLength ());
                        return _fStart[index];
                    }
                    inline     const Character*    ReadOnlyRep::_Rep::_Peek () const
                    {
                        Assert (_fStart <= _fEnd);
                        static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                        return ((const Character*)_fStart);
                    }


                }
            }
        }
    }
}
#endif // _Stroika_Foundation_Characters_String_Concrete_Private_ReadOnlyRep_inl_
