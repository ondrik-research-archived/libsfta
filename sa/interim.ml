(****************************************************************************************)
(*interim data structures*)
(****************************************************************************************)
type automaton = {
	mutable a_alphabet: symbol array;
	mutable a_states: state array;
	mutable a_rules: rule list;
}
and symbol = {
	mutable f_index: int;
	mutable f_name: string;
	mutable f_rank: int;
}
and state = {
	mutable s_index: int;
	mutable s_name: string;
	mutable s_final: bool;
}
and rule = {
	mutable r_symbol: int;
        mutable r_lhs: int array;
        mutable r_rhs: int;
}
type vector = {
	v_index: int;(*a scope of the symbol*)
	v_symbol: int;
	v_lhs: int array;
	v_rhs: int list;
	}
;;
(*
let generate_a a_size max_rank state_c ratio =
  let alph = Array.init a_size (fun i -> { f_index = i; f_name = ""; f_rank = Random.int (max_rank + 1) })
	and states = Array.init state_c (fun i -> { s_index = i; s_name = ""; s_final = (Random.float 1.) < 0.01 }) in
	  let rules = ref [] in
		  let f s l r = rules := { r_symbol = s; r_lhs = Array.of_list l; r_rhs = r }::!rules in (
		    Generator.get_rules_for_alphabet (Array.to_list (Array.map (fun x -> (x.f_index, x.f_rank)) alph)) state_c ratio f;
				{ a_alphabet = alph; a_states = states; a_rules = !rules }
			)
*)
(*

(****************************************************************************************)
(*interim of Taml*)
(****************************************************************************************)

let symbol_of_taml index taml_symbol = 
	{f_index = index; f_name = Taml.Symbol.to_string (fst (taml_symbol));f_rank = snd taml_symbol}
	
(*returnes array of symbols sorted by names and indexed from 0 to card*)
let alphabet_of_taml taml_alphabet =
	let taml_symbol_list = Taml.Alphabet.to_list taml_alphabet in
	let sorted_taml_symbols = List.fast_sort compare taml_symbol_list in
	let i = ref (-1) in
	let alph_list = List.map (function ts -> incr i; symbol_of_taml !i ts) sorted_taml_symbols in
	Array.of_list alph_list

let state_of_taml index final taml_state = {s_index = index; s_name = Taml.Term.to_string taml_state;s_final = final} 

(*returnes array of states sorted by names and indexed from 0 to card*)
let states_of_taml taml_stateset taml_final_stateset =
	let taml_state_list = Taml.State_set.to_list taml_stateset in
	let taml_final_state_list = Taml.State_set.to_list taml_final_stateset in
	let sorted_taml_states = List.fast_sort compare taml_state_list in
	let sorted_final_states = List.fast_sort compare taml_final_state_list in
	let rec state_list index states finals = match states,finals with
		[],[] -> []
		| [],_::_ -> failwith"states_of_taml"
		| h::t,[] -> (state_of_taml index false h)::(state_list (index+1) t [])
		| hs::ts,hf::tf -> 
			let final = hs=hf in
			let s = state_of_taml index final hs in
			s::(state_list (index+1) ts (if final then tf else finals))
	in
	let st_list = state_list 0 sorted_taml_states sorted_final_states in(*should be still sorted*)
	Array.of_list st_list

let rec binary_search cmp array a b elt = 
	if a>b then -1 else (
		let middle = a+((b-a)/2) in
		if cmp (array.(middle)) elt = 0 then middle
		else if cmp (array.(middle)) elt < 0 then binary_search cmp array (middle+1) b elt
			else binary_search cmp array a (middle-1) elt
	)
;;

(*rule of taml rule*)
let rule_of_taml alphabet alph_card states states_card taml_rule =
	let taml_lhs = Taml.Rewrite.lhs taml_rule in
	let taml_rhs = Taml.Rewrite.rhs taml_rule in
	let taml_lhs_list = Taml.Term.list_arguments taml_lhs in
   let taml_rhs = List.hd(Taml.Term.list_leaves taml_rhs) in
	let item_of_tstate taml_state = 
		let cmp p str = compare p.s_name str in
		let state = binary_search cmp states 0 (states_card-1) (Taml.Term.to_string taml_state) in
		if state<0 then failwith"rule_of_taml-item_of_tstate";
		state
	in
	let lhs_list = List.map item_of_tstate taml_lhs_list in
   let rhs = item_of_tstate taml_rhs in
	let taml_symbol = Taml.Term.top_symbol taml_lhs in 
	let cmp f str = compare f.f_name str in
	let f = binary_search cmp alphabet 0 (alph_card-1) (Taml.Symbol.to_string taml_symbol) in
	if f<0 then failwith"rule_of_taml";
   {r_symbol = f;r_rhs = rhs; r_lhs = Array.of_list lhs_list}

(*I expect simplified or "normal" taml automaton on the input*)
let of_taml taml_aut =
	let alphabet = alphabet_of_taml (Taml.Automaton.get_alphabet taml_aut) in
	let states = states_of_taml (Taml.Automaton.get_states taml_aut) (Taml.Automaton.get_final_states taml_aut) in
	let alpha_card = Array.length alphabet in
	let states_card = Array.length states in
	let taml_rule_list = Taml.Rewrite.to_list (Taml.Automaton.get_transitions taml_aut) in
	{
	 a_alphabet = alphabet; 
	 a_states = states; 
	 a_rules = List.map (rule_of_taml alphabet alpha_card states states_card) taml_rule_list
	}
*)
let final_states aut = List.filter (fun s -> s.s_final) (Array.to_list aut.a_states) 

let rank aut = Array.fold_left (fun max_rank f -> max max_rank f.f_rank)
 0 aut.a_alphabet


(*
(****************************************************************************************)
(*interim of crippled Taml (alternative to Taml.simplify)*)
(****************************************************************************************)
(*Here I expect cripple Taml automaton
 * Taml.simplify is uneffective, so I have to rewrite it*)
module Ordered_string =
   struct
      type t = string
      let compare s1 s2 = compare s1 s2   
   end

module String_map = Map.Make(Ordered_string)
module String_set = Set.Make(Ordered_string)

(*make a state from taml and add it inte the set of states (if it is not yet there)*)
let get_state_and_update_states final_names states_card states taml_state =
	let name = Taml.Term.to_string taml_state in
	try (String_map.find name states).s_index,states_card,states
	with Not_found -> 
		let new_state = 
			{s_name = "q"^(string_of_int states_card);s_index = states_card;s_final = String_set.mem name final_names} 
		in
		states_card,(states_card+1),(String_map.add name new_state states)

(*rule of taml rule*)
let rule_of_taml_and_update_states alphabet alph_card final_names states_card states taml_rule =
	let taml_lhs = Taml.Rewrite.lhs taml_rule in
	let taml_rhs = Taml.Rewrite.rhs taml_rule in
	let taml_lhs_list = Taml.Term.list_arguments taml_lhs in
   let taml_rhs = List.hd (Taml.Term.list_leaves taml_rhs) in
	let lhs_list_rev,new_states_card,new_states = 
		List.fold_left (fun (l,tmp_states_card,tmp_states) taml_lhs_item ->
			let state,new_tmp_states_card,new_tmp_states =
				get_state_and_update_states final_names tmp_states_card tmp_states taml_lhs_item
			in
			state::l,new_tmp_states_card,new_tmp_states
		)
		([],states_card,states) taml_lhs_list
	in
	let lhs_list = List.rev lhs_list_rev in
   let rhs,new_states_card,new_states = 
		get_state_and_update_states final_names new_states_card new_states taml_rhs 
	in
	let taml_symbol = Taml.Term.top_symbol taml_lhs in 
	let cmp f str = compare f.f_name str in
	let f = binary_search cmp alphabet 0 (alph_card-1) (Taml.Symbol.to_string taml_symbol) in
	if f<0 then failwith"rule_of_taml";
   {r_symbol = f;r_rhs = rhs; r_lhs = Array.of_list lhs_list},new_states_card,new_states

(*I expect any, even criple, taml automaton on the input*)
let of_cripple_taml cripple_taml_aut =
	let half_cripple_taml_aut = Taml.Automaton.accessibility_cleaning cripple_taml_aut in
	let alphabet = alphabet_of_taml (Taml.Automaton.get_alphabet half_cripple_taml_aut) in
	let alpha_card = Array.length alphabet in
	let taml_rule_list = Taml.Rewrite.to_list (Taml.Automaton.get_transitions half_cripple_taml_aut) in
	let final_names = 
		List.fold_left (fun set taml_state -> String_set.add (Taml.Term.to_string taml_state) set)
		String_set.empty (Taml.State_set.to_list (Taml.Automaton.get_final_states half_cripple_taml_aut))
	in
	let rules,state_set_card,state_set = 
		List.fold_left (fun (tmp_rules,old_state_set_card,old_state_set) taml_rule ->
			let new_rule,new_state_set_card,new_state_set = 
				rule_of_taml_and_update_states 
					alphabet alpha_card final_names old_state_set_card old_state_set taml_rule
			in
			new_rule::tmp_rules,new_state_set_card,new_state_set
		)
		([],0,String_map.empty) taml_rule_list 
	in
	let states = Array.of_list (String_map.fold (fun _ state l -> state::l) state_set []) in
 	Array.fast_sort (fun s1 s2 -> compare s1.s_index s2.s_index)  states;
	{
	 a_alphabet = alphabet; 
	 a_states = states;
	 a_rules = rules;	
	}

let renumber_rules renumbering_array rules =
	let renumber_rule renumbering_array {r_rhs = rhs;r_lhs = lhs;r_symbol = f} =
		let new_rhs = renumbering_array.(rhs) in
		let new_lhs = Array.map (function s -> renumbering_array.(s)) lhs in
		{r_rhs = new_rhs;r_lhs = new_lhs;r_symbol = f}
	in
	List.map (renumber_rule renumbering_array) rules

let utility_cleaning aut = 
	let rule_array = Array.of_list aut.a_rules in
	let rules_of_states = Array.make (Array.length aut.a_states) [] in
	Array.iteri (fun r {r_rhs = rhs} ->
		rules_of_states.(rhs) <-r::rules_of_states.(rhs)
	) rule_array;	
	let usefull = Array.make (Array.length aut.a_states) false in
	let finals = List.map (function s -> s.s_index) (final_states aut) in
	List.iter (function s -> usefull.(s)<-true) finals;

	let rec update_usefull = function [] -> () | in_usefull ->

		let add_usefull_from_state current_usefull s =
			List.fold_left (fun usf_list r ->
				Array.fold_left (fun tmp_nus lhs_state ->
					if usefull.(lhs_state) then tmp_nus
					else (usefull.(lhs_state) <- true; lhs_state::tmp_nus))
				usf_list rule_array.(r).r_lhs
			) current_usefull rules_of_states.(s)
		in

		let new_usefull =
			List.fold_left (fun tmp_new_usefull s ->
				add_usefull_from_state tmp_new_usefull s
			)
			[] in_usefull
		in
		update_usefull new_usefull
	in

	update_usefull finals;

	let usefull_rules = 
		let usefull_rule {r_lhs=lhs;r_rhs=rhs} = 
			usefull.(rhs) && (Array.fold_left (fun tmp s -> usefull.(s) && tmp) true lhs)
		in
		List.filter usefull_rule aut.a_rules 
	in

	let n = ref (-1) in
	let state_list = ref [] in
	let renumbering_array = Array.make (Array.length aut.a_states) (-1) in
	Array.iteri (fun s _ ->
		if usefull.(s) then (
			incr(n);
			renumbering_array.(s) <- !n;
			state_list:=
				{	
					s_index=(!n);
					s_final = aut.a_states.(s).s_final;
					s_name="q"^(string_of_int (!n));
				}
				::!state_list
		)
	) renumbering_array
	;

	let new_rules = renumber_rules renumbering_array usefull_rules in 

	{
		a_alphabet = aut.a_alphabet;
		a_states = (Array.of_list (List.rev !state_list));
		a_rules = new_rules;
	}
*)
(****************************************************************************************)
												(*Rename states*)
(****************************************************************************************)
let rename a =
	let new_states = 
		Array.mapi (fun i {s_final = final} -> {s_final = final;s_index = i;s_name = "q"^(string_of_int i)}) a.a_states 
	in
	{
		a_alphabet = a.a_alphabet;
		a_states = new_states;
  		a_rules = a.a_rules;
	}

(****************************************************************************************)
												(*union*)
(****************************************************************************************)
let union a b = 
	if a.a_alphabet <> b.a_alphabet then failwith"Interim.union";
	let na = Array.length a.a_states in
	let new_states = Array.mapi (
		fun i {s_final=final} -> 
			{s_index=i;s_name="q"^(string_of_int i);s_final=final})
							(Array.append a.a_states b.a_states)
	in
	let renumber {r_rhs=rhs;r_lhs=lhs;r_symbol=symbol} = 
		{r_rhs=rhs+na;r_lhs = (Array.map (function s -> s + na) lhs);r_symbol = symbol}
	in
	{
		a_alphabet = a.a_alphabet;
		a_states = new_states;
		a_rules = (List.append a.a_rules (List.map renumber b.a_rules))
	}

(****************************************************************************************)
												(*Completion*)
(****************************************************************************************)
let compare_lhs lhs1 lhs2 = 
	let l = Array.length lhs1 in
	let i = ref 0 in
	let res = ref 0 in
	while (!res = 0) && (!i < l) do
		res:=compare lhs1.(!i) lhs2.(!i);
  		incr(i)
	done;
	!res

let compare_vectors (sym1,lhs1) (sym2,lhs2) =
	let cmpsym = compare sym1 sym2 in
	if cmpsym <> 0 then cmpsym
	else compare_lhs lhs1 lhs2

let compare_by_vector {r_lhs=lhs1;r_symbol=symbol1} {r_lhs=lhs2;r_symbol=symbol2} =
	compare_vectors (symbol1,lhs1) (symbol2,lhs2)
(*let compare_by_vector = compare*)

let rec add_all_missing nst nsym symbols rules completed vector =

	let next_vector (symbol,lhs) =
		let l = Array.length lhs in
		let new_lhs = Array.copy lhs in
		let rec get_next i =
			if i < 0 then
				if symbol < nsym-1 then
					Some (symbol+1,Array.make symbols.(symbol+1).f_rank 0)
				else None
			else
				if new_lhs.(i) < (nst-1) then (
					new_lhs.(i) <- new_lhs.(i)+1;
					Some (symbol,new_lhs)
				)
				else (
					new_lhs.(i) <- 0;
					get_next (i-1)
				)
		in
		get_next (l-1)
	in

	match vector with 
	None -> completed
	| Some (sym,lhs) ->
		let rule_to_add,rules_rest,new_vect = 
			match rules with 
			[] -> {r_rhs = (nst-1); r_lhs = lhs; r_symbol = sym},[],(next_vector (sym,lhs))
			| h::t -> match h with {r_lhs = lhs_old; r_symbol = sym_old} ->
		  		match compare (sym,lhs) (sym_old,lhs_old) with
				0 -> h,t,next_vector (sym,lhs)
		 		| 1 -> h,t,vector
			 	| (-1) -> {r_rhs = nst-1;r_lhs=lhs;r_symbol=sym},rules,next_vector (sym,lhs)
				| _ -> failwith"add_all_missing"
		in
		add_all_missing nst nsym symbols rules_rest (rule_to_add::completed) new_vect

let complete a = 
	let n = Array.length a.a_states in
	let nsym = Array.length a.a_alphabet in
	let new_states = Array.append a.a_states [|{s_index = n;s_name = "q"^(string_of_int n);s_final=false}|] in
	let first_vector = Some (0,Array.make (a.a_alphabet.(0).f_rank) 0) in
	let sorted_rules = (List.fast_sort compare_by_vector a.a_rules) in
	let new_rules = add_all_missing (n+1) nsym a.a_alphabet sorted_rules [] first_vector in
	{a_states = new_states; a_rules = new_rules; a_alphabet = a.a_alphabet}

(****************************************************************************************)
									(*Inversion of a deterministic aut*)
(****************************************************************************************)
let inverse_deterministic a =
	let c = complete a in
	let new_states = 
		Array.map (function {s_index = indec; s_name = name; s_final = final} -> 
			{s_index = indec; s_name = name; s_final = not final}) c.a_states 
	in
	{a_states = new_states; a_alphabet = c.a_alphabet; a_rules = c.a_rules}

(****************************************************************************************)
									(*Is automaton deterministic?*)
(****************************************************************************************)
let is_deterministic a =
	let sorted = List.fast_sort compare_by_vector a.a_rules in
	let _,res = List.fold_left (fun (last,tmp) next -> next,((compare_by_vector last next) <> 0 && tmp)) 
	 ({r_rhs = -1; r_symbol = -1;r_lhs = [||]},true) sorted 
	in
	res
(*
let is_deterministic_taml ta = is_deterministic(of_taml ta)
*)
(****************************************************************************************)
(*to strings*)
(****************************************************************************************)
let symbol_to_string f = f.f_name 
let symbol_to_string_rank f = Printf.sprintf "%s:%d" f.f_name f.f_rank  
let symbol_to_string_verbose f = Printf.sprintf "%s:%d[%d]" f.f_name f.f_rank f.f_index 
let alphabet_to_string alphabet = String.concat " " (List.map
symbol_to_string_rank (Array.to_list alphabet))

let state_to_string s = s.s_name
let state_to_string_final s =  Printf.sprintf "%s:%s" s.s_name (if s.s_final then "F" else "N")  
let state_to_string_verbose s =  Printf.sprintf "%s:%s[%d]" s.s_name (if s.s_final then "F" else "N") s.s_index 

let lhs_to_string states lhs =
	match lhs with 
	[] ->
		Printf.sprintf ""
	| _->  
		Printf.sprintf "(%s)" (String.concat "," (List.map (function st -> state_to_string states.(st)) lhs))
	
let rule_to_string states alphabet r = 
	let lhs = Array.to_list r.r_lhs in
	let rhs = r.r_rhs in
	let lhs_str = lhs_to_string states lhs in
	Printf.sprintf "%s%s -> %s" (symbol_to_string alphabet.(r.r_symbol)) lhs_str (state_to_string states.(rhs))

let state_list_to_string states = String.concat " " (List.map state_to_string states)
let state_array_to_string states =  state_list_to_string (Array.to_list states)
let rule_list_to_string states alphabet rules = String.concat "\n" (List.map (rule_to_string states alphabet) rules)

let to_string a = 
	let states_str = state_array_to_string a.a_states in
	let final_states_str = state_list_to_string (final_states a) in 
	let rules_str = rule_list_to_string a.a_states a.a_alphabet  a.a_rules in
	Printf.sprintf "States %s\nFinal States %s\nTransitions\n%s\n" states_str final_states_str rules_str

let print a = print_string (to_string a) ;;

module Lisa = struct
	let lhs_to_string states lhs =
		match lhs with 
		[] ->
			Printf.sprintf ""
		| _->  
			Printf.sprintf "%s" (String.concat " " (List.map (function st -> state_to_string states.(st)) lhs))
		
	let rule_to_string states alphabet r = 
		let lhs = Array.to_list r.r_lhs in
		let rhs = r.r_rhs in
		let lhs_str = lhs_to_string states lhs in
		Printf.sprintf "%s %s %s" (symbol_to_string alphabet.(r.r_symbol)) (state_to_string states.(rhs)) lhs_str 

	let rule_list_to_string states alphabet rules = String.concat "\n" (List.map (rule_to_string states alphabet) rules)

	let to_string a = 
		let states_str = state_array_to_string a.a_states in
		let final_states_str = state_list_to_string (final_states a) in 
		let rules_str = rule_list_to_string a.a_states a.a_alphabet  a.a_rules in
		Printf.sprintf "# states\n%s\n# accepting\n%s\n# rules\n%s\n" states_str final_states_str rules_str

	let print a = print_string (to_string a) ;;
	let to_file a file = 
		let open_delete name = open_out_gen [Open_wronly;Open_creat;Open_text] 0o666 name in
		let f = open_delete file in
		Printf.fprintf f "%s"  (to_string a);
		close_out f
end

(*
(****************************************************************************************)
(*Taml of interim *)
(****************************************************************************************)
let alphabet_to_taml alpha =
	Array.fold_left (fun t_alph {f_name = name;f_rank = rank} ->
		Taml.Alphabet.add_symbol (Taml.Symbol.of_string name) rank t_alph)
		Taml.Alphabet.new_alphabet alpha

let states_to_ops states =
	Array.fold_left (fun ops {s_name = name} ->
		Taml.Alphabet.add_symbol (Taml.Symbol.of_string name) 0 ops)
		Taml.Alphabet.new_alphabet states

let states_to_taml states =  
	Array.fold_left (fun t_sts q ->
		Taml.State_set.add (Common.Const (Taml.Symbol.of_string q.s_name)) t_sts)
		Taml.State_set.empty states

let final_states_to_taml states =
	Array.fold_left (fun t_sts q ->
		if q.s_final then
			Taml.State_set.add (Common.Const (Taml.Symbol.of_string q.s_name)) t_sts
		else t_sts)
		Taml.State_set.empty states

let rule_to_taml alphabet states r = 
	let symbol = Taml.Symbol.of_string alphabet.(r.r_symbol).f_name in
	let rhs = Common.Special (Common.Const (Taml.Symbol.of_string states.(r.r_rhs).s_name)) in
	match r.r_lhs with
	[||] -> Taml.Rewrite.new_rule (Common.Const (symbol)) rhs
	| _ -> 
		let lhs = List.map 
			(function q -> Common.Special (Common.Const (Taml.Symbol.of_string states.(q).s_name))) 
			(Array.to_list r.r_lhs) 
		in
		Taml.Rewrite.new_rule (Common.Fsym (symbol,lhs)) rhs

let rules_to_taml alphabet states rules = 
	Taml.Rewrite.list_to_trs (List.map (rule_to_taml alphabet states) rules) 
		
let to_taml {a_alphabet = alphabet; a_states = states; a_rules = rules} = 
	Taml.Automaton.make_automaton
		(alphabet_to_taml alphabet)
		(states_to_ops states)
		(states_to_taml states)
		(final_states_to_taml states)
		(Taml.Rewrite.empty)
      (rules_to_taml alphabet states rules)
*)
(****************************************************************************************)
(*sorting and spliting transition function*)
(****************************************************************************************)

(*comparing rules*)
type rule_compare_method = By_symbol | By_lhs of int | By_rhs
let rule_comparator = function 	
	By_symbol ->  fun r1 r2 -> compare r1.r_symbol r2.r_symbol
	| By_lhs i -> fun r1 r2 -> compare r1.r_lhs.(i) r2.r_lhs.(i)
	| By_rhs -> fun r1 r2 -> compare r1.r_rhs r2.r_rhs

(*input - sorted (by comparator) list
 *output - list of lists with equal elements (by comparator))
 *lists of eq elts are in reversed order !!! *)
let split_sorted comparator list =
	let identical e1 e2 = comparator e1 e2 = 0 in
	let rec do_split_sorted list segments =
		match list with 
		[] -> segments
		| r::list_tail -> 
			match segments with
			[] -> do_split_sorted list_tail [[r]]
			| (sh::st)::segments_tail -> 
				if identical sh r then 
					do_split_sorted list_tail ((r::sh::st)::segments_tail) 
				else  do_split_sorted list_tail ([r]::(sh::st)::segments_tail) 
			| _ -> failwith"split_sorted"
	in
	do_split_sorted list []

(*sorts lists and divides to ekvivalence classes -- (lists in reversed order!!)*)
let sort_and_split comparator rules =
	let sorted = List.fast_sort comparator rules in
	split_sorted comparator sorted

(****************************************************************************************)
												(*Intersection*)
(****************************************************************************************)
module Ordered_pair =
   struct
      type t = (int*int)
		let compare a b = compare a b   
   end

module Pair_map = Map.Make(Ordered_pair)

let identify_rule_groups_with_same_symbol rules1 rules2 =
	let rll1 = sort_and_split (rule_comparator By_symbol) rules1 in
	let rll2 = sort_and_split (rule_comparator By_symbol) rules2 in
	let rec connect l1 l2 res =
		match l1,l2 with
		h1::t1,h2::t2 -> (
			match h1,h2 with {r_symbol=s1}::_,{r_symbol=s2}::_ ->
				if s1 < s2 then connect l1 t2 res
				else if s1 > s2 then connect t1 l2 res
				else connect t1 t2 ((h1,h2)::res)
			| _ -> failwith"connect"
		)
		| _ -> res
	in
	connect rll1 rll2 []
;;

let get_state_and_actualize_map (map,n) pair =
(*	Printf.printf"(%d.%d)" (fst pair) (snd pair);flush_all();*)
	try (map,n),(Pair_map.find pair map)
	with Not_found -> ((Pair_map.add pair n map),(n+1)),n
;;

let get_rule_and_actualize_map (map,n) symbol (rhs1,lhs1) (rhs2,lhs2) =
(*	Printf.printf"(%d.%d)" rhs1 rhs2;flush_all();*)
	let (map1,n1),rhs = get_state_and_actualize_map (map,n) (rhs1,rhs2) in
	let map2 = ref map1 in 
	let n2 = ref n1 in
	let l = Array.length lhs1 in
	let lhs = Array.make l (-1) in
	for i = 0 to l - 1 do
		let (map_tmp,n_tmp),q = get_state_and_actualize_map (!map2,!n2) (lhs1.(i),lhs2.(i)) in
		map2 := map_tmp; n2 := n_tmp;
		lhs.(i) <- q
	done;
	(!map2,!n2),{r_symbol=symbol; r_lhs = lhs; r_rhs = rhs}
;;

let intersect_rules_with_same_symbol  ((state_map,statesn),input_rules) (rules1,rules2) =
	List.fold_left (fun ((map,n),new_rules) {r_symbol = symbol;r_rhs = rhs1; r_lhs = lhs1} ->
		List.fold_left (fun ((map1,n1),new_rules1) {r_symbol= s;r_rhs = rhs2; r_lhs = lhs2} -> 
(*			if symbol<>s then failwith"fnfn";*)
			let (map2,n2),rule = get_rule_and_actualize_map (map1,n1) symbol (rhs1,lhs1) (rhs2,lhs2) in
			(map2,n2),(rule::new_rules1)
		) ((map,n),new_rules) rules2
	) ((state_map,statesn),input_rules) rules1
;;

let inter	{a_states = states1; a_rules = rules1; a_alphabet = alphabet1}
				{a_states = states2; a_rules = rules2; a_alphabet = alphabet2} =
	let rule_list_pair_list = identify_rule_groups_with_same_symbol rules1 rules2 in
	let (state_map,statesn),new_rules = 
		List.fold_left intersect_rules_with_same_symbol ((Pair_map.empty,0),[]) rule_list_pair_list
	in
	let state_list = Pair_map.fold (fun (r1,r2) q states -> 
(*		Printf.printf"(%d.%d)" r1 r2;flush_all();*)
		{s_index = q; s_name = "q"^(string_of_int q);s_final = states1.(r1).s_final && states2.(r2).s_final}::states)
		state_map []
	in
	let new_states = Array.of_list state_list in
	Array.fast_sort (fun q r -> compare q.s_index r.s_index)  new_states;
	let res = {a_alphabet = alphabet1; a_states = new_states; a_rules = new_rules} in
(*	print res;*)
	res
;;





(****************************************************************************************)
(*vector format*)
(****************************************************************************************)

(*vector reference comparator*)
let vector_comparator = function 	
	By_symbol ->  fun v1 v2 -> compare (v1.v_symbol) (v2.v_symbol)
	| By_lhs i -> fun v1 v2 -> compare v1.v_lhs.(i) v2.v_lhs.(i)
	| _->failwith"vector_comparator"

let rec split_by_lhs rule_ll i = 
	if i < 0 then rule_ll else
	let new_rll = List.concat (List.map (sort_and_split (rule_comparator (By_lhs i))) rule_ll) in
	split_by_lhs new_rll (i-1)
		

(*generates vectors of rules with symbol f*)
let fvectors_of_frules frules =
	let symbol = (List.hd frules).r_symbol in
	let frank = Array.length (List.hd frules).r_lhs in
	let lhs_groups = split_by_lhs [frules] (frank-1) in
	let vector_of_group i group =
		let group_by_rhs = sort_and_split (rule_comparator By_rhs) group in
		{
			v_index = i;
			v_symbol = symbol;
			v_lhs = (List.hd group).r_lhs;
			v_rhs = List.map (function rs -> (List.hd rs).r_rhs) group_by_rhs;
		}  
	in
	let i = ref (-1) in
	List.map (function g -> incr(i);vector_of_group !i g) lhs_groups

let get_vectors aut =
	let frules_list = sort_and_split (rule_comparator By_symbol) aut.a_rules in
	let fvectors_list = List.rev_map fvectors_of_frules frules_list in
	fvectors_list
;;

(*
(*faster alternative to Taml simplify*)
let simplify_taml taml_aut = to_taml (rename (utility_cleaning (of_cripple_taml taml_aut)))
(*faster alternative to Taml inter*)
let inter_taml a b = (to_taml (inter (of_taml a) (of_taml b)))
(*	(to_taml (inter (of_taml a) (of_taml b)))*)
(*	print_endline"fnfn";flush_all();*)
(*	Taml.Automaton.print a;*)
(*	Taml.Automaton.print b;*)
(*	let i = inter (of_taml a) (of_taml b) in*)
(*	print i;flush_all();*)
(*	to_taml i*)

*)
