# Stroika::[Foundation](../../../)::[DataExchange](../../)::[Variant](../)::XML

This folder contains all the Stroika Library [Foundation](../../../)::[DataExchange](../../)::[Variant](../)::XML source code.

A VariantValue written as XML, and read back - so a value mapped once (ObjectVariantMapper) serves whichever format a request
asked for, XML or JSON, since both writers take the same VariantValue.

- [Binding.h](Binding.h)
- [Reader.h](Reader.h)
- [Writer.h](Writer.h)

One key note to understand: VariantValue doesn't fit perfectly with XML (detecting arrays a bit ambiguous, no clear notion of attributes vs elements, and others). These ambiguities must somehow be addressible.

A [Binding](Binding.h) addresses them, and the Reader and Writer are given the same one - what one wrote, the other must
know to read. The Writer's Options add how it lays the text out (pretty printing, indentation); the Reader's, which parser
reads it, and a schema the document must hold to.

## The default <-> correspondence between VariantValue and XML

| VariantValue | XML |
|---|---|
| an object's member `K` | the element `<K>` |
| an array under member `K` | `<K>`, wrapping an `<Array>` per item |
| anything else | its element's text, as `VariantValue::As<String>` writes it (a date ISO 8601, a BLOB base64, as in JSON) |
| null | an empty element |

The document element is the value's one member: `{"device": {...}}` is `<device>...</device>`. A Binding with an `fName`
names it instead, and the value is its content.

Reading is the inverse - and with no types declared (the default; see [Declaring types](#declaring-types---a-schema-not-a-guess)),
it guesses: an element with child elements is an object, one with only text is that text, and children of one name
are an array. **Every leaf reads as a String** - XML says nothing of types, and `VariantValue::As` (so
`ObjectVariantMapper::ToObject`) coerces. An empty element reads as `""`, which is what `null`, `{}` and `[]` are all written
as - unless a Binding says the member is an array. An attribute reads as a member too, by its name, so reading a document of
someone else's drops nothing silently. And a key that is not an XML name (`"my key"`, `"1a"`) cannot be written: the Writer
throws, rather than write malformed XML.

That shape is enough for a service that defines its own XML, and needs nothing declared.

## Matching someone else's schema

A [Binding](Binding.h) says how a value differs from that shape: a member written as an attribute or as its element's text, a
name and namespace of its own, the order XML cares about (an object has none), and which members are arrays. It mirrors the
value's shape and says only what differs - what it does not name keeps the default.

```c++
// UPnP's <stateVariable sendEvents="yes"><name>..</name><dataType>..</dataType></stateVariable>
Binding b{.fName    = "stateVariable"sv,
          .fMembers = {{"sendEvents"sv, Binding{.fKind = Binding::Kind::eAttribute}}, {"name"sv}, {"dataType"sv}}};
```

A Binding is also what settles *"is a single element an array of one?"* - a member with `fItems` is an array however few
elements the document happens to have. Where that `fItems` has a name, it is the item element's, and the member's own element
wraps them - so one document can wrap two arrays differently, as UPnP's `<actionList><action>` and
`<allowedValueList><allowedValue>` do; where it has none, the member's own element is repeated, once per item. `fDefaultArrayItems`
says the same for the arrays, at and inside an element, that have no `fItems` of their own.

Nothing produces a Binding but hand-written code yet. An `ObjectVariantMapper` could, since it knows a type's field names, their
declaration order and their types - so complete, below (<https://github.com/SophistSolutions/Stroika/issues/1217>) - and so
could an XSD or WSDL, where the order is normative rather than incidental.

Namespaces are written as a default namespace (`xmlns=`), inherited by the elements inside - or with a prefix, where a
Binding's `fNamespacePrefixes` declares one on its element, which an attribute in a namespace requires.

Two things such schemas say that a VariantValue has no word for. A **presence marker**: a Boolean written as an empty element, there for
true - UPnP's `<retval/>` (`Binding::Kind::ePresenceMarker`). And **what a document may hold that the Binding does not declare** - a
vendor's own elements, an embedded `<deviceList>` nothing models: `fUndeclaredMembers` rejects it, skips it unread (as XSD's
`xs:any processContents="skip"`), or keeps it as untyped members. Skipping is how a complete Binding reads documents from the
field.

## Declaring types - a schema, not a guess

**Declaring a value's type (`Binding::fType`) changes what the Writer and Reader do with it, a great deal.** With no type
declared - everything above - a Binding says only how a value is written: the Writer takes any value, and the Reader any
document, guessing its shape. Declared, it also says which values there may be, as an XSD does, and the Writer and Reader
throw `BadFormatException` on any other:

| | `fType` nullopt - the default | `fType` declared |
|---|---|---|
| a leaf, read back | a String: `3` as `"3"` | that type: `3` as `3` |
| text not of its type, read | - (every leaf reads as a String) | throws |
| an empty element, read back | `""` (but `[]` where `fItems` is set) | `{}` for `eMap`, `[]` for `eArray` |
| a null member, written | as an empty element: read back `""` | as none: read back absent |
| another type's value, written | as it is (a leaf as its `As<String>`) | throws |
| a member it does not name | written after those it names; read | throws, writing and reading - unless `fUndeclaredMembers` says skip (reading) or keep |
| a child named `<Array>`, read | an item, unless `fMembers` names it | an item only where `fType` is `eArray` |

It is per Binding, so per value: a member of a declared object whose own Binding declares no type is still written and read
the default way.

A Binding that declares the type of every value in it, and leaves nothing else to the default shape, is **complete**
(`Binding::IsComplete ()`). **Given a complete Binding, the Writer and Reader round-trip a VariantValue faithfully, or throw
`BadFormatException` because the value is not one the Binding describes - never anything in between**: nothing guessed, nothing
silently lost, whatever values it holds - as an XSD or a WSDL does. A Binding that leaves things to defaults is shorter, and
particular values can then collide with them (a member keyed `"Array"` reads back as an array's item): the price of the short
Binding, not a bug. `ToString ()` says whether a Binding is complete, and if not, why.

```c++
// UPnP's <stateVariable sendEvents="yes">, every value's type declared: complete
Binding b{.fName    = "stateVariable"sv,
          .fType    = VariantValue::eMap,
          .fMembers = {{"sendEvents"sv, Binding{.fKind = Binding::Kind::eAttribute, .fType = VariantValue::eBoolean}},
                       {"name"sv, Binding{.fType = VariantValue::eString}}}};
```

The one exception is by design: XML has no way to write a null member, an empty array of repeated elements, or a text member
(`Kind::eText`) whose text is empty, but as nothing - so each reads back absent (as `ObjectVariantMapper` omits a null field
anyway). And not yet quite so, a bug: a few characters the XML escaping gets wrong (CR; tab or LF in an attribute) come back
changed - `Binding::IsComplete`'s `@todo`.

## Not expressible

Mixed content - text beside child elements - which the Reader throws on rather than drop the text silently.
<https://github.com/SophistSolutions/Stroika/issues/954>

Comments and processing instructions are dropped, so a document rewritten through a VariantValue loses them. Fixable here if
ever wanted, but `StructuredStreamEvents::IConsumer` reports neither, so the SAX reader must report them first.
