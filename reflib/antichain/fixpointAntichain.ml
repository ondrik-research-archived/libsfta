exception TooLong;;

(*some printing funcitons*)
let prs s jo = if jo then (Printf.printf "%s " s); flush_all ();;
let pri i jo = if jo then (Printf.printf "%d " i); flush_all();;

(*functions manipulating with automaton rewrite system*)

module AutomatonRules = struct
	let is_initial rule = rule.Interim.r_lhs = [||];;
	(*list of automata initial states*)
	let get_initial_states rule_list =
		List.map (fun {Interim.r_rhs = rhs} -> rhs) (List.filter is_initial rule_list);;

	(*list of automatons nonfinal states*)
	let get_nonfinal_states aut = 
		List.map (fun {Interim.s_index=i} -> i) 
			(List.filter (fun q -> not q.Interim.s_final) (Array.to_list aut.Interim.a_states))
	;;
end;;


module OrderedState = struct
	type t = int
	let compare = compare 
end;;

(*set of states and set of sets of states*)
module SS =	Set.Make(OrderedState)

(*construction of transition table from automaton list of rewrite rules*)
(*uses Interim, not Taml*)
module Table = struct

	let of_rule_list ?(shift = 0) alphabet rule_list = 


		let rule_class_list = 
			List.rev (Interim.sort_and_split (Interim.rule_comparator Interim.By_symbol) rule_list)
		in

		(*adds empty lists for symbols for which there is no class in rule_class_list
		 * (it reverses the order of classes)*)
		let rec add_lists_for_missing_symbols alphabet rcl result =
			match alphabet with
			[] -> result
			| {Interim.f_index=alph_symbol}::alphabet_tl -> (
				match rcl with 
				rule_class::rcl_tl -> (
						match rule_class with 
						({Interim.r_symbol=rule_symbol}::_) -> (
							match compare rule_symbol alph_symbol with 
							  0 -> add_lists_for_missing_symbols alphabet_tl rcl_tl (rule_class::result)
							| 1 -> add_lists_for_missing_symbols alphabet_tl rcl ([]::result)
							| _ -> failwith"add_lists_for_missing_symbols2"
						) 
						| _ -> failwith"add_lists_for_missing_symbols1"
					)
				| [] -> add_lists_for_missing_symbols alphabet_tl [] ([]::result)			
				)
		in

		let complete_rule_class_list = 
			add_lists_for_missing_symbols alphabet rule_class_list []
		in

		(*
		List.map (fun rule_class ->
			List.map (fun {Interim.r_lhs = lhs; Interim.r_rhs = rhs} -> (Array.to_list lhs, rhs)) rule_class
		) complete_rule_class_list
		*)

		(*we need to shift states of the second automaton when computing incusion with help of simulation*)
		let shift_lhs lhs = Array.map (fun q -> q+shift) lhs in

		List.map (fun rule_class ->
			List.map (fun {Interim.r_lhs = lhs; Interim.r_rhs = rhs} -> (Array.to_list (shift_lhs lhs), rhs+shift)) rule_class
		) complete_rule_class_list
	;;

end;;


(*maintaining leikographically ordered tuples of integers*)
module IndexTuple = struct
	(*returns next tuple in lexikogr. ord*)
	let rec succ index_tuple max_index = 
		match index_tuple with 
		[] -> None |
		h::t -> if h < max_index then Some ((h+1)::t) 
			else match (succ t max_index) with 
				None -> None | 
				Some tup -> Some (0::tup)
	;;

	(*returns first lexikogr. tuple of some length*)
	let rec first arity = 
		if arity <= 0  then [] else 0::(first (arity-1))
	;;
	(*retruns tuple of arrays elements - their positions are in index_tuple*)
	let rec get_tuple index_tuple element_array =
		match index_tuple with 
		[] -> [] |
		index::t -> (Array.get element_array index)::(get_tuple t element_array)
	;;
end;;

(*interface for element of antichain in fixed-point algorithm*)
module type ANTICHAIN_ELT =
    sig
      type t
      val compare : t -> t -> int
      val smaller : t -> t -> bool
      val bigger : t -> t -> bool
end;;


module type STATE_SET =
	sig
		include ANTICHAIN_ELT
		type elt = int
		val exists : (elt -> bool) -> t -> bool
		val add: elt -> t -> t
		val mem: elt -> t -> bool
		val empty : t 
		val subset : t -> t -> bool 
		val fold : (elt -> 'a -> 'a) -> t -> 'a -> 'a
	end;;

module type STATE_STATE_SET =
	sig
		module StateSet: STATE_SET
		include ANTICHAIN_ELT
		type elt = StateSet.elt 
	end;;


module type ANTICHAIN = 
	sig
		module StateSet :STATE_SET
		module Elt : ANTICHAIN_ELT
		type t
		val contains_smaller : Elt.t -> t -> bool
		val add : Elt.t -> t -> t
(*		val add_all : Elt.t list -> t -> t*)
		val cardinal : t -> int
		val elements : t -> Elt.t list
		val exists : (Elt.t -> bool) -> t -> bool
		val empty : t
		val equal : t -> t -> bool
		val reset : unit -> unit
		val generated : int ref 
		val added : int ref 
	end;;



(*antichain as set of antichain elements*)
module Antichain (E:ANTICHAIN_ELT) (SS:STATE_SET) =
	struct

	module Elt = E
	module StateSet = SS
	include Set.Make(Elt)

(*for measuring ala yu-fang*)
	let generated = ref 0
	let added = ref 0
	let reset() = generated :=0;added := 0

	let contains_smaller set antichain = 
		exists (Elt.bigger set) antichain 

	(*adds set according to antichain algorithm*)
	let add set antichain = 
		incr(generated);
		if contains_smaller set antichain then antichain 
		else (
			incr(added);
			add set (filter (function s -> not (Elt.smaller set s)) antichain)
		)

(*	let add_all l antichain =  *)
(*		List.fold_right add l antichain *)

end;;
	

(*fixed point algorithm (parametrized by antichain type. Antichain type is also parametrized by element type)*)

module FixpointAlg (Ach: ANTICHAIN) = struct
		
	module Antichain = Ach
	
	(*derives initial information needed by fixed point algorithm from automaton*)
	let init ?(shift = 0) aut = 
		let rule_list = aut.Interim.a_rules in
		let alph_list = Array.to_list aut.Interim.a_alphabet in
		let table = Table.of_rule_list ~shift:shift alph_list rule_list in

		let shift_list l = List.map (fun q -> q + shift) l in
		(*set from list of elements, maybe it should be outside, but this way it is simpler*)
		let rec set_of_list state_list = 
			List.fold_right Antichain.StateSet.add state_list Antichain.StateSet.empty in
		let init_set = set_of_list (shift_list (AutomatonRules.get_initial_states rule_list)) in
		let nonfinal_set = set_of_list (shift_list (AutomatonRules.get_nonfinal_states aut)) in
		let final_set = set_of_list (shift_list (List.map (fun {Interim.s_index=i} -> i) (Interim.final_states aut))) in
(*		let final_set = set_of_list (State_set.to_list (Automaton.get_final_states aut)) in*)

		(table,init_set,nonfinal_set,final_set)
	
	(*core of fixed point algorithm*)
	let fixpoint_loop initial_antichain next_step condition blabla =
		Antichain.reset();
		let antichain = ref initial_antichain in
		let next = ref Antichain.empty in
		let result = ref None in
		prs "antichain cardinalities:" blabla;
(*		let t0 = (Unix.times()).Unix.tms_utime in *)
		while !result = None do
(*			let t = (Unix.times()).Unix.tms_utime in *)
(*			if (t-.t0) > 600.0 then (raise TooLong);*)
(*			pri (Antichain.cardinal !antichain) true;*)
			next := next_step !antichain;
			if condition !next then result := Some false
			else if Antichain.equal !antichain !next then result := Some true
			else (antichain := !next; result := None)
		done
		;
		

		(*Printf.printf "Itr,%d,Visited,%d," !Antichain.generated * !Antichain.added;*)

		match !result with 
			Some true -> true |
			Some false -> false |
			None -> false

end;;

