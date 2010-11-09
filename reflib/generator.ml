
let fn = false

(*maintaining tuples of inteegrs*)
let rec exp x e = 
(*	if fn then print_endline"1";*)
(*	flush stdout;*)
	if e<0 then 0 else if e=0 then 1 else x * (exp x (e-1));;  

let num_of_tuple ic tuple =
	let rec get_num_of_tuple ic tuple = 
(*	if fn then print_endline"2";*)
(*	flush stdout;*)
		let arity = List.length tuple in
		match tuple with
		[] -> 0
		| h::t -> (h*(exp ic (arity-1)))+(get_num_of_tuple ic t) 
	in
	get_num_of_tuple ic (List.rev tuple)

;;

let tuple_of_num arity ic n =
	let rec get_tuple_of_num arity ic n =
(*	if fn then print_endline"3";*)
(*	flush stdout;*)
		if arity = 0 then [] else
		((((n/(exp ic (arity-1))))+ ic) mod ic)::(get_tuple_of_num (arity-1) ic (n mod (exp ic (arity-1))))
	in
	List.rev (get_tuple_of_num arity ic n)
;;

let tuples_of_nums arity ic nums = 
   List.rev (List.rev_map (tuple_of_num arity ic) nums)	
;;

let count_of_tuples arity ic = exp ic arity -1
;;

(*stolen from colapsing_v3.ml*)
(*mozna*)
(*
let rec add_to_occupied occupied elem =
	match occupied with
	[] ->  [ elem ]
	| elem2::occ_rest -> 
		if elem=elem2 then failwith"add_to_occupied"
		else if elem2>elem then elem::(elem2::occ_rest)
		else elem2::(add_to_occupied occ_rest elem)
;;
*)

let rec do_add_to_occupied big small elem =
(*	if fn then print_endline"4";*)
(*	flush stdout;*)
	match small with 
	[] -> elem::big
	| e2::small_tail -> 
		if e2 = elem then failwith "do_add_to_occupied"
(*		else if e2 > elem then small_tail@(elem::big)*)
		else if e2 > elem then List.rev_append (List.rev small_tail) (elem::big)
		else do_add_to_occupied small_tail (e2::big) elem 

let add_to_occupied occupied elem =
	do_add_to_occupied occupied [] elem

	
let rotate_occupied occupied elem =
	let rec do_rotate_occupied bigger smaller elem =
(*	if fn then print_endline"5";*)
(*	flush stdout;*)
(*tl*)
		match bigger with
		[] ->  []
		| elem2::bigger_rest -> 
(*			if elem=elem2 then bigger@(List.rev smaller)*)
			if elem=elem2 then List.rev_append (List.rev bigger) (List.rev smaller)
			else if elem2>elem then []
			else do_rotate_occupied bigger_rest (elem2::smaller) elem 
	in 
	do_rotate_occupied occupied [] elem
;;

(*tl*)
let rec get_nearest_unoccupied max occupied elem =
(*	if fn then print_endline"6";*)
(*	flush stdout;*)
	match occupied with
	[] -> elem
	| h::t -> 
		if h=elem then get_nearest_unoccupied max t ((elem + 1 + (max+1)) mod (max+1))
		else elem
;;

let get_random_num max_num occupied =
	let first_try = Random.int (max_num+1) in
	let r_occ = (rotate_occupied occupied first_try) in
	get_nearest_unoccupied max_num r_occ first_try 
;;

(*tl*)
let rec do_get_random_nums max_num occupied n =
(*	if fn then print_endline"7";*)
(*	flush stdout;*)
	if n<=0 then occupied
	else (
		let new_num = get_random_num max_num occupied in
		do_get_random_nums max_num (add_to_occupied occupied new_num) (n-1)
	)
;;

let get_random_nums max_num n = 
	if ((max_num+1) / n) < 2 then ( 
		let excluded = do_get_random_nums max_num [] ((max_num+1) - n) in (*it's ordered*)
		(*mozna*)
		(*
		let rec all_without_excluded current max_num excluded = 
			if current>max_num then []
			else (
				match excluded with
				[] -> current::all_without_excluded (current+1) max_num excluded
				| h::t -> if current<h then current::all_without_excluded (current+1) max_num excluded 
				  else all_without_excluded (current+1) max_num t
			)
		in
		*)

		let rec all_without_excluded current max_num excluded accum = 
(*			if fn then print_endline"8";*)
(*	flush stdout;*)
			if current>max_num then accum
			else (
				match excluded with
				[] -> all_without_excluded (current+1) max_num excluded (current::accum)
				| h::t -> if current<h then all_without_excluded (current+1) max_num excluded (current::accum)
				  else all_without_excluded (current+1) max_num t accum
			)
		in
		(*
		all_without_excluded 0 max_num excluded 
		*)
		all_without_excluded 0 max_num excluded []
	)
	else do_get_random_nums max_num [] n
;;

let get_random_tuples ic arity n =
	tuples_of_nums arity ic (get_random_nums (count_of_tuples arity ic) n)
;;


(*list of integers [0;...;m-1]*)
(*mozna*)
(*
let rec get_int_list m =
	let n = m-1 in
	if n<0 then [] else
	if n=0 then [0] else
	n::(get_int_list n);;
*)

let rec do_get_int_list m accum =
(*	if fn then (Printf.printf "il %d\n" m;print_newline();*)
(*	flush stdout);*)
  if m < 0 then accum
  else do_get_int_list (m-1) (m :: accum);;

let get_int_list m =
  do_get_int_list m [];;


(*name of state qx from integer x*)
let state_name m = Printf.sprintf "q%d" m;;

(*list of state-names from list of integers*)
let ints_to_states ints =
	List.rev (List.rev_map state_name ints);;
(*
let get_stateset_str count =
        let concat_string_int a b = String.concat "" [a;(string_of_int b)] in
        let ints = get_int_list count in
(*		print_endline"checkpoint1";*)
        let strings = Trlist.map (concat_string_int "q") ints in
(*		print_endline"checkpoint2";*)
        let res = String.concat " " strings in
(*		print_endline"checkpoint3";*)
		res
*)
(*let get_stateset_str count = 
	let ints = get_int_list count in
	let strings = List.map (Printf.sprintf "q%d") ints in
	String.concat " " strings;;*)

(*float round*)
let round x = int_of_float (x+.0.5);;

(*proportional part to an integer*)
let part count rate = round ((float count)*.rate);;

(*random list of ints with range 0..max-1 of length length*)
(*mozna*)
(*
let rec get_random_int_list length max =
	let l2 = length-1 in
	if length <= 0 then []
	else (Random.int(max))::(get_random_int_list l2 max);;
*)

let rec do_get_random_int_list length max accum =
(*	if fn then print_endline"a";*)
(*	flush stdout;*)
	if length <= 0 then accum
	else do_get_random_int_list (length-1) max ((Random.int(max))::accum);;

let get_random_int_list length max =
	 do_get_random_int_list length max []

(*list of count random integer lists (max, length) *)
(*mozna*)
(*
let rec get_random_lists length max count =
	if count<=0 then []
	else (get_random_int_list length max)::(get_random_lists length max (count-1));;
*)	
let rec do_get_random_lists length max count accum =
(*	if fn then print_endline"b";*)
(*	flush stdout;*)
	if count<=0 then accum
	else do_get_random_lists length max (count-1) ((get_random_int_list length max)::accum)

let get_random_lists length max count =
	do_get_random_lists length max count []
	
(*symbol name*)
(*let symbol_name syma = fst syma*)
(*	match syma with (s,_)->Taml.Symbol.to_string s;;*)

(*count of transitions of symbol acording to its arity, ratio and count of states*)
let transition_count syma ratio states_c = (*part states_c ratio;;*)
(*	(part (exp states_c (snd syma)) ratio) + 1??;;*)
	max 1 (int_of_float(0.5 +. (ratio *. (float_of_int (exp  states_c (snd syma))))));;(*at least one rule for each symbol*)
(*

(*list of str rules from symbol, list of lists of left states, list of right states*)
let get_rules syma left_int_llist right_int_list =
	let get_rule_syma l r = (fst syma, l, r) in(*?*)
	  List.rev_map2 get_rule_syma left_int_llist right_int_list;;
*)

(*list of random trans. rules strings for one symbol, acording to the ratio,
 count of states and arity of symbol*)
let get_rules_for_symbol state_c syma ratio f = 
	let trans_c = transition_count syma ratio state_c 	
(*	let left_states = get_random_lists (snd syma) state_c trans_c in
	let right_states = get_random_int_list (trans_c) state_c in*)
	and tbl = Hashtbl.create 53 in
	  for i = 0 to trans_c - 1 do
  	  let l = get_random_int_list (snd syma) state_c
	    and r = Random.int state_c in (
		    if not (Hashtbl.mem tbl (fst syma, l, r)) then (
			    Hashtbl.add tbl (fst syma, l, r) true;
			    f (fst syma) l r;
		    )
			)
		done
;;

(*I don't remember, propably it generates transition table string*)
let get_rules_for_alphabet alph state_c ratio f = 
(*	let alph_list = Taml.Alphabet.to_list alph in*)
	let get_rules_fs_state_c_ratio syma = get_rules_for_symbol state_c syma ratio f in
	  List.iter get_rules_fs_state_c_ratio alph
;;

let get_transition_table alph_size max_rank state_c ratio f =
  let alph = Array.to_list (Array.init alph_size (fun i -> (i, Random.int max_rank))) in
	  get_rules_for_alphabet alph state_c ratio f
;;
