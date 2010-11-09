
class virtual prPairBase (lts : Lts.lts) = object(self)

  val mutable _lts = lts
	val mutable _relation = new Relation.relation 16
	
	initializer
		if lts#nstates < 1 then
			failwith "invalid lts";

  method get_rel = _relation

	method virtual dump : unit
  method virtual get_sim : int -> bool array array
	method virtual fakeSplit : int list -> unit
	method virtual init : unit
	method virtual compute : unit

end;;
