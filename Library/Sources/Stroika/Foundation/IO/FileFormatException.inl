/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileFormatException_inl_
#define _Stroika_Foundation_IO_FileFormatException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO {

    /*
      ********************************************************************************
      ******************************** FileFormatException ***************************
      ********************************************************************************
      */
    class [[deprecated ("deprecated because never used, and probably best to use DataExchange::BadFormatException (or subclass) - use Activities to capture the file information Since Stroika v2.1d18")]] FileFormatException : public Execution::Exception<>
    {
    private:
        using inherited = Execution::Exception<>;

    public:
        FileFormatException (const String& fileName);

    public:
        nonvirtual String GetFileName () const
        {
            return fFileName_;
        }

    private:
        String fFileName_;
    };

}
#endif /*_Stroika_Foundation_IO_FileFormatException_inl_*/
