# Stroika::[Foundation](../../../)::[DataExchange](../../)::[Variant](../)::XML

This folder contains all the Stroika Library [Foundation](../../../)::[DataExchange](../../)::[Variant](../)::XML source code.

A VariantValue written as XML, and read back - so a value mapped once (ObjectVariantMapper) serves whichever format a request
asked for, XML or JSON, since both writers take the same VariantValue.

- [Binding.h](Binding.h)
- [Reader.h](Reader.h)
- [Writer.h](Writer.h)

One key note to understand: VariantValue doesn't fit perfectly with XML (detecting arrays a bit ambiguous, no clear notion of attributes vs elements, and others). These ambiguities must somehow be addressible.

A [Binding](Binding.h) addresses them, and the Reader and Writer are given the same one - what one wrote, the other must
know to read. The Writer's Options add only how it lays the text out (pretty printing, indentation).

## The default <-> correspondence between VariantValue and XML

| VariantValue | XML |
|---|---|
| an object's member `K` | the element `<K>` |
| an array under member `K` | `<K>`, wrapping an `<Array>` per item |
| anything else | its element's text, as `VariantValue::As<String>` writes it (a date ISO 8601, a BLOB base64, as in JSON) |
| null | an empty element |

The document element is the value's one member: `{"device": {...}}` is `<device>...</device>`. A Binding with an `fName`
names it instead, and the value is its content.

Reading is the inverse: an element with child elements is an object, one with only text is that text, and children of one name
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

Nothing produces a Binding but hand-written code yet. An `ObjectVariantMapper` could, since it knows a type's field names and
their declaration order (<https://github.com/SophistSolutions/Stroika/issues/1217>), and so could an XSD or WSDL, where the
order is normative rather than incidental.

Namespaces are written as a default namespace (`xmlns=`), inherited by the elements inside - or with a prefix, where a
Binding's `fNamespacePrefixes` declares one on its element, which an attribute in a namespace requires.

## Not expressible

Mixed content - text beside child elements - which the Reader throws on rather than drop the text silently.
<https://github.com/SophistSolutions/Stroika/issues/954>

Comments and processing instructions are dropped, so a document rewritten through a VariantValue loses them. Fixable here if
ever wanted, but `StructuredStreamEvents::IConsumer` reports neither, so the SAX reader must report them first.
