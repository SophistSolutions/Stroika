/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Modbus_IModbusService_h_
#define _Stroika_Frameworks_Modbus_IModbusService_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Containers/Mapping.h"
#include    "../../Foundation/Containers/Set.h"



/**
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  \note   This code is basically all based on
 *          http://www.modbus.org/docs/Modbus_Messaging_Implementation_Guide_V1_0b.pdf
 *
 * TODO:
 *
 *      @todo
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   Modbus {


            using   namespace   Stroika::Foundation;


            template    <typename ID_TYPE, typename VALUE_TYPE>
            struct ModbusRegisterDescriptor {
                /*
                 *  AKA address
                 */
                using   NameType    =   ID_TYPE;
                /**
                 */
                using   ValueType   =   VALUE_TYPE;
            };

            /**
             *  From http://www.modbus.org/docs/Modbus_Messaging_Implementation_Guide_V1_0b.pdf page 8
             *      Coils       Single bit      Read-Write      This type of data can be alterable by an application program
             */
            using   CoilsDescriptorType =   ModbusRegisterDescriptor<uint16_t, bool>;

            /**
             *  From http://www.modbus.org/docs/Modbus_Messaging_Implementation_Guide_V1_0b.pdf page 8
             *      Input Registers     16-bit word     Read-Only       This type of data can be provided by an I/O system
             */
            using   InputRegisterDescriptorType =   ModbusRegisterDescriptor<uint16_t, uint16_t>;

            /**
             *  This is for 2-byte read/write data - data read from the server, and which may be written to the server.
             */
            using   HoldingRegisterDescriptorType   =   ModbusRegisterDescriptor<uint16_t, uint16_t>;



            /**
             *  Users of this module simple subclass this interface, and provide answers to reads and writes, and
             *  then can invoke the server object with this, to create a quickie Modbus TCP server.
             */
            struct      IModbusService {
                virtual ~IModbusService () = default;

                template    <typename MODBUS_REGISTER_DESCRIPTOR>
                using   RegisterValuesMapping   =   Containers::Mapping<typename MODBUS_REGISTER_DESCRIPTOR::NameType, typename MODBUS_REGISTER_DESCRIPTOR::ValueType>;
                template    <typename MODBUS_REGISTER_DESCRIPTOR>
                using   SetRegisterNames    =   Containers::Set<typename MODBUS_REGISTER_DESCRIPTOR::NameType>;

                /**
                 */
                virtual RegisterValuesMapping<CoilsDescriptorType>  ReadCoils (const SetRegisterNames<CoilsDescriptorType>& names) = 0;

                /**
                 */
                virtual void    WriteCoils (const RegisterValuesMapping<CoilsDescriptorType>& values) = 0;

                /**
                 */
                virtual RegisterValuesMapping<InputRegisterDescriptorType>  ReadInputRegisters (const SetRegisterNames<InputRegisterDescriptorType>& names) = 0;

                /**
                 */
                virtual RegisterValuesMapping<HoldingRegisterDescriptorType>    ReadHoldingRegisters (const SetRegisterNames<HoldingRegisterDescriptorType>& names) = 0;

                /**
                 */
                virtual void    WriteHoldingRegisters (const RegisterValuesMapping<HoldingRegisterDescriptorType>& values) = 0;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "IModbusService.inl"

#endif  /*_Stroika_Frameworks_Modbus_IModbusService_h_*/
