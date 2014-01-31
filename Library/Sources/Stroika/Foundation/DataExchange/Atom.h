/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Atom_h_
#define _Stroika_Foundation_DataExchange_Atom_h_    1

#include    "../StroikaPreComp.h"

#include    <istream>

#include    "../Memory/SharedByValue.h"
#include    "../Streams/BinaryInputStream.h"
#include    "../Streams/TextInputStream.h"

#include    "VariantValue.h"


/**
 *  \file
 *
 * TODO:
 *      @todo   Very weak (performance), first draft implementaiton of AtomManager_Default.
 *
 *              Instead, store a linked list of BLOBs - and for each BLOB keep
 *              a map<> of strings in each BLOB.
 *
 *              Use that to fairly quickly and cheaply lookup (Intern).
 *              Then change ptrdiff_t to be the pointer to the actual string.
 *
 *      @todo   Better document and define having AtomManagers with custom arenas.
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            /**
             * Default is single global registey. Implemnt using compact storage - with strings allocated
             *  in memory blcoks next to one another (not full string objects)
             *  and then return String_Constant objectst hat point into it.
             *
             * like blockallocation - gains in performacne dueto not ever having to free anything
             *
             *  @todo - CLEANUP DOCS
             */
            struct  AtomManager_Default {
                typedef ptrdiff_t   AtomInternalType;
                DEFINE_CONSTEXPR_CONSTANT (AtomInternalType, kEmpty, -1);
                static  AtomInternalType    Intern (const String& s);
                static  String              Extract (AtomInternalType atomI);
            };


            /**
             *  \brief An Atom is like a String, except that its much cheaper to copy/store/compare, and the semantics of compare are queer
             *
             *  An Atom is a wrapper on an underlying String object. You can always extract the original String.
             *
             *  The name 'atom' comes from LISP, and how it represented symbol names.
             *
             *  An Atom is much cheaper to copy/store and compare than a String. However - note that the compare is NOT the same as
             *  the traditioanl string compare. One Atom is equal to another IFF the underlying Strings would be
             *  equal. But the < behavior is very arbitrary. The only garuantee is that its consistent for the lifetime
             *  of the ATOM_MANAGER.
             *
             *  Note converting a String to an Atom maybe expensive, but if you can store the values as Atoms, future lookups
             *  with something like a hashtable can be much faster.
             *
             *  Atom can even be used with Set_Bitstring () – esp if you don’t use generic Atom<> but one with its own custom arena!
             *
             *  \em Design Choice:
             *      In some ways, this could be more powerful if the Atom construction took a ATOM_MANAGER as parameter.
             *      Then we could store the pointer, and do wholesale clearing / throwing away of atom names. But to make
             *      that work would have alot of cost (always passing in extra param to construction), and
             *      tracking that pointer in each atom instance (would have to use shared_ptr to manage lifetimes or somehow
             *      assure lifetimes of atoms longer than their owning manager.
             *
             *      That might make sense todo, but would have a lot of cost. It COULD be done in the future (this API would then
             *      just be a type specialization).
             *
             *      But thats not where we start.
             *
             *  \em Extending Atom type and funky Managers
             *
             *      In HealthFrame, we use 'atoms' for user-defined strings, but specially formatted strings
             *      like C00013044, or H342341 are concepts. Also - the objects returned internally are concepts.
             *
             *      This fits perfectly. You can use:
             *          struct  AtomManager_Concepts {
             *              typedef ConceptKey  AtomInternalType;
             *              DEFINE_CONSTEXPR_CONSTANT (Concept, kEmpty, ConceptKey());
             *              static  AtomInternalType    Intern (const String& s);
             *              static  String  Extract (AtomInternalType atomI);
             *          };
             *
             *      and then use for
             *
             *      struct Enumeration : Atom<AtomManager_Concepts> {
             *          // just constructors /forwarding
             *          // and
             *          inline ConceptKey GetConceptKey () const { return _GetInternalRep (); }
             *      };
             *
             *      AtomManager_Concepts::Intern () can look to specailly formatted strings and treat them as concept ids and strore
             *      them differntly, and can look at the values in AtomInternalType (ConceptKey) - to use a different algorithm to
             *      genearte the name.
             */
            template    <typename   ATOM_MANAGER = AtomManager_Default>
            class   Atom {
            protected:
                using _AtomInternalType = typename ATOM_MANAGER::AtomInternalType;

            public:
                /**
                 */
                Atom  ();
                explicit Atom (const String& src);
                Atom (const Atom& src);
            protected:
                Atom (const _AtomInternalType& src);

            public:
                /**
                 */
                nonvirtual  String   GetName () const;

            public:
                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                 */
                nonvirtual  int Compare (Atom rhs) const;

            public:
                /**
                 *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const Atom& rhs)
                 */
                nonvirtual  bool operator< (Atom rhs) const;
                nonvirtual  bool operator<= (Atom rhs) const;
                nonvirtual  bool operator> (Atom rhs) const;
                nonvirtual  bool operator>= (Atom rhs) const;
                nonvirtual  bool operator== (Atom rhs) const;
                nonvirtual  bool operator!= (Atom rhs) const;

            public:
                /*
                 */
                nonvirtual  bool    empty () const;

            public:
                /*
                 */
                nonvirtual  void    clear ();

            protected:
                /*
                 */
                nonvirtual  _AtomInternalType    _GetInternalRep () const;

            private:
                _AtomInternalType  fValue_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Atom.inl"

#endif  /*_Stroika_Foundation_DataExchange_Atom_h_*/
