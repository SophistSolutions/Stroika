TODO (Foundation::Containers::Concrete)

	o	Need Dynamic_cast<> or some such for all concrete containers.
		
		EG:
			// useful generic casting tool for containers (like Sequence<int> to Sequence_Array<int> - - but returns null if not right type so you can use as:
			 If (Sequence_Array<int>* s = myS.Dyamic_Cast<Sequence_Array<int>>) {
				 s->SetSlotsAlloced (40);
			}
			BUT  trick is finding right target type. Maybe have generic private name in �ContainerOf� � and have all the ContainerOf�s have a generic �extended friend� for Iterable<>::Dynamic_cast<>??
			Template <typename ContainerOfT>
						ContaienrOfT* Iterable<T>::Dynamic_Cast () {
							  If (dynamic_cast<T::Rep*> (GetRep()) {
										 Return reinterpret_cast<ContainerOfT*> (this);
				   }
				 Return nullptr;
				}

		Regular conversion cast would make a COPY. We need some way to return the sequence iff its already of the right type
		so you can tweak a known type (like setcapacity).

	o	MAJOR and most important thing todo is copy forward Stroika 1.0 Archive concrete
		implementations and update them for Stroika 2.0

	o	Careful, precise documentation of semantics.
