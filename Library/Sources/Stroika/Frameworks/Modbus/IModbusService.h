/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Modbus_IModbusService_h_
#define _Stroika_Frameworks_Modbus_IModbusService_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Containers/Mapping.h"
#include "../../Foundation/Containers/Set.h"
#include "../../Foundation/Execution/StringException.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  \note   This code is basically all based on
 *          http://www.modbus.org/docs/Modbus_Messaging_Implementation_Guide_V1_0b.pdf
 *
 * TODO:
 *
 *      @todo
 */

namespace Stroika {
    namespace Frameworks {
        namespace Modbus {

            using namespace Stroika::Foundation;

            /**
             *
             *  \note   in the Modbus documentation and GUI (of ModScan64) - they list register/coil #s as 1 based,
             *          but the wire protocol uses zero based.
             *
             *      The numbers used here - NameType - are all the PUBLIC numbers, not the wire protocol numbers.
             *
             *      The Stroika wire protocol encoder/decoder does the mapping.
             */
            template <typename ID_TYPE, typename VALUE_TYPE>
            struct ModbusRegisterDescriptor {
                /*
                 *  AKA address
                 */
                using NameType = ID_TYPE;
                /**
                 */
                using ValueType = VALUE_TYPE;
            };

            /**
             *  From http://www.modbus.org/docs/Modbus_Messaging_Implementation_Guide_V1_0b.pdf page 8
             *      Coils       Single bit      Read-Write      This type of data can be alterable by an application program
             */
            using CoilsDescriptorType = ModbusRegisterDescriptor<uint16_t, bool>;

            /**
             *  From http://www.modbus.org/docs/Modbus_Messaging_Implementation_Guide_V1_0b.pdf page 8
             *      Discretes Input       Single bit      Read-Only      ThiThis type of data can be provided by an I/O system
             */
            using DiscreteInputDescriptorType = ModbusRegisterDescriptor<uint16_t, bool>;

            /**
             *  From http://www.modbus.org/docs/Modbus_Messaging_Implementation_Guide_V1_0b.pdf page 8
             *      Input Registers     16-bit word     Read-Only       This type of data can be provided by an I/O system
             */
            using InputRegisterDescriptorType = ModbusRegisterDescriptor<uint16_t, uint16_t>;

            /**
             *  This is for 2-byte read/write data - data read from the server, and which may be written to the server.
             */
            using HoldingRegisterDescriptorType = ModbusRegisterDescriptor<uint16_t, uint16_t>;

            /**
             *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf page 49
             *
             *  \note   These really don't appear to capture any application logic or application notion of failure,
             *          and principally deal with communications failure.
             *
             *          SLAVE_DEVICE_FAILURE may be the only exception, which is somewhat of a blanket - server failed -
             *          HTTP 503 like - (or so it appears).
             */
            enum class ExceptionCode : uint8_t {
                ILLEGAL_FUNCTION                        = 1,
                ILLEGAL_DATA_ADDRESS                    = 2,
                ILLEGAL_DATA_VALUE                      = 3,
                SLAVE_DEVICE_FAILURE                    = 4,
                ACKNOWLEDGE                             = 5,
                SLAVE_DEVICE_BUSY                       = 6,
                MEMORY_PARITY_ERROR                     = 8,
                GATEWAY_PATH_UNAVAILABLE                = 0xa,
                GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0xb,
            };

            /**
             */
            struct Exception : Execution::StringException {
                Exception (ExceptionCode exceptionCode);

                ExceptionCode fExceptionCode;
            };

            /**
             *  Users of this module simple subclass this interface, and provide answers to reads and writes, and
             *  then can invoke the server object with this, to create a quickie Modbus TCP server.
             *
             *  \note   Many of these APIs return uint16_t, but logically try to return other larger things like floats.
             *          So the ReadInputRegisters () and ReadHoldingRegisters () APIs may return mappings outside
             *          the strict range prescribed by their argument 'names'. Those returned values may or may not
             *          be transported over the wire, depending on the caller request.
             */
            struct IModbusService {
                virtual ~IModbusService () = default;

                template <typename MODBUS_REGISTER_DESCRIPTOR>
                using RegisterValuesMapping = Containers::Mapping<typename MODBUS_REGISTER_DESCRIPTOR::NameType, typename MODBUS_REGISTER_DESCRIPTOR::ValueType>;
                template <typename MODBUS_REGISTER_DESCRIPTOR>
                using SetRegisterNames = Containers::Set<typename MODBUS_REGISTER_DESCRIPTOR::NameType>;

                /**
                 *  Coils are boolean valued. OK to return any subset of argument coils. Illegal to return any not specified in argument list.
                 */
                virtual RegisterValuesMapping<CoilsDescriptorType> ReadCoils (const SetRegisterNames<CoilsDescriptorType>& names) = 0;

                /**
                 *  Coils are boolean valued.
                 */
                virtual void WriteCoils (const RegisterValuesMapping<CoilsDescriptorType>& values) = 0;

                /**
                 *  DiscreteInput values are boolean valued.
                 */
                virtual RegisterValuesMapping<DiscreteInputDescriptorType> ReadDiscreteInput (const SetRegisterNames<DiscreteInputDescriptorType>& names) = 0;

                /**
                 *  InputRegister values are boolean uint16_t, and are read-only.
                 */
                virtual RegisterValuesMapping<InputRegisterDescriptorType> ReadInputRegisters (const SetRegisterNames<InputRegisterDescriptorType>& names) = 0;

                /**
                 *  HoldingRegister values are boolean uint16_t, and are read-write.
                 */
                virtual RegisterValuesMapping<HoldingRegisterDescriptorType> ReadHoldingRegisters (const SetRegisterNames<HoldingRegisterDescriptorType>& names) = 0;

                /**
                 *  HoldingRegister values are boolean uint16_t, and are read-write.
                 */
                virtual void WriteHoldingRegisters (const RegisterValuesMapping<HoldingRegisterDescriptorType>& values) = 0;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IModbusService.inl"

#endif /*_Stroika_Frameworks_Modbus_IModbusService_h_*/
