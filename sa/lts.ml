
(* lts *)
class lts nsymbols nstates = object (self)

	val mutable _data_pre = Array.make_matrix nsymbols nstates []
(*	val mutable _data_post = Array.make_matrix nsymbols nstates []*)
	val mutable _sym_pre = Array.init nstates (fun q -> new SmartSet.smartSet nsymbols)
	val mutable _nsymbols = nsymbols
	val mutable _nstates = nstates
	val mutable _ntransitions = 0
	val mutable _finalStates : int list = []

	method newTransition q a r = (
		_data_pre.(a).(r) <- (q::(_data_pre.(a).(r)));
(*		_data_post.(a).(q) <- (r::(_data_post.(a).(q)));*)
		_sym_pre.(r)#add a;
		_ntransitions <- _ntransitions + 1
	)

	method newFinalState q = (
		_finalStates <- (q::_finalStates);
	)

  method pre q a = _data_pre.(a).(q)

	method iter f =
    Array.iteri (fun a pre_a -> Array.iteri (fun r pre_a_r -> List.iter (fun q -> f q a r) pre_a_r) pre_a) _data_pre

	method data_pre = _data_pre
	method data_post a = (
		let arr = Array.make _nstates [] in (
			Array.iteri (fun r pre_r ->
				List.iter (fun q -> arr.(q) <- r::arr.(q)) pre_r
			) _data_pre.(a);
			arr
		)
	)
	method sym_pre = _sym_pre
(*
		method setData d = (
		_data <- d;
		_nsymbols <- Array.length _data;
		_nstates <- Array.length _data.(0)
	)
*)	
	method nsymbols = _nsymbols
	method nstates = _nstates
	method ntransitions = _ntransitions
	method finalStates = _finalStates;
	
	method stats =
		Printf.sprintf "states: %d, symbols: %d, transitions: %d" _nstates _nsymbols _ntransitions

	method dump = self#iter (Printf.printf "%d --%d--> %d\n")
	
end;;
(*
let generate_lts a_size state_c ratio =
  let alph = Array.to_list (Array.init a_size (fun i -> (i, 1)))
	and lts = new lts a_size state_c in
		let f s l r = lts#newTransition (List.hd l) s r in (
		  Generator.get_rules_for_alphabet alph state_c ratio f;
		  lts
		)
;;
*)
