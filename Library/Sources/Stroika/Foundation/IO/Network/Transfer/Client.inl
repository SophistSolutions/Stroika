/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_Network_Transfer_Client_inl_
#define	_Stroika_Foundation_IO_Network_Transfer_Client_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {
				namespace	Transfer {


					// class Connection::_IRep
					inline	Connection::_IRep::_IRep ()
						{
						}
					inline	Connection::_IRep::~_IRep ()
						{
						}


					// class Connection
					inline	Connection::Connection (const Memory::SharedPtr<_IRep>& rep)
						: fRep_ (rep)
						{
						}
					inline	URL		Connection::GetURL () const
						{
							return fRep_->GetURL ();
						}
					inline	void	Connection::SetURL (const URL& url)
						{
							fRep_->SetURL (url);
						}
					inline	void	Connection::Close ()
						{
							fRep_->Close ();
						}
					inline	Response	Connection::SendAndRequest (const Request& r)
						{
							return fRep_->SendAndRequest (r);
						}


				}
			}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_Network_Transfer_Client_inl_*/
