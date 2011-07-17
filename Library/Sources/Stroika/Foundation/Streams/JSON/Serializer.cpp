/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#include	<sstream>

#include	"../../Characters/StringUtils.h"

#include	"Serializer.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Streams;





/*
 ********************************************************************************
 ************************** Streams::JSON::PrettyPrint **************************
 ********************************************************************************
 */
namespace	{
	void	Indent_ (ostream& out, int indentLevel)
		{
			for (int i = 0; i < indentLevel; ++i) {
				out << "    ";
			}
		}
}
namespace	{
	void	PrettyPrint_ (const Memory::VariantValue& v, ostream& out, int indentLevel);
	void	PrettyPrint_Null_ (ostream& out)
		{
			out << "null";
		}
	void	PrettyPrint_ (bool v, ostream& out)
		{
			if (v) {
				out << "true";
			}
			else {
				out << "false";
			}
		}
	void	PrettyPrint_ (int v, ostream& out)
		{
			out << v;
		}
	void	PrettyPrint_ (float v, ostream& out)
		{
			out << v;
		}
	void	PrettyPrint_ (const wstring& v, ostream& out)
		{
			out << "\"";
			string	tmp	=	Characters::WideStringToUTF8 (v);
			for (string::const_iterator i = tmp.begin (); i != tmp.end (); ++i) {
				switch (*i) {
					case '\"':		out << "\\\""; break;
					case '\\':		out << "\\\\"; break;
					case '\n':		out << "\\n"; break;
					case '\r':		out << "\\r"; break;
// unclear if we need to quote other chars such as \f\t\b\ but probably not...
					default:		out << *i; break;
				}
			}
			out << "\"";
		}
	void	PrettyPrint_ (const vector<Memory::VariantValue>& v, ostream& out, int indentLevel)
		{
			out << "[" << endl;
			for (vector<Memory::VariantValue>::const_iterator i = v.begin (); i != v.end (); ++i) {
				Indent_ (out, indentLevel + 1);
				PrettyPrint_ (*i, out, indentLevel + 1);
				if (i + 1 != v.end ()) {
					out << ",";
				}
				out << endl;
			}
			Indent_ (out, indentLevel);
			out << "]";
		}
	void	PrettyPrint_ (const map<wstring, Memory::VariantValue>& v, ostream& out, int indentLevel)
		{
			out << "{" << endl;
			for (map<wstring, Memory::VariantValue>::const_iterator i = v.begin (); i != v.end ();) {
				Indent_ (out, indentLevel + 1);
				PrettyPrint_ (i->first, out, indentLevel + 1);
					out << " : ";
				PrettyPrint_ (i->second, out, indentLevel + 1);
				++i;
				if (i != v.end ()) {
					out << ",";
				}
				out << endl;
			}
			Indent_ (out, indentLevel);
			out << "}";
		}
	void	PrettyPrint_ (const Memory::VariantValue& v, ostream& out, int indentLevel)
		{
			switch (v.GetType ()) {
				case	Memory::VariantValue::eNull:		PrettyPrint_Null_ (out); break;
				case	Memory::VariantValue::eBoolean:		PrettyPrint_ (static_cast<bool> (v), out); break;
				case	Memory::VariantValue::eInteger:		PrettyPrint_ (static_cast<int> (v), out); break;
				case	Memory::VariantValue::eFloat:		PrettyPrint_ (static_cast<float> (v), out); break;
				case	Memory::VariantValue::eString:		PrettyPrint_ (static_cast<wstring> (v), out); break;
				case	Memory::VariantValue::eMap:			PrettyPrint_ (static_cast<map<wstring, Memory::VariantValue>> (v), out, indentLevel); break;
				case	Memory::VariantValue::eArray:		PrettyPrint_ (static_cast<vector<Memory::VariantValue>> (v), out, indentLevel); break;
				default:	RequireNotReached ();		// only certain types allowed
			}
		}
}

void	Streams::JSON::PrettyPrint (const Memory::VariantValue& v, ostream& out)
{
	PrettyPrint_ (v, out, 0);
}
