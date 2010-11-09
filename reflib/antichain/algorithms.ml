open Incl
open FixpointAntichain

(*module StateSet = Sim.StateSet*)
(*module StateStateSetF = Sim.StateStateSetF*)

module UnIn (StSe:STATE_SET) (StStSe:STATE_STATE_SET with type t = StSe.elt * StSe.t) = struct

module StateSet = StSe
module StateStateSet = StStSe
module Universality = FixpointAlg(Antichain(StateSet) (StateSet));;
module Inclusion = FixpointAlg(Antichain(StateStateSet) (StateSet));;

(*
let sort_alphabet f = 
	let str_l = Alphabet.to_list f in
	let sorted_l = List.sort compare str_l in
	let add nf (s,a) = Alphabet.add_symbol s a nf in
	List.fold_left add Alphabet.new_alphabet sorted_l 

let with_sorted_alphabet aut =
	let aut_str = Taml.Automaton.to_string  aut in
	Taml.automaton (sort_alphabet (Taml.Automaton.get_alphabet aut)) aut_str
*)

let rec all_in_tuple l tuple = 
	match (l,tuple) with 
	([],[]) -> true |
	(hl::tl,hr::tr) -> if StateSet.mem hl hr then all_in_tuple tl tr
			else false |
	(_,_) -> false
;;

(*generates post set from n-ary transition rules in argRes_list and from n-ary tuple of sets of states*)
let post_of_set_tuple symbol_rules tuple =
	List.fold_left 
		(fun set ar -> match ar with (a,r) -> if all_in_tuple a tuple then StateSet.add r set else set)
		StateSet.empty 
		symbol_rules 
;;

	(*returns list of possible destination states of state tuple relativelly to rule_list*)
let dest_list_of_state_tuple symbol_rules tuple = 
	List.fold_left (fun dests rule -> if (fst rule) = tuple then (snd rule)::dests else dests) [] symbol_rules
	
let is_nonfinal ?(shift=0) aut set = not (StateSet.exists (fun q -> aut.Interim.a_states.(q-shift).Interim.s_final) set) 

(****************************************************************************)
(**UNIVERSALITY**************************************************************)
(****************************************************************************)
	(*are all statets of list in appropriate sets from tuple (list)?*)
	(*recursion is here dangerous, many tuples. Using while*)

	let add_posts_univ set_array symbol_rules antichain =
		let max = (Array.length set_array)-1 in
		let result = ref antichain in
		let indexes = ref (match symbol_rules with 
					[] -> Some [] | h::_ -> Some (IndexTuple.first (List.length (fst h)))) in
		let inds = ref [] in
		while (match !indexes with None -> false | Some x -> inds:= x;true) do
			let post = post_of_set_tuple symbol_rules (IndexTuple.get_tuple !inds set_array) in
(*			result := StateSet.add post !result;*)
			result := Universality.Antichain.add post !result;
			indexes := IndexTuple.succ !inds max
		done;
		!result

	(*second version, next antich. generation is started from current antich (not from empty). Slightly faster*)
	let next_step_univ table antichain = 
		let set_array = Array.of_list((Universality.Antichain.elements antichain)) in
		List.fold_right (add_posts_univ set_array) table antichain 

(*	let pr l = print_string"{";StateSet.fold (fun q _ -> Printf.printf "%d," q) l ();print_string "}"*)

	let is_universal ?(blabla = false) aut = 
(*		let aut = Interim.of_taml aut in*)
		match (Universality.init aut) with (table,init_set,nonfinal_set,_) ->
(*			pr nonfinal_set;*)
(*		let contains_nonfinal ach = Universality.Antichain.contains_smaller nonfinal_set ach in*)
		let contains_nonfinal ach = 
			Universality.Antichain.exists
(*			(function set -> StateSet.subset set nonfinal_set) ach *)
			(is_nonfinal aut) ach 
		in
		let initial_antichain = Universality.Antichain.add init_set Universality.Antichain.empty in
		Universality.fixpoint_loop initial_antichain (next_step_univ table) contains_nonfinal blabla
	;;

(****************************************************************************)
(**INCLUSION*****************************************************************)
(****************************************************************************)

	let add_posts_incl state_array set_array symbol_rules1 symbol_rules2 antichain =
		let max = (Array.length state_array)-1 in
		let result = ref antichain in
		let indexes = ref (match symbol_rules1 with 
					[] -> Some [] | h::_ -> Some (IndexTuple.first (List.length (fst h)))) in
		
		let inds = ref [] in
		while (match !indexes with None -> false | Some x -> inds:= x;true) do
			let state_tuple = IndexTuple.get_tuple !inds state_array in
			let post_variants = dest_list_of_state_tuple symbol_rules1 state_tuple in
			if post_variants <> [] then ( 
				let set_tuple = IndexTuple.get_tuple !inds set_array in
				let post_set = post_of_set_tuple symbol_rules2 set_tuple in
				result := (List.fold_left 
					(fun ach state -> Inclusion.Antichain.add (state,post_set) ach) 
					!result post_variants)
			);
			
			indexes := IndexTuple.succ !inds max
		done;
		!result

	(*second version, next antich. generation is started from current antich (not from empty). Slightly faster*)
	let next_step_incl table1 table2 antichain = 
		let eset_list = (Inclusion.Antichain.elements antichain) in

		let set_array = Array.of_list (List.map snd eset_list) in
		let state_array = Array.of_list (List.map fst eset_list)  in
		List.fold_right2 (add_posts_incl state_array set_array) table1 table2 antichain 

	let is_language_included ?(blabla = false) small1 big2 =
		let shift = (Interim.sn small1) in
(*		let small1 = Interim.of_taml small1_in in*)
(*		let big2 = Interim.of_taml big2_in in*)
		if  small1.Interim.a_alphabet <> big2.Interim.a_alphabet then (
			failwith "alphabets not equal"; 
		)
		else (
		match (Inclusion.init small1, Inclusion.init ~shift:shift big2) with 
(*		match (Inclusion.init small1, Inclusion.init big2) with *)
			((table1,init_set1,_,final_set1),(table2,init_set2,nonfinal_set2,_)) ->
		let contains_countraexample ach = Inclusion.Antichain.exists 
(*			(function (s,set) -> StateSet.mem s final_set1 && StateSet.subset set nonfinal_set2) ach in*)
			(function (s,set) -> StateSet.mem s final_set1 && is_nonfinal ~shift:shift big2 set) ach in
		let initial_antichain = 
			StateSet.fold (fun s ach -> Inclusion.Antichain.add (s,init_set2) ach) 
				(init_set1) (Inclusion.Antichain.empty) in
		Inclusion.fixpoint_loop initial_antichain (next_step_incl table1 table2) contains_countraexample blabla
		)
end
;;

