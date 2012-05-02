/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
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


					// class ISession
					inline	ISession::ISession ()
						{
						}
					inline	ISession::~ISession ()
						{
						}


					// class Session
					inline	Session::Session (const Memory::SharedPtr<ISession>& rep)
						: fRep_ (rep)
						{
						}
					inline	URL		Session::GetURL () const
						{
							return fRep_->GetURL ();
						}
					inline	void	Session::SetURL (const URL& url)
						{
							fRep_->SetURL (url);
						}
					inline	void	Session::Close ()
						{
							fRep_->Close ();
						}
					inline	Response	Session::SendAndRequest (const Request& r)
						{
							return fRep_->SendAndRequest (r);
						}


				}
			}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_Network_Transfer_Client_inl_*/
